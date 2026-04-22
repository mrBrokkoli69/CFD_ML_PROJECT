CXX = g++
CXXFLAGS = -Wall -Wextra -g3 -std=c++17

# Папки
SRCDIR_LBM = src/lbm
SRCDIR_EDITOR = src/mask_editor
SRCDIR_IO = src/io
SRCDIR_POST = src/post
SRCDIR_GEOMETRY = src/geometry
SRCDIR_APPS = src

# Итоговые приложения
TARGET_INTERACTIVE = Interactive_app
TARGET_VALIDATION = Validation_app

# -------------------------
# Общие solver-файлы
# -------------------------
COMMON_SOLVER_SRCS = \
	$(SRCDIR_LBM)/lbm_core.cpp \
	$(SRCDIR_LBM)/lbm_runner.cpp \
	$(SRCDIR_POST)/postprocessing.cpp \
	$(SRCDIR_IO)/results_io.cpp \
	$(SRCDIR_IO)/case_io.cpp \
	$(SRCDIR_EDITOR)/mask_loader.cpp

# -------------------------
# Файлы interactive app
# -------------------------
INTERACTIVE_EDITOR_SRCS = \
	$(SRCDIR_EDITOR)/editor.cpp \
	$(SRCDIR_EDITOR)/file_io.cpp \
	$(SRCDIR_EDITOR)/flood_fill.cpp \
	$(SRCDIR_EDITOR)/shape_generator.cpp

INTERACTIVE_APP_SRCS = \
	$(SRCDIR_APPS)/interactive_main.cpp \
	$(SRCDIR_APPS)/main_lbm.cpp \
	$(INTERACTIVE_EDITOR_SRCS) \
	$(COMMON_SOLVER_SRCS)

INTERACTIVE_APP_OBJS = $(INTERACTIVE_APP_SRCS:.cpp=.o)

# -------------------------
# Заранее прописываем, что пойдёт в validation app
# (пока можно не собирать, но список уже готов)
# -------------------------
VALIDATION_APP_SRCS = \
	$(SRCDIR_APPS)/validation_main.cpp \
	$(SRCDIR_GEOMETRY)/geometry.cpp \
	$(COMMON_SOLVER_SRCS)

VALIDATION_APP_OBJS = $(VALIDATION_APP_SRCS:.cpp=.o)

# -------------------------
# По умолчанию собираем только interactive app
# -------------------------
all: $(TARGET_INTERACTIVE)

# -------------------------
# Interactive app
# -------------------------
$(TARGET_INTERACTIVE): $(INTERACTIVE_APP_OBJS)
	$(CXX) $(INTERACTIVE_APP_OBJS) -o $(TARGET_INTERACTIVE) -lncurses

# -------------------------
# Validation app
# Пока можно не вызывать, но цель уже готова
# -------------------------
$(TARGET_VALIDATION): $(VALIDATION_APP_OBJS)
	$(CXX) $(VALIDATION_APP_OBJS) -o $(TARGET_VALIDATION)

# -------------------------
# Общее правило компиляции
# -------------------------
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -------------------------
# Утилиты
# -------------------------
clean:
	rm -f $(INTERACTIVE_APP_OBJS) $(VALIDATION_APP_OBJS) $(TARGET_INTERACTIVE) $(TARGET_VALIDATION)

run: $(TARGET_INTERACTIVE)
	./$(TARGET_INTERACTIVE)

run_solver_example: $(TARGET_INTERACTIVE)
	./$(TARGET_INTERACTIVE) --solver ./mask.dat 0.6 0.05 1.0 3000 1000 0 20.0
