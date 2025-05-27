# éxécution

## k-medoids
g++ -std=c++14 main.cpp solver.cpp SolverInterval.cpp solverDP.cpp medoidsDP.cpp -o medoids

./medoids

## p-median
g++ -std=c++14 main-median.cpp  solver.cpp SolverInterval.cpp solverDP.cpp medianDP.cpp -o median

./median