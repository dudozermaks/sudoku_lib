#include "sudoku_lib.hpp"
#include "tdoku/tdoku.h"
#include <bits/chrono.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>

class InputParser {
public:
  InputParser(int &argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
      tokens.push_back(std::string(argv[i]));
    }
  }
  const std::string get(const std::string &option) const {
    std::vector<std::string>::const_iterator itr;

    itr = std::find(tokens.begin(), tokens.end(), option);

    if (itr != tokens.end() && itr+1 != tokens.end()) {
      return *(itr+1);
    }
    return "";
  }

  bool has(const std::string &option) const {
    return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
  }

private:
  std::vector<std::string> tokens;
};

Sudoku::HumanSolver::Result test_puzzle(std::string puzzle_string) {
  Sudoku::Puzzle puzzle;
  if (!puzzle.load(puzzle_string)) {
    std::exit(1);
  }
  Sudoku::HumanSolver solver{puzzle};
  Sudoku::HumanSolver::Result solver_result = solver.solve();
  if (!solver_result.is_solved) {
    std::exit(1);
  }
  return solver_result;
}

void compare_res(std::string puzzle, Sudoku::HumanSolver::Result res_to_check) {
  Sudoku::HumanSolver::Result res = test_puzzle(puzzle);
  if (res != res_to_check) {
    std::cout << "\033[31mFailed!\033[0m\n";
    std::cout << res;
    std::exit(1);
  }
}

