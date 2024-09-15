#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <sstream>

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

int KMPSearch(const string& text, const string& pattern) {
    int M = pattern.length();
    int N = text.length();

    vector<int> lps(M);  // Crear array LPS
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

string readFile(const string& file_path) {
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo: " << file_path << endl;
        return "";
    }
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

void searchPatterns(const string& text, const vector<string>& patterns, int start, int end) {
    for (int i = start; i < end && i < patterns.size(); ++i) {
        int count = KMPSearch(text, patterns[i]);
        cout << "El patrón " << i << " aparece " << count << " veces\n";
    }
}

void searchPatternsWithThreads(const string& text, const vector<string>& patterns) {
    const int numThreads = 4;
    vector<thread> threads;
    int patternsPerThread = (patterns.size() + numThreads - 1) / numThreads;  // Redondear hacia arriba

    for (int i = 0; i < numThreads; ++i) {
        int start = i * patternsPerThread;
        int end = min((i + 1) * patternsPerThread, (int)patterns.size());

        // Crear un hilo para procesar un rango de patrones
        thread searchThread(searchPatterns, text, patterns, start, end);
        threads.push_back(move(searchThread));  // Mover el hilo al vector
    }

    // Esperar a que todos los hilos terminen
    for (thread& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
}


int main() {
    string patternsPath = "./patrones.txt";
    string textPath = "./texto.txt";

    string textContent = readFile(textPath);
    string patternsContent = readFile(patternsPath);

    istringstream patternsStream(patternsContent);
    vector<string> patterns;
    string pattern;

    while (getline(patternsStream, pattern)) {
        if (!pattern.empty()) {
            patterns.push_back(pattern);
        }
    }

    // Comparar la versión sin hilos y con hilos

    cout << "Versión sin hilos:\n";
    searchPatterns(textContent, patterns, 0, patterns.size()-1);

    cout << "\nVersión con hilos:\n";
    searchPatternsWithThreads(textContent, patterns);

    return 0;
}
