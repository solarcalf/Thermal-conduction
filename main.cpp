#include <iostream>
#include <fstream>
#include "ThermalConduction.h"
#include "TMA.h"

using namespace std;

void TMA_test() {
	vector<double> A{ 3, 2, 0 };
	vector<double> B{ 1, 3, 5 };
	vector<double> C{ 1, 9, 3, 1 };
	vector<double> b{ 2, 15, 10, 1 };

	auto sol = TMA(A, C, B, b);

	for (auto x : sol)
		cout << x << ' ';
}

void write(const string& path, const thermal& solution) {
	ofstream fout;
	fout.open(path);

	auto x = solution.x_grid;
	auto v = solution.v_vals;
	auto u = solution.u_vals;

	for (size_t i = 0; i < v.size(); ++i)
		fout << x[i] << ' ';
	fout << '\n';

	for (size_t i = 0; i < v.size(); ++i)
		fout << v[i] << ' ';
	fout << '\n';

	for (size_t i = 0; i < v.size(); ++i)
		fout << u[i] << ' ';
	
	fout.close();
}

void test_problem() {
	string path_main = "main_method.txt";
	string path_half = "half_method.txt";

	double xi = 0.25;
	double mu1 = 0;
	double mu2 = 1;

	function<double(double)> k1 = [](double x) {return 1.0 / 2; };
	function<double(double)> k2 = [](double x) {return 1.25; };
	function<double(double)> q1 = [](double x) {return 1; };
	function<double(double)> q2 = [](double x) {return 1.0 / 16; };
	function<double(double)> f1 = [](double x) {return 1; };
	function<double(double)> f2 = [](double x) {return 2.5; };

	pair<function<double(double)>, function<double(double)>> k = { k1, k2 };
	pair<function<double(double)>, function<double(double)>> q = { q1, q2 };
	pair<function<double(double)>, function<double(double)>> f = { f1, f2 };

	thermal solution(0, 1, 100, k, q, f, mu1, mu2, xi);
	thermal half_solution(0, 1, 200, k, q, f, mu1, mu2, xi);

	write(path_main, solution);
	write(path_half, half_solution);
}

int main(int argc, char* argv[]) {

	test_problem();

	return 0;
}
