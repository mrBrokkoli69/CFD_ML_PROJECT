#include "file_io.h"
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
namespace fs = std::filesystem;



std::string saveMask(const Mask& mask) {  // теперь возвращаем строку
	std::string filename = getNextMaskFilename();

	std::ofstream file(filename);
	if (!file.is_open()) {
		return "";  // пустая строка — ошибка
	}

	file << mask.WIDTH << " " << mask.HEIGHT << "\n";

	for (int y = 0; y < mask.HEIGHT; y++) {
		for (int x = 0; x < mask.WIDTH; x++) {
			file << (mask.isSolid(y, x) ? '1' : '0');
		}
		file << "\n";
	}

	file.close();

	return filename;  // возвращаем имя сохранённого файла
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


std::string getNextMaskFilename() {
	int maxNum = 0;
	std::string path = "data/masks/";

	// Проверяем, существует ли папка, если нет — создаём
	if (!fs::exists(path)) {
		fs::create_directories(path);
	}

	// Проходим по всем файлам в папке
	for (const auto& entry : fs::directory_iterator(path)) {
		std::string filename = entry.path().filename().string();

		// Проверяем, подходит ли файл под шаблон "mask_XXX.dat"
		if (filename.rfind("mask_", 0) == 0 && filename.size() > 8) {
			// Извлекаем число между "mask_" и ".dat"
			std::string numStr = filename.substr(5, filename.size() - 9);

			// Пробуем преобразовать в число
			try {
				int num = std::stoi(numStr);
				if (num > maxNum) maxNum = num;
			} catch (...) {
				// Если не число — игнорируем
			}
		}
	}

	// Формируем имя для нового файла
	return path + "mask_" + std::to_string(maxNum + 1) + ".dat";
}

std::vector<std::string> getMaskList()
{
	std::vector<std::string> masks;
	std::string path = "data/masks/";
	if ( !(fs::exists(path))) {
		return masks;
	}

	for (const auto& entry: fs::directory_iterator(path)) {
		std::string filename = entry.path().filename().string();

		if(filename.rfind("mask_",0) == 0 && filename.size() > 8 ) {
			masks.push_back(filename);
		}
	}

	std::sort(masks.begin(), masks.end());

	return masks;
}


