//============================================================================
// Name        : build_kdtree.cpp
// Author      : Thomas J. Meehan
// Version     : 1.7
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

// Description : Build a kdtree based on a given csv and then write it to a specified location
//============================================================================

#include "kdTree.h"

int main(int argc, char *argv[]) {

	vector<nPoint*> pointVector;

	string source = "sample_data.csv";
	string dest = "treeOut.txt";
	int axisMode = 1;

	if (argc > 1) {
		source = argv[1];
	}
	if (argc > 2) {
		dest = argv[2];
	}
	if (argc > 3) {
		axisMode = atoi(argv[3]);
	}
	if (axisMode == 0) {
		cout << "Using rotating heuristic for axis selection" << endl;
	} else {
		cout << "Using range heuristic for axis selection" << endl;
	}

	cout << "Reading in tree data from " << source << endl;

	int k = getDataFile(source, pointVector); //return how many dimensions (k) data is

	treeNode * root = new treeNode;

	root->makeTree(pointVector, 0, k, axisMode); //recursively create tree

	cout << "Tree creation complete" << endl;

	root->writeOut(dest, k); //write tree to location

	cout << "Tree output to " << dest << endl;

	delete root; //deletes entire tree and associated data

	pointVector.clear(); //points already destroyed when tree is deleted, so simply clear vector

}

