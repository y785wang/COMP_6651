#ifndef T_SIMPLE_MATRIX_H
#define T_SIMPLE_MATRIX_H

#include <vector>

template<class T>
class SMatrix {
  private:
    
    // Storage
    std::vector<T> v;

    int N;
    
  public:

    SMatrix<T>(int M){v.resize(M*M);N = M;}
    SMatrix<T>(){N = 0;}

    void resize(int M){
        v.resize(M*M);
        N = M;
    }
    
    int size(){
        return N;
    }


    // access matrix(r, c) (row and column)  for R/W operations
    T& operator() (int r, int c){
        return  v[N*r+c];
    }
    const T& operator()(int r, int c) const{
        return  v[N*r+c];
    }

};

#endif // 
