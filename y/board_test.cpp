
#include "../lib/catch.hpp"
#include "../lib/string.h"

#include "board.h"


using namespace Morat;
using namespace Y;

void test_game(Board b, std::vector<std::string> moves, Outcome outcome) {
	REQUIRE(b.moves_made() == 0);
	Side side = Side::P1;
	int made = 0, remain = 28;
	for(auto s : moves) {
		Outcome expected = (s == moves.back() ? outcome : Outcome::UNKNOWN);
		Move move(s);
		CAPTURE(move);
		CAPTURE(b);
		REQUIRE(b.moves_made() == made);
		REQUIRE(b.moves_remain() == remain);
		REQUIRE(b.valid_move(move));
		REQUIRE(b.to_play() == side);
		REQUIRE(b.test_outcome(move) == expected);
		REQUIRE(b.move(move));
		REQUIRE(b.outcome() == expected);
		side = ~side;
		made++;
		remain--;
	}
	REQUIRE(b.moves_made() == made);
	REQUIRE(b.moves_remain() == (outcome == Outcome::UNKNOWN ? remain : 0));
}
void test_game(Board b, std::string moves, Outcome outcome) {
	test_game(b, explode(moves, " "), outcome);
}

TEST_CASE("Y::Board [y][board]") {
	Board b("7");

	SECTION("Basics") {
		REQUIRE(b.size() == "7");
		REQUIRE(b.lines() == 7);
		REQUIRE(b.moves_remain() == 28);
	}

	SECTION("sizes") {
		for (int size = Board::min_size; size <= Board::max_size; size++) {
			CAPTURE(size);
			Board b(to_str(size));
			REQUIRE(b.move(Move(0, 0)));
			REQUIRE(b.move(Move(0, size - 1)));
			REQUIRE(b.move(Move(size - 1, 0)));
		}
	}

	SECTION("valid moves") {
		std::string valid[] = {"A1", "D4",
		"a1", "a2", "a3", "a4", "a5", "a6", "a7",
		   "b1", "b2", "b3", "b4", "b5", "b6",
		      "c1", "c2", "c3", "c4", "c5",
		         "d1", "d2", "d3", "d4",
		            "e1", "e2", "e3",
		               "f1", "f2",
		                  "g1",
		};
		for(auto m : valid){
			REQUIRE(b.on_board(m));
			REQUIRE(b.valid_move(m));
		}
	}

	SECTION("invalid moves") {
		std::string invalid[] = {"a0", "a8", "a10", "b7", "c6", "g2", "e8", "f8", "f0", "h1", "f0"};
		for(auto m : invalid){
			REQUIRE_FALSE(b.on_board(m));
			REQUIRE_FALSE(b.valid_move(m));
		}
	}

	SECTION("duplicate moves") {
		Move m("a1");
		REQUIRE(b.valid_move(m));
		REQUIRE(b.move(m));
		REQUIRE_FALSE(b.valid_move(m));
		REQUIRE_FALSE(b.move(m));
	}

	SECTION("hash") {
		hash_t h = b.test_hash(b.move_valid("a2"));
		REQUIRE_FALSE(h == b.test_hash(b.move_valid("a1")));
		for (auto s : {"a2", "b1", "a6", "b6", "f1", "f2"}) {
			CAPTURE(s);
			b.clear();
			REQUIRE(b.gethash() == 0);
			MoveValid m = b.move_valid(s);
			REQUIRE(h == b.test_hash(m));
			b.move(m);
			REQUIRE(h == b.gethash());
		}
	}

	SECTION("move distance") {
		SECTION("x") {
			REQUIRE(b.dist(Move("b2"), Move("b1")) == 1);
			REQUIRE(b.dist(Move("b2"), Move("b3")) == 1);
		}
		SECTION("y") {
			REQUIRE(b.dist(Move("b2"), Move("a2")) == 1);
			REQUIRE(b.dist(Move("b2"), Move("c2")) == 1);
		}
		SECTION("z") {
			REQUIRE(b.dist(Move("b2"), Move("a3")) == 1);
			REQUIRE(b.dist(Move("b2"), Move("c1")) == 1);
		}
		SECTION("farther") {
			REQUIRE(b.dist(Move("b2"), Move("a1")) == 2);
			REQUIRE(b.dist(Move("b2"), Move("c3")) == 2);
			REQUIRE(b.dist(Move("b2"), Move("d4")) == 4);
			REQUIRE(b.dist(Move("b2"), Move("d3")) == 3);
			REQUIRE(b.dist(Move("b2"), Move("d1")) == 2);
			REQUIRE(b.dist(Move("b2"), Move("e3")) == 4);
		}
	}

	SECTION("Unknown_1") {
		test_game(b, {      "a1", "b1", "a2", "b2", "a3", "b3", "a4"}, Outcome::UNKNOWN);
		test_game(b, {"d4", "a1", "b1", "a2", "b2", "a3", "b3", "a4"}, Outcome::UNKNOWN);
	}

	SECTION("Unknown_2") {
		test_game(b, {      "b1", "c1", "b2", "c2", "b3", "c3", "b4", "c4", "b5", "c5", "a2"}, Outcome::UNKNOWN);
		test_game(b, {"d4", "b1", "c1", "b2", "c2", "b3", "c3", "b4", "c4", "b5", "c5", "a2"}, Outcome::UNKNOWN);
	}

	SECTION("White Connects") {
		test_game(b,
			"c3 e2 c4 c5 e3 d3 d4 b1 c2 g1 b5 d2 d1 a6 a5",
			Outcome::P1);
	}

	SECTION("Black Connects") {
		test_game(b,
			"a1 b2 c3 e2 c1 b5 c5 a4 e3 c2 b6 c4 g1 d4 f2 d3 a3 a5 e1 f1",
			Outcome::P2);
	}
}
