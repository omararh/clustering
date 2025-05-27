CXX = g++-14
CXXFLAGS = -fopenmp
COMMON_SOURCES = solver.cpp SolverInterval.cpp solverDP.cpp

medoids:
	$(CXX) $(CXXFLAGS) main.cpp $(COMMON_SOURCES) medoidsDP.cpp -o o.out
	@echo "✓ K-medoids compilé. Lancez: ./o.out"

median:
	$(CXX) $(CXXFLAGS) main-median.cpp $(COMMON_SOURCES) medianDP.cpp -o o.out
	@echo "✓ P-median compilé. Lancez: ./o.out"

clean:
	rm -f o.out