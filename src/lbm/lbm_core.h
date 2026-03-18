#ifndef LBM_CORE_H
#define LBM_CORE_H


#include <cmath>
#include <vector>


static const int D = 2;
static const int Q = 9;

static const int cx[Q] = {
	0,
	1,
	0,
	-1,
	0,
	1,
	-1,
	-1,
	1
};

static const int cy[Q] = {
	0,
	0,
	1,
	0,
	-1,
	1,
	1,
	-1,
	-1
};

static const double w[Q] = {
	4.0/9.0,
	1.0/9.0,
	1.0/9.0,
	1.0/9.0,
	1.0/9.0,
	1.0/36.0,
	1.0/36.0,
	1.0/36.0,
	1.0/36.0
};

static const int CS2 = 1.0/3.0;
static const int CS = std::sqrt(CS2);

struct LBMField {
	int nx;
	int ny;

	std::vector<std::vector<std::vector<double>>> f; //f[i][y][x] -- трехмерный массив функции распределения по каждому направлению в узлах

	std::vector<std::vector<double>> rho;
	std::vector<std::vector<double>> ux;
	std::vector<std::vector<double>> uy;

	LBMField(int ny, int nx);


};

void computeMacroscopic(LBMField& field);

void equilibrium(double feq[Q], double rho, double ux, double uy);

void collision(LBMField& field, double tau);

void streaming(LBMField& field);

void initField(LBMField& field, double rho0, double ux0, double uy0);


#endif
