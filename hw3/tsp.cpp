// CPSC 4600 / 5600 - Traveling Salesman Problem
//
#include <iostream>
#include <string>
#include <fstream>
#include <map>
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
	int totalNumOfNodes;	
	int numOfNodesVisited;			//count of number of nodes in currentPath	
	int *currentPath;				//array for current path
	bool *visited;					//keep track of which nodes visited
	int *totalCostPerNodeInPath;	//running total cost till last node in currentPath		

	map <int, vector<int> > minsAtEachPathNode;		//list of mins at each path node 


    Path(int n, int numNodesVisited) {
		totalNumOfNodes = n;
		currentPath = new int[n];
		totalCostPerNodeInPath = new int[n];
		numOfNodesVisited = numNodesVisited;
		visited = new bool[n];
		for (int i = 0; i < n; i++) {
			currentPath[i] = 0;
			totalCostPerNodeInPath[i] = 0;
			visited[i] = false;
		}
	}

    Path(const Path& p) : Path(p.totalNumOfNodes, p.numOfNodesVisited) {	
		totalNumOfNodes = p.totalNumOfNodes;	
		numOfNodesVisited = p.numOfNodesVisited;	
		memcpy(currentPath, p.currentPath, sizeof(int) * p.totalNumOfNodes);
		memcpy(totalCostPerNodeInPath, p.totalCostPerNodeInPath, sizeof(int) * p.totalNumOfNodes);
		memcpy(visited, p.visited, sizeof(bool)*p.totalNumOfNodes);
		minsAtEachPathNode.insert(p.minsAtEachPathNode.begin(), p.minsAtEachPathNode.end());
	}

    ~Path() {
		delete[] currentPath;
		delete[] totalCostPerNodeInPath;
		delete[] visited;
		minsAtEachPathNode.erase(minsAtEachPathNode.begin(), minsAtEachPathNode.end());
	} 
};



/* Store Final Result */
class WinningPath {

public:
	tbb::mutex lock;
	int size;
	int *path;
	int winningCost;
	bool pathFound;

	WinningPath(int n) {
		size = n;
		path = new int[size];
		winningCost = -1;
		pathFound = false;
	}

	void printWinningPath() {
		for (int i = 0; i < size; i++) {
			cout << path[i] << " ";
		}
		cout << endl << "Total Cost: " << winningCost << endl;
	}
};



