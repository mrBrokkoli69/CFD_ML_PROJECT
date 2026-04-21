CXX = g++
CXXFLAGS = -Wall -Wextra -g3 -std=c++17

# Папки с исходниками
SRCDIR_EDITOR = src/mask_editor
SRCDIR_LBM = src/lbm
SRCDIR_GEOMETRY = src/geometry

# Исполняемые файлы
TARGET_EDITOR = mask_editor
TARGET_LBM = lbm_test

# Файлы для редактора
SRCS_EDITOR = $(SRCDIR_EDITOR)/editor.cpp \
              $(SRCDIR_EDITOR)/flood_fill.cpp \
              $(SRCDIR_EDITOR)/file_io.cpp \
              $(SRCDIR_EDITOR)/shape_generator.cpp

# Файлы для LBM (без editor.cpp!)
SRCS_LBM = $(SRCDIR_LBM)/lbm_core.cpp \
           src/main_lbm.cpp \
	   $(SRCDIR_GEOMETRY)/geometry.cpp \
	   $(SRCDIR_LBM)/lbm_runner.cpp
	   
# Объектные файлы для редактора
OBJS_EDITOR = $(SRCS_EDITOR:.cpp=.o)

# Объектные файлы для LBM
OBJS_LBM = $(SRCS_LBM:.cpp=.o)

# Правило по умолчанию
all: $(TARGET_EDITOR) $(TARGET_LBM)

# Сборка редактора
$(TARGET_EDITOR): $(OBJS_EDITOR)
	$(CXX) $(OBJS_EDITOR) -o $(TARGET_EDITOR) -lncurses

# Сборка LBM-теста
$(TARGET_LBM): $(OBJS_LBM)
	$(CXX) $(OBJS_LBM) -o $(TARGET_LBM) -lncurses

# Правило компиляции для .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f $(OBJS_EDITOR) $(OBJS_LBM) $(TARGET_EDITOR) $(TARGET_LBM)

# Запуск редактора
run: $(TARGET_EDITOR)
	./$(TARGET_EDITOR)

# Запуск LBM-теста
run_lbm: $(TARGET_LBM)
	./$(TARGET_LBM)
