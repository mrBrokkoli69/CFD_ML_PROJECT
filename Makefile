CXX = g++
CXXFLAGS = -Wall -Wextra -g3 -std=c++17 -fopenmp
LDFLAGS = -fopenmp

# Папки
SRCDIR_LBM = src/lbm
SRCDIR_EDITOR = src/mask_editor
SRCDIR_IO = src/io
SRCDIR_POST = src/post
SRCDIR_GEOMETRY = src/geometry
SRCDIR_APPS = src
SRCDIR_ML = src/ml

# Итоговые приложения
TARGET_INTERACTIVE = Interactive_app
TARGET_VALIDATION = Validation_app
TARGET_DATASET = Dataset_generator
TARGET_MLP = MLP_app

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
# Validation app
# -------------------------
VALIDATION_APP_SRCS = \
	$(SRCDIR_APPS)/validation_main.cpp \
	$(SRCDIR_GEOMETRY)/geometry.cpp \
	$(COMMON_SOLVER_SRCS)

VALIDATION_APP_OBJS = $(VALIDATION_APP_SRCS:.cpp=.o)

# -------------------------
# Dataset generator
# -------------------------
DATASET_APP_SRCS = \
	$(SRCDIR_APPS)/dataset_main.cpp \
	$(SRCDIR_EDITOR)/shape_generator.cpp \
	$(COMMON_SOLVER_SRCS)

DATASET_APP_OBJS = $(DATASET_APP_SRCS:.cpp=.o)

# -------------------------
# MLP app
# -------------------------
MLP_APP_SRCS = \
	$(SRCDIR_ML)/dataset.cpp \
	$(SRCDIR_ML)/mlp.cpp \
	$(SRCDIR_APPS)/ml_main.cpp

MLP_APP_OBJS = $(MLP_APP_SRCS:.cpp=.o)

# -------------------------
# По умолчанию собираем все приложения
# -------------------------
all: $(TARGET_INTERACTIVE) $(TARGET_VALIDATION) $(TARGET_DATASET) $(TARGET_MLP)

# -------------------------
# Interactive app
# -------------------------
$(TARGET_INTERACTIVE): $(INTERACTIVE_APP_OBJS)
	$(CXX) $(INTERACTIVE_APP_OBJS) -o $(TARGET_INTERACTIVE) -lncurses $(LDFLAGS)

# -------------------------
# Validation app
# -------------------------
$(TARGET_VALIDATION): $(VALIDATION_APP_OBJS)
	$(CXX) $(VALIDATION_APP_OBJS) -o $(TARGET_VALIDATION) $(LDFLAGS)

# -------------------------
# Dataset generator
# -------------------------
$(TARGET_DATASET): $(DATASET_APP_OBJS)
	$(CXX) $(DATASET_APP_OBJS) -o $(TARGET_DATASET) $(LDFLAGS)

# -------------------------
# MLP app
# -------------------------
$(TARGET_MLP): $(MLP_APP_OBJS)
	$(CXX) $(MLP_APP_OBJS) -o $(TARGET_MLP) $(LDFLAGS)

# -------------------------
# Общее правило компиляции
# -------------------------
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -------------------------
# Утилиты
# -------------------------
clean:
	rm -f $(INTERACTIVE_APP_OBJS) $(VALIDATION_APP_OBJS) $(DATASET_APP_OBJS) $(MLP_APP_OBJS) \
	      $(TARGET_INTERACTIVE) $(TARGET_VALIDATION) $(TARGET_DATASET) $(TARGET_MLP)

run: $(TARGET_INTERACTIVE)
	./$(TARGET_INTERACTIVE)

run_solver_example: $(TARGET_INTERACTIVE)
	./$(TARGET_INTERACTIVE) --solver ./mask.dat 0.6 0.05 1.0 3000 1000 0 20.0

run_mlp: $(TARGET_MLP)
	./$(TARGET_MLP)
