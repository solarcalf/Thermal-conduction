#pragma once
#include <vector>

                                    /*
TMA - tridiagonal matrix algorithm  
                                     
A - vector under the main diagonal  
C - main diagonal
B - vector over the main diagonal
b - the right part of the system
                                    */


template <typename T>
std::vector<T> TMA(const std::vector<T>& A, const std::vector<T>& C,
    const std::vector<T>& B, const std::vector<T>& b) {

    size_t n = size(b);

    std::vector<T> alpha(n);    //modified main diagonal
    std::vector<T> b_new(n);    //modified vector b
    std::vector<T> solution(n);
    
    alpha[0] = C[0];    //remove vector under the main diagonal
    b_new[0] = b[0];

    for (size_t i = 1; i < n; i++) {
        alpha[i] = C[i] - A[i - 1] * B[i - 1] / alpha[i - 1];
        b_new[i] = b[i] - b_new[i - 1] * A[i - 1] / alpha[i - 1];
    }

    solution[n - 1] = b_new[n - 1] / alpha[n - 1];  //remove vector over the main diagonal

    for (size_t i = n - 2; i + 1 > 0; i--) 
        solution[i] = 1.0 / alpha[i] * (b_new[i] - solution[i + 1] * B[i]);
    
    return solution;
}
