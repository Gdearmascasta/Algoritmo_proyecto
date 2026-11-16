CXX ?= g++

comparar: src/main.cpp src/libro.h src/arbol_bst.h src/arbol_avl.h src/tabla_hash.h
	$(CXX) -std=c++17 -O2 -Wall -o comparar src/main.cpp

clean:
	rm -f comparar
