#include <iostream>
#include <fstream>
#include "simple_matrix.hpp"

using namespace std;


// only for self-tsest purpose
void outputMatrix(SMatrix<float> A) {
    int N = A.size();
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << A(i, j) << " ";
        }
        cout << endl;
    }
}


SMatrix<float> matrixAddition(SMatrix<float> A, SMatrix<float> B) {
    int N = A.size(); // get matrix dimension
    SMatrix<float> M(N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            M(i,j) = A(i,j) + B(i,j);
        }
    }

    /*
    cout << "check addition " << endl;
    outputMatrix(M);
    */

    return M;
}


SMatrix<float> matrixSubtraction(SMatrix<float> A, SMatrix<float> B) {
    int N = A.size(); // get matrix dimension
    SMatrix<float> M(N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            M(i, j) = A(i, j) - B(i, j);
        }
    }

    return M;
}


SMatrix<float> matrixMultiplication(SMatrix<float> A, SMatrix<float> B) {
    int N = A.size(); // get matrix dimension
    int halfN = N / 2; // get divided matrix dimension
    SMatrix<float> C(N);

    // base case
    if(1 == N){
        C(0,0) = A(0,0) * B(0,0);
    } else { // apply Strassen's Method
        // generate A11 ~ A22, B11 ~ B22
        SMatrix<float> A11(halfN), A12(halfN), A21(halfN), A22(halfN);
        SMatrix<float> B11(halfN), B12(halfN), B21(halfN), B22(halfN);
        for (int i = 0; i < halfN; ++i) {
            for(int j = 0; j < halfN; ++j) {
                A11(i, j) = A(i, j);
                B11(i, j) = B(i, j);
            }
        }
        for (int i = 0; i < halfN; ++i) {
            for(int j = 0; j < halfN; ++j) {
                A12(i, j) = A(i, j + halfN);
                B12(i, j) = B(i, j + halfN);
            }
        }
        for (int i = 0; i < halfN; ++i) {
            for(int j = 0; j < halfN; ++j) {
                A21(i, j) = A(i + halfN, j);
                B21(i, j) = B(i + halfN, j);
            }
        }
        for (int i = 0; i < halfN; ++i) {
            for(int j = 0; j < halfN; ++j) {
                A22(i, j) = A(i + halfN, j + halfN);
                B22(i, j) = B(i + halfN, j + halfN);
            }
        }

        
        cout << "check A11 ~ A22, B11 ~ B22" << endl;
        outputMatrix(A11);
        outputMatrix(A12);
        outputMatrix(A21);
        outputMatrix(A22);
        outputMatrix(B11);
        outputMatrix(B12);
        outputMatrix(B21);
        outputMatrix(B22);
        

        // generate M1 ~ M7
        SMatrix<float> M1(halfN), M2(halfN), M3(halfN), M4(halfN), M5(halfN), M6(halfN), M7(halfN);
        M1 = matrixMultiplication(matrixAddition(A11, A22), matrixAddition(B11, B22));
        M2 = matrixMultiplication(matrixAddition(A21, A22), B11);
        M3 = matrixMultiplication(A11, matrixSubtraction(B12, B22));
        M4 = matrixMultiplication(A22, matrixSubtraction(B21, B11));
        M5 = matrixMultiplication(matrixAddition(A11, A12), B22);
        M6 = matrixMultiplication(matrixSubtraction(A21, A11), matrixAddition(B11, B12));
        M7 = matrixMultiplication(matrixSubtraction(A12, A22), matrixAddition(B21, B22));

        
        cout << "check M1 ~ M7" << endl;
        outputMatrix(M1);
        outputMatrix(M2);
        outputMatrix(M3);
        outputMatrix(M4);
        outputMatrix(M5);
        outputMatrix(M6);
        outputMatrix(M7);
        

        // generate C11~C22
        SMatrix<float> C11(halfN), C12(halfN), C21(halfN), C22(halfN);
        C11 = matrixAddition(matrixSubtraction(matrixAddition(M1, M4), M5), M7);
        C12 = matrixAddition(M3, M5);
        C21 = matrixAddition(M2, M4);
        C22 = matrixAddition(matrixAddition(matrixSubtraction(M1, M2), M3), M6);

        float a = matrixSubtraction(matrixAddition(M1, M4), M5)(0, 0);
        float b = M7(0, 0);
        cout << "check C11 ~ C22 " << a << " " << b << " " << a + b << endl;
        outputMatrix(C11);
        outputMatrix(C12);
        outputMatrix(C21);
        outputMatrix(C22);

        // set C11~C22 back to C
        for (int i = 0; i < halfN; ++i) {
            for(int j = 0; j < halfN; ++j) {
                C(i, j) = C11(i, j);
            }
        }
        for (int i = 0; i < halfN; ++i) {
            for(int j = 0; j < halfN; ++j) {
                C(i, j + halfN) = C12(i, j);
            }
        }
        for (int i = 0; i < halfN; ++i) {
            for(int j = 0; j < halfN; ++j) {
                C(i + halfN, j) = C21(i, j);
            }
        }
        for (int i = 0; i < halfN; ++i) {
            for(int j = 0; j < halfN; ++j) {
                C(i + halfN, j + halfN) = C22(i, j);
            }
        }
    }
    
    return C;
}


int main(int argc, char* argv[]) {

    // check the number of command line arguments
    if (3 != argc) {
        cerr << "ERROR: Accept two arguments which are input/output file names." << endl;
        return -1;
    }

    // check input file and set output file
    ifstream inputFile(argv[1]);
    if (!inputFile) {
        cerr << "ERROR: Input file does not exist." << endl;
        return -1;
    }
    ofstream outputFile(argv[2]);

    // read input file
    int dataSets; // total number of dataSets
    inputFile >> dataSets; 
    for (int i = 0; i < dataSets; ++i) { // read values for each dataSets
        int N; // matrix dimension
        inputFile >> N;
        SMatrix<float> A (N), B(N), C(N);
        for (int j = 0; j < N; ++j) { // store value in matrix A
            for (int k = 0; k < N; ++k) {
                inputFile >> A(j,k);
            }
        }
        for (int j = 0; j < N; ++j) { // store value in matrix B
            for (int k = 0; k < N; ++k) {
                inputFile >> B(j,k);
            }
        }

        C = matrixMultiplication(A, B); // use Strassen's Method

        // only for self-tsest purpose
         outputMatrix(C);

        // transfer matrix C to output file
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                outputFile << C(i,j)<<" ";
            }
        }
        outputFile << endl;
    }

    return 0;
}
