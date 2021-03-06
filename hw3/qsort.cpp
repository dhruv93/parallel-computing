// CPSC 4600 / 5600 - Quicksort
//
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include <tbb/tbb.h>
using namespace tbb;

const int NUM_THREADS = 8;

class PrefixSum; //Forward Declaration

// Functions for testing and debugging
void check_array(int *a, int size);
void print_array(string name, int *src, int size);

class PrefixSum
{
      int* src;
      int* dest;
      int sum;
      
      public:
          PrefixSum(int* _src, int *_dest)
            : src(_src), dest(_dest) { sum = 0; }
      
          PrefixSum(PrefixSum &s, split)
            : src(s.src), dest(s.dest) { sum = 0; }
      
          int getSum() const { return sum; }
        

          template<typename Tag>
      
        void operator()
        (const blocked_range<int> r, Tag tag) {
          
          for (int i = r.begin(); i < r.end(); i++) {
            sum += src[i];
            if (tag.is_final_scan()) dest[i] = sum;
            }
          }
      
      void reverse_join(PrefixSum& s) { sum += s.sum; }
      
      void assign(PrefixSum& s) { sum = s.sum; }
      
} ;


int partition(int *a, int low, int high)
{
  int pivot, temp, i;

    if(low < high)
    {
        pivot = a[high];
        i = low-1;
        for(int j = low; j<high; j++) {
          if (a[j] <= pivot)
           { 
                i++;
                temp = a[i];
                a[i] = a[j];
                a[j] = temp; 
              }
        }

        temp = a[i+1];
        a[i+1] = a[high];
        a[high] = temp;

        return i+1;
    }
}

void qsort_seq(int *a, int low, int high)
{
      if(low < high) {
        int pivot = partition(a, low, high);
        qsort_seq(a, low, pivot-1);
        qsort_seq(a, pivot+1, high);
      }
}


void p_qsort_inplace(int *a, int low, int high, int cutoff)
{
  if(high - low < cutoff) 
       qsort_seq(a, low, high);
  
  else {
        int pivot = partition(a, low, high);
        task_group g;
        g.run([&]{p_qsort_inplace(a, low, pivot-1, cutoff);});
        g.run([&]{p_qsort_inplace(a, pivot+1, high, cutoff);});
        g.wait(); 
      }
}

int p_partition(int *src, int low, int high, int x)
{
  int n = high - low + 1;
    if (n == 1)
      return low;

    int *B = new int[n];
    int *lt = new int[n]; 
    int *gt = new int[n];
    int *ltTemp = new int[n];
    int *gtTemp = new int[n];

    parallel_for(blocked_range<int>(0,n),

        [&](const blocked_range<int>& r) 
          {
            for(int i=r.begin(); i < r.end(); i++) 
              {      
                  B[i] = src[low + i];

                  if(B[i] < x){ 
                      lt[i] = 1; 
                  }
                  else {
                      lt[i] = 0;
                  }

                  if (B[i] > x){
                      gt[i] = 1;
                  }
                  else{ 
                      gt[i] = 0;
                  }
              } 
          });


    PrefixSum ps1(lt, ltTemp);
    parallel_scan ( blocked_range<int>(0,n), ps1 );

    PrefixSum ps2(gt, gtTemp);
    parallel_scan ( blocked_range<int>(0,n), ps2 );

    int k = low + ltTemp[n-1];
    src[k] = x;

    parallel_for(blocked_range<int>(0,n),
        [&](const blocked_range<int>& r)
         {
          for(int i = r.begin(); i < r.end(); i++) 
          { 
                if (B[i] < x){
                  src[low + ltTemp[i] - 1] = B[i];
                }

                if (B[i] > x){ 
                    src[k + gtTemp[i]] = B[i]; 
                }
          }
    });

    return k;
}

   

void p_qsort_best_span(int *a, int low, int high, int cutoff)
{
    if((high - low) <= cutoff){
      qsort_seq(a,low, high);
    }
    else{
     int x  = a[high];
     int pivot = p_partition(a, low, high, x);

     task_group g;
     g.run([&]{p_qsort_best_span(a, low, pivot - 1, cutoff);});
     g.run([&]{p_qsort_best_span(a, pivot + 1, high, cutoff);});
     g.wait();
   }

}

// NOTE: No modifications are permitted / needed beyond this point

int
main(int argc, char *argv[])
{
  if (argc != 3) {
    cerr << "Invalid command line - usage: <input file> <parallel cutoff>" << endl;
    exit(-1);
  }

  // Extract parameters
  ifstream ifile(argv[1]);
  int cutoff = atoi(argv[2]);

  // Set the number of threads
  task_scheduler_init init(8);

  // Read in the file
  int size;
  ifile >> size;
  int *orig = new int[size];
  int *a = new int[size];
  for (int i = 0; i < size; i++) {
    ifile >> orig[i];
  }
   
  // Run qsort_seq
  for (int i = 0; i < size; i++) a[i] = orig[i];
  tick_count start_time = tick_count::now();
  qsort_seq(a, 0, size - 1);
  tick_count end_time = tick_count::now();
  double seq_time = (end_time - start_time).seconds();
  cout << "qsort_seq results: " << endl;
  cout << "Time: " << seq_time << endl;
  cout << "Checking: ";
  check_array(a, size);
  cout << endl;

  // Run p_qsort_inplace
  for (int i = 0; i < size; i++) a[i] = orig[i];
  start_time = tick_count::now();
  p_qsort_inplace(a, 0, size - 1, cutoff);
  end_time = tick_count::now();
  double p_inplace_time = (end_time - start_time).seconds();
  cout << "p_qsort_inplace results: " << endl;
  cout << "Time: " << p_inplace_time << endl;
  cout << "Checking: ";
  check_array(a, size);
  cout << endl;

  // Run p_qsort_best_span
  for (int i = 0; i < size; i++) a[i] = orig[i];
  start_time = tick_count::now();
  p_qsort_best_span(a, 0, size - 1, cutoff);
  end_time = tick_count::now();
  double p_best_span_time = (end_time - start_time).seconds();
  cout << "p_qsort_best_span results: " << endl;
  cout << "Time: " << p_best_span_time << endl;
  cout << "Checking: ";
  check_array(a, size);
  cout << endl;

  // Append the peformance results to the results file.
  ofstream ofile("qsort.csv", ios::app);
  ofile << size << "," << cutoff << ",";
  ofile << seq_time << ",",
  ofile << p_inplace_time << ",",
  ofile << p_best_span_time << endl;

  ofile.close();

  return 0;
}

// check_array: checks the contents of an array
void check_array(int *a, int size)
{
  for (int i = 0; i < size; i++) {
    if (a[i] != i + 1) {
      cout << "FAILED - first non-matching index is " << i << endl;
      print_array("contents", a, size);
      return;
    }
  }
  cout << "PASSED" << endl;
}

// print_array: prints the contents of an array
void print_array(string name, int *src, int size)
{
  cout << "Array " << name << ":" << endl;
  for (int i = 0; i < size; i++ ) {
    cout << src[i] << " ";
  }
  cout << endl;
}

