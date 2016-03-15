gcc -std=c++11 kmeans_main.cpp -I $MLPACK_INC -I $BOOST_ROOT -I $PAPIDIR/src -I $ARMADILLO_INCLUDE_DIR  -lpapi -lmlpack 
./a.out -c 1000 -i data.csv