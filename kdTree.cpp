/*
 * kdTree.cpp
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

#include "kdTree.h"

//nPoint method definitions

nPoint::nPoint(int i, int d, double * c) :
		index(i), dims(d), cords(c) {
}

nPoint::~nPoint() {
	delete cords;
}

int nPoint::getIndex() const {
	return index;
}

double * nPoint::getCords() const {
	return cords;
}

int nPoint::getDims() const {
	return dims;
}

//print out info about a point
void nPoint::print() const {
	cout << "Point " << index << " at ";
	for (int i = 0; i < dims; i++) {
		cout << cords[i];
		if (i < dims - 1)
			cout << ",";
	}
}

//get the coordinate for a particualr axis specified by dim
const double nPoint::getAxisCord(int dim) const {
	return cords[dim];
}

//get euclidian distance between 2 points
double nPoint::getAbDist(const nPoint* B) const {
	if (this->getDims() != B->getDims()) {
		cout << "points are not of same dimension\n";
		return -1;
	} else {
		double sum = 0;
		double dif = 0;

		for (int dim = 0; dim < dims; dim++) { // every dim
			dif = this->getCords()[dim] - B->getCords()[dim];
			sum += dif * dif;
		}
		return sqrt(sum);
	}
}

//find nearest neighbor and print index and distance
void nPoint::findNear(const treeNode* tree, nPoint*& bestPoint,
		double& bestDistance, int k) const {

	bool lFirst;
	if (tree->getLeft() == nullptr && tree->getRight() == nullptr) { //found leaf
		double tempDist = this->getAbDist(tree->getPoint());
		if (tempDist < bestDistance) {
			bestDistance = tempDist;
			bestPoint = tree->getPoint();
		}
	} else { //decide whether to look left or right first
		if (this->getAxisCord(tree->getAxis()) <= tree->getVal()) {
			lFirst = true;
		} else {
			lFirst = false;
		}
		if (lFirst) { //left first
			if (this->getAxisCord(tree->getAxis()) - bestDistance
					<= tree->getVal()) {

				this->findNear(tree->getLeft(), bestPoint, bestDistance, k);
			}
			if (this->getAxisCord(tree->getAxis()) + bestDistance
					> tree->getVal()) {

				this->findNear(tree->getRight(), bestPoint, bestDistance, k);
			}
		} else { //right first
			if (this->getAxisCord(tree->getAxis()) + bestDistance
					> tree->getVal()) {

				this->findNear(tree->getRight(), bestPoint, bestDistance, k);
			}
			if (this->getAxisCord(tree->getAxis()) - bestDistance
					<= tree->getVal()) {

				this->findNear(tree->getLeft(), bestPoint, bestDistance, k);
			}
		}
	}
}

//treeNode method defnitions

treeNode::treeNode(int a, double v, treeNode* l, treeNode* r, nPoint * p) :
		axis(a), val(v), point(p), left(l), right(r) {
}

treeNode::~treeNode() {
	if (point != nullptr) {
		delete point;
	}
	if (left != nullptr) {
		delete left;
	}
	if (right != nullptr) {
		delete right;
	}
}

nPoint * treeNode::getPoint() const {
	return point;
}

int treeNode::getAxis() const {
	return axis;
}

double treeNode::getVal() const {
	return val;
}

treeNode* treeNode::getLeft() const {
	return left;
}

treeNode* treeNode::getRight() const {
	return right;
}

//recursive write function to write tree to file
void treeNode::wRecur(const string fileName, const int totalDim,
		ofstream& stream, int totalNodes) const {

	totalNodes++;

	stream.precision(dbl::max_digits10); //max precision for writing to file

	bool leaf;
	int axis;
	double val;
	nPoint * point;
	int index;
	int dims;
	double * cords;

	axis = this->getAxis();
	val = this->getVal();

	point = this->getPoint();

	if (point != nullptr) {
		index = point->getIndex();
		dims = point->getDims();
		cords = point->getCords();
		leaf = true;
	} else {
		leaf = false;
	}

	treeNode *left = this->getLeft();
	treeNode *right = this->getRight();

	stream << axis << "," << val << ",";
	if (leaf) {
		stream << index << "," << dims << ",";
		for (int i = 0; i < totalDim; i++) {
			stream << cords[i] << ",";
		}
	} else {
		stream << "NOT_LEAF,";
	}

	if (left == nullptr) {
		stream << "NULL,";
	} else {
		this->getLeft()->wRecur(fileName, totalDim, stream, totalNodes);
	}
	if (right == nullptr) {
		stream << "NULL,";
	} else {
		this->getRight()->wRecur(fileName, totalDim, stream, totalNodes);
	}

}

//base function to write tree to file, calls helper recursively
void treeNode::writeOut(const string fileName, const int totalDim) const {
	int totalNodes = 0;

	ofstream myfile(fileName);
	if (myfile.is_open()) {

		this->wRecur(fileName, totalDim, myfile, totalNodes);

		myfile.close();
	} else {
		cout << "Could not open file to write out tree contents.\n";
	}

}

//recursively creates a k-d tree with points sorted less than eqaul on left, greater to right
treeNode * treeNode::makeTree(vector<nPoint*> points, int dIndex,
		int const totalDepth, int axisMode) {
	int curDepth;
	static int leafCount = 0;
	static int totalCount = 0;
	totalCount++;

	if (points.size() < 1) {
		cout << "ERROR, LIST OF POINTS TOO SMALL" << endl;
		return nullptr;
	}
	if (points.size() == 1) { //create leaf

		curDepth = dIndex - 1;

		double pos = points.at(0)->getAxisCord(curDepth);

		this->axis = curDepth;
		this->val = pos;
		this->left = nullptr;
		this->right = nullptr;
		this->point = points.at(0);

		leafCount++;

		return this;
	}
	if (points.size() == 2) {

		if (axisMode == 0) { //simple rotation
			curDepth = dIndex % totalDepth;
		} else { //largest range
			curDepth = findLargestRange(points, totalDepth);
		}

		int middle = 0;

		nth_element(points.begin(), points.begin() + middle, points.end(),
				Sort<nPoint *>(curDepth)); //get median value, sort all less points to left and greater to right

		double median = points.at(0)->getAxisCord(curDepth);

		vector<nPoint *> lessOrEqual(points.begin(), points.begin() + 1);
		vector<nPoint *> greater(points.begin() + 1, points.end());

		treeNode * left = new treeNode;
		left->makeTree(lessOrEqual, curDepth + 1, totalDepth, axisMode);

		treeNode * right = new treeNode;
		right->makeTree(greater, curDepth + 1, totalDepth, axisMode);

		this->axis = curDepth;
		this->val = median;
		this->left = left;
		this->right = right;
		this->point = nullptr;

		return this;
	}

	//for size greater than 2
	if (axisMode == 0) { //simple rotation
		curDepth = dIndex % totalDepth;
	} else { //by range
		curDepth = findLargestRange(points, totalDepth);
	}

	int middle = (points.size() / 2);

	nth_element(points.begin(), points.begin() + middle, points.end(),
			Sort<nPoint *>(curDepth));

	double median = points.at(middle)->getAxisCord(curDepth);

	vector<nPoint *> lessOrEqual(points.begin(), points.begin() + middle + 1);
	vector<nPoint *> greater(points.begin() + middle + 1, points.end());

	treeNode * left = new treeNode;
	left->makeTree(lessOrEqual, curDepth + 1, totalDepth, axisMode);
	treeNode * right = new treeNode;
	right->makeTree(greater, curDepth + 1, totalDepth, axisMode);

	this->axis = curDepth;
	this->val = median;
	this->left = left;
	this->right = right;
	this->point = nullptr;

	return this;
}

//recursive function to read in input from file to reconstruct tree
treeNode * treeNode::recurIn(stringstream& stream, int totalNodes,
		const int totalDim) {
	string cell;

	totalNodes++;

	stream.precision(dbl::max_digits10); //max precision to read in file

	bool leaf;
	int axis;
	const char* ch;
	double val;
	nPoint * point;
	int index;
	int dims;
	double * cords;
	treeNode * left;
	treeNode * right;

	getline(stream, cell, ','); //axis
	axis = stoi(cell);
	getline(stream, cell, ','); //val
	ch = cell.c_str();
	val = atof(ch);
	getline(stream, cell, ','); //val

	if (cell == "NOT_LEAF") {
		leaf = false;
	} else {
		leaf = true;
	}
	if (leaf) {

		index = stoi(cell); //index
		getline(stream, cell, ','); //dims

		dims = stoi(cell);
		cords = new double[totalDim];
		for (int i = 0; i < totalDim; i++) {
			getline(stream, cell, ','); //val
			ch = cell.c_str();
			cords[i] = atof(ch);
		}

		point = new nPoint(index, dims, cords);
		left = nullptr;
		right = nullptr;
		getline(stream, cell, ','); //left null
		getline(stream, cell, ','); //right null

	} else {
		point = nullptr;
		left = new treeNode;
		right = new treeNode;
		left->recurIn(stream, totalNodes, totalDim);
		right->recurIn(stream, totalNodes, totalDim);
	}
	this->axis = axis;
	this->val = val;
	this->left = left;
	this->right = right;
	this->point = point;

	return this;
}

//base function to read file to reconstruct tree, calls helper recursively and upon completion returns root of new tree
treeNode* treeNode::readTree(const string fileName, const int k) {
	std::string line;

	ifstream myfile(fileName);
	if (myfile.is_open()) {
		string cell;

		while (getline(myfile, line)) { //should be just one line
			stringstream lineStream(line);
			this->recurIn(lineStream, 0, k);
		}

		myfile.close();

		return this;

	} else {
		cout << "Unable to open tree data file to rebuild tree from disk\n";
		return nullptr;
	}
}

//helper functions

//returns the number of dimensions by determining how many commas are in csv
int countDims(string const fileName) {
	ifstream myfile(fileName);
	string line;
	if (myfile.is_open()) {
		if (getline(myfile, line)) { //get every point
			size_t n = std::count(line.begin(), line.end(), ',');
			return n + 1; //last point will have comma afterwards
		}
		cout << "Could not count commas, file was empty";
		return -1;
	} else {
		cout << "Could not count commas, file failed to open\n";
	}
	return -1;
}

//compares contents of two files and returns if they are identical
//based on the code published at http://stackoverflow.com/questions/6163611/compare-two-files
//by username Christoph
bool compareFile(FILE* file_compared, FILE* file_checked) {
	bool diff = 0;
	int N = 65536;
	char* b1 = (char*) calloc(1, N + 1);
	char* b2 = (char*) calloc(1, N + 1);
	size_t s1, s2;

	do {
		s1 = fread(b1, 1, N, file_compared);
		s2 = fread(b2, 1, N, file_checked);

		if (s1 != s2 || memcmp(b1, b2, s1)) {
			diff = 1;
			break;
		}
	} while (!feof(file_compared) || !feof(file_checked));

	free(b1);
	free(b2);

	if (diff)
		return false;
	else
		return true;
}

//read in data from csv and create vector of points of n dimensions
//returns number of dimensions normally, or -1 if unsucessful
int getDataFile(string const fileName, vector<nPoint*>& pointVector) {
	std::string line;
	double * pCords;

	int k = 0;
	int rows = 0;

	ifstream myfile(fileName);
	if (myfile.is_open()) {
		string cell;
		const char* ch;
		double val;

		while (getline(myfile, line)) { //get every point

			if (k == 0) { // first time determine width, initializes sums
				k = countDims(fileName);
			}
			pCords = new double[k];

			stringstream lineStream(line);

			for (int i = 0; i < k; i++) {
				if (i < k - 1)	//get every value except last, stop at comma
					getline(lineStream, cell, ',');
				else
					getline(lineStream, cell);

				ch = cell.c_str();
				val = atof(ch);
				pCords[i] = val;
			}

			pointVector.push_back(new nPoint(rows, k, pCords));
			rows++;
		}

		myfile.close();
		return k; //return number of dimensions

	} else {
		cout << "Unable to open points file\n";
		return -1;
	}
}

//used to determine the dimension with largest range, used for tree building
//returns int corresponding to dimension with largest range
int findLargestRange(vector<nPoint*> const points, int const totalDepth) {

	double maxDif = 0;
	double curDif = 0;
	nPoint * big;
	nPoint * small;
	int retVal = -1;

	for (int i = 0; i < totalDepth; i++) {

		big = *max_element(points.begin(), points.end(), Sort<nPoint *>(i));
		small = *min_element(points.begin(), points.end(), Sort<nPoint *>(i));
		curDif = (big->getAxisCord(i)) - (small->getAxisCord(i));

		if (curDif > maxDif) {
			maxDif = curDif;
			retVal = i;
		}
	}
	return retVal;
}

