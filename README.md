# éxécution

# requis : 
- cpp 14 ou plus
- openMP

## k-medoids
g++-14 -fopenmp main.cpp solver.cpp SolverInterval.cpp solverDP.cpp medoidsDP.cpp -o medoids

./medoids

## p-median
g++-14 -fopenmp main-median.cpp solver.cpp SolverInterval.cpp solverDP.cpp medianDP.cpp -o median

./median


## ou utiliser tous simplement le `makefile`
### k-medoids
make medoids 

./o.out

### p-median
make median

./o.out