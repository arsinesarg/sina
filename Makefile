CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-deprecated-declarations -pthread

TARGET = mmm
SRC = mmm.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

