
all: test


test_Util.o: ../Util.cpp ../Util.hpp test_Util.cpp
	g++ -c -I.. ../Util.cpp
	g++ -c -I.. test_Util.cpp

test_Util: test_Util.o Util.o
	g++ -o test_Util $^

test: test_Util
	./test_Util
