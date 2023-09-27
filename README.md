# Ultimate Sudoku library.
Featuring Generator (that uses fastest solver [Tdoku](https://github.com/t-dillon/tdoku)) and HumanSolver, that solves puzzle with human methods. Also, there is some helper classes like Pos, Figure or Puzzle, that provide greate API for interacting with puzzle itself.
One of the fings I care about is simplicity. This library has minimal dependeces list possible, but it is still quite fast.

> **Note**
> This is very young project. Bugs expected. Please write issues about them.
> Mainly this library was build for my app for Sudoku solving - [Gudoku](https://github.com/dudozermaks/Gudoku).
> Because those projects are activly developing, you might see some changes of API along the new releases.

## How to build
We don't have complex build systems. Just add folder with library to include directory and some C++ files to your build list and that's it!

### For using in your own projects
1. Clone this repo.
2. Add `sudoku_lib.hpp` to your inclue list or put file in the same folder your files are.
3. Add `tdoku/solver_dpll_triad_simd.cc` and `tdoku/util.cc` to your build list.
4. That's it! No more dependences!

### For tests
1. Clone this repo.
2. `g++ test.cpp tdoku/solver_dpll_triad_simd.cc tdoku/util.cc -o test`

## A few words about internals.
### Pos
This class represents position in gird, like `Pos{0, 8}` means it's column 1 and row 9. Nothing special.

### Figure
Set of Pos'es. This is an interesting thing.

+ `Figure(9, 9)` generates Figure, which contains positions from {0, 0} to {8, 8}. Using this you can quickly itterate through all pencilmarks or all clues in the puzzle.
+ `Figure().col(x)`, `Figure().row(x)` `Figure().square(x)` generates a Figure number x. If x=3, than it will return Figure of 3'rd column, row, or square.
+ Plus, you can stack those, meaning: `Figure.col(x).square(y)` returns Figure with both column X and square Y positions.
+ `Figure.get_neighbours(Pos pos)` returns Figure of column, row and square this pos belongs to.

There is more, but this is some of the most useful features.

### Puzzle
This class stores clues and pencilmarks. Can also generate pencilmarks using clues.
There is quite a few methods, and they all are rarely used, but when used, they are quite convenient.

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
Generates minimized sudoku puzzle.

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
+ Add ImageMaker, that makes picture out of Puzzle. Probably will have some additional dependecies (but maybe not). Probably will be based on SVG format.
+ Make additional app to generate Sudoku puzzles (for journals, personal use, etc.).
+ Add good documentation, maybe even with Doxygen.

### Maybe...
+ ...Completely re-write this, to Rust or even Zig or something else... The reason I wrote it in C++ is that I know this language very well. I know that start a project in C++ now is a bad idea, so maybe i re-write it.
+ ...Add programmable validator of sudoku puzzles with it's own syntax, for killer and other variations. I think it will be slow, so you will be able only validate puzzles with it.

## Useful resources
### Sites
- [Sudoku Wiki](https://www.sudokuwiki.org/sudoku.htm) - Sudoku solver that uses human methods. Every method explained with details and examples. Love this one!
- [Sudoku Coach](http://www.taupierbw.be/SudokuCoach) - Detailed description of almost all sudoku methods with examples.
- [Sudoku Of The Day](https://www.sudokuoftheday.com) - Good example of rating a Sudoku puzzle. Scores from methods was taken from their website.
### Youtube
- [Cracking The Cryptic](https://www.youtube.com/@CrackingTheCryptic) - Youtube chanell, where to men solve logical puzzles. I got so much inspirations from this one! It's definitely content made mainly for older generation, but I enjoy it so much! Also, I've got some sudoku strateges from them (like Phistomefel Ring).
### Libraries
- [Tdoku](https://github.com/t-dillon/tdoku) - Fastest sudoku solver. It's shipped with library in most minimal form possible.
