#include "lbm_core.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif



LBMField::LBMField(int nx_, int ny_) : nx(nx_), ny(ny_) {
	f.resize(Q);

	for (int q = 0; q < Q; ++q) {
		f[q].resize(ny);
		for (int y = 0; y < ny; ++y) {
			f[q][y].resize(nx, 0.0);
		}
	}

	rho.resize(ny, std::vector<double>(nx, 1.0));
	ux.resize(ny, std::vector<double>(nx, 0.0));
	uy.resize(ny, std::vector<double>(nx, 0.0));
}

void computeMacroscopic(LBMField& field,const std::vector<std::vector<bool>>& mask)
{
	int nx = field.nx;
	int ny = field.ny;
#pragma omp parallel for collapse(2)
	for(int i = 0; i < ny; i++) {
		for(int j = 0; j < nx; j++) {
			if(mask[i][j]) {
				field.rho[i][j] = 1.0;
				field.ux[i][j] = 0;
				field.uy[i][j] = 0;
				continue;

			}



			double rho = 0;
			double ux = 0;
			double uy = 0;
			for ( int k = 0; k < Q; k++) {
				rho +=field.f[k][i][j];
				ux += field.f[k][i][j] * cx[k];
				uy += field.f[k][i][j] * cy[k];
			}
			field.rho[i][j] = rho;
			if(rho >=1e-4) {
				field.ux[i][j] = ux / rho;
				field.uy[i][j] = uy / rho;
			}
			else {
				std::cerr<<"Error in x = " << j << "and y = " << i << "rho = " << rho<<std::endl;

				std::exit(1);

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

void collision(LBMField& field, double tau,const std::vector<std::vector<bool>>& mask) {
	int nx = field.nx;
	int ny = field.ny;

#pragma omp parallel for collapse(2)
	for(int y = 0; y < ny; y++) {
		for(int x = 0; x < nx; x++) {
			if(mask[y][x]) continue;
			double feq[Q];
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

void streaming(LBMField& field, const std::vector<std::vector<bool>>& mask, double& Fx, double& Fy) {
	int nx = field.nx;
	int ny = field.ny;

	const int opposite[Q] = {0, 3, 4, 1, 2, 7, 8, 5, 6};

	std::vector<std::vector<std::vector<double>>> f_old = field.f;
	std::vector<std::vector<std::vector<double>>> f_new = field.f;

	Fx = 0.0;
	Fy = 0.0;

#pragma omp parallel for collapse(2) reduction(+:Fx,Fy)
	for (int y = 0; y < ny; y++) {
		for (int x = 0; x < nx; x++) {

			if (mask[y][x]) {
				for (int i = 0; i < Q; i++) {
					f_new[i][y][x] = f_old[i][y][x];
				}
				continue;
			}

			for (int i = 0; i < Q; i++) {
				int src_x = x - cx[i];
				int src_y = y - cy[i];

				if (src_y < 0 || src_y >= ny) {
					f_new[i][y][x] = f_old[opposite[i]][y][x];
				}
				else if (src_x >= 0 && src_x < nx) {
					if (mask[src_y][src_x]) {
						f_new[i][y][x] = f_old[opposite[i]][y][x];
						Fx += 2.0 * f_old[opposite[i]][y][x] * cx[opposite[i]];
						Fy += 2.0 * f_old[opposite[i]][y][x] * cy[opposite[i]];
					}
					else {
						f_new[i][y][x] = f_old[i][src_y][src_x];
					}
				}
				else {
					f_new[i][y][x] = 0.0;
				}
			}
		}
	}

	field.f.swap(f_new);
}


double poiseuilleProfile(int y, int ny , double u_max)   //Функция, задающая параболический поток(чтобы со стенками не было проблем)
{
	double H = static_cast<double>(ny-2);
	double yy = static_cast<double>(y-1) / H;

	return 4.0 * u_max * yy * (1.0-yy);



}





void initField(LBMField& field, double rho0, double ux0, double uy0) {
	int nx = field.nx;
	int ny = field.ny;

	double feq[Q];
	double ux;
	double uy;
	for (int y = 0; y < ny; y++) {
		for (int x = 0; x < nx; x++) {
			if( y == 0 || y == ny - 1 ) {
				ux = 0;
				uy = 0;
			}
			else {
				ux = poiseuilleProfile (y , ny, ux0); // Скорость в конкретном Y
				uy = 0;
			}

			equilibrium(feq , rho0, ux, uy);

			for (int i = 0; i < Q; i++) {
				field.f[i][y][x] = feq[i];
			}
			field.rho[y][x] = rho0;
			field.ux[y][x] = ux;
			field.uy[y][x] = uy;
		}
	}

}


void applyZouHeLeft(LBMField& field, double u_in) {
	int nx = field.nx;
	int ny = field.ny;
	int x = 0 ;

	for(int y = 1 ; y < ny-1 ; y++) {

		double ux_in = poiseuilleProfile(y, ny, u_in);

		double f0 = field.f[0][y][x];
		double f2 = field.f[2][y][x];
		double f3 = field.f[3][y][x];
		double f4 = field.f[4][y][x];
		double f6 = field.f[6][y][x];
		double f7 = field.f[7][y][x];

		double rho = (f0 + f2 + f4 + 2 * ( f3 + f6 + f7)) / (1-ux_in) ;

		double f1 = f3 + (2.0/3.0) * rho * ux_in;
		double f5 = f7 - 0.5 * (f2-f4) + (1.0/6.0) * rho * ux_in;
		double f8 = f6 + 0.5 * (f2 - f4) + (1.0/6.0) * rho * ux_in;


		field.f[1][y][x] = f1;
		field.f[5][y][x] = f5;
		field.f[8][y][x] = f8;

		field.rho[y][x] = rho;
		field.ux[y][x] = ux_in;
		field.uy[y][x] = 0.0; 
	}
}




void applyOutflowRight(LBMField& field) {
	int nx = field.nx;
	int ny = field.ny;
	int x = nx - 1;

	for (int y = 1; y < ny - 1; ++y) {
		double rho_b = 1;
		double ux_b = field.ux[y][x-1];
		double uy_b = field.uy[y][x-1];
		double feq_b[Q];

		equilibrium(feq_b , rho_b , ux_b , uy_b);


		double rho_f = field.rho[y][x - 1];
		double ux_f = field.ux[y][x - 1];
		double uy_f = field.uy[y][x - 1];
		double feq_f[Q];

		equilibrium(feq_f, rho_f, ux_f, uy_f);

		for (int i = 0; i < Q; ++i) {
			field.f[i][y][x] = feq_b[i] + (field.f[i][y][x - 1] - feq_f[i]);
		}
	}
}



void resetCornersToRest(LBMField& field) {
	const int corners[4][2] = {
		{0, 0},
		{field.nx - 1, 0},
		{0, field.ny - 1},
		{field.nx - 1, field.ny - 1}
	};

	double feq[Q];
	equilibrium(feq, 1.0, 0.0, 0.0);

	for (int k = 0; k < 4; ++k) {
		int x = corners[k][0];
		int y = corners[k][1];

		field.rho[y][x] = 1.0;
		field.ux[y][x] = 0.0;
		field.uy[y][x] = 0.0;

		for (int i = 0; i < Q; ++i) {
			field.f[i][y][x] = feq[i];
		}
	}
}





void writeVTK(const LBMField& field, int step, const std::string& outputDir) {
	int nx = field.nx;
	int ny = field.ny;

	// Формируем имя файла: flow_0001.vtk, flow_0002.vtk и т.д.
	std::ostringstream filename;
	filename << outputDir << "/flow_" << std::setw(4) << std::setfill('0') << step << ".vtk";

	std::ofstream file(filename.str());
	if (!file.is_open()) {
		std::cerr << "Cannot open file " << filename.str() << " for writing!" << std::endl;
		return;
	}

	// Заголовок VTK-файла
	file << "# vtk DataFile Version 3.0\n";
	file << "LBM simulation at step " << step << "\n";
	file << "ASCII\n";
	file << "DATASET STRUCTURED_POINTS\n";
	file << "DIMENSIONS " << nx << " " << ny << " 1\n";
	file << "ORIGIN 0 0 0\n";
	file << "SPACING 1 1 1\n";

	// Данные по точкам
	file << "POINT_DATA " << nx * ny << "\n";

	// Поле плотности (скаляр)
	file << "SCALARS density float\n";
	file << "LOOKUP_TABLE default\n";
	for (int y = 0; y < ny; y++) {
		for (int x = 0; x < nx; x++) {
			file << field.rho[y][x] << " ";
		}
		file << "\n";
	}

	// Поле скорости (вектор)
	file << "VECTORS velocity float\n";
	for (int y = 0; y < ny; y++) {
		for (int x = 0; x < nx; x++) {
			file << field.ux[y][x] << " " << field.uy[y][x] << " 0.0\n";
		}
	}

	file.close();
}


void loadMaskToLBM( LBMField& field, const std::vector<std::vector<bool>>& mask) {
	int nx = field.nx;
	int ny = field.ny;

	for ( int y = 0; y < ny; y++ ) {
		for ( int x = 0; x < nx; x++) {
			if (mask[y][x]) {
				for ( int i = 0 ; i < Q ; i++) {
					field.f[i][y][x] = 0;
				}
				field.rho[y][x] = 1.0;
				field.ux[y][x] = 0;
				field.uy[y][x] = 0;


			}



		}



	}



}



