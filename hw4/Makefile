all: clist llist

clist: main.cpp CoarseList.cpp ConLinkList.h
	g++ -ltbb -std=c++11 -O3 main.cpp CoarseList.cpp -o clist

llist: main.cpp LazyList.cpp ConLinkList.h
	g++ -ltbb -std=c++11 -O3 -DLAZY main.cpp LazyList.cpp -o llist

clean:
	rm -f clist llist
