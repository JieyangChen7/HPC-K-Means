#include<iostream>
#include<cstdlib>

#define DATA 10000
#define FEATURE 1000

using namespace std;


int main(){
  for (int i = 0; i < DATA; i++) {
    for (int j = 0; j < FEATURE; j++) {
      cout << (double)rand() / RAND_MAX;
      if (j < FEATURE - 1) {
	cout << ",";
      }
    }
    cout << endl;

  }

}
