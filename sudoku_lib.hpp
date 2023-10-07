#pragma once

#include <cstddef>
#include <cstring>
#include <iostream>

#include <map>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <algorithm>
#include <numeric>
#include <random>

namespace Tdoku {
#include "tdoku/tdoku.h"
};

namespace Sudoku {
namespace Utility {
class NDIncrementCount {
private:
  int N;
  std::vector<int> possible_pos;
  int max_index;
  void increment_pos(int i) {
    if (possible_pos[i] == max_index - 1 - (N - i - 1)) {
      increment_pos(i - 1);
      possible_pos[i] = possible_pos[i - 1] + 1;
    } else {
      possible_pos[i]++;
      return;
    }
  }

  bool can_increment() {
    for (int i = 0; i < N; i++) {
      if (possible_pos[i] < max_index - N + i) {
        return true;
      }
    }
    return false;
  }
  void create_possible_pos() {
    for (int i = 0; i < N; i++) {
      possible_pos.push_back(i);
    }
  }

public:
  NDIncrementCount(int N, size_t max_index) {
    this->N = N;
    this->max_index = max_index;
  }
  bool increment() {
    if (possible_pos.size() == 0) {
      create_possible_pos();
      return true;
    }
    if (!can_increment()) {
      return false;
    }
    // increment last position
    increment_pos(N - 1);
    return true;
  }
  std::vector<int> get_positions() { return possible_pos; }
};
} // namespace Utility
class Pos {
public:
  int col, row;
  Pos(int col, int row) {
    this->col = col;
    this->row = row;
  }
  Pos() {
    col = 0;
    row = 0;
  }
  friend std::ostream &operator<<(std::ostream &os, const Pos &pos) {
    os << "{" << pos.col << ", " << pos.row << "}";
    return os;
  }
  friend bool operator<(const Pos &lhs, const Pos &rhs) {
    if (lhs.row < rhs.row) {
      return true;
    } else if (lhs.row > rhs.row) {
      return false;
    } else {
      if (lhs.col < rhs.col) {
        return true;
      } else if (lhs.col > rhs.col) {
        return false;
      }
    }
    // if they are equal
    return false;
  }
  bool operator==(const Pos &rhs) const {
    return col == rhs.col && row == rhs.row;
  }
};

class Figure : public std::set<Pos> {
public:
  Figure(std::set<Pos> f) { insert(f.begin(), f.end()); }
  Figure(int col_max, int row_max) { fill_grid(col_max, row_max); }
  Figure() {}

  Figure &fill_grid(int col_max, int row_max) {
    for (int row = 0; row < row_max; row++) {
      for (int col = 0; col < col_max; col++) {
        insert({col, row});
      }
    }
    return *this;
  }

  Figure &square(int number) {
    int first_col = (number % 3) * 3;
    int first_row = (number / 3) * 3;

    for (int col = first_col; col < first_col + 3; col++) {
      for (int row = first_row; row < first_row + 3; row++) {
        insert({col, row});
      }
    }
    return *this;
  }
  Figure &square(Pos of_pos) {
    int first_col = (of_pos.col / 3) * 3;
    int first_row = (of_pos.row / 3) * 3;

    for (int row = first_row; row < first_row + 3; row++) {
      for (int col = first_col; col < first_col + 3; col++) {
        insert({col, row});
      }
    }
    return *this;
  }

  Figure &col(int number) {
    for (int row = 0; row < 9; row++) {
      insert({number, row});
    }
    return *this;
  }

  Figure &row(int number) {
    for (int col = 0; col < 9; col++) {
      insert({col, number});
    }
    return *this;
  }

  Figure &neighbours_of(Pos pos) {
    row(pos.row);
    col(pos.col);
    square(pos);
    return *this;
  }

  Figure &remove(Figure &other) {
    for (Pos pos : other) {
      erase(pos);
    }
    return *this;
  }

  Figure sub_figure(std::vector<int> positions) {
    Figure res;
    for (int pos : positions) {
      res.insert((*this)[pos]);
    }
    return res;
  }

  std::set<int> get_rows_occupied() {
    std::set<int> rows;
    for (Pos pos : *this) {
      rows.insert(pos.row);
    }
    return rows;
  }

  std::set<int> get_cols_occupied() {
    std::set<int> cols;
    for (Pos pos : *this) {
      cols.insert(pos.col);
    }
    return cols;
  }

