CXX = g++
CXXFLAGS = -std=c++11 -pthread -Wall

all: libparallel_scheduler.so demo-app

parallel_scheduler.o: parallel_scheduler.cpp parallel_scheduler.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

libparallel_scheduler.so: parallel_scheduler.o
	$(CXX) -shared $^ -o $@

demo-app: demo.cpp libparallel_scheduler.so
	$(CXX) $(CXXFLAGS) demo.cpp -L. -lparallel_scheduler -Wl,-rpath,. -o $@

clean:
	rm -f *.o *.so demo-app

.PHONY: all clean
