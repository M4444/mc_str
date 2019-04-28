CXX = g++
CXXFLAGS = -g -Wall

test: str_test
	./str_test

str_test: str_test.o
	$(CXX) $(CXXFLAGS) -o str_test str_test.o

str_test.o: str.h

clean:
	rm -f str_test.o str_test