  static Figure intersect(Figure f1, Figure f2) {
    Figure intersection;
    std::set_intersection(f1.begin(), f1.end(), f2.begin(), f2.end(),
                          std::inserter(intersection, intersection.begin()));
    return intersection;
  }

  friend std::ostream &operator<<(std::ostream &os, const Figure &figure) {
    os << "{";
    for (Pos pos : figure) {
      os << pos << ", ";
    }
    if (figure.size() != 0) {
      os << "\b\b";
    }
    os << "}";
    return os;
  }

  Pos operator[](int idx) { return *next(begin(), idx); }
};

class Puzzle {
private:
  std::map<Pos, int> clues;
  std::map<Pos, std::vector<int>> pencilmarks;

  void generate_pencimlmark_for_cell(Pos pos) {
    if (clues[pos] != 0) {
      pencilmarks[pos] = {};
      return;
    }
    Figure neighbours_cells = Figure().neighbours_of(pos);

    // generating list of numbers [1-9]
    std::vector<int> candidates = {};
    candidates.resize(10);
    std::iota(candidates.begin(), candidates.end(), 0);

    for (Pos cell : neighbours_cells) {
      auto cell_pos_in_numbers =
          std::find(candidates.begin(), candidates.end(), clues[cell]);
      if (cell_pos_in_numbers != candidates.end()) {
        candidates.erase(cell_pos_in_numbers);
      }
    }

    pencilmarks[pos] = candidates;
  }
public:
  Puzzle() { load(std::string(9 * 9, '0')); }

  Puzzle(std::string puzzle) { load(puzzle); }
  bool load(std::string from) {
    if (from.size() != 9 * 9) {
      std::cout << "Puzzle size (" << from.size() << ") != 9*9"
                << "\nPuzzle: " << from << std::endl;
      return false;
    }

    for (Pos pos : Figure(9, 9)) {
			char c = from[pos.row * 9 + pos.col];
			if (c == '.'){ 
				c = '0';
			}
      clues[pos] = c - '0';
    }

    if (!is_right()) {
      std::cout << "Puzzle is not right. Puzzle: " << from << std::endl;
      return false;
    }

    generate_pencilmarks();
    return true;
  }

  void generate_pencilmarks() {
    for (Pos pos : Figure(9, 9)) {
      generate_pencimlmark_for_cell(pos);
    }
  }

  void print_clues() {
    for (Pos pos : Figure(9, 9)) {
      std::cout << clues[pos];
      if (pos.col == 8) {
        std::cout << std::endl;
      }
    }
  }
  void print_pencilmarks() {
    for (Pos pos : Figure(9, 9)) {
      if (pos.col == 0 && pos.row != 0) {
        std::cout << std::endl;
      }
      std::cout << "{";
      for (int pencilmark : pencilmarks[pos]) {
        std::cout << pencilmark;
      }
      std::cout << "}, ";
    }
    std::cout << std::endl;
  }

  bool is_space_for_clues_avalible() {
    for (Pos pos : Figure(9, 9)) {
      if (clues[pos] == 0) {
        return true;
      }
    }
    return false;
  }

  bool is_right() {
    auto is_figure_right = [this](Figure figure) {
      std::set<int> numbers;
      for (Pos pos : figure) {
        int number = clues[pos];
        // if number is not inserted
        if (number != 0 && !numbers.insert(number).second) {
          return false;
        }
      }
      return true;
    };
    for (int i = 0; i < 9; i++) {
      // if some of figures isn't right
      if (!is_figure_right(Figure().col(i)) ||
          !is_figure_right(Figure().row(i)) ||
          !is_figure_right(Figure().square(i))) {
        return false;
      }
    }
    return true;
  }

  bool is_solved() { return !is_space_for_clues_avalible() && is_right(); }

  bool is_valid_clue(Pos pos, int clue) {
    auto is_valid_for_figure = [clue, this](Figure f) {
      std::set<int> clues = get_clues_set(f);
      return clues.find(clue) == clues.end();
    };
    // TODO: get_neighbours instead of this
    Figure col = Figure().col(pos.col);
    Figure row = Figure().row(pos.row);
    Figure square = Figure().square(pos);

    return is_valid_for_figure(col) && is_valid_for_figure(row) &&
           is_valid_for_figure(square);
  }

