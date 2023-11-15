#pragma once

#include <functional>
#include <vector>
#include "TMA.h"

class thermal {
private:

	using FP = double;
	using VecFP = std::vector<FP>;
	using FPair = std::pair<std::function<FP(FP)>, std::function<FP(FP)>>;
	
	VecFP x_grid;
	VecFP v_vals;

	static const inline FP integrate(FP a, FP h, std::function<FP(FP)> f) {
		return f(a + h) * h;
	}

public:
	thermal (FP a, FP b, size_t N, FPair k, FPair q, FPair f, FP mu1, FP mu2, FP xi) {
		
		FP h = (b - a) / N;
		VecFP x_grid{a};
		VecFP v_vals;

		VecFP A(N - 1);
		VecFP B(N - 1);
		VecFP C(N - 1);
		VecFP phi(N);

		FP a = integrate(x_grid.back(), h, [k](FP x) { return 1 / k.first(x); });
		size_t i = 1;

		A[0] = 1 / (a + h);

		while (x_grid.back() + h < xi) {
			FP a = integrate(x_grid.back(), h, [k](FP x) { return 1 / k.first(x); });

			A[i] = 1 / (a * h);
			B[i - 1] = A[i];
			C[i] += A[i];
			C[i - 1] += A[i++];
		}
		
		FP a = integrate(x_grid.back(), xi - x_grid.back(), [k](FP x) { return 1 / k.first(x); }) +
			integrate(xi, x_grid.back() + h - xi, [k](FP x) { return 1 / k.second(x); });

		A[i] = 1 / (a * h);
		B[i - 1] = A[i];
		C[i] += A[i];
		C[i - 1] += A[i++];
		
		while (x_grid.back() < b) {
			FP a = integrate(x_grid.back(), h, [k](FP x) { return 1 / k.second(x); });

			A[i] = 1 / (a * h);
			B[i - 1] = A[i];
			C[i] += A[i];
			C[i - 1] += A[i++];
		}


	}

};
