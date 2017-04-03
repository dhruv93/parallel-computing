// CPSC 4600 / 5600 - Life
//
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include "tbb/tbb.h"
using namespace tbb;

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

  // Start the timer
  tick_count start = tick_count::now();

  // TODO: Execute the parallel algorithm

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
