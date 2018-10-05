#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

bool DEBUG = false;



/*******************************
 
 reverseString:
 
 reverse a string. e.g.: abc -> cba
 
 1) string: input string
 
 return string: reversed string
 
 ******************************/
string reverseString(string s) {
    int i = 0, j = s.size() - 1;
    while(i < j){
        swap(s[i], s[j]);
        ++i;
        --j;
    }
    return s;
}




/**************************************************************
 
 finAllCycle:
 
 find all hamiltonian cycles
 
 1）int[]: vertices array (for back tracking usage)
 2) itn: length or vertices array
 3) string: fo in-process path/cycle generation purpose
 
 return string: all hamiltonian cycles separeted by new line
                character
 
 *************************************************************/
string findAllCycle (int vertices[], int length, string path) {
    ostringstream allCycles;
    bool empty = true;
    for (int i = 0; i < length; ++i) {
        if (vertices[i] != 0) {
            empty = false;
            // update first recursion variables
            int vertices_copy[length];
            for (int j = 0; j < length; ++j) {
                vertices_copy[j] = vertices[j];
            }
            vertices_copy[i] = 0;
            
            // update last recursion variables
            ostringstream os;
            os << i;
            string newPath = path + os.str();
            
            // recursion call
            string oneCycle = findAllCycle (vertices_copy, length, newPath);
            allCycles << oneCycle;
        }
    }
    
    if (empty) {
        allCycles << path;
        return path + "0" + "\n";
    } else {
        return allCycles.str();
    }
}




/***********************************
 
 main:
 
 1) int: # of command arguments
 2) char**: command arguments
 
 return int：if program exit
             correctly or not
 
 **********************************/
int main ( int argc, char** argv ) {
    
    // command line arguments check
    if(argc != 3){
        cout << "Usage: ../a.out input_file output_file" << endl;
        return -1;
    }
    ifstream inputFile(argv[1]);
    if(! inputFile){
        cout<<"Cannot load input file"<<endl;
        return -1;
    }
    ofstream outputFile(argv[2]);
    
    // process input file
    int numOfDataset;
    inputFile >> numOfDataset;
    for (int i = 0; i < numOfDataset; ++i) {
        if (DEBUG) cout << "\n~~~~~~~~~~~~~~~~~~\n\n" << "DATASET " << i+1 << endl << endl;
        int numOfVertices;
        int numOfEdges;
        inputFile >> numOfVertices >> numOfEdges;
        int weightMatrix[numOfVertices][numOfVertices];
        
        // store weight in matrix
        for (int j = 0; j < numOfEdges; ++j) {
            int vertexA;
            int vertexB;
            int weightAB;
            inputFile >> vertexA >> vertexB >> weightAB;
            weightMatrix[vertexA][vertexB] = weightAB;
            weightMatrix[vertexB][vertexA] = weightAB;
        }

        if (DEBUG) {
            cout << "edge weight matrix" << endl;
            for (int rows = 0; rows < numOfVertices; ++rows) {
                for (int cols = 0; cols < numOfVertices; ++cols) {
                    if (rows == cols) {
                        cout << "0 ";
                    } else {
                        cout << weightMatrix[rows][cols] << " ";
                    }
                }
                cout << endl;
            }
        }
        
//        if (DEBUG) numOfVertices = 4;
        
        // find all cycles
        int vertices[numOfVertices];
        vertices[0] = 0;
        for (int j = 1; j < numOfVertices; ++j) {
            vertices[j] = 1;
        }
        string path = "0";
        string result;
        stringstream allCycles(findAllCycle (vertices, numOfVertices, path));
        if (DEBUG) cout << "\nduplicated(weight) cycles" << endl << allCycles.str();
        
        // remove duplicate(weight) cycles, s.t. 0->1->2->0 equals 0->2->1->0, weight are same
        vector<string> uniqueCycles;
        int numOfCycles = 1;
        for (int i = 1; i < numOfVertices; ++i) {
            numOfCycles *= i;
        }
        for (int i = 0; i < numOfCycles; ++i) {
            string cycle;
            allCycles >> cycle;
            bool duplicated = false;
            for (int j = 0; j < uniqueCycles.size(); ++j) {
                if (uniqueCycles[j].compare(cycle) == 0 ||
                    uniqueCycles[j].compare(reverseString(cycle)) == 0) {
                    duplicated = true;
                    break;
                }
            }
            if (! duplicated) {
                uniqueCycles.push_back(cycle);
            }
        }
        if (DEBUG) {
            cout << "\nunique(weight) cycles" << endl;
            for (int i = 0; i < uniqueCycles.size(); ++i) {
                cout << uniqueCycles[i] << endl;
            }
        }
        
        // calculate all lengths of unique(weight) Hamiltonian cycles
        int minSumWeight = -1;
        string resultCycle;
        for (int i = 0; i < uniqueCycles.size(); ++i) {
            int sumWeight = 0;
            string cycle = uniqueCycles[i];
            for (int i = 0; i < cycle.length()-1; ++i) {
                stringstream ssA;
                stringstream ssB;
                int vertexA;
                int vertexB;
                ssA << cycle.substr(i, 1);
                ssA >> vertexA;
                ssB << cycle.substr(i+1, 1);
                ssB >> vertexB;
                sumWeight += weightMatrix[vertexA][vertexB];
            }
            if (sumWeight < minSumWeight || minSumWeight == -1) {
                minSumWeight = sumWeight;
                resultCycle = cycle;
            }
        }
        if (DEBUG) cout << "\nminSumWeight cycle is " << resultCycle << endl;
        if (DEBUG) cout << "minSumWeight is " << minSumWeight << endl << endl;
        
        // write result to output file
        outputFile << minSumWeight<< endl;
        
    }
    
    if (DEBUG) cout << "~~~~~~~~~~~~~~~~~~\n\n";
    
    return 0;
}