void test_human_solver(){
  compare_res("0010005707060500039006300400250730903670801540805402300700620096"
              "00090702093000400",
              {420, true, {"Single Candidate"}});
  compare_res("0350000011000070490900650085297000007000000040000097528004200603"
              "50800007900000820",
              {490, true, {"Single Candidate"}});
  compare_res("1050708040280003100000800003096085010000000002065014070000900000"
              "93000740407060908",
              {490, true, {"Single Candidate"}});
  compare_res("0720000000010480000030070400400010035000900048003000700207004000"
              "00150900000000720",
              {560, true, {"Single Position", "Single Candidate"}});
  compare_res(
      "005403670006002400000100300070046003000000000600750020001008000003200900"
      "098504100",
      {565, true, {"Single Position", "Single Candidate", "Candidate Lines"}});
  compare_res(
      "005403670006002400000100300070046003000000000600750020001008000003200900"
      "098504100",
      {565, true, {"Candidate Lines", "Single Candidate", "Single Position"}});
  compare_res(
      "460700103000380000700000200800002500650090028002800006006000002000064000"
      "508007039",
      {575, true, {"Candidate Lines", "Single Candidate", "Single Position"}});
  compare_res(
      "934060050006004923008900046800546007600010005500390062360401270470600500"
      "080000634",
      {585, true, {"Candidate Lines", "Double Pairs", "Single Candidate"}});
  compare_res(
      "040158060005000100000204000023000890700000005064000210000402000006000400"
      "080369050",
      {605, true, {"Candidate Lines", "Single Candidate", "Single Position"}});
  compare_res(
      "104320070280000103300008209000500000009203400000006000403600002906000014"
      "020034507",
      {605, true, {"Candidate Lines", "Single Candidate", "Single Position"}});
  compare_res("0020900600000409020007004050000001099300000745040000004090060001"
              "07080000080020700",
              {795,
               true,
               {"Candidate Lines", "Multiple Lines", "Single Candidate",
                "Single Position"}});

  compare_res("9000517301073982055000760918107243502001650070759830120215370007"
              "58649123390812570",
              {715,
               true,
               {"Double Pairs", "Multiple Lines", "Naked Pair",
                "Single Candidate", "X-Wing"}});
  compare_res("0000600100000103001008030040206009000095374000070020809007010060"
              "03040000080020000",
              {720,
               true,
               {"Candidate Lines", "Double Pairs", "Naked Pair",
                "Single Candidate", "Single Position"}});
  compare_res("8000040000209000000007014200010003093000100075060008000425060000"
              "00008090000300005",
              {880,
               true,
               {"Candidate Lines", "Hidden Pair", "Naked Pair",
                "Single Candidate", "Single Position"}});
  compare_res("5000400237001000003080070001500090000000500000003000870009006010"
              "00003008460010005",
              {880,
               true,
               {"Candidate Lines", "Multiple Lines", "Naked Pair",
                "Single Candidate", "Single Position"}});
  compare_res("0000065090003000700180000300090300042000600076000508000400007100"
              "50003000107800000",
              {895,
               true,
               {"Candidate Lines", "Double Pairs", "Hidden Pair",
                "Single Candidate", "Single Position"}});
  compare_res("0892000010001000040000007800010308090000600004020703000150000007"
              "00003000900008610",
              {915,
               true,
               {"Candidate Lines", "Hidden Pair", "Multiple Lines",
                "Single Candidate", "Single Position"}});
  compare_res("0000012000080900060600031000120040000900000600008005900061000707"
              "00030600005400000",
              {965,
               true,
               {"Candidate Lines", "Hidden Pair", "Multiple Lines",
                "Single Candidate", "Single Position"}});
  compare_res("3020540000800090009000005060000003801000600020980000002040000010"
              "00100030000480705",
              {990,
               true,
               {"Candidate Lines", "Hidden Pair", "Multiple Lines",
                "Naked Pair", "Single Candidate", "Single Position"}});
  compare_res("0643000001000200700700015005000000800004630000400000070018000500"
              "30090002000005790",
              {1000,
               true,
               {"Candidate Lines", "Hidden Pair", "Naked Pair",
                "Single Candidate", "Single Position"}});
  compare_res("0040050101000000000280700005007206000600000400020840090000507800"
              "00000003070800200",
              {1120,
               true,
               {"Candidate Lines", "Multiple Lines", "Naked Pair",
                "Single Candidate", "Single Position", "X-Wing"}});
  compare_res("6249000007391000088150040004000093703000400065910030029004002001"
              "00296004248357169",
              {1135,
               true,
               {"Candidate Lines", "Double Pairs", "Naked Pair", "Naked Quad",
                "Single Candidate", "Single Position"}});
  // test_puzzle("901500046425090081860010020502000000019000460600000002196040253200060817000001694");
  // test_puzzle("600802735702356940300407062100975024200183079079624003400560207067240300920738406");
  // // naked 3
  // test_puzzle("128400700973001040456070010002010096860503071710064800091040320000100000007005104");
  // test_puzzle("800000000003600000070090200050007000000045700000100030001000068008500010090000400");
  // // Hardest sudoku ever
}

void test_generator(){
	Sudoku::Generator generator;
	Sudoku::Puzzle puzzle = generator.generate();
	puzzle.print_clues();
	// TODO: add test on correctness of sudoku.
}

int main(int argc, char **argv) {
	InputParser args(argc, argv);
  auto benchmark_start = std::chrono::high_resolution_clock::now();

	if (args.has("--help") || args.has("-h")){
		std::cout << "no output: --no-output, -no" << std::endl;
		std::cout << "test human solver: --human-solver, -hs" << std::endl;
		std::cout << "test generator: --generator, -g" << std::endl;
		return 0;
	}

	if (args.has("--no-output") || args.has("-no")){
		std::cout.setstate(std::ios::failbit);
	}
	if (args.has("--human-solver"), args.has("-hs")){
		std::cout << "testing human solver" << std::endl;
		test_human_solver();
	}

	if (args.has("--generator"), args.has("-g")){
		std::cout << "testing generator" << std::endl;
		test_generator();
	}

	if (args.has("--no-output") || args.has("-no")){
		std::cout.clear();
	}

  auto benchmark_end = std::chrono::high_resolution_clock::now();
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
                   benchmark_end - benchmark_start)
                   .count()
            << std::endl;
}
