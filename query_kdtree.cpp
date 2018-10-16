//============================================================================
// Name        : query_kdtree.cpp
// Author      : Thomas J. Meehan
// Version     : 1.4
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

// Description : Read in a previously created tree and find the nearest
// neighbor for a series of query points
//============================================================================
#include "kdTree.h"

int main(int argc, char *argv[]) {

	vector<nPoint*> queries; //store data from csv

	string treeFile = "treeOut.txt";
	string input = "query_data.csv";
	string output = "results.txt";

	if (argc > 1) {
		treeFile = argv[1];
	}
	if (argc > 2) {
		input = argv[2];
	}
	if (argc > 3) {
		output = argv[3];
	}

	cout << "Reading in query data from " << input << endl;
	int k = getDataFile(input, queries); //return how many dimensions (k) data is
	treeNode * root = new treeNode;
	if( root->readTree(treeFile, k)){//create tree from file, also test for success
		cout << "Tree read in success" << endl << endl;
	}else{
		cout << "Error reading in tree from file, exiting\n";
		exit(1);
	}


	double bestDistance = DBL_MAX; //max value for double
	nPoint * bestPt = nullptr;
	ofstream myfile(output);

	myfile.precision(dbl::max_digits10); //max precision for writing to file
	if (myfile.is_open()) {

		for (int i = 0; i < queries.size(); i++) { //iterate through all queries and search tree to find nearest neighbor
			queries.at(i)->findNear(root, bestPt, bestDistance, k);

			cout << "For query " << i << " closest node was ";
			bestPt->print();
			cout << " with distance of " << bestDistance << endl;
			myfile << bestPt->getIndex() << "," << bestDistance << endl; //write to file

			bestDistance = DBL_MAX;
			bestPt = nullptr;
		}
		cout << "Successful write out to " << output << endl;
		myfile.close();

	} else {
		cout << "could not open file to read queries\n";
	}

	delete bestPt;

	delete root; //delete entire tree, including associated data

	queries.clear();

}

