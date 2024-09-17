#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;
mutex mtx;
float sum_total = 0;

// Lleno la matriz con el valor deseado
vector<vector<float>> llenar_matriz(float value, long int size) {
    return vector<vector<float>>(size, vector<float>(size, value));
}

// Función que multiplica las matrices en un rango de filas y actualiza la suma total
void multiplicar_matriz(vector<vector<float>>& matriz1, vector<vector<float>>& matriz2, vector<vector<float>>& resultado, long int start, long int end) {
	float sum = 0.f;
	long int size = matriz1.size();

	for (long int i = start; i < end; i++) { // Comienzo en la fila start y termino en la fila end
		for (long int j = 0; j < size; j++) { // Itero todas las columnas de la segunda matriz
		    resultado[i][j] = 0;
		    
		    // Itero la fila de la matriz1 y la columna de la matriz2 al mismo tiempo
		    for (long int k = 0; k < size; k++) {
			resultado[i][j] += matriz1[i][k] * matriz2[k][j];
		    }
		    // Sumatoria de todos los elementos de la matriz resultado
		    sum += resultado[i][j];
		}
	}

	// Acceso concurrente seguro a la variable global sum_total
	mtx.lock();
	sum_total += sum;
	mtx.unlock();
}

float multiplicar_matriz_sin_hilos(vector<vector<float>>& matriz1, vector<vector<float>>& matriz2, vector<vector<float>>& resultado, long int start, long int end) {
	float sum = 0.f;
	long int size = matriz1.size();

	for (long int i = start; i < end; i++) { // Comienzo en la fila start y termino en la fila end
		for (long int j = 0; j < size; j++) { // Itero todas las columnas de la segunda matriz
		    resultado[i][j] = 0;
		    
		    // Itero la fila de la matriz1 y la columna de la matriz2 al mismo tiempo
		    for (long int k = 0; k < size; k++) {
			resultado[i][j] += matriz1[i][k] * matriz2[k][j];
		    }
		    // Sumatoria de todos los elementos de la matriz resultado
		    sum += resultado[i][j];
		}
	}

	return sum;
}

void print_esquinas(vector<vector<float>>& matriz) {
	long int size = matriz.size();
	cout << "Esquinas de la matriz:" << endl;
	cout << "Esquina superior izquierda: " << matriz[0][0] << endl;
	cout << "Esquina superior derecha: " << matriz[0][size - 1] << endl;
	cout << "Esquina inferior izquierda: " << matriz[size - 1][0] << endl;
	cout << "Esquina inferior derecha: " << matriz[size - 1][size - 1] << endl;
	cout << "-----------------------------------------------------" << endl;
}

int main() {
	float value1 = 0.1f;
	float value2 = 0.2f;
	long int size = 3000; 

	// Inicializo las matrices
	vector<vector<float>> matriz1 = llenar_matriz(value1, size);
	vector<vector<float>> matriz2 = llenar_matriz(value2, size);
	vector<vector<float>> resultado_sin_hilos(size, vector<float>(size, 0));
	vector<vector<float>> resultado(size, vector<float>(size, 0));

	cout << "Comienzo de la multiplicación sin hilos: " << endl;
	auto start_time_sin_hilos = chrono::high_resolution_clock::now();
	float sum_total_sin_hilos = multiplicar_matriz_sin_hilos(ref(matriz1), ref(matriz2), ref(resultado_sin_hilos), 0, size);
	print_esquinas(resultado_sin_hilos);
	auto end_time_sin_hilos = chrono::high_resolution_clock::now();
	chrono::duration<double> duracion_sin_hilos = end_time_sin_hilos - start_time_sin_hilos;
	cout << "Resultado de la sumatoria sin hilos: " << sum_total_sin_hilos << endl;
	cout << "Tiempo de ejecucion sin hilos: " << duracion_sin_hilos.count() << " segundos" << endl;
	cout << "*******************************************************" << endl;
	
	
	cout << "Comienzo de la multiplicación con hilos: " << endl;
	int num_threads = 8; // Máximos de hilos de mi pc, con más hilos tarda más
	int part = size / num_threads; // Partición
	int end;
	
	auto start_time = chrono::high_resolution_clock::now();
	
	thread array_de_hilos[num_threads];
	// Creo y ejecuto los hilos 
	for (int i = 0; i < num_threads; i++) {
		long int start = i * part;
		if (i == num_threads - 1){
			end = size;
		}else {
			end = (i+1) * part;		
		}
		// Paso por referencia las matrices para que el resultado se escriba directamente 
		array_de_hilos[i] = thread(multiplicar_matriz, ref(matriz1), ref(matriz2), ref(resultado), start, end);
	}

	
	for (int i = 0; i < num_threads; i++) {
		if (array_de_hilos[i].joinable()) {
		    array_de_hilos[i].join();
		}
	}
	
	print_esquinas(resultado);
	auto end_time = chrono::high_resolution_clock::now();
	chrono::duration<double> duracion = end_time - start_time;

	cout << "Resultado de la sumatoria con hilos" << sum_total << endl;
	cout << "Tiempo de ejecucion con hilos: " << duracion.count() << " segundos" << endl;

	return 0;
}

