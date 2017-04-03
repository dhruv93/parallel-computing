// CPSC 4600 / 5600 - Concurrent Linked List
// Lazy Synchronization

#include "tbb/tbb.h"
using namespace tbb;

#include "ConLinkList.h"


ConLinkList::Node::Node() {
	toBeDeleted = false;
	next = NULL;
}

ConLinkList::Node::Node(int _key, int _data) {
	key = _key;
	data = _data;
	toBeDeleted = false;
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
	bool result = true;
	Node *prevNode;
	Node *currNode;

	while (true) {
		prevNode = head;
		currNode = prevNode->next;

		while (currNode->key < key && currNode->key != HIGH_SENTINEL) {
			prevNode = currNode;
			currNode = prevNode->next;
		}

		//add
		if ((prevNode->key < key && key < currNode->key)) {

			prevNode->nodeLock.lock();
			currNode->nodeLock.lock();

			if (!prevNode->toBeDeleted && !currNode->toBeDeleted && prevNode->next->key == currNode->key) {

				Node *newNode = new Node(key, data);
				if (key == currNode->key) {
					result = false;
				}
				else {				
					newNode->next = currNode;
					prevNode->next = newNode;
					result = true;
				}
				prevNode->nodeLock.unlock();
				currNode->nodeLock.unlock();
				break;
			}
			prevNode->nodeLock.unlock();
			currNode->nodeLock.unlock();
		}
	}		//end while
	return result;
}

bool ConLinkList::remove(int key)
{
	bool result = true;
	Node *prevNode;
	Node *currNode;

	while (true) {
		prevNode = head;
		currNode = prevNode->next;

		if (currNode->key == HIGH_SENTINEL) {
			return false;
		}
		//traversal
		while (currNode->key < key && currNode->key != HIGH_SENTINEL) {
			prevNode = currNode;
			currNode = prevNode->next;
		}
	
		//check and remove
		if (prevNode->key < key && key == currNode->key) {

			prevNode->nodeLock.lock();
			currNode->nodeLock.lock();

			if (!prevNode->toBeDeleted && !currNode->toBeDeleted && prevNode->next->key == currNode->key) {

				if (key == currNode->key) {
					currNode->toBeDeleted = true;
					prevNode->next = currNode->next;
					result = true;
				}
				else {
					result = false;
				}
				prevNode->nodeLock.unlock();
				currNode->nodeLock.unlock();
				break;
			}
			prevNode->nodeLock.unlock();
			currNode->nodeLock.unlock();
		}
	}		//end while
	return result;
}



bool ConLinkList::contains(int key)
{
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
	if (currNode->key == key && !currNode->toBeDeleted)
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
		if (!prevNode->toBeDeleted)
			count++;
	}
	return count;
}

