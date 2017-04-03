// CPSC 4600 / 5600 - Traveling Salesman Problem
//
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include "tbb/tbb.h"
using namespace tbb;

// NOTE: It is not recommended that you modify the Graph class

class Graph
{
  public:
    Graph(int _size) {
      size = _size;
      array = new int[size * size];
    }

    ~Graph() { delete [] array; }

    int& operator()(int from, int to) {
      return array[from * size + to];
    }

    inline int getSize() const { return size; }

  private:
    int size;
    int *array;
};

// TODO: This class is incomplete

class Path
{
  public:
    Path() {}

    Path(const Path& p) {}

    ~Path() {}

  private:
};

// TODO: Complete this function. You are allowed to modify the parameter list and/or return type of this function.

bool tsp(Graph &g, Path p, int cost, int limit)
{
  return false;
}

// NOTE: Only modify the code segments with a TODO comment

int
main(int argc, char *argv[])
{
  if (argc != 3) {
    cerr << "Invalid command line - usage: <input file> <limit>" << endl;
    exit(-1);
  }

  // Extract parameters
  ifstream ifile(argv[1]);
  int limit = atoi(argv[2]);

  // Get the size of the problem - on the first line of the input file.
  int size;
  ifile >> size;

  // Create and initialize data structures
  Graph g(size);

  for (int row = 0; row < size; row++) {
    for (int col = 0; col < size; col++) {
      ifile >> g(row, col);
    }
  }

  // Initialize path
  // TODO: Modify to use appropriate constructor
  Path p;

  // Start the timer
  tick_count start_time = tick_count::now();

  // Execute the parallel algorithm
  // TODO: Modify if altered function signature.
  bool result = tsp(g, p, 0, limit);

  // Stop the timer
  tick_count end_time = tick_count::now();
  double run_time = (end_time - start_time).seconds();
  cout << "Time: " << run_time << endl;

  // Print out results
  // TODO: Modify to print out cost and path 
  if (result) {
    cout << "Tour found!" << endl;

    // Print out cost and path
  }
  else {
    cout << "Tour not found" << endl;
  }

  return 0;
}
