//============================================================================
// Name        : tests.cpp
// Author      : Thomas J. Meehan
// Version     : 2.0
// Copyright   : Copyright 2016 Thomas J. Meehan
/*
 This file is part of kdTree.

 kdTree is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 kdTree is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with kdTree.  If not, see <http://www.gnu.org/licenses/>.
 */

// Description : Tests functions from kdTree.h and kdTree.cpp
//============================================================================
#include "kdTree.h"

//fill vector with points from csv
vector<double*> fillVector(vector<double*> arr, string file) {

	cout << endl << "Filling vector with values from " << file << endl;

	int cols = 0;
	int rows = 0;
	std::string line;

	ifstream myfile(file);
	if (myfile.is_open()) {
		string cell;
		const char* ch;
		double val;

		while (getline(myfile, line)) { //get every point
			if (cols == 0) { //figure out how many dimensions to each point
				size_t n = std::count(line.begin(), line.end(), ',');
				cols = n + 1;
			}

			arr.push_back(new double[cols]); // add new row
			stringstream lineStream(line);

			for (int i = 0; i < cols; i++) {
				if (i < cols - 1)	//get every value except last, stop at comma
					getline(lineStream, cell, ',');
				else
					getline(lineStream, cell);

				ch = cell.c_str();
				val = atof(ch);
				arr[rows][i] = val;
			}
			rows++;
		}

		cout << "array contains " << rows << " rows with " << cols << " columns"
				<< endl;
		myfile.close();
		return arr;
	} else {
		cout << "Unable to open file for array data\n";
		return arr; // returns empty vector, used later for error checking
	}
}

//test getting distance between points, returns if any tests failed
bool testDist(vector<double*>& tree, vector<double*>& Q, treeNode * root,
		vector<nPoint*> queries, int dims) {

	//use this to overwrite text file with new distance values to test against.
	bool overWriteAnswers = false;
	//use this to check point distance methods against external text file reference
	bool checkFile = true;
	//track if any distance tests fail
	bool someTestFail = false;

	vector<double*> A;
	vector<double*> B;
	int d = 3;
	int numTest = 5;

	//basic test data to ensure distance calculations work properly

	double org[3] = { 0, 0, 0 };
	double tri[3] = { 3, 4, 5 };
	double big[3] = { 100, 100, 100 };
	double trip[3] = { 6, 8, 10 };
	double perf[3] = { 5, 12, 13 };

	double one[3] = { 1, 1, 1 };
	double neg[3] = { -1, -1, -1 };
	double four[3] = { 4, 4, 4 };
	double ott[3] = { 1, 2, 3 };
	double rand[3] = { 0.1243425, 0.37934759, 0.9487525 };

	A.push_back(org);
	A.push_back(tri);
	A.push_back(big);
	A.push_back(trip);
	A.push_back(perf);

	B.push_back(one);
	B.push_back(neg);
	B.push_back(four);
	B.push_back(ott);
	B.push_back(rand);

	vector<nPoint *> p1;
	vector<nPoint *> p2;

	for (int i = 0; i < numTest; i++) {
		p1.push_back(new nPoint(0, 3, A[i]));
		p2.push_back(new nPoint(0, 3, B[i]));
	}

	//write to file for potential future comparisons
	ofstream pStream("pOutput.txt");
	if (pStream.is_open()) {

		int i = 0;
		int j = 0;
		for (nPoint* p : p1) {
			for (nPoint* q : p2) {

				pStream << p->getAbDist(q) << endl;
				j++;
			}
			i++;
		}
	} else {
		cout << "Could not open output file for distance calculation tests\n";
	}

	ofstream vStream;

	if (overWriteAnswers) {
		vStream.open("distComp.txt");
		cout << "overwriting distances used in test comparisons\n";
	}

	//compare class method versus brute force to verify correctness
	//can also write to external file for later comparisons
	double difference;
	double total = 0;
	for (int a = 0; a < numTest; a++) {
		for (int b = 0; b < numTest; b++) {
			for (int dim = 0; dim < d; dim++) { // every dim
				difference = A[a][dim] - B[b][dim];
				total += difference * difference;
			}
			if (overWriteAnswers) {
				vStream << sqrt(total) << endl;
			}
			nPoint * temp = p1[a];
			nPoint * temp2 = p2[b];
			if (temp->getAbDist(temp2) == sqrt(total)) {
				//cout << "DIST EQUAL \n";
			} else { //report inconsistancies
				cout << "DISTANCES FOR POINTS ";
				temp->print();
				cout << " and ";
				temp2->print();
				cout << " ARE NOT EQUAL\n";
				someTestFail = true;
			}
			total = 0;
		}
	}
	if (someTestFail) {
		cout
				<< "\nSOME BASIC DISTANCE TESTS FAILED, SEE ABOVE OUTPUT FOR DETAIL\n";
	} else {
		cout << "\nALL BASIC DISTANCE TESTS PASSED\n\n";
	}

	//optionally check against external text file
	if (checkFile) {
		FILE * pFile;
		FILE * qFile;
		pFile = fopen("pOutput.txt", "r");
		qFile = fopen("distComp.txt", "r");
		if (pFile != NULL && qFile != NULL) {
			int isId = compareFile(pFile, qFile);
			if (isId) {
				cout << "ALL DISTANCE TESTS IN FILE PASSED\n";

			} else {
				cout << "SOME DISTANCE TESTS IN FILE FAILED\n";
				someTestFail = true;
			}
		} else {
			cout << "ERROR, distance test files could not be located\n";
			someTestFail = true;
		}
	}

	double bestDistance = DBL_MAX;
	nPoint * bestPt = nullptr;

	int treeSize = tree.size();

	int qSize = Q.size();
	double sum = 0;
	double dif = 0;

	int cIndex = -1;
	double minDist = DBL_MAX;

	//brute force nearest neighbor search: search everything exhaustively
	for (int q = 0; q < 10; q++) { // every query point

		for (int t = 0; t < treeSize; t++) { // every tree node
			sum = 0;
			//cout <<
			for (int dim = 0; dim < dims; dim++) { // every dimension
				dif = tree[t][dim] - Q[q][dim];
				sum += dif * dif;
			}
			if (sum < minDist) {
				minDist = sum;
				cIndex = t;
			}
		}

		queries.at(q)->findNear(root, bestPt, bestDistance, dims); //use nearest neighbor algorithim

		if (bestPt->getIndex() == cIndex && bestDistance == sqrt(minDist)) {
		} else {
			cout << "DISTANCE TEST FAIL FOR QUERRY " << q << endl;
			someTestFail = true;
		}

		bestPt = nullptr;
		minDist = DBL_MAX;
		bestDistance = DBL_MAX;

	}
	if (someTestFail) {
		cout
				<< "\nSOME QUERRY DISTANCE TESTS FAILED, SEE ABOVE OUTPUT FOR DETAIL\n";
	} else {
		cout << "\nALL QUERRY DISTANCE TESTS PASSED\n";
	}
	return someTestFail;
}