  void set_clue(Pos pos, int clue) {
    clues[pos] = clue;
    pencilmarks[pos] = {};

    Figure neighbours_to_update = Figure().neighbours_of(pos);
    for (Pos neighbour : neighbours_to_update) {
      auto pencilmark_to_remove = std::find(pencilmarks[neighbour].begin(),
                                            pencilmarks[neighbour].end(), clue);
      if (pencilmark_to_remove != pencilmarks[neighbour].end()) {
        pencilmarks[neighbour].erase(pencilmark_to_remove);
      }
    }
  }

  std::map<Pos, std::vector<int>> get_pencilmarks() { return pencilmarks; }
  std::map<Pos, std::vector<int>> get_pencilmarks(Figure &figure) {
    std::map<Pos, std::vector<int>> res;
    for (Pos pos : figure) {
      // TODO, optimize: if pencilmarks[pos] == 0, then do not insert. Right now
      // this change causes some bugs with puzzle
      // 000006509000300070018000030009030004200060007600050800040000710050003000107800000
      res.insert({pos, pencilmarks[pos]});
    }
    return res;
  }
  std::map<Pos, int> get_clues() { return clues; }

  std::map<int, int> count_pencilmarks(Figure &figure) {
    std::map<int, int> clues_count = {};
    for (Pos pos : figure) {
      for (int clue : pencilmarks[pos]) {
        clues_count[clue]++;
      }
    }
    return clues_count;
  }

  // max and min count are inclusive
  std::vector<int> pencilmarks_with_count(Figure &figure, int min_count,
                                          int max_count) {
    std::map<int, int> clues_count = count_pencilmarks(figure);
    std::vector<int> res;
    for (std::pair<int, int> clue : clues_count) {
      if (clue.second >= min_count && clue.second <= max_count) {
        res.push_back(clue.first);
      }
    }
    return res;
  }

  Figure get_pencilmark_positions(Figure &figure, int number) {
    Figure positions = {};
    for (Pos pos : figure) {
      for (int clue : pencilmarks[pos]) {
        if (clue == number) {
          positions.insert(pos);
        }
      }
    }
    return positions;
  }
  std::set<int> get_pencilmarks_set(Figure &figure) {
    std::set<int> res;
    for (Pos pos : figure) {
      for (int pencilmark : pencilmarks[pos]) {
        res.insert(pencilmark);
      }
    }
    return res;
  }

  std::set<int> get_clues_set(Figure &figure) {
    std::set<int> res;
    for (Pos pos : figure) {
      res.insert(clues[pos]);
    }
    return res;
  }

  bool remove_pencilmark_from_cell(Pos pos, int pencilmark) {
    auto pos_to_remove =
        std::find(pencilmarks[pos].begin(), pencilmarks[pos].end(), pencilmark);
    if (pos_to_remove != pencilmarks[pos].end()) {
      pencilmarks[pos].erase(pos_to_remove);
      return true;
    }
    return false;
  };
  bool remove_pencilmarks(Figure &figure, int pencilmark_number) {
    bool is_pencilmarks_removed = false;
    for (Pos pos : figure) {
      if (remove_pencilmark_from_cell(pos, pencilmark_number)) {
        is_pencilmarks_removed = true;
      }
    }
    return is_pencilmarks_removed;
  }
  bool remove_all_pencilmarks_except(Figure &figure,
                                     std::vector<int> pencilmarks_to_left) {
    bool is_pencilmarks_removed = false;
    for (Pos pos : figure) {
      std::vector<int> pencilmarks_in_cell = pencilmarks[pos];
      for (int i = 1; i <= 9; i++) {
        if (std::find(pencilmarks_to_left.begin(), pencilmarks_to_left.end(),
                      i) != pencilmarks_to_left.end()) {
          continue;
        }
        if (remove_pencilmark_from_cell(pos, i)) {
          is_pencilmarks_removed = true;
        }
      }
    }
    return is_pencilmarks_removed;
  }

	std::string clues_as_string(){
		std::string res;
		for (Pos pos : Figure(9, 9)){
			res.push_back(clues[pos] + '0');
		}
		return res;
	}
};

class HumanSolver {
private:
  Puzzle puzzle;
  std::map<std::string, int> method_scores_subsequent;
  std::multimap<int, std::string> methods_score;

