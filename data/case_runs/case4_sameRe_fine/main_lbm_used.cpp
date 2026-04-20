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
    const std::string outputDir = "/run/media/deck/SD512/6sem/IT_PROJECT/CFD_ML_PROJECT/data/case_runs/case4_sameRe_fine";

    const int nx = 512;
    const int ny = 200;
    const double tau = 0.65;
    const double u_in = 0.075;

    int cx = nx / 3;
    int cy = ny / 2;
    int r = 20;

    const int maxSteps = 6000;
    const int vtkInterval = 10;
	const int coutInterval = 100;

	// Параметры для расчёта Re
	double cs2 = 1.0/3.0;
	double nu = cs2 * (tau - 0.5);  // кинематическая вязкость
	double L = 2 * r;                 // характе:Жрный размер (диаметр)
	double Re = u_in * L / nu;


	const double rho0 = 1.0;        // начальная плотность

	std::cout << "Re = " << Re << std::endl;



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
		// Вычисляем макроскопические величины
		computeMacroscopic(field, mask);

		// Выполняем столкновение
		collision(field, tau, mask);

		// Перенос
		streaming(field,mask);

		//	applyBounceBack(field);

		// Граничные условия
		applyZouHeLeft(field, u_in);           // вход слева
		applyOutflowRight(field);               // выход справа
							//applySpongeZone(field,1, u_in, 0 ,15);
		resetCornersToRest(field);    //настройка углов
					      //	applyBounceBackMask(field, mask);       // отражение от тела

					      // Сохраняем VTK с заданным интервалом
		if (step % vtkInterval == 0) {
			writeVTK(field, step, outputDir);
			if(step % coutInterval == 0 ) {
				std::cout << "Step " << step << " completed" << std::endl;
			}
		}

	}

	// Сохраняем последний шаг
	writeVTK(field, maxSteps, outputDir);

	// Вычисляем силу (опционально)
	auto force = computeForce(field, mask);
	std::cout << "Final force: Fx = " << force.first << ", Fy = " << force.second << std::endl;
	// После computeForce
	double Fx = force.first;
	double rho_in = 1.0;  // плотность на входе (решёточная)

	double Cd = 2.0 * Fx / (rho_in * u_in * u_in * L);

	std::cout << "Cd = " << Cd << std::endl;
	std::cout << "Simulation finished!" << std::endl;
	return 0;
}
