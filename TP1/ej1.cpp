#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <iomanip>
using namespace std;

long double taylor_sin_hilos(long int x , long int n_taylor){

	long double sum = 0.0;
	long double t2 = (long double)(x-1)/(x+1);

	for (int i=0; i<n_taylor; i++){
		long int t1 = (2*i) + 1;  
		sum = sum + (1.0/t1) * pow(t2,(2*i) + 1);  
	}	
	return 2 * sum;
}


int main(){
	
	long int x = 1500000;
	long int n_taylor = 10000000;
	long double result = taylor_sin_hilos(x , n_taylor);
	cout<<"Result: "<< setprecision(15) << result <<endl;
	
}

