#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <iomanip>
#include <thread>
using namespace std;

long double sum_total = 0;
long double taylor_sin_hilos(long int x , long int n_taylor){

	long double sum = 0.0;
	long double t2 = (long double)(x-1)/(x+1);

	for (int i=0; i<n_taylor; i++){
		long int t1 = (2*i) + 1;  
		sum = sum + (1.0/t1) * pow(t2,(2*i) + 1);  
	}	
	return 2 * sum;
}

void taylor_con_hilos(long int x , long int start , long int end){

	long double sum = 0.0;
	long double t2 = (long double)(x-1)/(x+1);

	for (int i=start; i<end; i++){
		long int t1 = (2*i) + 1;  
		sum = sum + (1.0/t1) * pow(t2,(2*i) + 1);  
	}	
	sum_total = sum_total + sum;
}

int main(){
	
	long int x = 1500000;
	long int n_taylor = 10000000;
	
	cout << "Comienzo de taylor sin hilos: " << endl;
	auto start_time_sin_hilos = chrono::high_resolution_clock::now();
	long double result = taylor_sin_hilos(x , n_taylor);
	auto end_time_sin_hilos = chrono::high_resolution_clock::now();
	chrono::duration<double> duracion_sin_hilos = end_time_sin_hilos - start_time_sin_hilos;
	cout<<"Resultado sin hilos: "<< setprecision(15) << result <<endl;
	cout << "Tiempo de ejecucion sin hilos: " << duracion_sin_hilos.count() << " segundos" << endl;
	cout << "*******************************************************" << endl;
	
	
	
	int n_threads = 4;
	long int part = n_taylor/n_threads; // particion
	long int start;
	long int end;
	
	auto start_time = chrono::high_resolution_clock::now();
	thread array_de_hilos[n_threads];
	
	for (int i=0 ; i<n_threads ; i++){
		if (i == 3){
			end = n_taylor;
		}else {
			end = (i+1) * part;		
		}
		start = i * part;
		array_de_hilos[i] = thread(taylor_con_hilos,x,start,end);
	}
	
	
	for (int i=0; i < n_threads; i++){
		if (array_de_hilos[i].joinable() == true){
			array_de_hilos[i].join();
		}
	}
	
	auto end_time = chrono::high_resolution_clock::now();
	chrono::duration<double> duracion = end_time - start_time;
	cout << "Resultado con hilos: " << setprecision(15) << 2 * sum_total << endl;
	cout << "Tiempo de ejecucion con hilos: " << duracion.count() << " segundos" << endl;
	cout << "*******************************************************" << endl;
	
}

