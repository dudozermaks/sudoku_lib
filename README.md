# Ultimate Sudoku library.
Featuring Generator (that uses fastest solver [Tdoku](https://github.com/t-dillon/tdoku)) and HumanSolver, that solves puzzle with human methods. Also, there are some helper classes like Pos, Figure or Puzzle, that provide great API for interacting with puzzle itself.
One of the things I care about is simplicity. This library has minimal dependencies list possible, but it is still quite fast.

<!--toc:start-->
- [Ultimate Sudoku library.](#ultimate-sudoku-library)
  - [How to build](#how-to-build)
    - [For using in your own projects](#for-using-in-your-own-projects)
    - [For tests](#for-tests)
  - [A few words about internals.](#a-few-words-about-internals)
    - [Pos](#pos)
    - [Figure](#figure)
    - [Puzzle](#puzzle)
    - [Human Solver](#human-solver)
    - [Generator](#generator)
  - [Let's talk about performance!](#lets-talk-about-performance)
  - [Plans](#plans)
    - [Maybe...](#maybe)
  - [Useful resources](#useful-resources)
    - [Site](#site)
    - [YouTube](#youtube)
    - [Libraries](#libraries)
<!--toc:end-->

> **Note**
> This is very young project. Bugs expected. Please write issues about them.
> Mainly this library was build for my app for Sudoku solving - [Gudoku](https://github.com/dudozermaks/Gudoku).
> Because those projects are actively developing, you might see some changes of API along the new releases.

## How to build
We don't have complex build systems. Just add folder with library to include directory and some C++ files to your build list and that's it!

### For using in your own projects
1. Clone this repo.
2. Add `sudoku_lib.hpp` to your include list or put file in the same folder your files are.
3. Add `tdoku/solver_dpll_triad_simd.cc` and `tdoku/util.cc` to your build list.
4. That's it! No more dependencies!

### For tests
1. Clone this repo.
2. `g++ test.cpp tdoku/solver_dpll_triad_simd.cc tdoku/util.cc -o test`

## A few words about internals.
### Pos
This class represents position in gird, like `Pos{0, 8}` means it's column 1 and row 9. Nothing special.

### Figure
Set of Pos'es. This is an interesting thing.

+ `Figure(9, 9)` generates Figure, which contains positions from {0, 0} to {8, 8}. Using this you can quickly iterate through all pencilmarks or all clues in the puzzle.
+ `Figure().col(x)`, `Figure().row(x)` `Figure().square(x)` generates a Figure number x. If x=3, then it will return Figure of 3'rd column, row, or square.
+ Plus, you can stack those, meaning: `Figure.col(x).square(y)` returns Figure with both column X and square Y positions.
+ `Figure.get_neighbours(Pos pos)` returns Figure of column, row and square this pos belongs to.

There is more, but this is some of the most useful features.

### Puzzle
This class stores clues and pencilmarks. Can also generate pencilmarks using clues.
There are quite a few methods, and they all are rarely used, but when used, they are quite convenient.

### Human Solver
As for now HumanSolver supports few methods: 
+ Single Candidate, Single Position
+ Candidate Lines
+ Double Pairs
+ Multiple Lines
+ Naked Pair, Naked Triple, Naked Quad
+ Hidden Pair, Hidden Triple, Hidden Quad
+ X-Wing

### Generator
Generates minimized Sudoku puzzle.

It can solve puzzles and return methods, used for solving and puzzle score.
Score for each method:

| Method           | Score for the first time | Score for subsequent use |
| ---------------- | ------------------------ | ------------------------ |
| Single Candidate | 10                       | 10                       |
| Single Position  | 10                       | 10                       |
| Candidate Lines  | 35                       | 20                       |
| Double Pairs     | 50                       | 25                       |
| Multiple Lines   | 70                       | 40                       |
| Naked Pair       | 75                       | 50                       |
| Hidden Pair      | 150                      | 120                      |
| Naked Triple     | 200                      | 140                      |
| Hidden Triple    | 240                      | 160                      |
| X-Wing           | 280                      | 160                      |
| Naked Quad       | 500                      | 400                      |
| Hidden Quad      | 700                      | 500                      |

It's worth mentioning, that HumanSolver sometimes can't solve some puzzles due to lack of methods.

## Let's talk about performance!
Tdoku solves about 24000 hard puzzles per second in ideal conditions. I haven't benchmarked it on my machine.

HumanSolver is tested on my machine, which has i3-4170 Intel processor and Nvidia GeForce GTX 750 Ti. I'm using NixOS.
HumanSolver (as for version 1.0.0) with -O3 flag solves about 200-250 medium puzzles per second.

## Plans
+ Add some more methods to HumanSolver (like Swordfish and others). I probably won't add guessing based methods like all kinds of Forcing Chains. But I will add some techniques that have not been implemented in this king of solvers(as long as I know) like Phistomefel Ring, Reverse Phistomefel Ring, etc.
+ Add rating-based generation of puzzles. Right now, when you generate a puzzle you recive puzzle with random difficulty.
+ Speed up HumanSolver. Optimize and improve optimizable and improvable:).
+ Add caching to HumanSolver and Generator. The technique for HumanSolver is already made up, only thing left is to implement it.
+ Add ImageMaker, that makes picture out of Puzzle. Probably will have some additional dependencies (but maybe not). Probably will be based on SVG format.
+ Make additional app to generate Sudoku puzzles (for journals, personal use, etc.).
+ Add good documentation, maybe even with Doxygen.
+ Add better tests.
+ Create basic website for this project.

### Maybe...
+ ... Completely re-write this, to Rust or even Zig or something else... The reason I wrote it in C++ is that I know this language very well. I know that start a project in C++ now is a bad idea, so maybe I re-write it.
+ ... Add programmable validator of Sudoku puzzles with its own syntax, for killer and other variations. I think it will be slow, so you will be able only validate puzzles with it.

## Useful resources
### Site
- [Sudoku Wiki](https://www.sudokuwiki.org/sudoku.htm) - Sudoku solver that uses human methods. Every method explained with details and examples. Love this one!
- [Sudoku Coach](http://www.taupierbw.be/SudokuCoach) - Detailed description of almost all Sudoku methods with examples.
- [Sudoku Of The Day](https://www.sudokuoftheday.com) - Good example of rating a Sudoku puzzle. Scores from methods was taken from their website.

<<<<<<< HEAD
### YouTube
- [Cracking The Cryptic](https://www.youtube.com/@CrackingTheCryptic) - YouTube Chanel, where to men solve logical puzzles. I got so many inspirations from this one! It's definitely content made mainly for older generation, but I enjoy it so much! Also, I've got some Sudoku strategies from them (like Phistomefel Ring).
=======
### Youtube
- [Cracking The Cryptic](https://www.youtube.com/@CrackingTheCryptic) - Youtube channel, where to men solve logical puzzles. I got so much inspirations from this one! It's definitely content made mainly for older generation, but I enjoy it so much! Also, I've got some sudoku strateges from them (like Phistomefel Ring).
>>>>>>> refs/remotes/origin/main

### Libraries
- [Tdoku](https://github.com/t-dillon/tdoku) - Fastest Sudoku solver. It's shipped with library in most minimal form possible.
