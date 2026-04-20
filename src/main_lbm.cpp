#include "lbm/lbm_core.h"
#include <iostream>
#include <cmath>
void addCylinder(std::vector<std::vector<bool>>& mask, int cx, int cy, int r)  {  //функция добавления цилиндра для проверки
	int ny = mask.size();
	int nx = mask[0].size();

	for (int y = 0; y < ny; y++) {
		for (int x = 0; x < nx; x++) {
			int dx = x - cx;
			int dy = y - cy;
			if (dx*dx + dy*dy <= r*r) {
				mask[y][x] = true;
			}
		}
	}
}
int main() {
	const std::string outputDir = "./data/case_runs/test_case";

	const int nx = 441;
	const int ny = 84;
	const double tau = 0.6;
	const double u_in = 0.05;
	
	int cx = 40;
	int cy = 41;
	int r = 10;
	
	const int maxSteps = 10000;
	const int vtkInterval = 200;
	const int coutInterval = 1000;
	
	// Параметры для расчёта Re
	double cs2 = 1.0/3.0;
	double nu = cs2 * (tau - 0.5);  // кинематическая вязкость
	double L = 2 * r;                 // характе:Жрный размер (диаметр)
	double Re = (2.0/3.0) *  u_in * L / nu;


	const double rho0 = 1.0;        // начальная плотность

	std::cout << "Re = " << Re <<" V "<<u_in<<" L "<<L<< std::endl;
	//Расчет сил
	
	double Fx = 0;
	double Fy = 0;
	double FsumX = 0;
	double FsumY = 0;
	double FmeanX = 0 ;
	double FmeanY = 0;

	// Создаём поле
	LBMField field(nx, ny);

	// Инициализация (равновесие с rho0, ux=u_in,  uy=0)
	initField(field, rho0, u_in , 0.0);

	// Создаём пустую маску (пока нет тела)
	std::vector<std::vector<bool>> mask(ny, std::vector<bool>(nx, false));

	addCylinder(mask,cx,cy,r);


	// Применяем маску (обнуляем твёрдые ячейки)
	loadMaskToLBM(field, mask);
	std::cout << "Starting LBM simulation..." << std::endl;
	std::cout << "Grid: " << nx << " x " << ny << std::endl;
	std::cout << "tau = " << tau << ", u_in = " << u_in << std::endl;
	
	for (int step = 0; step < maxSteps; step++) {
   		 Fx = 0.0;
   		 Fy = 0.0;

    		computeMacroscopic(field, mask);
    		collision(field, tau, mask);
    		streaming(field, mask, Fx, Fy);

    		applyZouHeLeft(field, u_in);
    		applyOutflowRight(field);
    		resetCornersToRest(field);

    		FsumX += Fx;
    		FsumY += Fy;

    		if (step % vtkInterval == 0) {
        		writeVTK(field, step, outputDir);
    		}

    		if ((step + 1) % coutInterval == 0) {
        		FmeanX = FsumX / coutInterval;
        		FmeanY = FsumY / coutInterval;

        		std::cout << "Step " << step + 1
                 		 << " avg Fx = " << FmeanX
                 		 << ", avg Fy = " << FmeanY
                 		 << std::endl;

        		FsumX = 0.0;
        		FsumY = 0.0;
    		}
		}
	// Сохраняем последний шаг
	writeVTK(field, maxSteps, outputDir);

	// Вычисляем силу (опционально)
	std::cout << "Final force: Fx = " << FmeanX << ", Fy = " << FmeanY << std::endl;
	double rho_in = 1.0;  // плотность на входе (решёточная)

	double Cd = 2.0 * FmeanX / (rho_in * 2 / 3 * u_in * 2 / 3 * u_in * L);

	std::cout << "Cd = " << Cd << std::endl;
	std::cout << "Simulation finished!" << std::endl;
	return 0;
}
