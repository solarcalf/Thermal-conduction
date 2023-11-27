#pragma once

#include <functional>
#include <vector>
#include <cmath>
#include "TMA.h"

#include <iostream>
#define TEST_PROBLEM

class thermal {
private:

	using FP = double;
	using Vec_FP = std::vector<FP>;
	using VVec_FP = std::vector<Vec_FP>;
	using FP_Pair = std::pair<FP, FP>;
	using F_Pair = std::pair<std::function<FP(FP)>, std::function<FP(FP)>>;

	size_t N;
	FP x0, xn;
	FP mu1, mu2;
	FP h;
	FP xi;
	F_Pair k, q, f; 

private:

	VVec_FP get_system_matrix() {
		x_grid[0] = x0;

		Vec_FP a = Vec_FP(N);
		Vec_FP d = Vec_FP(N - 1);
		Vec_FP phi = Vec_FP(N);

		for (size_t i = 1; i < N; i++) {
			x_grid[i] = x_grid[i - 1] + h;
			a[i - 1] = get_a(i);	// смещение индексов происходит из-за того, что мы используем 0-индексацию
			d[i - 1] = get_d(i);
			phi[i - 1] = get_phi(i);
		}

		x_grid[N] = x_grid[N - 1] + h;
		a[N - 1] = get_a(N);
		phi[0] = mu1;
		phi[N - 1] = mu2;

		//======================= DEBUG ========================//

		//for (size_t i = 0; i < N - 1; i++)
		//	std::cout << a[i] << ' ' << phi[i] << ' ' << d[i] << '\n';
		//std::cout << a[N - 1] << ' ' << phi[N - 1] << '\n';
		
		//======================================================//

		Vec_FP A = Vec_FP(N - 1);
		Vec_FP B = Vec_FP(N - 1);
		Vec_FP C = Vec_FP(N);

		A[N - 2] = 0;
		B[0] = 0;
		C[0] = 1;
		C[N - 1] = 1;

		for (size_t i = 0; i < N - 2; i++) {
			A[i] = a[i] / pow(h, 2);
			B[i + 1] = a[i + 1] / pow(h, 2);
			C[i + 1] = -(a[i] + a[i + 1]) / pow(h, 2) - d[i];
		}

		return { A, B, C, phi};
	}

	inline void set_numeric_solution(const Vec_FP& A, const Vec_FP& C, const Vec_FP& B, const Vec_FP& phi) {
		
		//======================= DEBUG ========================//

		//for (size_t i = 0; i < N - 2; i++)
		//	std::cout << A[i] << ' ' << C[i + 1] << ' ' << B[i + 1] << '\n';
		//
		//std::cout << C[0] << ' ' << B[0] << '\n';
		//std::cout << C[N - 1] << ' ' << A[N - 2] << '\n';

		//======================================================//
		
		v_vals = TMA(A, C, B, phi);
	}

private:

	inline FP get_a(size_t i) {
		FP x = x_grid[i];
		FP xl = x_grid[i] - h / 2;

		if (xi >= x)
			return k.first(xl);

		else if (x - h < xi && xi < x)
			return h / ((xi - (x - h)) / k.first(((x - h) + xi) / 2) + (x - xi) / k.second((xi + x) / 2));

		else
			return k.second(xl);
	}

	inline FP get_d(size_t i) {
		FP x = x_grid[i];
		FP xl = x_grid[i] - h / 2;
		FP xr = x_grid[i] + h / 2;

		if (xi >= x + h / 2)
			return q.first(x);

		else if (x - h / 2 < xi && xi < x + h / 2)
			return (q.first((xl + xi) / 2) * (xi - xl) + q.second((xi + xr) / 2) * (xr - xi)) / h;

		else
			return q.second(x);
	}

	inline FP get_phi(size_t i) {
		FP x = x_grid[i];
		FP xl = x_grid[i] - h / 2;
		FP xr = x_grid[i] + h / 2;

		if (xi >= x + h / 2)
			return f.first(x);

		else if (x - h / 2 < xi && xi < x + h / 2) 
			return (f.first((xl + xi) / 2) * (xi - xl) + f.second((xi + xr) / 2) * (xr - xi)) / h;
		
		else
			return f.second(x);
	}


public:
	
	Vec_FP x_grid;
	Vec_FP v_vals;

#ifdef TEST_PROBLEM
	Vec_FP u_vals;
#endif

	thermal(FP x0, FP xn, size_t N, F_Pair k, F_Pair q, F_Pair f, FP mu1, FP mu2, FP xi) : 
		x0(x0), xn(xn), N(N), k(k), q(q), f(f), mu1(mu1), mu2(mu2), xi(xi) {

		x_grid = Vec_FP(N + 1);
		h = (xn - x0) / N;

		VVec_FP system_matrix = get_system_matrix();

		Vec_FP A = std::move(system_matrix[0]);
		Vec_FP B = std::move(system_matrix[1]);
		Vec_FP C = std::move(system_matrix[2]);
		Vec_FP phi = std::move(system_matrix[3]);

		set_numeric_solution(A, C, B, phi);

#ifdef TEST_PROBLEM
		set_real_solution();
#endif 

	}

#ifdef TEST_PROBLEM
private:

	void set_real_solution() {
		FP C1 = get_C1();
		FP C2 = get_C2();
		FP C3 = get_C3();
		FP C4 = get_C4();

		u_vals.push_back(mu1);

		for (FP x_i = x0 + h; x_i < xn; x_i += h) {

			if (x_i <= xi)
				u_vals.push_back(C1 * exp(sqrt(2) * x_i) + C2 * exp(-sqrt(2) * x_i) + 1);

			else
				u_vals.push_back(C3 * exp(x_i / (2 * sqrt(5))) + C4 * exp(-x_i / (2 * sqrt(5))) + 40);
		}

		u_vals.push_back(mu2);
	}

private:

	FP get_C1() {
		return -1 - get_C2();
	}

	FP get_C2() {
		return (get_C4() * (exp(-1 / (8 * sqrt(5))) - exp(-7 / (8 * sqrt(5)))) - 39 * exp(-3 / (8 * sqrt(5))) + 39 + exp(sqrt(2) / 4)) / (exp(-sqrt(2) / 4) - exp(sqrt(2) / 4));
	}

	FP get_C3() {
		return -39 * exp(-1 / (2 * sqrt(5))) - get_C4() * exp(-1 / sqrt(5));
	}

	FP get_C4() {
		FP coef = (-sqrt(2) * (exp(-1 / (8 * sqrt(5))) - exp(-7 / (8 * sqrt(5)))) / (exp(2) / 2 - 1) + exp(-7 / (8 * sqrt(5))) / (2 * sqrt(5)) + exp(-1 / (8 * sqrt(5))) / (2 * sqrt(5)));
		return (-39 / (2 * sqrt(5)) * exp(-3 / (8 * sqrt(5))) + sqrt(2) * exp(sqrt(2) / 4) + sqrt(2) * ((-39 * exp(-3 / (8 * sqrt(5))) + 39 + exp(sqrt(2) / 4)) / (exp(2) / 2 - 1))) / coef;
	}

#endif

};
