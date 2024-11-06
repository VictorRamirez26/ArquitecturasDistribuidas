#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mpi.h>
#include <chrono>

using namespace std;

// Función que calcula el array LPS
void LPS(const string& pattern, vector<int>& lps) {
    int length = 0;
    int i = 1;
    lps[0] = 0;

    while (i < pattern.length()) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        } else {
            if (length != 0) {
                length = lps[length - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

// Función que realiza la búsqueda KMP y cuenta las apariciones del patrón
int KMPSearch(const string& text, const string& pattern) {
    int M = pattern.length();
    int N = text.length();

    vector<int> lps(M);
    LPS(pattern, lps);

    int i = 0;  // índice para text[]
    int j = 0;  // índice para pattern[]
    int count = 0;  // Contador de apariciones

    while (i < N) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == M) { // Patrón encontrado
            count++;
            j = lps[j - 1];
        } else if (i < N && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    return count;
}

// Función que lee el contenido de un archivo en una cadena
string readFile(const string& file_path) {
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo: " << file_path << endl;
        return "";
    }
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Me aseguro de que tenemos exactamente 32 procesos
    if (size != 32) {
        if (rank == 0) {
            cerr << "Este programa requiere exactamente 32 procesos." << endl;
        }
        MPI_Finalize();
        return 1;
    }

    // El proceso 0 lee los archivos
    string textContent;
    vector<string> patterns(32);

    if (rank == 0) {
        textContent = readFile("./texto.txt");

        ifstream patternsFile("./patrones.txt");
        if (!patternsFile.is_open()) {
            cerr << "Error al abrir el archivo de patrones" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        for (int i = 0; i < 32; i++) {
            getline(patternsFile, patterns[i]);
        }
        patternsFile.close();
    }

    // Broadcast del texto a todos los procesos
    int textSize = textContent.size();
    MPI_Bcast(&textSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) textContent.resize(textSize); // Redimenciono el buffer donde se almacena el texto para los procesos distintos a rank 0
    MPI_Bcast(textContent.data(), textSize, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Broadcast de los patrones a todos los procesos
    string pattern = patterns[rank];
    int result = 0;

    // Medir el tiempo paralelo (comienza aquí en todos los procesos)
    MPI_Barrier(MPI_COMM_WORLD); // Asegurarnos de que todos los procesos empiezan al mismo tiempo
    chrono::time_point<chrono::high_resolution_clock> start, end;
    if (rank == 0) {
        start = chrono::high_resolution_clock::now();
    }

    // Cada proceso busca su propio patrón en el texto
    result = KMPSearch(textContent, pattern);

    // Recopilar resultados en el proceso maestro
    vector<int> results(32);
    MPI_Gather(&result, 1, MPI_INT, results.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Detener el cronómetro paralelo en el proceso maestro
    if (rank == 0) {
        end = chrono::high_resolution_clock::now();
        double parallelTime = chrono::duration<double>(end - start).count();

        // Imprimir resultados
        cout << "Resultados paralelos (con 32 procesos):" << endl;
        for (int i = 0; i < 32; i++) {
            cout << "El patrón " << i << " aparece " << results[i] << " veces\n";
        }

        // Imprimir el tiempo de ejecución paralelo
        cout << "Tiempo de ejecución paralelo: " << parallelTime << " segundos" << endl;
    }

    MPI_Finalize();
    return 0;
}

