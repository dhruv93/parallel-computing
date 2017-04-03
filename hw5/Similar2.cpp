#include <string.h>
#include <malloc.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#include "tbb/tbb.h"
#include "tbb/queuing_rw_mutex.h"
#include "tbb/spin_rw_mutex.h"

using namespace tbb;

#define ADD_DELETE_COST 2

class Matrix {

	private:
		int rows;
		int cols;
		int *array;

	public:

		Matrix(int _rows, int _cols)
		{
			rows = _rows;
			cols = _cols;
			array = new int[rows * cols];
		}

		int& operator() (int i, int j)
		{
			return array[i*cols + j];
		}

};

bool inline isVowel(const char &ch) {

	if (ch == 'a'
		|| ch == 'e'
		|| ch == 'i'
		|| ch == 'o'
		|| ch == 'u')
		return true;

	else
		return false;
}


inline const int getCost(const char &a, const char &b) {

	if (a == b)
		return 0;

	else {

		if (isVowel(a)) {
			if (isVowel(b))
				return 1;
			else
				return 3;
		}

		else {  /* a is a consonant */
			if (isVowel(b))
				return 3;

			else {
				return 1;
			}
		}
	}
}

int optimalCost(const string &str1, const string &str2)
{
	int len1 = str1.size();
	int len2 = str2.size();

	Matrix m(len1+1, len2+1);

	for (int i = 0; i <= len1; i++) {
		m(i, 0) = i * 2;
	}

	for (int i = 0; i <= len2; i++) {

		m(0, i) = i * 2;
	}

	for (int i = 1; i <= (len1 + len2)-2; i++) {

		int z1 = (i < len2) ? 0 : i - len2;
		int z2 = (i < len1) ? 0 : i - len1;

		tbb::parallel_for(blocked_range<int>(z1+1, i - z2), [&](blocked_range<int> r) {

			for (int j = r.begin(); j<= r.end(); j++) {

				int b = i + 1 - j;
				int a = j;

				if (str1[a - 1] == str2[b - 1]) {
					m(a, b) = m(a - 1, b - 1);
				}
				else {
					m(a, b) = std::min<int>({
						m(a - 1,b) + ADD_DELETE_COST,
						m(a,b - 1) + ADD_DELETE_COST,
						m(a - 1,b - 1) + getCost(str1[a - 1], str2[b - 1])
					});
				}
			}     //end inner for
		});
	} // end outer for 
	return m(str1.size()-1, str2.size()-1);
}



int main(int argc, char *argv[])
{
	int answer;

	if (argc != 2) {
		cerr << "Invalid command line - usage: <input file>" << endl;
		exit(-1);
	}

	tick_count start_time = tick_count::now();

	//Reading File  
	ifstream ifile(argv[1]);
	std::string  str1;
	std::string  str2;

	// Extract strings
	while (!ifile.eof()) {
		ifile >> str1;
		ifile >> str2;
	}

	answer = optimalCost(str1, str2);

	// Stop the timer
	tick_count end_time = tick_count::now();
	double run_time = (end_time - start_time).seconds();

	cout << "\nAnswer " << answer << endl;
	cout << "Time:   " << run_time << endl;

	return 0;
}