CXX ?= g++
CXXFLAGS = -std=c++17 -O2 -Wall
COMUN = $(wildcard comun/*.h)

todo: 1_comparar_todas/comparar 2_bst/app_bst 3_avl/app_avl 4_hash/app_hash

1_comparar_todas/comparar: 1_comparar_todas/main.cpp $(COMUN)
	$(CXX) $(CXXFLAGS) -o $@ $<

2_bst/app_bst: 2_bst/main.cpp $(COMUN)
	$(CXX) $(CXXFLAGS) -o $@ $<

3_avl/app_avl: 3_avl/main.cpp $(COMUN)
	$(CXX) $(CXXFLAGS) -o $@ $<

4_hash/app_hash: 4_hash/main.cpp $(COMUN)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f 1_comparar_todas/comparar 2_bst/app_bst 3_avl/app_avl 4_hash/app_hash
