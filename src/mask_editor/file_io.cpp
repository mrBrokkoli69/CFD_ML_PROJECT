#include "file_io.h"
#include <fstream>




void saveMask(const Mask& mask, const std::string& filename) {
	std::ofstream file(filename);
	if(!(file.is_open())) {
		return;
	}

	file<< mask.WIDTH<< " "<< mask.HEIGHT<< "\n" ;

	for(int y = 0; y< mask.HEIGHT; y++) {
		for(int x = 0 ; x < mask.WIDTH; x++ ) {
			if(mask.isSolid(y,x)) {
						file <<'1';
					} else {
						file<<'0';
					}
		}
		file <<"\n";
	}
	file.close();
}

void loadMask(Mask& mask, const std::string& filename) {
    // Открываем файл для чтения
    std::ifstream file(filename);

    // Проверяем, открылся ли файл
    if (!file.is_open()) {
        return;  // если не открылся — выходим
    }

    // Читаем размеры
    int w, h;
    file >> w >> h;

    // Проверяем, совпадают ли размеры с нашей маской
    if (w != mask.WIDTH || h != mask.HEIGHT) {
        file.close();
        return;  // размеры не совпадают — не загружаем
    }

    // Читаем все клетки построчно
    for (int y = 0; y < mask.HEIGHT; y++) {
        std::string line;
        file >> line;  // читаем целую строку из файла

        // Обрабатываем каждый символ в строке
        for (int x = 0; x < mask.WIDTH && x < (int)line.size(); x++) {
            // Если символ '1' — ставим твёрдое тело, иначе воздух
            mask.setSolid(y, x, line[x] == '1');
        }
    }

    file.close();  // явно закрываем файл
}
