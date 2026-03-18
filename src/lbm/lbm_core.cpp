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

void streaming(LBMField& field) {
	int nx = field.nx;
	int ny = field.ny;

	std::vector<std::vector<std::vector<double>>> f_temp = field.f;

	for(int i = 0; i < Q; i++) {
		for(int y = 0 ; y < ny; y++) {
			for(int x = 0; x < nx; x++) {
				field.f[i][y][x] = 0;


			}
		}

	}

	for(int i = 0; i < Q; i++) {
		for(int y = 0 ; y < ny; y++) {
			for(int x = 0; x < nx; x++) {
				int nx_next = x + cx[i];
				int ny_next = y + cy[i];

				if(nx_next >= 0 && nx_next < nx && ny_next >=0 && ny_next < ny) {
					field.f[i][ny_next][nx_next] += f_temp[i][y][x];



				} 


			}
		}





	}

}


void initField(LBMField& field, double rho0, double ux0, double uy0) {
	int nx = field.nx;
	int ny = field.ny;

	double feq[Q];

	equilibrium(feq , rho0, ux0, uy0);

	for (int y = 0; y < ny; y++) {
		for (int x = 0; x < nx; x++) {
			for (int i = 0; i < Q; i++) {
				field.f[i][y][x] = feq[i];
			}
			field.rho[y][x] = rho0;
			field.ux[y][x] = ux0;
			field.uy[y][x] = uy0;
		}
	}
}








