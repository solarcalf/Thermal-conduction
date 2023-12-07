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

	auto x = solution.x_grid;
	auto v = solution.v_vals;
	auto u = solution.u_vals;

	if (path == "v_values.txt") {
		ofstream fout_x;
		fout_x.open("x_grid.txt");
		for (size_t i = 0; i < x.size(); ++i)
			fout_x << x[i] << "\n";
		fout_x.close();

		ofstream fout_v;
		fout_v.open(path);
		for (size_t i = 0; i < v.size(); ++i)
			fout_v << v[i] << "\n";
		fout_v.close();

		ofstream fout_u;
		fout_u.open("u_values.txt");
		for (size_t i = 0; i < u.size(); ++i)
			fout_u << u[i] << "\n";
		fout_u.close();

		ofstream fout_e;
		fout_e.open("e_vals_test.txt");
		for (size_t i = 0; i < u.size(); ++i)
			fout_e << fabs(u[i] - v[i]) << "\n";
		fout_e.close();
	}
	else if (path == "v_half_values.txt") {
		ofstream fout_x;
		fout_x.open("x_half_grid.txt");
		for (size_t i = 0; i < x.size(); ++i)
			fout_x << x[i] << "\n";
		fout_x.close();

		ofstream fout_v;
		fout_v.open(path);
		for (size_t i = 0; i < v.size(); ++i)
			fout_v << v[i] << "\n";
		fout_v.close();
	}

	//std::cout << " x  |     v     |    u   |" << std::endl;
	//for (size_t i = 0; i < v.size(); ++i){
	//	std::cout << x[i] << "    "<< v[i] << "    "<< u[i]<<std::endl;
	//}
}

void test_problem() {
	ifstream source_fin;

	source_fin.open("Source.txt");
	if (!source_fin.is_open()) {
		std::cout << "ERROR: Failed to open Source.txt" << std::endl;
		exit(1);
	}
	size_t number;
	source_fin >> number;
	source_fin.close();

	string path_main = "v_values.txt";
	string path_half = "v_half_values.txt";

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

	size_t main_N = number;
	size_t double_N = number * 2;

	thermal solution(0, 1, main_N, k, q, f, mu1, mu2, xi);
	thermal half_solution(0, 1, double_N, k, q, f, mu1, mu2, xi);

	auto v = solution.v_vals;
	auto v_h = half_solution.v_vals;

	size_t ni = 0;
	ofstream fout_e;
	fout_e.open("e_vals_main.txt");
	for (size_t i = 0; i < v.size(); ++i) {
		fout_e << fabs(v[i] - v_h[ni]) << "\n";
		ni += 2;
	}
	fout_e.close();

	write(path_main, solution);
	write(path_half, half_solution);
}

int main(int argc, char* argv[]) {

	test_problem();

	return 0;
}
