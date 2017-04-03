// CPSC 4600 / 5600 - Life
//
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
using namespace std;

#include "tbb/tbb.h"

class Grid;

using namespace tbb;


class Grid {

  public:

    int gridSize = 0;
    int **grid;

    Grid(int _size) {

      gridSize = _size;
      grid = new int*[gridSize];
      for (int i = 0; i < gridSize; i++) {
          grid[i] = new int[gridSize];
      }

    }

    ~Grid() {
      for (int i = 0; i < gridSize; i++) {
        delete[] grid[i];
      }
      delete[] grid;

      delete &gridSize;
    }

    int getGridSize() const { return gridSize; }

    int getElementValue(int i, int j)  { return grid[i][j]; }

    int getNeighborSum (int i, int j)
    {
      /*
       *   Cell 1 (i-1, j-1)  ||  Cell 2 (i-1, j)   ||  Cell 3 (i-1, j+1)
       *  ----------------------------------------------------------------
       *   Cell 4  (i, j-1)   ||    <<Current Cell>>        ||  Cell 5 (i, j+1)
       *  ----------------------------------------------------------------
       *   Cell 6  (i+1, j-1) ||  Cell 7 (i+1, j)   ||  Cell 8 (i+1, j+1)
       */
      int prevRow = ((i-1) < 0) ? gridSize-1 : (i-1);
      int afterRow = ((i+1) >= gridSize) ? 0 : (i+1);

      int prevCol = ((j-1) < 0) ? gridSize-1 : (j-1);
      int afterCol = ( (j+1) >= gridSize) ? 0: (j+1);

      return( grid[prevRow][prevCol] + grid[prevRow][j] + grid[prevRow][afterCol]
          + grid[i][prevCol] +                      grid[i][afterCol]
          + grid[afterRow][prevCol] + grid[afterRow][j] + grid[afterRow][afterCol] 
      );
    }

    // Alive = 1
    // Dead = 0
    int generateNewValue (int i, int j, Grid &A)
    {
        int present = A.getElementValue(i,j);
        int newState = present;

        int sum = A.getNeighborSum(i,j);

        if (present == 1) 
        {
          if (sum < 2 || sum > 3) 
            newState = 0;   
          else
            newState = 1;
        }
        else {
          if (sum == 3)
            newState = 1;
          else
            newState = 0;
        }
        return newState;
    }

    void updateGridForNewGeneration_para(Grid &A) 
    {
      tbb::parallel_for(0,
              gridSize,
              [&] (int i) 
              {
                for (int j=0; j<gridSize; j++)
                  grid[i][j] = generateNewValue(i, j, A);
              });
    }
};



/*
 * Copy everything from B to A and zero out B
 */

void 
switchGrids(Grid* A, Grid* B, int size) 
{
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      A->grid[i][j] = B->grid[i][j];
      B->grid[i][j] = 0;
    }
  }
}



int
main(int argc, char *argv[])
{
  if (argc != 3) {
    cerr << "Invalid command line - usage: <input file> <number of threads>" << endl;
    exit(-1);
  }

  // Extract parameters
  ifstream ifile(argv[1]);
  int num_threads = atoi(argv[2]);

  // Set the number of threads
  task_scheduler_init init(num_threads);

  // Get the size of the problem - on the first line of the input file.
  int size;
  ifile >> size;


  // TODO: Create and initialize data structures
  Grid *A = new Grid(size);

  //read from file into A
  for (int i=0; i< A->getGridSize(); i++) {        
    for (int j=0; j< A->getGridSize(); j++) {
      ifile >> A->grid[i][j];
    }
  }
  ifile.close();

  Grid *B = new Grid(size);

  // Start the timer
  tick_count start = tick_count::now();

  // TODO: Execute the parallel algorithm

  //sequential loop for 10 generations (steps)

  for (int i=0; i<10; i++) {
    B->updateGridForNewGeneration_para(*A);
    switchGrids(A, B, size);
  }

  //print A (after swap) for result
  for (int i=0; i<A->getGridSize(); ++i) 
  {
    for (int j=0; j<A->getGridSize(); ++j)
    { 
      if (j == size-1)
        cout<<A->grid[i][j];
      else
        cout<<A->grid[i][j]<<" ";
    }
    cout<<endl;
  }

  // Stop the timer
  tick_count end = tick_count::now();
  double run_time = (end-start).seconds();
  cout << "Time: " << (end-start).seconds() << endl;

  // TODO: Print the output to a file
  ofstream outfile("output.txt");


  outfile.close();

  // Append the peformance results to the results file.
  ofstream ofile("life.csv", ios::app);
  ofile << size << "," << num_threads << "," << run_time << endl;
  ofile.close();

  return 0;
}
