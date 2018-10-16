all: query_kdtree build_kdtree tests

query_kdtree: query_kdtree.o kdTree.o
	g++ -o query_kdtree query_kdtree.o kdTree.o

query_kdtree.o: query_kdtree.cpp kdTree.h
	g++ -c -std=c++11 query_kdtree.cpp -o query_kdtree.o
	
build_kdtree: build_kdtree.o kdTree.o
	g++ -o build_kdtree build_kdtree.o kdTree.o

build_kdtree.o: build_tree.cpp kdTree.h
	g++ -c -std=c++11 build_tree.cpp -o build_kdtree.o

tests: tests.o kdTree.o
	g++ -o tests tests.o kdTree.o

tests.o: tests.cpp kdTree.h
	g++ -c -std=c++11 tests.cpp -o tests.o

kdTree.o: kdTree.cpp kdTree.h
	g++ -c -std=c++11 kdTree.cpp -o kdTree.o

clean:
	rm *.o