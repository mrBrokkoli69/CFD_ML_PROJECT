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
	// Размеры сетки
	const int nx = 128;
	const int ny = 128;

	// Параметры течения
	const double tau = 0.7;        // время релаксации
	const double u_in = 0.01;       // скорость на входе
	const double rho0 = 1.0;        // начальная плотность


	// Параметры цилиндра
	int cx = nx/2;      // центр по x
	int cy = ny/2;      // центр по y
	int r = 30;          // радиус в ячейках
			    // Параметры для расчёта Re
	double cs2 = 1.0/3.0;
	double nu = cs2 * (tau - 0.5);  // кинематическая вязкость
	double L = 2 * r;                 // характерный размер (диаметр)
	double Re = u_in * L / nu;

	std::cout << "Re = " << Re << std::endl;



	// Создаём поле
	LBMField field(nx, ny);

	// Инициализация (равновесие с rho0, u=0)
	initField(field, rho0, 0.0, 0.0);

	// Создаём пустую маску (пока нет тела)
	std::vector<std::vector<bool>> mask(ny, std::vector<bool>(nx, false));
	addCylinder(mask, cx, cy, r);

	// Применяем маску (обнуляем твёрдые ячейки)
	loadMaskToLBM(field, mask);

	// Временной цикл
	const int maxSteps = 300;
	const int vtkInterval = 10;

	std::cout << "Starting LBM simulation..." << std::endl;
	std::cout << "Grid: " << nx << " x " << ny << std::endl;
	std::cout << "tau = " << tau << ", u_in = " << u_in << std::endl;

	for (int step = 0; step < maxSteps; step++) {
		// Вычисляем макроскопические величины
		computeMacroscopic(field, mask);

		// Выполняем столкновение
		collision(field, tau);

		// Перенос
		streaming(field);

		applyBounceBack(field);
	
		// Граничные условия
		applyZouHeLeft(field, u_in);           // вход слева
		applyOutflowRight(field);               // выход справа
		applyBounceBackMask(field, mask);       // отражение от тела

		// Сохраняем VTK с заданным интервалом
		if (step % vtkInterval == 0) {
			writeVTK(field, step);
			std::cout << "Step " << step << " completed" << std::endl;
		}
	}

	// Сохраняем последний шаг
	writeVTK(field, maxSteps);

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
