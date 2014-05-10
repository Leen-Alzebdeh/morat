
#pragma once

#include "../lib/gtpcommon.h"
#include "../lib/string.h"

#include "board.h"
#include "game.h"
#include "move.h"
#include "player.h"
#include "solver.h"
#include "solverab.h"
#include "solverpns.h"
#include "solverpns2.h"
#include "solverpns_tt.h"


class GTP : public GTPCommon {
	Game game;

public:
	int verbose;
	bool genmoveextended;
	bool colorboard;

	int mem_allowed;
	bool allow_swap;

	Player player;

	SolverAB    solverab;
	SolverPNS   solverpns;
	SolverPNS2  solverpns2;
	SolverPNSTT solverpnstt;

	GTP(FILE * i = stdin, FILE * o = stdout) : GTPCommon(i, o), game(10) {
		verbose = 1;
		genmoveextended = false;
		colorboard = true;

		mem_allowed = 1000;
		allow_swap = false;

		set_board();

		newcallback("name",            bind(&GTP::gtp_name,          this, _1), "Name of the program");
		newcallback("version",         bind(&GTP::gtp_version,       this, _1), "Version of the program");
		newcallback("verbose",         bind(&GTP::gtp_verbose,       this, _1), "Set verbosity, 0 for quiet, 1 for normal, 2+ for more output");
		newcallback("extended",        bind(&GTP::gtp_extended,      this, _1), "Output extra stats from genmove in the response");
		newcallback("debug",           bind(&GTP::gtp_debug,         this, _1), "Enable debug mode");
		newcallback("colorboard",      bind(&GTP::gtp_colorboard,    this, _1), "Turn on or off the colored board");
		newcallback("showboard",       bind(&GTP::gtp_print,         this, _1), "Show the board");
		newcallback("print",           bind(&GTP::gtp_print,         this, _1), "Alias for showboard");
		newcallback("dists",           bind(&GTP::gtp_dists,         this, _1), "Similar to print, but shows minimum win distances");
//		newcallback("zobrist",         bind(&GTP::gtp_zobrist,       this, _1), "Output the zobrist hash for the current move");
		newcallback("clear_board",     bind(&GTP::gtp_clearboard,    this, _1), "Clear the board, but keep the size");
		newcallback("clear",           bind(&GTP::gtp_clearboard,    this, _1), "Alias for clear_board");
		newcallback("boardsize",       bind(&GTP::gtp_boardsize,     this, _1), "Clear the board, set the board size");
		newcallback("size",            bind(&GTP::gtp_boardsize,     this, _1), "Alias for board_size");
		newcallback("swap",            bind(&GTP::gtp_swap,          this, _1), "Enable/disable swap: swap <0|1>");
		newcallback("play",            bind(&GTP::gtp_play,          this, _1), "Place a stone: play <color> <location>");
		newcallback("white",           bind(&GTP::gtp_playwhite,     this, _1), "Place a white stone: white <location>");
		newcallback("black",           bind(&GTP::gtp_playblack,     this, _1), "Place a black stone: black <location>");
		newcallback("undo",            bind(&GTP::gtp_undo,          this, _1), "Undo one or more moves: undo [amount to undo]");
		newcallback("genmove",         bind(&GTP::gtp_genmove,       this, _1), "Generate a move: genmove [color] [time]");
		newcallback("move_stats",      bind(&GTP::gtp_move_stats,    this, _1), "Output the move stats for the player tree as it stands now");
		newcallback("player_solve",    bind(&GTP::gtp_player_solve,  this, _1), "Run the player, but don't make the move, and give solve output");
		newcallback("player_solved",   bind(&GTP::gtp_player_solved, this, _1), "Output whether the player solved the current node");
		newcallback("player_hgf",      bind(&GTP::gtp_player_hgf,    this, _1), "Output an hgf of the current tree");
		newcallback("player_load_hgf", bind(&GTP::gtp_player_load_hgf,this, _1), "Load an hgf generated by player_hgf");
		newcallback("pv",              bind(&GTP::gtp_pv,            this, _1), "Output the principle variation for the player tree as it stands now");
		newcallback("time",            bind(&GTP::gtp_time,          this, _1), "Set the time limits and the algorithm for per game time");
		newcallback("player_params",   bind(&GTP::gtp_player_params, this, _1), "Set the algorithm for the player, no args gives options");
		newcallback("player_gammas",   bind(&GTP::gtp_player_gammas, this, _1), "Load the gammas for weighted random from a file");
		newcallback("patterns",        bind(&GTP::gtp_patterns,      this, _1), "List all legal moves plus their local pattern");
		newcallback("all_legal",       bind(&GTP::gtp_all_legal,     this, _1), "List all legal moves");
		newcallback("history",         bind(&GTP::gtp_history,       this, _1), "List of played moves");
		newcallback("playgame",        bind(&GTP::gtp_playgame,      this, _1), "Play a list of moves");
		newcallback("winner",          bind(&GTP::gtp_winner,        this, _1), "Check the winner of the game");

		newcallback("ab_solve",        bind(&GTP::gtp_solve_ab,        this, _1),  "Solve with alpha-beta");
		newcallback("ab_params",       bind(&GTP::gtp_solve_ab_params, this, _1),  "Set Parameters for alpha-beta");
		newcallback("ab_stats",        bind(&GTP::gtp_solve_ab_stats,  this, _1),  "Output the stats for the alpha-beta solver");
		newcallback("ab_clear",        bind(&GTP::gtp_solve_ab_clear,  this, _1),  "Stop the solver and release the memory");

		newcallback("pns_solve",       bind(&GTP::gtp_solve_pns,        this, _1),  "Solve with proof number search and an explicit tree");
		newcallback("pns_params",      bind(&GTP::gtp_solve_pns_params, this, _1),  "Set Parameters for PNS");
		newcallback("pns_stats",       bind(&GTP::gtp_solve_pns_stats,  this, _1),  "Output the stats for the PNS solver");
		newcallback("pns_clear",       bind(&GTP::gtp_solve_pns_clear,  this, _1),  "Stop the solver and release the memory");

		newcallback("pns2_solve",      bind(&GTP::gtp_solve_pns2,        this, _1),  "Solve with proof number search and an explicit tree");
		newcallback("pns2_params",     bind(&GTP::gtp_solve_pns2_params, this, _1),  "Set Parameters for PNS");
		newcallback("pns2_stats",      bind(&GTP::gtp_solve_pns2_stats,  this, _1),  "Output the stats for the PNS solver");
		newcallback("pns2_clear",      bind(&GTP::gtp_solve_pns2_clear,  this, _1),  "Stop the solver and release the memory");

		newcallback("pnstt_solve",     bind(&GTP::gtp_solve_pnstt,        this, _1),  "Solve with proof number search and a transposition table of fixed size");
		newcallback("pnstt_params",    bind(&GTP::gtp_solve_pnstt_params, this, _1),  "Set Parameters for PNSTT");
		newcallback("pnstt_stats",     bind(&GTP::gtp_solve_pnstt_stats,  this, _1),  "Outupt the stats for the PNSTT solver");
		newcallback("pnstt_clear",     bind(&GTP::gtp_solve_pnstt_clear,  this, _1),  "Stop the solver and release the memory");
	}

