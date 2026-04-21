#ifndef LBM_CORE_H
#define LBM_CORE_H


#include <cmath>
#include <vector>
#include <utility>
#include <string>


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

static const double CS2 = 1.0/3.0;
static const double CS = std::sqrt(CS2);

struct SimulationConfig {
    int nx;
    int ny;
    int maxSteps;
    int vtkInterval;
    int coutInterval;

    double tau;
    double rho0;
    double uMax;

    std::string outputDir;
};

struct SimulationResult {
    double fx;
    double fy;
    double avgFx;
    double avgFy;
    int stepsDone;
};

struct Mask {
    int nx;
    int ny;
    std::vector<std::vector<bool>> solid;
};


struct LBMField {
	int nx;
	int ny;

	std::vector<std::vector<std::vector<double>>> f; //f[i][y][x] -- трехмерный массив функции распределения по каждому направлению в узлах

	std::vector<std::vector<double>> rho;
	std::vector<std::vector<double>> ux;
	std::vector<std::vector<double>> uy;

	LBMField(int nx, int ny);


};

void computeMacroscopic(LBMField& field, const std::vector<std::vector<bool>>& mask); //выч макроскопиических вариантов

void equilibrium(double feq[Q], double rho, double ux, double uy); // определение равновесной функции распределения

void collision(LBMField& field, double tau, const std::vector<std::vector<bool>>& mask); //столкновение

void streaming(LBMField& field, const std::vector<std::vector<bool>>& mask, double& Fx, double& Fy); //перенос

void initField(LBMField& field, double rho0, double ux0, double uy0); // инициализация поля начальными данными 

void applyZouHeLeft(LBMField& field, double u_in);  //гран условие слева

void applyOutflowRight(LBMField& field); //гран условие справа

void resetCornersToRest(LBMField& field);

void writeVTK(const LBMField& field, int step, const std::string& outputDir); //функция записи результатов в файл для визуализации в OpenFoam

void loadMaskToLBM( LBMField& field, const std::vector<std::vector<bool>>& mask) ; //функция связи маски с LBM-сеткой






#endif
