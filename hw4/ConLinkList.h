// CPSC 4600 / 5600 - Concurrent Linked List

#include <iostream>
#include <climits>
using namespace std;

#include "tbb/tbb.h"
#include "tbb/mutex.h"

using namespace tbb;

class ConLinkList {

private:

	const int LOW_SENTINEL = -1;
	const int HIGH_SENTINEL = INT_MAX;

	typedef spin_mutex FreeListMutexType;
	FreeListMutexType ListMutex;

	typedef tbb::mutex NodeMutexType;


#ifdef LAZY

	// TODO: Complete node definition and private variables and methods
	// for lazy synchronization

	struct Node {

		int key;
		int data;
		bool toBeDeleted;
		struct Node* next;
		mutable NodeMutexType nodeLock;

		Node();
		Node(int _key, int _data);
		Node(const Node& n);
	};

#else

	// TODO: Complete node definition and private variables and methods
	// for coarse-grained lock synchronization

	struct Node {
		int key;
		int data;
		struct Node* next;

		Node();
		Node(int _key, int _data);
	};

	
#endif
	// TODO: Place private variables and methods common to both techniques here
	
	Node *head;

	// NOTE: You are not allowed to modify the public section.
public:

	ConLinkList();
	bool add(int key, int data);
	bool remove(int key);
	bool contains(int key);
	int count();
};