#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
#include <utility>
#include <vector>
namespace Sudoku {
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

  std::vector<int> get_rows_occupied() {
    std::set<int> rows;
    for (Pos pos : *this) {
      rows.insert(pos.row);
    }
    return std::vector<int>(rows.begin(), rows.end());
  }

  std::vector<int> get_cols_occupied() {
    std::set<int> cols;
    for (Pos pos : *this) {
      cols.insert(pos.col);
    }
    return std::vector<int>(cols.begin(), cols.end());
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
  void generate_pencilmarks() {
    for (Pos pos : Figure(9, 9)) {
      generate_pencimlmark_for_cell(pos);
    }
  }

public:
  Puzzle(std::string from,
         std::vector<std::vector<int>> user_pencilmarks = {}) {
    if (from.size() != 9 * 9) {
      std::cout << "Puzzle size (" << from.size() << ") != 9*9. Exiting!";
      std::exit(1);
    }

    for (Pos pos : Figure(9, 9)) {
      clues[pos] = from[pos.row * 9 + pos.col] - '0';
    }

    if (!is_right()) {
      std::cout << "Puzzle is not right. Exiting!";
      std::exit(1);
    }

    if (user_pencilmarks.empty()) {
      generate_pencilmarks();
    } else {
      Figure field = Figure(9, 9);
      int pos = 0;
      for (std::vector<int> pencilmark : user_pencilmarks) {
        pencilmarks[field[pos]] = pencilmark;
        pos++;
      }
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
      res.insert({pos, pencilmarks[pos]});
    }
    return res;
  }

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

  bool remove_pencilmark_from_cell(Pos pos, int pencilmark) {
    auto pos_to_remove =
        std::find(pencilmarks[pos].begin(), pencilmarks[pos].end(), pencilmark);
    if (pos_to_remove != pencilmarks[pos].end()) {
      pencilmarks[pos].erase(pos_to_remove);
      return true;
    }
    return false;
  }
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
};

class Solver {
private:
  Puzzle puzzle;

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
      // find pencilmarks with count 1;
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
    // this map stores founded candidate lines. after we found some candidate
    // line in square, we write it to the founded candidate lines like this:
    // founded_candidate_lines[square_number].push_back(number_that_we_found);
    static std::map<int, std::vector<int>> founded_candidate_lines;
    for (int square_number = 0; square_number < 9; square_number++) {
      // counting all numbers in square pencilmarks
      Figure square = Figure().square(square_number);
      // finding numbers, which count in square equals to 2
      std::vector<int> clues_count =
          puzzle.pencilmarks_with_count(square, 2, 2);
      for (int clue : clues_count) {
        // check is founded
        bool is_founded_already = false;
        for (std::pair<int, std::vector<int>> founded :
             founded_candidate_lines) {
          if (founded.first == square_number &&
              std::find(founded.second.begin(), founded.second.end(), clue) !=
                  founded.second.end()) {
            is_founded_already = true;
          }
        }
        if (is_founded_already) {
          continue;
        }

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

        // TODO: if no pencilmarks remove, continue searching
        puzzle.remove_pencilmarks(positions_to_blacklist, clue);

        founded_candidate_lines[square_number].push_back(clue);

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
      std::vector<int> cols_occupied = pencilmarks_position.get_cols_occupied();
      std::vector<int> rows_occupied = pencilmarks_position.get_rows_occupied();
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
    // TODO: split into 2 separate functions
    static std::vector<OccupiedColsAndRows> founded;
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

          // TODO: if no pencilmarks remove, continue searching
          puzzle.remove_pencilmarks(figure_to_remove_from, c1.number);
          founded.push_back(c1);
          print_founded(c1, square1, square2);
          return true;
        }
      }
      return false;
    };

    auto is_founded = [](OccupiedColsAndRows candidate) {
      auto check = [candidate](OccupiedColsAndRows &other) {
        return candidate.number == other.number &&
               candidate.cols_and_rows[0] == other.cols_and_rows[0] &&
               candidate.cols_and_rows[1] == other.cols_and_rows[1];
      };
      return std::find_if(founded.begin(), founded.end(), check) !=
             founded.end();
    };

    // TODO: optimize that loop
    for (int square_number1 = 0; square_number1 < 8; square_number1++) {
      for (OccupiedColsAndRows candidate1 : candidates[square_number1]) {

        if (is_founded(candidate1)) {
          continue;
        }

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
  class PossibleCellsPosNPT {
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
    PossibleCellsPosNPT(int N, size_t max_index) {
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
      PossibleCellsPosNPT possible_pos{N, optimized_figure.size()};

      // TODO: Add proper function to check if can increment, and only then
      // increment
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

      PossibleCellsPosNPT possible_pos{N, set_of_pencilmarks.size()};

      // TODO: Add proper function to check if can increment, and only then
      // increment
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

public:
  Solver(Puzzle _puzzle) : puzzle{_puzzle} {}
  int get_difficulty() {
    while (puzzle.is_space_for_clues_avalible()) {
      if (single_candidate_spot()) {
      } else if (single_position_spot()) {
      } else if (candidate_lines_spot()) {
      } else if (double_pairs_or_multiple_lines_spot(true)) {
      } else if (double_pairs_or_multiple_lines_spot(false)) {
      } else if (naked_nth_spot(2)) {
      } else if (hidden_nth_spot(2)) {
      } else if (naked_nth_spot(3)) {
      } else if (hidden_nth_spot(3)) {
      } else if (naked_nth_spot(4)) {
      } else if (hidden_nth_spot(4)) {

      } else {
        std::cout << "Can't solve this puzzle!:(\n";
        puzzle.print_clues();
        puzzle.print_pencilmarks();
        std::exit(1);
      }
    }
    if (puzzle.is_solved()) {
      std::cout << "solved!\n";
      puzzle.print_clues();
    }
    return 0;
  }
};
} // namespace Sudoku
