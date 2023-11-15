#pragma once
#include <vector>

// TMA - tridiagonal matrix algorithm
// 
//a1 - вектор под главной диагональю
//a2 - главная диагональ
// a3 - над главной диагональю
// b - правая часть системы уравнений
template <typename T>
std::vector<T> TMA(const std::vector<T>& a1, const std::vector<T>& a2,
    const std::vector<T>& a3,const std::vector<T>& b) {
        size_t n = size(b);
        std::vector<T> alpha(n);//измененная главная диагональ
        std::vector<T> b_new(n);// измененный вектор b
        std::vector<T> v(n);//решение

        //зануляем вектор под главной диагональю 
        alpha[0] = a2[0];
        b_new[0] = b[0];
        for (size_t i = 1; i < n; i++) {
            alpha[i] = a2[i] - a1[i - 1] * a3[i - 1] / alpha[i - 1];
            b_new[i] = b[i] - b_new[i - 1] * a1[i - 1] / alpha[i - 1];
        }
        //зануляем вектор над главной диагональю
        v[n - 1] = b_new[n - 1] / alpha[n - 1];
        for (size_t i = n - 2; i + 1 > 0; i--) {
            v[i] = 1.0 / alpha[i] * (b_new[i] - v[i + 1] * a3[i]);
        }

        return v;
}



