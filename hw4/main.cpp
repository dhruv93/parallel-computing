// CPSC 4600 / 5600 - Concurrent Linked List
//
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include "tbb/tbb.h"
using namespace tbb;

#include "ConLinkList.h"

const int NUM_THREADS = 8;

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		cerr << "Invalid command line - usage: <input file>" << endl;
		exit(-1);
	}

	// Extract parameters
	ifstream ifile(argv[1]);

	// Set the number of threads
	task_scheduler_init init(NUM_THREADS);

	// Declare the list
	ConLinkList clist;

	// Read in the file
	int size;
	ifile >> size;
	int *data = new int[size];
	for (int i = 0; i < size; i++) {
		ifile >> data[i];
	}

	atomic<int> count;
	count = 0;

	tick_count start_time, end_time;
	double seq_time;
	// -------------------------------------------------------
	// Test 1: Parallel adds
	start_time = tick_count::now();
	tbb::parallel_for(blocked_range<int>(0, size), [&](blocked_range<int> r) {
		for (int i = r.begin(); i < r.end(); i++) {
			int x = data[i];
			clist.add(x, x * 2);
		}
	});
	end_time = tick_count::now();
	seq_time = (end_time - start_time).seconds();
	cout << "Test 1 results: " << endl;
	cout << "Time: " << seq_time << endl;
	cout << "Size (correct): " << size << endl;
	cout << "Size (actual):  " << clist.count() << endl << endl;


	// -------------------------------------------------------
	// Test 2: Parallel contains
	start_time = tick_count::now();
	count = 0;
	tbb::parallel_for(blocked_range<int>(0, size), [&](blocked_range<int> r) {
		for (int i = r.begin(); i < r.end(); i++) {
			if (clist.contains(data[i])) {
				count++;
			}
		}
	});
	end_time = tick_count::now();
	seq_time = (end_time - start_time).seconds();
	cout << "Test 2 results: " << endl;
	cout << "Time: " << seq_time << endl;
	cout << "Count (correct): " << size << endl;
	cout << "Count (actual):  " << count << endl << endl;



	// -------------------------------------------------------

	// Test 3: Parallel removes (1st half of data) / parallel contains (2nd half of data)
	start_time = tick_count::now();

	//outer parallel_invoke to trigger removes and contains simultaneosly
	tbb::parallel_invoke(
		[size, &clist, data, &count]() {
			//remove first n/2 elements, and decrement count
			int first_half = size / 2;
			tbb::parallel_for(blocked_range<int>(0, first_half), [&](blocked_range<int> r) {
				for (int i = r.begin(); i < r.end(); i++) {
					if (clist.remove(data[i])) {
						count--;
					}
				}
			});
		},
		[size, &clist, data, &count]() {
			//check to see if second half is still there ...
			// .... if not, decrement count to reflect the change
			int second_half_start = (size / 2);
			tbb::parallel_for(blocked_range<int>(second_half_start, size), [&](blocked_range<int> r) {
				for (int i = r.begin(); i < r.end(); i++) {
					if (!clist.contains(data[i])) {
						count--;
					}
				}
			});
		}
	);
	end_time = tick_count::now();
	seq_time = (end_time - start_time).seconds();
	cout << "Test 3 results: " << endl;
	cout << "Time: " << seq_time << endl;
	cout << "Size  (correct): " << size / 2 << endl;
	cout << "Size  (actual):  " << clist.count() << endl;
	cout << "Count (correct): " << size / 2 << endl;
	cout << "Count (actual):  " << count << endl<<endl;



	// Test 4: Parallel adds (1st half of data) / parallel removes (2nd half of data)
	start_time = tick_count::now();

	//outer parallel_invoke to trigger adds (first_half) and removes(second_half) simultaneosly
	tbb::parallel_invoke(

		[size, &clist, data, &count]() {
			//add first n/2 elements, and increment count
			int first_half = size / 2;
			tbb::parallel_for(blocked_range<int>(0, first_half), [&](blocked_range<int> r) {
				for (int i = r.begin(); i < r.end(); i++) {
					if (clist.add(data[i], data[i]*2)) {
						count++;
					}
				}
			});
		},
		[size, &clist, data, &count]() {
			//remove second half elements from the list ... and decrement count accordingly
			int second_half_start = (size / 2);
			tbb::parallel_for(blocked_range<int>(second_half_start, size), [&](blocked_range<int> r) {
				for (int i = r.begin(); i < r.end(); i++) {
					if (clist.remove(data[i])) {
						count--;
					}
				}
			});
		}
	);

	end_time = tick_count::now();
	seq_time = (end_time - start_time).seconds();
	cout << "Test 4 results: " << endl;
	cout << "Time: " << seq_time << endl;
	cout << "Size (correct): " << size / 2 << endl;
	cout << "Size (actual):  " << clist.count() << endl << endl;

	return 0;
}
