// CPSC 4600 / 5600 - ConcurrNodeent Linked List
// Course-grained Lock Synchronization

#include "tbb/tbb.h"
#include "tbb/spin_mutex.h"
using namespace tbb;

#include "ConLinkList.h"

ConLinkList::Node::Node() {

}

ConLinkList::Node::Node(int _key, int _data) {
	key = _key;
	data = _data;
	next = NULL;
}

ConLinkList::ConLinkList()
{
	//initialize head
	head = new Node(LOW_SENTINEL, 0);
	head->next = new Node(HIGH_SENTINEL, 0);
}


bool ConLinkList::add(int key, int data)
{
	FreeListMutexType::scoped_lock mylock(ListMutex);

	if (key <= LOW_SENTINEL || key == HIGH_SENTINEL)
		return false;

	Node *prevNode = head;
	Node *currNode = prevNode->next;
	Node *newNode = new Node(key, data);

	//check for tail
	if (currNode->key == HIGH_SENTINEL) {
		newNode->next = currNode;
		head->next = newNode;
		return true;
	}

	//iterate to find correct insert position for this key
	while (currNode->key != HIGH_SENTINEL && currNode->key < key) {
		prevNode = currNode;
		currNode = currNode->next;
	}
	if (currNode->key != HIGH_SENTINEL && key == currNode->key) {
		return false;
	}
	else {
		newNode->next = currNode;
		prevNode->next = newNode;
		return true;
	}
}

bool ConLinkList::remove(int key)
{
	FreeListMutexType::scoped_lock mylock(ListMutex);
	Node *prevNode = head;
	Node *currNode = prevNode->next;

	//iterate
	while (currNode->key < key && currNode->key != HIGH_SENTINEL) {
		prevNode = currNode;
		currNode = currNode->next;
	}
	if (currNode->key == key) {
		prevNode->next = currNode->next;
		return true;
	}
	else
		return false;
}


bool ConLinkList::contains(int key)
{
	FreeListMutexType::scoped_lock mylock(ListMutex);
	Node *prevNode = head;
	Node *currNode = prevNode->next;

	if (currNode->key == HIGH_SENTINEL) {
		return false;
	}
	//iterate
	while (currNode->key < key && currNode->key != HIGH_SENTINEL) {
		prevNode = currNode;
		currNode = prevNode->next;
	}
	//check
	if (currNode->key == key)
		return true;
	else
		return false;
}



// Unsafe to run concurrently, used for debugging only
int ConLinkList::count()
{
	Node *prevNode = head;
	Node *currNode = prevNode->next;
	int count = 0;
	while (currNode->next != NULL) {
		prevNode = currNode;
		currNode = prevNode->next;
		count++;
	}
	return count;
}
