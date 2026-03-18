#include "lbm_core.h"
#include <iostream>


LBMField::LBMField(int nx_ , int ny_) : nx(nx_), ny(ny_) {
	f.resize(Q);

	for(int i = 0 ; i < Q; i ++) {
		f[i].resize(ny);
		for (int j = 0; j < ny; j++) {
			f[i][j].resize(nx, 0.0);
		}
	}
	rho.resize(ny, std::vector<double>(nx, 1.0));
	ux.resize(ny, std::vector<double>(nx, 0.0));
	uy.resize(ny, std::vector<double>(nx, 0.0));



}

void computeMacroscopic(LBMField& field)
{
	int nx = field.nx;
	int ny = field.ny;

	for(int i = 0; i < ny; i++) {
		for(int j = 0; j < nx; j++) {
			double rho = 0;
			double ux = 0;
			double uy = 0;
			for ( int k = 0; k < Q; k++) {
				rho +=field.f[k][i][j];
				ux += field.f[k][i][j] * cx[k];
				uy += field.f[k][i][j] * cy[k];
			}
			field.rho[i][j] = rho;
			if(rho >= 1e-12) {
				field.ux[i][j] = ux / rho;
				field.uy[i][j] = uy / rho;
			}
			else {
				field.ux[i][j] = 0.0;
				field.uy[i][j] = 0.0;
			}
		}
	}
}


void equilibrium(double feq[Q], double rho, double ux,double uy) {
	double u2 = ux*ux + uy*uy;
	for (int i = 0; i < Q; i++) {
		double cu = cx[i]*ux + cy[i]*uy;

		feq[i] = rho * w[i] * ( 1 + 3*cu + 4.5 * cu * cu - 1.5 *u2);




	}


}

void collision(LBMField& field, double tau) {
	int nx = field.nx;
	int ny = field.ny;

	double feq[Q];

	for(int y = 0; y < ny; y++) {
		for(int x = 0; x < nx; x++) {
			double rho = field.rho[y][x];
			double ux = field.ux[y][x];
			double uy = field.uy[y][x];


			equilibrium(feq , rho , ux, uy);

			for(int i = 0 ; i < Q; i++) {
				double fi = field.f[i][y][x];

				field.f[i][y][x] = fi - (1.0 / tau ) * (fi - feq[i]);



			}


		}


	}



}