//runs all tests
int main(int argc, char *argv[]) {
	//track if any tests fail
	bool anyTestFail = false;

	//use this to check point distance methods against external text file reference
	bool checkFile = true;

	treeNode * root = new treeNode;

	int axisMode;
	vector<double*> treeArr;
	vector<double*> qArr;

	vector<nPoint*> pointVector;
	vector<nPoint*> queries;

	string fileName = "sample_data.csv";
	string qFile = "query_data.csv";
	axisMode = 1;

	if (argc > 1) {
		fileName = argv[1];
	}
	if (argc > 2) {
		qFile = argv[2];
	}
	if (argc > 3) {
		axisMode = atoi(argv[3]);
	}
	if (axisMode == 0) {
		cout << "Using rotating hueristic for axis choice\n" << endl;
	} else {
		cout << "Using range hueristic for axis choice\n" << endl;
	}

	int k = getDataFile(fileName, pointVector); //return how many dimensions (k) data is

	if (k == -1) {
		cout << "getData() test failed, exiting\n";
		anyTestFail = true;
		exit(3);
	} else {
		cout << "getData() test PASSED\n";
	}

	int len = 5;

	vector<nPoint *>::iterator beg = pointVector.begin();
	vector<nPoint *>::iterator end = pointVector.begin() + len;

	vector<nPoint *> L(pointVector.begin(), pointVector.begin() + len);

	cout << "\nTest print and check for data collection PASSED\n\n";
	for (vector<nPoint *>::iterator it = beg; it < end; it++) {
		nPoint * thing = *it;
		thing->print();
		cout << endl;
	}

	root->makeTree(pointVector, 0, k, axisMode); //make tree

	if (root == nullptr) {
		cout << "\nError creating tree, exiting\n";
		anyTestFail = true;
		exit(4);
	} else {
		cout << "\nTree created SUCCESS\n";
	}

	getDataFile(qFile, queries); //get query data

	ofstream q1Strm("q1.txt");

	cout << endl << "Get nearest neighbor for first 10 queries" << endl << endl;
	double bestDistance = DBL_MAX;
	nPoint * bestPt = nullptr;
	for (int i = 0; i < 10; i++) {
		queries.at(i)->findNear(root, bestPt, bestDistance, k);

		cout << "For querry point " << i << " best node was ";
		bestPt->print();
		cout << " with distance of " << bestDistance << endl;

		q1Strm << bestPt->getIndex() << "," << bestDistance << endl;

		bestDistance = DBL_MAX;
		bestPt = nullptr;
	}

	root->writeOut("newTree.txt", k); //write tree to file

	//check tree write against external text file
	if (checkFile) {
		FILE * pFile;
		FILE * qFile;
		pFile = fopen("outComp.txt", "r");
		qFile = fopen("newTree.txt", "r");
		if (pFile != NULL && qFile != NULL) {
			int isId = compareFile(pFile, qFile);
			if (isId) {
				cout << "\nFIRST TREE WRITE PASSED\n";

			} else {
				cout << "\nFIRST TREE WRITE FAILED\n";
				anyTestFail = true;
			}
		} else {
			cout
					<< "ERROR, first tree write out test files could not be located\n";
			anyTestFail = true;
		}
	}

	//new root node for new tree read in through file I/O
	treeNode * otherTree = new treeNode;

	otherTree->readTree("newTree.txt", k); //read back

	otherTree->writeOut("re-write.txt", k); //write again

	//check to make sure first write matches second write
	if (checkFile) {
		FILE * pFile;
		FILE * qFile;
		pFile = fopen("newTree.txt", "r");
		qFile = fopen("re-write.txt", "r");
		if (pFile != NULL && qFile != NULL) {
			int isId = compareFile(pFile, qFile);
			if (isId) {
				cout << "\nSECOND TREE WRITE PASSED\n";

			} else {
				cout << "\nSECOND TREE WRITE FAILED\n";
				anyTestFail = true;
			}
		} else {
			cout
					<< "ERROR, first and second write out comparison test files could not be located\n";
			anyTestFail = true;
		}
	}

	//test destructor and correct copying by deleting original tree
	delete root;

	cout << "\nDELETE SUCCESS\n";

	//test new tree for same answers as last time, this time using tree from disk

	ofstream q2Strm("q2.txt");

	bestDistance = DBL_MAX;
	bestPt = nullptr;
	cout << endl << "Get nearest neighbor for first 10 queries in new tree"
			<< endl << endl;

	for (int i = 0; i < 10; i++) {
		queries.at(i)->findNear(otherTree, bestPt, bestDistance, k); //use new build tree

		cout << "For querry point " << i << " best node was ";
		bestPt->print();
		cout << " with distance of " << bestDistance << endl;

		q2Strm << bestPt->getIndex() << "," << bestDistance << endl;

		bestDistance = DBL_MAX;
		bestPt = nullptr;
	}

	//check to see if querry results are the same as original tree.
	if (checkFile) {
		FILE * pFile;
		FILE * qFile;
		pFile = fopen("q1.txt", "r");
		qFile = fopen("q2.txt", "r");
		if (pFile != NULL && qFile != NULL) {
			int isId = compareFile(pFile, qFile);
			if (isId) {
				cout << "\nCOPY GENERATES SAME QUERRIES PASSED\n";

			} else {
				cout << "\nCOPY GENERATES SAME QUERRIES FAILED\n";
				anyTestFail = true;
			}
		} else {
			cout
					<< "ERROR, querry comparison test files could not be located\n";
			anyTestFail = true;
		}
	}

	//fill arrays for brute force test
	treeArr = fillVector(treeArr, "sample_data.csv");

	if (treeArr.empty()) {
		cout << "filling array with sample_data FAILED\nEXITING\n";
		exit(1);
	}

	vector<double*> Q;

	Q = fillVector(Q, "query_data.csv");

	if (Q.empty()) {
		cout << "filling array with query_data FAILED\nEXITING\n";
		exit(2);
	}

	//begin distance method testing
	//also conducts brute force testing against nearest neighbor algorithm
	anyTestFail = testDist(treeArr, Q, otherTree, queries, k);

	//final cleanup
	//delete tree and clean up vectors
	delete otherTree;

	for (nPoint * p : queries) {
		delete p;
	}
	queries.clear();
	for (double * p : Q) {
		delete p;
	}
	Q.clear();
	for (double * p : treeArr) {
		delete p;
	}
	treeArr.clear();
	cout << "\nAll deletions successful\n";

	cout << "\nTests finished\n";

	if (anyTestFail) {
		cout << "SOME TESTS FAILED, SEE ABOVE OUTPUT FOR DETAILS\n";
	} else {
		cout << "\nALL TESTS PASSED!\n";
	}
}
