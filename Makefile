CXX = g++
CXXFLAGS = -Wall -Wextra -g3
TARGET = mask_editor
SRCDIR = src/mask_editor
SRCS = $(SRCDIR)/editor.cpp $(SRCDIR)/flood_fill.cpp $(SRCDIR)/file_io.cpp $(SRCDIR)/shape_generator.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) -lncurses

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
clean:
	rm -f $(OBJS) $(TARGET)
run: $(TARGET)
	./%(TARGET)


