Toutes les informations essentielles concernant le projet sont présentées dans le fichier `rapport.pdf`

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

## Lancement du benchMark pour vérifier la ressemblance des solutions
g++-14 -std=c++17 -fopenmp -O3 -o benchmark test-main.cpp medoidsDP.cpp medianDP.cpp solverDP.cpp solverInterval.cpp solver.cpp -I.

./benchmark

un fichier `benchmark_cross_validation.csv` sera générer dans le dossier `results`
