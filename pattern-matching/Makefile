CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2
TARGET   := pattern-matching
SRC      := pattern-matching.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
