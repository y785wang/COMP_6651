#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

#define TEST 0 // SELF-TEST purpose




/***********************
 
 toString():
 
 Convert input integer
 to string
 
 1) int: an integer
 
 **********************/
string toString(int n) {
    stringstream ss;
    ss << n;
    return ss.str();
}




/**********************************************************************************************
 
 checkExist():
 
 Check if an integer exists in a vector
 
 1) int: an element
 2) vector<int>: an array of elements
 
 *********************************************************************************************/
bool checkExist(int element, vector<int> elements) {
    for (int i = 0; i < elements.size(); ++i) {
        if (element == elements[i]) {
            return true;
        }
    }
    return false;
}




/*****************************************************************************************************
 
 checkCombination():
 
 Check if an integer N can be directly combined by some of the elements, s.t. 2 9 -> 299. If N can be
 directly combined, then update the optimal sequence array.
 
 1) int: integer N
 2) vector<int>: all elements
 3) vector<int>: all length of the optimal sequence for each N.
 4）vector<string>: SELF-TEST purpose
 
 ****************************************************************************************************/
void checkCombination(int N, vector<int> &elements, vector<int> &minCharLength, vector<int> &newElements,
                      vector<string> &exp) {
    int intLength = 0;
    int tempN = N;
    while (tempN) {
        int lastDigit = tempN % 10;
        if (checkExist(lastDigit, elements)) {
            ++intLength;
            tempN /= 10;
        } else {
            return;
        }
    }
    minCharLength[N-1] = intLength;
    newElements.push_back(N);
    if (TEST) exp[N-1] = toString(N); // SELF-TEST
}




/********************************************************************************************************
 
 checkMultiplication():
 
 Check if an integer N can be obtained from multiplication by using some of the elements, s.t. 2 9 -> 18;
 2 9 -> 81. If N can be obtained this way, then update the optimal sequence array.
 1) int: integer N
 2) vector<int>: all elements
 3) vector<int>: all length of the optimal sequence for each N.
 4）vector<string>: SELF-TEST purpose
 
 *******************************************************************************************************/
void checkMultiplication(int N, vector<int> &elements, vector<int> &minCharLength, vector<int> &newElements,
                         vector<string> &exp) {
    for (int i = 0; i < elements.size(); ++i) {
        if (0 == minCharLength[N-1]) { // not directly-combined case
            if (elements[i] != 0 && N >= elements[i]  && N % elements[i] == 0) {
                int quotient = N / elements[i];
                int prevLength = minCharLength[quotient-1];
                if (prevLength) {
                    minCharLength[N-1] = prevLength + 1 + toString(elements[i]).length();
                    newElements.push_back(N);
                    if (TEST) { // SELF-TEST
                        exp[N-1] = exp[quotient - 1];
                        exp[N-1] += "*";
                        exp[N-1] += toString(elements[i]);
                    }
                }
            }
        }
    }
}




/********************************************************************************************************
 
 checkSummation():
 
 Check if an integer N can be obtained from summation by using some of the elements, s.t. 2 9 -> 11.
 If N can be obtained this way, then update the optimal sequence array.
 
 1) int: integer N
 2) vector<int>: all elements
 3) vector<int>: all length of the optimal sequence for each N.
 4）vector<string>: SELF-TEST purpose
 
 *******************************************************************************************************/
void checkSummation(int N, vector<int> &elements, vector<int> &minCharLength, int numOldElements,
                    vector<string> &exp) {
    for (int i = 0; i < elements.size(); ++i) {
        int diff = N - elements[i];
        if (diff == 0 && elements[i] != 0 && i < numOldElements) { // N equals one of the origianl elements
            minCharLength[N-1] = toString(elements[i]).length();
            if (TEST) exp[N-1] = toString(elements[i]);
        } else if (diff > 0 && elements[i] != 0) {
            int prevLength = minCharLength[N - elements[i] - 1];
            if (0 != prevLength) { // if previous one cannot be formed, than current one cannot either
                int newLength = prevLength + 1 + minCharLength[elements[i] - 1];
                if (0 == minCharLength[N-1] || newLength < minCharLength[N - 1]) {
                    minCharLength[N-1] = newLength;
                    if (TEST) { // SELF-TEST
                        exp[N-1] = exp[N - 1 - elements[i]];
                        exp[N-1] += "+";
                        exp[N-1] += exp[elements[i] - 1];
                    }
                }
            }
        }
    }
}





/****************************************************************
 
 expressionPuzzle():
 
 follows the E2.pdf requirements
 
 1)char*: input file name
 2)char*: output file name
 
 ****************************************************************/
void expressionPuzzle(char* inputFileName, char* outputFileName) {
    // check input file and set output file
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "ERROR: Input file does not exist." << endl;
        return;
    }
    ofstream outputFile(outputFileName);

    int datasets; // total number of datasets
    inputFile >> datasets;
    for (int i = 0; i < datasets; ++i) { // for each dataset
        // reading elements and N, store them
        int numElements;
        inputFile >> numElements;
        vector<int> elements = vector<int>(numElements);
        for (int j = 0; j < numElements; ++j) {
            int element;
            inputFile >> element;
            elements[j] = element;
        }
        int N;
        inputFile >> N;
        
        // all length of optimal sequence for each N
        vector<int> minCharLength = vector<int>(N);
        
        // store new elements which are generated by direct-combinaion method and element-multiplication
        // e.g.: if 4*5=20, than 4*5 could be use for further summation as entirety
        // e.g.: if input is 2 2 9 229, then 29, 92, 99 etc. could be use for further summation as entirety
        vector<int> newElements = vector<int>();
        
        // SELF-TEST purpose, store expressions for each step
        vector<string> exp = vector<string>(N);
        
        if (N == 0) { // Special case: N = 0
            if (checkExist(0, elements)) {
                outputFile << 1 << endl;
            } else {
                outputFile << "N" << endl;
            }
        } else { // N != 0
            for (int j = 1; j <= N; ++j) { // pre setup
                checkCombination(j, elements, minCharLength, newElements, exp);
                checkMultiplication(j, elements, minCharLength, newElements, exp);
            }
            for (int j = 0; j < newElements.size(); ++j) { // update elements
                elements.push_back(newElements[j]);
            }
            for (int j = 1; j <= N; ++j) { // final calculation
                checkSummation(j, elements, minCharLength, numElements, exp);
            }
            
            // output result
            if (minCharLength[N-1]) {
                outputFile << minCharLength[N-1] << endl;
            } else {
                outputFile << "N" << endl;
            }
        }
        
        
        if (TEST) { // SELF-TEST
            for (int j = 1; j <= N; ++j) {
                cout << j << " = " << exp[j-1] << endl;
            }
            if (N != 0) {
                cout << "Min length = " << minCharLength[N-1] << endl;
            } else if (N == 0 && checkExist(0, elements)) {
                cout << "Min length = 1" << endl;
            } else {
                cout << "Min length = 0" << endl;
            }
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        }
    }
}




/*********************************
 
 main():
 
 1) input file name
 2) output file name
 
 ********************************/
int main(int argc, char* argv[]) {
    // check the number of command line arguments
    if (3 != argc) {
        cerr << "ERROR: Accept two arguments which are input/output file names." << endl;
        return -1;
    }
    
    expressionPuzzle(argv[1], argv[2]);
    return 0;
}





