#include<iostream>
#include<cstdlib>
#include"acml.h"
//#include"cblas.h"
#include"papi.h"
#include<ctime>
using namespace std;

#define N 10000
#define d 1000
#define K 1000


int main(){
  double * matrix1 = new double[N * d];
  double * matrix2 = new double[d * K];
  double * matrix3 = new double[N * K];
  int ld1 = N;
  int ld2 = d;
  int ld3 = N;

  for (int i = 0; i < N*d; i++) {
    matrix1[i] = (double)rand();
  }
  for (int i = 0; i < d*K; i++) {
    matrix2[i] = (double)rand();
  }
  char Ntrans = 'N';
  double alpha = 1.0;
  double beta = 0.0;
  float real_time = 0.0;                                                                                              
  float proc_time = 0.0;                                                                                                    
  long long flpins = 0.0;                                                                                                   
  float mflops = 0.0;                                                                                                       
  //  enum CBLAS_TRANSPOSE NN = CblasNoTrans;
  //enum CBLAS_ORDER OR = CblasColMajor;
  if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) < PAPI_OK) {                                                     
    std::cout << "PAPI ERROR" << std::endl;                                                                                 
    //return -1;                                                                                                            
  }            
  clock_t start = clock(), diff;        
  cout <<"start" << endl;
    for (int i = 0; i < 10; i++)
    dgemm(Ntrans, Ntrans, N, K, d, alpha, matrix1, ld1, matrix2, ld2, beta, matrix3, ld3);
  cout << "end" << endl;
  diff = clock() - start;
  int msec = diff * 1000 / CLOCKS_PER_SEC;
  cout<<"Time taken" << msec/1000 << " seconds "<<msec%1000<<" milliseconds" << endl;

  if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) < PAPI_OK) {
    std::cout << "PAPI ERROR" << std::endl;
    //return -1;                                                                                                            
  }
  std::cout << "time:" << real_time <<"---flpins:"<<flpins<< "---mflops:" << mflops << std::endl;                                                 
  PAPI_shutdown();    

}