  bool single_candidate_spot() {
    std::map<Pos, std::vector<int>> pencilmarks = puzzle.get_pencilmarks();
    for (Pos pos : Figure(9, 9)) {
      if (pencilmarks[pos].size() != 1) {
        continue;
      }
      puzzle.set_clue(pos, pencilmarks[pos][0]);
      std::cout << "single candidate (" << pencilmarks[pos][0]
                << ") spotted at: " << pos << std::endl;
      return true;
    }
    return false;
  }
  bool single_position_spot() {
    std::map<Pos, std::vector<int>> pencilmarks = puzzle.get_pencilmarks();
    auto find_single_position = [this, &pencilmarks](Figure figure) -> bool {
      std::vector<int> single_position_pencilmarks =
          puzzle.pencilmarks_with_count(figure, 1, 1);
      if (single_position_pencilmarks.size() == 0) {
        return false;
      }
      // get position of this pencilmark
      Figure pencilmark_pos = puzzle.get_pencilmark_positions(
          figure, single_position_pencilmarks[0]);
      puzzle.set_clue(pencilmark_pos[0], single_position_pencilmarks[0]);
      std::cout << "single position (" << single_position_pencilmarks[0]
                << ") spotted at: " << pencilmark_pos[0] << std::endl;
      return true;
    };
    for (int number = 0; number < 9; number++) {

      Figure col = Figure().col(number);
      Figure row = Figure().row(number);
      Figure square = Figure().square(number);

      if (find_single_position(square) || find_single_position(col) ||
          find_single_position(row)) {
        return true;
      }
    }
    return false;
  }
  bool candidate_lines_spot() {
    for (int square_number = 0; square_number < 9; square_number++) {
      // counting all numbers in square pencilmarks
      Figure square = Figure().square(square_number);
      // finding numbers, which count in square equals to 2
      std::vector<int> clues_count =
          puzzle.pencilmarks_with_count(square, 2, 2);
      for (int clue : clues_count) {

        Figure numbers_position = puzzle.get_pencilmark_positions(square, clue);

        Figure positions_to_blacklist;
        if (numbers_position[0].col == numbers_position[1].col) {
          positions_to_blacklist = Figure().col(numbers_position[0].col);
        } else if (numbers_position[0].row == numbers_position[1].row) {
          positions_to_blacklist = Figure().row(numbers_position[0].row);
        }
        // if they aren't on the same row or col, skip this number
        else {
          continue;
        }
        // remove first and second number positions (they are guaranteed to be
        // in blacklist)
        positions_to_blacklist.remove(numbers_position);

        if (!puzzle.remove_pencilmarks(positions_to_blacklist, clue)) {
          continue;
        }

        std::cout << "candidate line (" << clue
                  << ") spotted at: " << numbers_position[0] << " and "
                  << numbers_position[1] << std::endl;

        return true;
      }
    }
    return false;
  }
  struct OccupiedColsAndRows {
    // cols_and_rows[0] - cols
    // cols_and_rows[1] - rows
    std::vector<int> cols_and_rows[2];
    int number;
  };
  std::vector<OccupiedColsAndRows>
  candidates_for_dpt_or_mlt(Figure &figure,
                            std::vector<int> pencilmarks_candidates) {
    std::vector<OccupiedColsAndRows> res;

    for (int pencilmark : pencilmarks_candidates) {
      Figure pencilmarks_position =
          puzzle.get_pencilmark_positions(figure, pencilmark);

      OccupiedColsAndRows to_push;
      to_push.number = pencilmark;
      std::vector<int> cols_occupied;
      std::vector<int> rows_occupied;

      {
        std::set<int> cols_occupied_set =
            pencilmarks_position.get_cols_occupied();
        cols_occupied = std::vector<int>(cols_occupied_set.begin(),
                                         cols_occupied_set.end());

        std::set<int> rows_occupied_set =
            pencilmarks_position.get_rows_occupied();
        rows_occupied = std::vector<int>(rows_occupied_set.begin(),
                                         rows_occupied_set.end());
      }

      if (cols_occupied.size() == 2) {
        to_push.cols_and_rows[0] = cols_occupied;
      }
      if (rows_occupied.size() == 2) {
        to_push.cols_and_rows[1] = rows_occupied;
      }
      res.push_back(to_push);
    }
    return res;
  };
  bool double_pairs_or_multiple_lines_spot(bool double_pairs) {
    std::map<int, std::vector<OccupiedColsAndRows>> candidates;

    for (int square_number = 0; square_number < 9; square_number++) {
      Figure square = Figure().square(square_number);
      std::vector<int> pencilmarks;
      if (double_pairs) {
        pencilmarks = puzzle.pencilmarks_with_count(square, 2, 2);
      } else {
        pencilmarks = puzzle.pencilmarks_with_count(square, 2, 6);
      }
      candidates[square_number] =
          candidates_for_dpt_or_mlt(square, pencilmarks);
    }

    auto print_founded = [double_pairs](OccupiedColsAndRows candidate,
                                        int square_number1,
                                        int square_number2) {
      if (double_pairs) {
        std::cout << "double pairs";
      } else {
        std::cout << "multiply lines";
      }
      std::cout << " (" << candidate.number
                << ") spotted in squares: " << square_number1 << " and "
                << square_number2 << std::endl;
    };

    auto next_square_in_col = [](int square1, int square2) {
      int square_col = square1 % 3;
      while (square1 == square_col || square2 == square_col) {
        square_col += 3;
      }
      return square_col;
    };

    auto next_square_in_row = [](int square1, int square2) {
      int square_row = (square1 / 3) * 3;
      while (square1 == square_row || square2 == square_row) {
        square_row += 1;
      }
      return square_row;
    };

    auto check = [this, next_square_in_row, next_square_in_col,
                  print_founded](OccupiedColsAndRows c1, OccupiedColsAndRows c2,
                                 int square1, int square2) {
      if (c1.number != c2.number) {
        return false;
      }

      // i=0 - checking cols
      // i=1 - checking rows
      for (int i = 0; i < 2; i++) {
        if (c1.cols_and_rows[i] == c2.cols_and_rows[i] &&
            c1.cols_and_rows[i].size() != 0) {
          Figure cols_or_rows;
          Figure figure_to_remove_from;
          if (i == 0) {
            cols_or_rows = Figure()
                               .col(c1.cols_and_rows[i][0])
                               .col(c1.cols_and_rows[i][1]);
            figure_to_remove_from =
                Figure().square(next_square_in_col(square1, square2));
          } else {
            cols_or_rows = Figure()
                               .row(c1.cols_and_rows[i][0])
                               .row(c1.cols_and_rows[i][1]);
            figure_to_remove_from =
                Figure().square(next_square_in_row(square1, square2));
          }

          figure_to_remove_from =
              Figure::intersect(figure_to_remove_from, cols_or_rows);

          if (!puzzle.remove_pencilmarks(figure_to_remove_from, c1.number)) {
            continue;
          }
          print_founded(c1, square1, square2);
          return true;
        }
      }
      return false;
    };

    // TODO: optimize that loop
    for (int square_number1 = 0; square_number1 < 8; square_number1++) {
      for (OccupiedColsAndRows candidate1 : candidates[square_number1]) {

        for (int square_number2 = square_number1 + 1; square_number2 < 9;
             square_number2++) {
          for (OccupiedColsAndRows candidate2 : candidates[square_number2]) {

            if (check(candidate1, candidate2, square_number1, square_number2)) {
              return true;
            }
          }
        }
      }
    }

    return false;
  }
  bool naked_nth_spot(int N) {
    auto find_in_figure = [N, this](Figure figure) {
      Figure optimized_figure = figure;
      std::map<Pos, std::vector<int>> pencilmarks =
          puzzle.get_pencilmarks(optimized_figure);

      for (auto pencilmark : pencilmarks) {
        if (pencilmark.second.size() > N || pencilmark.second.size() == 0) {
          optimized_figure.erase(pencilmark.first);
        }
      }

      if (optimized_figure.size() < N) {
        return false;
      }
      Utility::NDIncrementCount possible_pos{N, optimized_figure.size()};

      while (possible_pos.increment()) {

        Figure sub_figure =
            optimized_figure.sub_figure(possible_pos.get_positions());

        std::set<int> set_of_pencilmarks =
            puzzle.get_pencilmarks_set(sub_figure);

        if (set_of_pencilmarks.size() == N) {
          bool is_pencilmarks_removed = false;
          Figure figure_to_remove = figure;
          figure_to_remove.remove(sub_figure);
          for (int pencilmark : set_of_pencilmarks) {
            if (puzzle.remove_pencilmarks(figure_to_remove, pencilmark)) {
              is_pencilmarks_removed = true;
            }
          }
          if (!is_pencilmarks_removed) {
            continue;
          }
          std::cout << "naked " << N << " (";
          for (int pencilmark : set_of_pencilmarks) {
            std::cout << pencilmark << ", ";
          }
          std::cout << "\b\b) spotted at: " << sub_figure << std::endl;
          return true;
        }
      }
      return false;
    };
    for (int figure_number = 0; figure_number < 9; figure_number++) {
      Figure col = Figure().col(figure_number);
      Figure row = Figure().row(figure_number);
      Figure square = Figure().square(figure_number);
      if (find_in_figure(col) || find_in_figure(row) ||
          find_in_figure(square)) {
        return true;
      }
    }
    return false;
  }
  bool hidden_nth_spot(int N) {
    auto find_in_figure = [N, this](Figure figure) {
      Figure optimized_figure = figure;
      std::map<Pos, std::vector<int>> pencilmarks =
          puzzle.get_pencilmarks(optimized_figure);

      for (auto pencilmark : pencilmarks) {
        // TODO: insert in Puzzle::get_pencilmarks function.
        if (pencilmark.second.size() == 0) {
          optimized_figure.erase(pencilmark.first);
        }
      }

      if (optimized_figure.size() < N) {
        return false;
      }

      std::vector<int> set_of_pencilmarks =
          puzzle.pencilmarks_with_count(optimized_figure, 2, N);

      if (set_of_pencilmarks.size() < N) {
        return false;
      }

      Utility::NDIncrementCount possible_pos{N, set_of_pencilmarks.size()};

      while (possible_pos.increment()) {
        Figure sub_figure;
        for (int i : possible_pos.get_positions()) {
          Figure figure_to_insert =
              puzzle.get_pencilmark_positions(figure, set_of_pencilmarks[i]);
          sub_figure.insert(figure_to_insert.begin(), figure_to_insert.end());
        }

        if (sub_figure.size() == N) {
          std::vector<int> pencilmarks_to_left;
          for (int i : possible_pos.get_positions()) {
            pencilmarks_to_left.push_back(set_of_pencilmarks[i]);
          }
          if (!puzzle.remove_all_pencilmarks_except(sub_figure,
                                                    pencilmarks_to_left)) {
            continue;
          }
          std::cout << "hidden " << N << " (";
          for (int i : possible_pos.get_positions()) {
            std::cout << set_of_pencilmarks[i] << ", ";
          }
          std::cout << "\b\b) spotted at: " << sub_figure << std::endl;
          return true;
        }
      }
      return false;
    };
    for (int figure_number = 0; figure_number < 9; figure_number++) {
      Figure col = Figure().col(figure_number);
      Figure row = Figure().row(figure_number);
      Figure square = Figure().square(figure_number);
      if (find_in_figure(col) || find_in_figure(row) ||
          find_in_figure(square)) {
        return true;
      }
    }
    return false;
  }
  class XWingCandidate {
  public:
    bool is_row;
    int digit;
    Figure positions;
    bool is_situable(XWingCandidate &other) {
      if (is_row != other.is_row || digit != other.digit) {
        return false;
      }
      if (is_row) {
        return positions[0].col == other.positions[0].col &&
               positions[1].col == other.positions[1].col;
      }
      return positions[0].row == other.positions[0].row &&
             positions[1].row == other.positions[1].row;
    }
  };
  bool xwing_spot() {
    // TODO: Optimizable. Use std::map<std::vector<XWingCandidate>>, where key
    // is the digit
    std::vector<XWingCandidate> candidates;
    auto add_candidates_from_figure = [&candidates, this](Figure &f,
                                                          bool is_row) {
      std::vector<int> pencilmarks_with_count_2 =
          puzzle.pencilmarks_with_count(f, 2, 2);
      for (int pencilmark : pencilmarks_with_count_2) {
        Figure positions = puzzle.get_pencilmark_positions(f, pencilmark);
        candidates.push_back({});
        candidates.back().is_row = is_row;
        candidates.back().digit = pencilmark;
        candidates.back().positions = positions;
      }
    };
    auto founded = [this](XWingCandidate candidate, Figure positions_involved) {
      Figure positions_to_remove_from;
      for (Pos pos : positions_involved) {
        Figure f;
        if (candidate.is_row) {
          f = Figure().col(pos.col);
        } else {
          f = Figure().row(pos.row);
        }
        positions_to_remove_from.insert(f.begin(), f.end());
      }
      positions_to_remove_from.remove(positions_involved);
      if (!puzzle.remove_pencilmarks(positions_to_remove_from,
                                     candidate.digit)) {
        return false;
      }
      std::cout << "x-wing (" << candidate.digit << ") found at "
                << positions_involved << std::endl;
      return true;
    };
    for (int i = 0; i < 9; i++) {
      Figure row = Figure().row(i);
      Figure col = Figure().col(i);
      add_candidates_from_figure(row, true);
      add_candidates_from_figure(col, false);
    }
		if (candidates.size() == 0){ return false; }
    for (int i = 0; i < candidates.size() - 1; i++) {
      XWingCandidate candidate1 = candidates[i];
      for (int j = i + 1; j < candidates.size(); j++) {
        XWingCandidate candidate2 = candidates[j];
        if (candidate1.is_situable(candidate2)) {
          Figure positions_involved = candidate1.positions;
          positions_involved.insert(candidate2.positions.begin(),
                                    candidate2.positions.end());
          if (!founded(candidate1, positions_involved)) {
            continue;
          }
          return true;
        }
      }
    }
    return false;
  }

public:
  class Result {
  public:
    int score = 0;
    bool is_solved = false;
    std::set<std::string> used_methods;
    bool operator==(Result const &other) {
      return other.score == score && other.is_solved == is_solved &&
             other.used_methods == used_methods;
    };
    bool operator!=(Result const &other) { return !(*this == other); }
    friend std::ostream &operator<<(std::ostream &os,
                                    const Result &solver_res) {
      os << "Score: " << solver_res.score << std::endl;
      os << "Is solved: " << solver_res.is_solved << std::endl;
      os << "Used methods: {";
      for (std::string used_method : solver_res.used_methods) {
        std::cout << "\"" << used_method << "\", ";
      }
      os << "\b\b}" << std::endl;
      return os;
    }
  };
  HumanSolver(Puzzle _puzzle) : puzzle{_puzzle} {
		puzzle.generate_pencilmarks();
    methods_score = {
        {10, "Single Candidate"}, {10, "Single Position"},
        {35, "Candidate Lines"},  {50, "Double Pairs"},
        {70, "Multiple Lines"},   {75, "Naked Pair"},
        {150, "Hidden Pair"},     {200, "Naked Triple"},
        {240, "Hidden Triple"},   {280, "X-Wing"},
        {420, "Forcing Chains"},  {500, "Naked Quad"},
        {700, "Hidden Quad"},     {800, "Swordfish"},
    };

    method_scores_subsequent = {
        {"Single Candidate", 10}, {"Single Position", 10},
        {"Candidate Lines", 20},  {"Double Pairs", 25},
        {"Multiple Lines", 40},   {"Naked Pair", 50},
        {"Hidden Pair", 120},     {"Naked Triple", 140},
        {"Hidden Triple", 160},   {"X-Wing", 160},
        {"Forcing Chains", 210},  {"Naked Quad", 400},
        {"Hidden Quad", 500},     {"Swordfish", 600},
    };
  }
  std::string solving_step(Result &res) {
    for (auto method : methods_score) {
      if (method.second == "Single Candidate" && single_candidate_spot()) {
        return method.second;
      } else if (method.second == "Single Position" && single_position_spot()) {
        return method.second;
      } else if (method.second == "Candidate Lines" && candidate_lines_spot()) {
        return method.second;
      } else if (method.second == "Double Pairs" &&
                 double_pairs_or_multiple_lines_spot(true)) {
        return method.second;
      } else if (method.second == "Multiple Lines" &&
                 double_pairs_or_multiple_lines_spot(false)) {
        return method.second;
      } else if (method.second == "Naked Pair" && naked_nth_spot(2)) {
        return method.second;
      } else if (method.second == "Hidden Pair" && hidden_nth_spot(2)) {
        return method.second;
      } else if (method.second == "Naked Triple" && naked_nth_spot(3)) {
        return method.second;
      } else if (method.second == "Hidden Triple" && hidden_nth_spot(3)) {
        return method.second;
      } else if (method.second == "Naked Quad" && naked_nth_spot(4)) {
        return method.second;
      } else if (method.second == "Hidden Quad" && hidden_nth_spot(4)) {
        return method.second;
      } else if (method.second == "X-Wing" && xwing_spot()) {
        return method.second;
      }
    }

    std::cout << "can't solve this puzzle!:(\n";
    puzzle.print_clues();
    puzzle.print_pencilmarks();
    return "";
  }
  void update_result_and_methods(Result &res, std::string used_method) {
    res.used_methods.insert(used_method);
    auto it_to_remove = methods_score.end();
    for (auto it = methods_score.begin(); it != methods_score.end(); it++) {
      if (it->second == used_method) {
        res.score += it->first;
        if (it->first != method_scores_subsequent[used_method]) {
          it_to_remove = it;
        }
        break;
      }
    }
    if (it_to_remove != methods_score.end()) {
      methods_score.erase(it_to_remove);
      methods_score.emplace(method_scores_subsequent[used_method], used_method);
    }
  }

