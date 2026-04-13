CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2
TARGET   := dijkstras
SRC      := dijkstras.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
