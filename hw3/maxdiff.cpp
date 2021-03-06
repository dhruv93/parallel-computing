// CPSC 4600 / 5600 - Maximum Difference
//
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "tbb/tbb.h"
using namespace tbb;

// Returns highest value between three vars
int maxOf(int a, int b, int c) {
  return a > b ? (a > c ? a : c) : (b > c ? b : c);
}


int minVal(int *a, int low, int high) {
  
  int min = a[low];

  for(int i=low; i <= high; i++) {
    if(a[i] < min)
      min = a[i];
      }

  return min;
}

int maxVal(int *a, int low, int high){
  
  int max = a[low];

  for(int i=low; i <= high; i++) {
                if(a[i] > max)
                  max = a[i];
  }

  return max;
}


int max_sub_array(int *src, int low, int high)
{
  int left_sub_array;
    int right_sub_array;
    int min;
    int max;

  if (low >= high) {
      return 0; }
  
  else {
    int mid = (low + high) / 2;

    task_group g;

    g.run([&]{left_sub_array = max_sub_array(src, low, mid);});
    g.run([&]{right_sub_array = max_sub_array(src, mid+1, high);});
    g.wait();
    
    min = minVal(src, low, mid);
    max = maxVal(src, mid+1, high);

    return maxOf(left_sub_array, right_sub_array, max-min);
    }
   
  
}

int compute_max_diff(int *src, int size) {
 
  return max_sub_array(src, 0, size-1);

}

// NOTE: No modifications are permitted / needed beyond this point

int main(int argc, char *argv[])
{
  if (argc != 2) {
    cerr << "Invalid command line - usage: <input file>" << endl;
    exit(-1);
  }

  // Extract parameters
  ifstream ifile(argv[1]);

  // Read in the file
  int size;
  ifile >> size;
  int *src = new int[size];
  for (int i = 0; i < size; i++) {
    ifile >> src[i];
  }

  // Run algorithm
  int max_diff = compute_max_diff(src, size);
  cout << max_diff << endl;

  return 0;
}