  Result solve() {
    Result res = {};
    std::cout << "solving: " << std::endl;
    puzzle.print_clues();
    while (puzzle.is_space_for_clues_avalible()) {
      std::string used_method = solving_step(res);
      if (used_method == "") {
        return res;
      };
      update_result_and_methods(res, used_method);
    }
    if (puzzle.is_solved()) {
      std::cout << "solved!\nscore: " << res.score << std::endl;
    }
    res.is_solved = true;
    return res;
  }
  Puzzle &get_puzzle() { return puzzle; }
};

class TdokuLib{
	Puzzle puzzle;
	char cstr_puzzle[81];
	void generate_cstr(){
		std::string clues_string = puzzle.clues_as_string();
		std::replace(clues_string.begin(), clues_string.end(), '0', '.');
		std::strcpy(cstr_puzzle, clues_string.c_str());
	}
public:
	TdokuLib(Puzzle puzzle){
		load(puzzle);
	}
	void load(Puzzle puzzle){
		this->puzzle = puzzle;
		generate_cstr();
	}

	int count_solutions(size_t limit = 99){
		char solution[81];
		size_t number_of_guesses = 0;
		return Tdoku::SolveSudoku(cstr_puzzle, limit, 0, solution, &number_of_guesses);
	}

	bool has_unique_solution(){
		return count_solutions(2) == 1;
	}

