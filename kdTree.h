/*
 * kdTree.h
 *
 *  Created on: Nov 12, 2016
 *      Author: Thomas J. Meehan
 *
 *      Copyright 2016 Thomas J. Meehan
 *
 */

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

#ifndef KDTREE_H_
#define KDTREE_H_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cfloat>
#include <list>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <limits>
#include <cstdio>
#include <cstring>

//constants
#define AXIS_METHOD 0
#define MEDIAN_METHOD 0
#define BRUTE 0

class treeNode;
using namespace std;

typedef std::numeric_limits<double> dbl;

//used to compare points by a specified dimension in sorting algorithims
template<typename T>
struct Sort: std::binary_function<T, T, bool> {
	Sort(int dim) :
			d(dim) {
	}
	bool operator()(T lhs, T rhs) const {
		return lhs->getAxisCord(d) < rhs->getAxisCord(d);
	}
	std::size_t d;
};

//an n-dimensional point
class nPoint {
protected:
	int index;
	int dims;
	double* cords;

public:
	//constructor to initialize class nPoint
	nPoint(int i = 0, int d = 0, double * c = nullptr);

	int getIndex() const;

	double * getCords() const;

	int getDims() const;

	//get the coordinate for a particualr axis specified by dim
	const double getAxisCord(int dim) const;

	//print out information about a point
	void print() const;

	//get absolute distance between two points
	double getAbDist(const nPoint* B) const;

	//nearest neighbor algorithm
	void findNear(const treeNode* tree, nPoint*& bestPoint,
			double& bestDistance, const int k) const;

	~nPoint();
};

//node for k-d tree
class treeNode {

protected:
	int axis;
	double val;
	nPoint * point;
	treeNode * left;
	treeNode * right;
public:

	treeNode(int a = 0, double v = 0, treeNode * l = nullptr, treeNode * r =
			nullptr, nPoint * p = nullptr);

	nPoint * getPoint() const;

	int getAxis() const;

	double getVal() const;

	treeNode* getLeft() const;

	treeNode* getRight() const;

	//recursively build tree
	treeNode * makeTree(vector<nPoint*> points, int dIndex,
			int const totalDepth, int axisMode);

	//recursive function called by writeOut
	void wRecur(const string fileName, const int totalDim, ofstream& stream,
			int totalNodes) const;

	//write tree to file for later use
	void writeOut(const string fileName, const int totalDim) const;

	//recursive function called by readTree
	treeNode * recurIn(stringstream& stream, int totalNodes,
			const int totalDim);

	//read in and build tree from external file
	treeNode* readTree(const string fileName, const int k);

	~treeNode();
};

//helper functions

//count commas to check dimensions of points
int countDims(string const fileName);

//get data from csv file and put it in point vector
int getDataFile(string const fileName, vector<nPoint*>& pointVector);

//compares contents of two files and returns if they are identical
//based on the code published at http://stackoverflow.com/questions/6163611/compare-two-files
//by username Christoph
bool compareFile(FILE* file_compared, FILE* file_checked);

//find the dimension with the largest range for a given set of points
int findLargestRange(vector<nPoint*> const points, int const totalDepth);

#endif /* KDTREE_H_ */