bool tsp_backtracking (int currentRowNum,
	int numNodes,
	Graph &g,
	Path p,
	int costTillNow,
	int limit,
	WinningPath &wp,
	int globalMaxLimit) {

	p.visited[currentRowNum] = true;
	if (p.numOfNodesVisited == numNodes - 1) {
		//for moving back from last node to original starting position
		p.visited[0] = false;
	}

	if (p.numOfNodesVisited >= numNodes && costTillNow > globalMaxLimit) {
		return false;
	}

	if (costTillNow >= globalMaxLimit && p.numOfNodesVisited < numNodes) {
		return false;
	}

	if (p.numOfNodesVisited == numNodes && costTillNow <= globalMaxLimit) {
		//path found
		wp.lock.lock();		
		wp.pathFound = true;
		wp.winningCost = p.totalCostPerNodeInPath[p.numOfNodesVisited];
		for (int i = 0; i <= numNodes; i++) {
			wp.path[i] = p.currentPath[i];
		}
		wp.lock.unlock();
		return true;
	}
	else {
		while (p.numOfNodesVisited <= numNodes) {

			int minCostToNextNode = globalMaxLimit;
			int newLimit = limit;

			//find valid min from this currentRowNum
			for (int i = 0; i < numNodes; i++) {
				if (p.visited[i] || i == currentRowNum)
					continue;
				int tempCost = g(currentRowNum, i);
				if (tempCost>0 && tempCost <= minCostToNextNode) {
					minCostToNextNode = tempCost;
				}
			}
			//find all indices for this min 
			for (int i = 0; i < numNodes; i++) {
				int tempCost = g(currentRowNum, i);
				if (tempCost == minCostToNextNode) {
					p.minsAtEachPathNode[currentRowNum].push_back(i);
				}
			}
			//iterate over vector for this node and recurse
			vector<int> tempV = p.minsAtEachPathNode[currentRowNum];

			for (int i = 0; i < tempV.size(); i++) {

				if (!p.visited[tempV[i]]) {
				
					if (g(currentRowNum, tempV[i]) > limit) {		//cannot add node
						
						//backtrack and erase all min information till we get to element with more than 1 min
						p.visited[tempV[i]] = false;
						p.visited[currentRowNum] = false;	
						int targetMinToRemove = tempV[i];
						int nextNode = currentRowNum;
						int newLimit = limit;
						int newCostTillNow;

						for (int k = numNodes - 1; k > 0; k--) {

							int prevNode = p.currentPath[k];
							if (prevNode == 0)
								continue;

							if (k == 1 && p.currentPath[k-1] == 0) {
								nextNode = 0;
								newLimit = globalMaxLimit;
								newCostTillNow = 0;
								break;
							}

							vector<int> tempVec = p.minsAtEachPathNode[prevNode];
							if (std::find(tempVec.begin(), tempVec.end(), targetMinToRemove) != tempVec.end()) {
								//remove 'targetMinToRemove' from list of mins for prevNode
								tempVec.erase(std::remove(tempVec.begin(), tempVec.end(), targetMinToRemove), tempVec.end());
								p.minsAtEachPathNode[prevNode] = tempVec;
							}

							if (p.minsAtEachPathNode[prevNode].size() > 1 && p.minsAtEachPathNode[prevNode].back() != 0 ) {
								//calculate what should be the next node
								nextNode = p.minsAtEachPathNode[prevNode].back();
								int indexOfNextNode;
								for (int y = numNodes -1 ; y > 0; y--) {
									if (p.currentPath[y] == nextNode) {
										indexOfNextNode = y;
										break;
									}
								}
								newLimit = globalMaxLimit - p.totalCostPerNodeInPath[indexOfNextNode];
								newCostTillNow = p.totalCostPerNodeInPath[indexOfNextNode];
								break;
							}
							else {
								targetMinToRemove = prevNode;
								p.currentPath[k] = 0;
								p.totalCostPerNodeInPath[k] = 0;
								int indexOfPrevNode;
								for (int x = numNodes; x > 0; x--) 
									if (p.currentPath[x] != 0) {
										indexOfPrevNode = x;
										break;
									}												
								p.totalCostPerNodeInPath[indexOfPrevNode] = 0;
								p.numOfNodesVisited--;
								p.visited[prevNode] = false;
							}
						}
						return tsp_backtracking(nextNode, numNodes, g, p, newCostTillNow, newLimit, wp, globalMaxLimit);
					}
					else {
						//no backtrack
						p.numOfNodesVisited++;
						p.currentPath[p.numOfNodesVisited] = tempV[i];
						p.totalCostPerNodeInPath[p.numOfNodesVisited] = p.totalCostPerNodeInPath[p.numOfNodesVisited - 1] + g(currentRowNum, tempV[i]);
						newLimit = limit - g(currentRowNum,tempV[i]);
						return tsp_backtracking(tempV[i], numNodes, g, p, p.totalCostPerNodeInPath[p.numOfNodesVisited], newLimit, wp, globalMaxLimit);
					}
				}		//end if
			}		//end for
		}		//end while
	}		//end if
	return false;
}


// TODO: Complete this function. You are allowed to modify the parameter list and/or return type of this function.
bool tsp(Graph &g, Path p, int cost, int limit, int size, WinningPath &wp)
{
	return tsp_backtracking(0,size, g, p, 0, limit, wp, limit);
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
  Path p(size+1,0);

  // Start the timer
  tick_count start_time = tick_count::now();

  // Execute the parallel algorithm
  // TODO: Modify if altered function signature.
  WinningPath wp(size+1);
  bool result = tsp(g, p, 0, limit, size, wp);

  // Stop the timer
  tick_count end_time = tick_count::now();
  double run_time = (end_time - start_time).seconds();
  cout << "Time: " << run_time << endl;

  // Print out results
  // TODO: Modify to print out cost and path 
  if (result) {
    cout << "Tour found!" << endl;
	wp.printWinningPath();
  }
  else {
    cout << "Tour not found" << endl;
  }

  return 0;
}