	std::string minimize(){
		char res[81];
		strcpy(res, cstr_puzzle);
		Tdoku::TdokuMinimize(false, false, res);
		return std::string(res);
	}
};

class Generator {
private:
  Puzzle puzzle;
  std::mt19937 rg;

  void fill_diagonals() {
    for (int square_number = 0; square_number < 9; square_number += 4) {
      std::vector<int> numbers;
      numbers.resize(9);
      std::iota(numbers.begin(), numbers.end(), 1);
      std::shuffle(numbers.begin(), numbers.end(), rg);
      Figure square = Figure().square(square_number);
      for (Pos pos : square) {
        puzzle.set_clue(pos, numbers.back());
        numbers.pop_back();
      }
    }
  }
  bool fill(Pos pos) {
    // if we reached end of the puzzle
    if (pos == Pos{9, 8}) {
      return true;
    }
    // if we reached end of the row
    if (pos.col == 9) {
      pos.row += 1;
      pos.col = 0;
    }
    if (puzzle.get_clues()[pos] != 0) {
      pos.col += 1;
      return fill(pos);
    }
    for (int num = 1; num < 10; num++) {
      if (puzzle.is_valid_clue(pos, num)) {
        puzzle.set_clue(pos, num);
        Pos pos_to_fill = pos;
        pos_to_fill.col += 1;
        if (fill(pos_to_fill)) {
          return true;
        }
        puzzle.set_clue(pos, 0);
      }
    }
    // No valid value was found, so backtrack
    return false;
  }

	void minimize(){
		TdokuLib tdoku(puzzle);
		puzzle.load(tdoku.minimize());
	}

public:
  Generator(unsigned int seed = 0) {
		Tdoku::TdokuSetSeed(seed);
		rg.seed(seed);
	}
  Puzzle generate() {
    puzzle = {};

    fill_diagonals();
    // starting from second square
    fill({3, 0});

		minimize();
    return puzzle;
  }
};
} // namespace Sudoku
