#pragma once

#include <functional>
#include <vector>
#include "TMA.h"

#include <iostream>

class thermal {
public:

	using FP = double;
	using VecFP = std::vector<FP>;
	using FPair = std::pair<std::function<FP(FP)>, std::function<FP(FP)>>;
	
	VecFP x_grid;
	VecFP v_vals;

public:
	thermal (FP x0, FP xn, size_t N, FPair k, FPair q, FPair f, FP mu1, FP mu2, FP xi) {
		
		VecFP a(N);
		VecFP d(N);
		VecFP phi(N);

		FP h = (xn - x0) / N;
		VecFP x(N + 1);
		x[0] = x0;
		x[1] = x0 + h;

		size_t i = 1;

		while (xi >= x[i] + h / 2) {
			a[i-1] = k.first(x[i] - h / 2);
			d[i-1] = q.first(x[i]);
			phi[i-1] = f.first(x[i]);

			x[i + 1] = x[i] + h;
			i += 1;
		}

		if (x[i] - h / 2 <= xi && xi <= x[i] + h / 2) {
			if (xi < x[i])
				a[i-1] = h / ((xi - x[i - 1]) / k.first((x[i - 1] + xi) / 2) + (x[i] - xi) / k.second((xi + x[i]) / 2));
			else
				a[i-1] = k.first(x[i] - h / 2);

			FP xl = x[i] - h / 2;
			FP xr = x[i] + h / 2;
			d[i-1] = (q.first((xl + xi) / 2) * (xi - xl) + q.second((xi + xr) / 2) * (xr - xi)) / h;
			phi[i-1] = (f.first((xl + xi) / 2) * (xi - xl) + f.second((xi + xr) / 2) * (xr - xi)) / h;
			x[i + 1] = x[i] + h;
			i += 1;
		}

		if (x[i - 1] < xi && xi < x[i]) {
			a[i-1] = h / ((xi - x[i - 1]) / k.first((x[i - 1] + xi) / 2) + (x[i] - xi) / k.second((xi + x[i]) / 2));

			d[i-1] = q.second(x[i]);
			phi[i-1] = f.second(x[i]);

			x[i + 1] = x[i] + h;
			i += 1;
		}

		while (i != N) {
			a[i-1] = k.second(x[i] - h / 2);
			d[i-1] = q.second(x[i]);
			phi[i-1] = f.second(x[i]);

			x[i + 1] = x[i] + h;
			i += 1;
		}

		a[i-1] = k.second(x[i] - h / 2);
		d[i-1] = q.second(x[i]);
		phi[i-1] = f.second(x[i]);

		phi[0] = mu1;
		phi[N-1] = mu2;

		VecFP A(N - 1);
		VecFP B(N - 1);
		VecFP C(N);

		A[N - 2] = 0;
		B[0] = 0;
		C[0] = 1;
		C[N - 1] = 1;
			
		for (i = 1; i < N - 1; i++) {
			A[i - 1] = a[i - 1] / pow(h, 2);
			B[i] = a[i + 1] / pow(h, 2);
			C[i] = -1 * ((a[i] + a[i + 1]) / pow(h, 2) + d[i]);
		}
		
		//=================================DEBUG=====================================//
		/*
		std::cout << 'i' << '\t' << 'a' << "\t\t" << 'd' << "\t\t" << "phi\n\n";
		for (i = 0; i < N - 1; i++)
			std::cout << i << '\t' << a[i] << "\t\t" << d[i] << "\t\t" << phi[i] << '\n';

		std::cout << "\n\n\n";

		std::cout << 'i' << '\t' << 'A' << "\t\t" << 'B' << "\t\t" << "C\n\n";
		for (i = 0; i < N - 1; i++)
			std::cout << i << '\t' << A[i] << "\t\t" << B[i] << "\t\t" << C[i] << '\n';
		std::cout << C[N - 1];
		*/

		x_grid = std::move(a);
		v_vals = TMA(A, C, B, phi);
	}
};