	void set_board(bool clear = true){
		player.set_board(game.getboard());
		solverab.set_board(game.getboard());
		solverpns.set_board(game.getboard());
		solverpns2.set_board(game.getboard());
		solverpnstt.set_board(game.getboard(), clear);
	}

	void move(const Move & m){
		game.move(m);
		player.move(m);
		solverab.move(m);
		solverpns.move(m);
		solverpns2.move(m);
		solverpnstt.move(m);
	}

	GTPResponse gtp_print(vecstr args);
	GTPResponse gtp_zobrist(vecstr args);
	string won_str(int outcome) const;
	GTPResponse gtp_swap(vecstr args);
	GTPResponse gtp_boardsize(vecstr args);
	GTPResponse gtp_clearboard(vecstr args);
	GTPResponse gtp_undo(vecstr args);
	GTPResponse gtp_all_legal(vecstr args);
	GTPResponse gtp_history(vecstr args);
	GTPResponse gtp_patterns(vecstr args);
	GTPResponse play(const string & pos, int toplay);
	GTPResponse gtp_playgame(vecstr args);
	GTPResponse gtp_play(vecstr args);
	GTPResponse gtp_playwhite(vecstr args);
	GTPResponse gtp_playblack(vecstr args);
	GTPResponse gtp_winner(vecstr args);
	GTPResponse gtp_name(vecstr args);
	GTPResponse gtp_version(vecstr args);
	GTPResponse gtp_verbose(vecstr args);
	GTPResponse gtp_extended(vecstr args);
	GTPResponse gtp_colorboard(vecstr args);
	GTPResponse gtp_debug(vecstr args);
	GTPResponse gtp_dists(vecstr args);

	GTPResponse gtp_move_stats(vecstr args);
	GTPResponse gtp_player_solve(vecstr args);
	GTPResponse gtp_player_solved(vecstr args);
	GTPResponse gtp_pv(vecstr args);
	GTPResponse gtp_genmove(vecstr args);
	GTPResponse gtp_player_params(vecstr args);
	GTPResponse gtp_player_gammas(vecstr args);
	GTPResponse gtp_player_hgf(vecstr args);
	GTPResponse gtp_player_load_hgf(vecstr args);
	GTPResponse gtp_confirm_proof(vecstr args);

	string solve_str(int outcome) const;
	string solve_str(const Solver & solve);

	GTPResponse gtp_solve_ab(vecstr args);
	GTPResponse gtp_solve_ab_params(vecstr args);
	GTPResponse gtp_solve_ab_stats(vecstr args);
	GTPResponse gtp_solve_ab_clear(vecstr args);

	GTPResponse gtp_solve_pns(vecstr args);
	GTPResponse gtp_solve_pns_params(vecstr args);
	GTPResponse gtp_solve_pns_stats(vecstr args);
	GTPResponse gtp_solve_pns_clear(vecstr args);

	GTPResponse gtp_solve_pns2(vecstr args);
	GTPResponse gtp_solve_pns2_params(vecstr args);
	GTPResponse gtp_solve_pns2_stats(vecstr args);
	GTPResponse gtp_solve_pns2_clear(vecstr args);

	GTPResponse gtp_solve_pnstt(vecstr args);
	GTPResponse gtp_solve_pnstt_params(vecstr args);
	GTPResponse gtp_solve_pnstt_stats(vecstr args);
	GTPResponse gtp_solve_pnstt_clear(vecstr args);
};
