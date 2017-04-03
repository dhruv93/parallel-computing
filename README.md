# parallel-computing
Parallel algorithmic analysis and development using divide and conquer, map and reduce, and data decomposition. Parallel computing implementations and performance factors. 

hw2
----------------
Part 1 – Grades
In this part, you will be creating a program that reads in grades (integer scores from 0 to 100) and performs various algorithms on them. Your responsibility is to write sequential and parallel versions of the following three algorithms:
adjust (adjust_seq / adjust_para): Adjust all grades such that each grade receives five additional points but the resulting score can be at most 100. For example, a 63 would be adjusted to a 68 and a 97 would be adjusted to 100.
count (count_seq / count_para): This function takes parameters low and high and returns the number of grades that are in the inclusive range [low, high]. For example, if the grades array is [80, 83, 94, 87, 95] and low is 80 and high is 87, the function returns 3.
scan (scan_seq / scan_para): This function also takes parameters low and high but returns an array where each element i in the array is the number of scores within in the range in the first i elements of that grades array. For example, if the grades array is [80, 83, 94, 87, 95] and low is 80 and high is 87, the function returns [1, 2, 2, 3, 3].

Part 2 – Life
Implement a toroidal version of Conway’s Game of Life for 10 steps. The universe is a fixed two-dimensional n × n grid of square cells (normally played on an infinite grid of square cells). Each cell is in one of two possible states: live or dead. Every cell interacts with its eight neighbors that are directly horizontally, vertically, or diagonally adjacent. At each step in time, the following transitions occur:
1. Any live cell with fewer than two live neighbors dies (loneliness).
2. Any live cell with more than three live neighbors dies (overcrowding).
3. Any live cell with two or three live neighbors lives, unchanged, to the next generation.
4. Any dead cell with exactly three live neighbors comes to life.
Notes:
• The toroidal nature of the grid means that the cells wrap around: cells on the left edge are adjacent to the cells on the right edge and cells on the top are adjacent to cells on the bottom.
• Births and deaths are based on the state of the grid at the beginning of the time step.

hw3
----------------
Part 1 - Maximum Difference
Write a parallel algorithm find the maximum difference of two integers in the array such that the smaller number appears earlier in the list than the larger number.

Part 2 - Quicksort
void qsort_seq(int *a, int low, int high): Sequential version of quicksort.
void p_qsort_inplace(int *a, int low, int high, int cutoff): Parallel version of quicksort that has average-case span of Θ(𝑛𝑛). 
                                                              This version should use a sequential partition function such that the sorting is in-place, no extra storage is necessary.
void p_qsort_best_span(int *a, int low, int high, int cutoff): Parallel version of quicksort that has optimal average-case span of Θ(lg2𝑛𝑛). 
                                                                This version should use a parallel partition function but will need to additional memory.

Part 3 - Travelling Salesman Problem
Write a parallel algorithm for the travelling salesman problem

hw4
----------------
Two concurrent linked list implementations: one using coarse-grained synchronization and another using lazy synchronization.

hw5
----------------
Write a parallel dynamic programming algorithm that computes the similarity of two strings as described below.
Given two strings a and b, a similarity score is given by finding the optimal alignment of the two strings such that similarity score is minimized. 
The lower the similarity score, the more similar the strings are. If a and b are identical, the similarity score is zero.
