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
		Vec_FP phi = Vec_FP(N + 1);

		for (size_t i = 1; i < N; i++) {
			x_grid[i] = x_grid[i - 1] + h;
			a[i - 1] = get_a(i);	
			d[i - 1] = get_d(i);
			phi[i] = get_phi(i);
		}

		x_grid[N] = x_grid[N - 1] + h;
		a[N - 1] = get_a(N);
		phi[0] = mu1;
		phi[N] = mu2;

		Vec_FP A = Vec_FP(N);
		Vec_FP B = Vec_FP(N);
		Vec_FP C = Vec_FP(N + 1);

		A[N - 1] = 0;
		B[0] = 0;
		C[0] = 1;
		C[N] = 1;

		for (size_t i = 0; i < N - 1; i++) {
			A[i] = a[i] / pow(h, 2);
			B[i + 1] = a[i + 1] / pow(h, 2);
			C[i + 1] = -(a[i] + a[i + 1]) / pow(h, 2) - d[i];
		}

		for (size_t i = 1; i < N; i++){
			phi[i] *= -1;
		}

		return { A, B, C, phi};
	}

	inline void set_numeric_solution(const Vec_FP& A, const Vec_FP& C, const Vec_FP& B, const Vec_FP& phi) {
		
		//======================= DEBUG ========================//
		/*for (int i = 0; i < size(A); i++){
			std::cout<<A[i]<<std::endl;
		}
		std::cout<<std::endl;
		for (int i = 0; i < size(B); i++){
			std::cout<<B[i]<<std::endl;
		}
		std::cout<<std::endl;
		for (int i = 0; i < size(C); i++){
			std::cout<<C[i]<<std::endl;
		}
		std::cout<<std::endl;
		for (int i = 0; i < size(phi); i++){
			std::cout<<phi[i]<<std::endl;
		}
		std::cout<<std::endl;
		*/
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
		FP C1 = 0.58744204136927;
		FP C2 = -1.5874420413627;
		FP C3 = -16.23731986542228;
		FP C4 = -23.37825943652864;
		//======================= DEBUG ========================//
		//std::cout<<C1<<std::endl;
		//std::cout<<C2<<std::endl;
		//std::cout<<C3<<std::endl;
		//std::cout<<C4<<std::endl;
		//======================================================//
		u_vals.push_back(mu1);

		for (size_t i = 1; i < N; i++) {

			FP x_i = static_cast<FP>(i) * h;
			if (x_i <= xi)
				u_vals.push_back(C1 * exp(sqrt(2) * x_i) + C2 * exp(-sqrt(2) * x_i) + 1);

			else
				u_vals.push_back(C3 * exp(x_i / (2 * sqrt(5))) + C4 * exp(-x_i / (2 * sqrt(5))) + 40);
		}

		u_vals.push_back(mu2);
	}

private:
#endif

};
