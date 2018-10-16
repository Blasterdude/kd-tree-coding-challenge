KD-Tree Coding Exercise
Thomas J. Meehan
tmeehan@wpi.edu
11/14/16 

My submission consists of two library files (kdTree.h and kdTree.cpp) and 3 source files that utilize them. When compiled they generate the following 3 executables:

build_kdtree handles building a kdtree by first reading in a list of points from a .csv file, building the tree, and then writing the structure to a location specified by the user to be later utilized by other programs (such as query_kdtree). Once compiled you use it like so:

./build_kdtree source destination axisMode

source specifies the .csv file from which the data to build the tree is parsed (defaults to sample_data.csv). Destination specifies the location where the tree is saved on disk (it defaults to treeOut.txt in the local directory, and if the specified file does not yet exist it will be created accordingly). Finally, axisMode specifies which heuristic to use when choosing which axis to split on when constructing the tree. Specifying 0 will tell the program to simply cycle through all valid axes while any other value will invoke the (default) behavior of splitting on the axis with the largest range.


query_kdtree first reads in the .csv file containing points that are used to search the kdtree. It then reads in a file containing a kdtree saved to disk by a previous program and then reconstructs the tree in memory. Once the tree is reconstructed it then iterates though the list of query points, and for each point it searches the tree for the exact nearest neighbor, printing the results to both standard output and a user specified file with the index of the node and euclidian distance between it and the query point. Once compiled you can use it like so:

./query_tree treeFile input output

treeFile specifies the path to the file containing a previously constructed tree saved to disk. It defaults to build_kdtree’s default output, “treeOut.txt”. Input specifies the .csv file containing all the nodes to be queried later. It defaults to query_data.csv Finally, output specifies where the program outputs the nearest neighbor and smallest euclidian distance info for each query point, defaulting to results.txt


Finally we have the “tests” executable. tests.cpp simply contains a bunch of unit and integration tests to ensure that functions behave as expected. It was difficult to define “correct” behavior for some of these functions to compare against, but tricks such as brute forcing the correct nearest neighbors and repeatedly reading and writing to test the input and output functions for reading and writing trees to disk and then performing operations on those new trees helped to ensure that functions were consistent and correct. Once compiled you can use it like this:

./tests input queries axisMode 

input is the .csv file containing the points needed to construct a kdtree (defaults to sample_data.csv) queries is a .csv file that corresponds to the list of query points used to search the tree (defaults to query_data.csv) and axisMode is identical to how it works in build_kdtree in that 0 specifies a rotating choice of axis and any other value results in the default behavior of using range to choose the axis instead. Note that the user is given the option (through modifying simple parameters such as checkFile and overWriteAnswers in the source file tests.cpp) to generate and/or check against text files for the purpose of comparing test output to a text file containing expected “correct” output for a given dataset. Sample comparison files have been included for your convenience. 

In terms of compiling, a makefile is included so all you should have to do is invoke “make” inside the directory (tested on Ubuntu 14.04). All files are compiled with G++ and the c++11 -std=c++11 flag. Standard libraries are utilized but no external libraries should be necessary to compile. If any issues arise refer to the makefile.


All code contained within is entirely my own work with the exception of a singular helper function, “compareFile”, found in kdTree.h and kdTree.cpp

The code for that function is a modified version of the code published at http://stackoverflow.com/questions/6163611/compare-two-files written by username Christoph

All code contained within in licensed under the GNU General Public License 3.0, a copy of which has been included within “license.txt”.

