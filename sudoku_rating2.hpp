#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
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
};

// TODO: Inherit from std::vector and remove f.
class Figure : public std::set<Pos> {
public:
  Figure(std::set<Pos> f) { insert(f.begin(), f.end()); }
  Figure(int col_max, int row_max) { fill_grid(col_max, row_max); }
  Figure() {}

  Figure fill_grid(int col_max, int row_max) {
    for (int row = 0; row < row_max; row++) {
      for (int col = 0; col < col_max; col++) {
        insert({col, row});
      }
    }
    return *this;
  }

  Figure square(int number) {
    int first_col = (number % 3) * 3;
    int first_row = (number / 3) * 3;

    for (int col = first_col; col < first_col + 3; col++) {
      for (int row = first_row; row < first_row + 3; row++) {
        insert({col, row});
      }
    }
    return *this;
  }
  Figure square(Pos of_pos) {
    int first_col = (of_pos.col / 3) * 3;
    int first_row = (of_pos.row / 3) * 3;

    for (int row = first_row; row < first_row + 3; row++) {
      for (int col = first_col; col < first_col + 3; col++) {
        insert({col, row});
      }
    }
    return *this;
  }

  Figure col(int number) {
    for (int row = 0; row < 9; row++) {
      insert({number, row});
    }
    return *this;
  }

  Figure row(int number) {
    for (int col = 0; col < 9; col++) {
      insert({col, number});
    }
    return *this;
  }

  Figure neighbours_of(Pos pos) {
    row(pos.row);
    col(pos.col);
    square(pos);
    return *this;
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
  Puzzle(std::string from) {
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

    generate_pencilmarks();
  }
  void print_clues() {
    for (Pos pos : Figure(9, 9)) {
      // std::cout << pos;
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

  std::map<int, int> count_clues(Figure &figure) {
    std::map<int, int> clues_count = {};
    for (Pos pos : figure) {
      for (int clue : pencilmarks[pos]) {
        clues_count[clue]++;
      }
    }
    return clues_count;
  }
};
class Solver {
private:
  Puzzle puzzle;

  bool single_candidate_spot(std::map<Pos, std::vector<int>> pencilmarks) {
    for (Pos pos : Figure(9, 9)) {
      if (pencilmarks[pos].size() != 1) {
        continue;
      }
      puzzle.set_clue(pos, pencilmarks[pos][0]);
      std::cout << "single candidate (" << pencilmarks[pos][0]
                << ") spotted at: " << pos.col << " " << pos.row << std::endl;
      return true;
    }
    return false;
  }

  bool single_position_spot(std::map<Pos, std::vector<int>> pencilmarks) {
    for (int number = 0; number < 9; number++) {
      Figure col = Figure().col(number);
      Figure row = Figure().row(number);
      Figure square = Figure().square(number);

      auto find_single_position = [this, &pencilmarks](Figure figure) -> bool {
        std::map<int, int> clues_count = puzzle.count_clues(figure);

        // find clue with count 1;
        int single_position_clue = -1;
        for (std::pair<int, int> clue : clues_count) {
          if (clue.second == 1) {
            single_position_clue = clue.first;
          }
        }
        if (single_position_clue == -1) {
          return false;
        }
        // get position of this clue;
        for (Pos pos : figure) {
          for (int clue : pencilmarks[pos]) {
            if (clue == single_position_clue) {
              puzzle.set_clue(pos, single_position_clue);
              std::cout << "single position (" << single_position_clue
                        << ") spotted at: " << pos.col << " " << pos.row
                        << std::endl;
              return true;
            }
          }
        }
        return false;
      };
      if (find_single_position(square) || find_single_position(col) ||
          find_single_position(row)) {
        return true;
      }
    }
    return false;
  }

public:
  Solver(Puzzle _puzzle) : puzzle{_puzzle} {}
  int get_difficulty() {
    while (puzzle.is_space_for_clues_avalible()) {
      auto current_pencilmarks = puzzle.get_pencilmarks();
      if (single_candidate_spot(current_pencilmarks)) {
      } else if (single_position_spot(current_pencilmarks)) {
      } else {
        std::cout << "Can't solve this puzzle!:(\n";
        std::exit(1);
      }
    }
    puzzle.print_clues();
    if (puzzle.is_solved()) {
      std::cout << "solved!\n";
    }
    return 0;
  }
};
} // namespace Sudoku
