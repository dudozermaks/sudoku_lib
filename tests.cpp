#include "sudoku_lib.hpp"
#include <bits/chrono.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>


Sudoku::HumanSolver::Result test_puzzle(std::string puzzle_string){
	Sudoku::Puzzle puzzle;
	if (!puzzle.load(puzzle_string)){
		std::exit(1);
	}
	Sudoku::HumanSolver solver{puzzle};
	Sudoku::HumanSolver::Result solver_result = solver.solve();
	if (!solver_result.is_solved){
		std::exit(1);
	}
	return solver_result;
}

void compare_res(std::string puzzle, Sudoku::HumanSolver::Result res_to_check){
	Sudoku::HumanSolver::Result res = test_puzzle(puzzle);
	if (res != res_to_check){
		std::cout << "\033[31mFailed!\033[0m\n";
		std::cout << res;
		std::exit(1);
	}
}

int main() {
	// test_puzzle("901500046425090081860010020502000000019000460600000002196040253200060817000001694");
	// test_puzzle("600802735702356940300407062100975024200183079079624003400560207067240300920738406"); // naked 3
	// test_puzzle("128400700973001040456070010002010096860503071710064800091040320000100000007005104");
	// test_puzzle("800000000003600000070090200050007000000045700000100030001000068008500010090000400"); // Hardest sudoku ever
	auto benchmark_start = std::chrono::high_resolution_clock::now();
	std::cout.setstate(std::ios::failbit);
	compare_res(
			"001000570706050003900630040025073090367080154080540230070062009600090702093000400", 
			{420, true, {"Single Candidate"}}
	);
	compare_res(
			"035000001100007049090065008529700000700000004000009752800420060350800007900000820", 
			{490, true, {"Single Candidate"}}
	);
	compare_res(
			"105070804028000310000080000309608501000000000206501407000090000093000740407060908", 
			{490, true, {"Single Candidate"}}
	);
	compare_res(
			"072000000001048000003007040040001003500090004800300070020700400000150900000000720", 
			{560, true, {"Single Position", "Single Candidate"}}
	);
	compare_res(
			"005403670006002400000100300070046003000000000600750020001008000003200900098504100", 
			{565, true, {"Single Position", "Single Candidate", "Candidate Lines"}}
	);
	compare_res(
			"005403670006002400000100300070046003000000000600750020001008000003200900098504100", 
			{565, true, {"Candidate Lines", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"460700103000380000700000200800002500650090028002800006006000002000064000508007039", 
			{575, true, {"Candidate Lines", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"934060050006004923008900046800546007600010005500390062360401270470600500080000634", 
			{585, true, {"Candidate Lines", "Double Pairs", "Single Candidate"}}
	);
	compare_res(
			"040158060005000100000204000023000890700000005064000210000402000006000400080369050", 
			{605, true, {"Candidate Lines", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"104320070280000103300008209000500000009203400000006000403600002906000014020034507", 
			{605, true, {"Candidate Lines", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"002090060000040902000700405000000109930000074504000000409006000107080000080020700", 
			{795, true, {"Candidate Lines", "Multiple Lines", "Single Candidate", "Single Position"}}
	);

	compare_res(
			"900051730107398205500076091810724350200165007075983012021537000758649123390812570", 
			{715, true, {"Double Pairs", "Multiple Lines", "Naked Pair", "Single Candidate", "X-Wing"}}
	);
	compare_res(
			"000060010000010300100803004020600900009537400007002080900701006003040000080020000", 
			{720, true, {"Candidate Lines", "Double Pairs", "Naked Pair", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"800004000020900000000701420001000309300010007506000800042506000000008090000300005", 
			{880, true, {"Candidate Lines", "Hidden Pair", "Naked Pair", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"500040023700100000308007000150009000000050000000300087000900601000003008460010005", 
			{880, true, {"Candidate Lines", "Multiple Lines", "Naked Pair", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"000006509000300070018000030009030004200060007600050800040000710050003000107800000", 
			{895, true, {"Candidate Lines", "Double Pairs", "Hidden Pair", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"089200001000100004000000780001030809000060000402070300015000000700003000900008610", 
			{915, true, {"Candidate Lines", "Hidden Pair", "Multiple Lines", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"000001200008090006060003100012004000090000060000800590006100070700030600005400000", 
			{965, true, {"Candidate Lines", "Hidden Pair", "Multiple Lines", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"302054000080009000900000506000000380100060002098000000204000001000100030000480705", 
			{990, true, {"Candidate Lines", "Hidden Pair", "Multiple Lines", "Naked Pair", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"064300000100020070070001500500000080000463000040000007001800050030090002000005790", 
			{1000, true, {"Candidate Lines", "Hidden Pair", "Naked Pair", "Single Candidate", "Single Position"}}
	);
	compare_res(
			"004005010100000000028070000500720600060000040002084009000050780000000003070800200", 
			{1120, true, {"Candidate Lines", "Multiple Lines", "Naked Pair", "Single Candidate", "Single Position", "X-Wing"}}
	);
	compare_res(
			"624900000739100008815004000400009370300040006591003002900400200100296004248357169", 
			{1135, true, {"Candidate Lines", "Double Pairs", "Naked Pair", "Naked Quad", "Single Candidate", "Single Position"}}
	);
	//
	// Sudoku::BrutforceSolver bf_solver;
	// bf_solver.load_puzzle(Sudoku::Puzzle("624900000739100008815004000400009370300040006591003002900400200100296004248357169"));
	// std::cout << bf_solver.solve().size() << std::endl;
	// bf_solver.load_puzzle(Sudoku::Puzzle("295743861431865900876192543387459216612387495549216738763524189928671354154938000"));
	// std::cout << bf_solver.solve().size() << std::endl;
	//
	// Sudoku::Generator generator;
	// Sudoku::Puzzle puzzle = generator.generate();
	// puzzle.print_clues();
	// Sudoku::HumanSolver hs = {puzzle};
	// hs.solve();
	//
	std::cout.clear();
	auto benchmark_end = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(benchmark_end - benchmark_start).count() << std::endl;
}
