CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -pthread

all: first second

first: first.cpp shared_array.cpp
	$(CXX) $(CXXFLAGS) first.cpp shared_array.cpp -o first $(LDFLAGS)

second: second.cpp shared_array.cpp
	$(CXX) $(CXXFLAGS) second.cpp shared_array.cpp -o second $(LDFLAGS)

clean:
	rm -f first second

