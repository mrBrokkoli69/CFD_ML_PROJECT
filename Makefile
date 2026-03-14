CXX = g++
CXXFLAGS = -Wall -Wextra -g3 -std=c++17
TARGET = mask_editor
SRCDIR = src/mask_editor
SRCS = $(SRCDIR)/editor.cpp $(SRCDIR)/flood_fill.cpp $(SRCDIR)/file_io.cpp $(SRCDIR)/shape_generator.cpp $(SRCDIR)/lbm/lbm_core.cpp 
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) -lncurses	

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
clean:
	rm -f $(OBJS) $(TARGET)
run: $(TARGET)
	./%(TARGET)


