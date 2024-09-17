#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex mtx;

struct ResultadoPrimos {
    int cantidad;
    std::vector<int> listaPrimos;
};

// Criba de Eratóstenes
ResultadoPrimos contarPrimos(int n) {
    if (n < 2) return {0, {}};

    std::vector<bool> esPrimo(n + 1, true);
    esPrimo[0] = esPrimo[1] = false;  
    ResultadoPrimos resultado;

    for (int i = 2; i * i <= n; ++i) {
        if (esPrimo[i]) {
            for (int j = i * i; j <= n; j += i) {
                esPrimo[j] = false;
            }
        }
    }

    for (int i = 2; i <= n; ++i) {
        if (esPrimo[i]) {
            resultado.listaPrimos.push_back(i);
        }
    }

    resultado.cantidad = resultado.listaPrimos.size();

    return resultado;
}

bool esPrimo(int num) {
    if (num <= 1) return false;
    if (num <= 3) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return false;
    }
    return true;
}

void contarPrimosEnRango(int start, int end, int& count, std::vector<int>& primes) {
    int local_count = 0;
    std::vector<int> local_primes;

    for (int i = start; i <= end; ++i) {
        if (esPrimo(i)) {
            local_count++;
            local_primes.push_back(i);
        }
    }

    // Bloqueo del mutex para proteger la actualización de variables compartidas
    //std::lock_guard<std::mutex> lock(mtx);
    count += local_count;
    primes.insert(primes.end(), local_primes.begin(), local_primes.end());
}

int main() {
    int numero;

    std::cout << "Introduce un número: ";
    std::cin >> numero;


    std::cout << "Números primos sin hilos:" << std::endl;
    std::cout << "Criba de Eratóstenes:" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    ResultadoPrimos resultado = contarPrimos(numero);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
  
    std::cout << "Cantidad de números primos hasta " << numero << ": " << resultado.cantidad << std::endl;
    std::cout << "Los 10 números primos más grandes son: ";

    // Mostrar los 10 últimos primos
    for (int i = resultado.cantidad - 10; i < resultado.cantidad; ++i) {
        std::cout << resultado.listaPrimos[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Tiempo: " << elapsed.count() << " segundos"<< std::endl;

    std::cout << "Algoritmo no tan eficiente:" << std::endl;
    int count;
    std::vector<int> primeList;
    start = std::chrono::high_resolution_clock::now();
    contarPrimosEnRango(0, numero, count, primeList);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
 
    std::cout << "Cantidad de números primos hasta " << numero << ": " << count << std::endl;
    std::cout << "Los 10 números primos más grandes son: ";

    // Mostrar los 10 últimos primos
    for (int i = count - 10; i < count; ++i) {
        std::cout << primeList[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Tiempo: " << elapsed.count() << " segundos"<< std::endl;

    std::cout << "Números primos con hilos:" << std::endl;
    int rango = numero / 4;
    std::vector<std::thread> threads;
    primeList = {};
    count = 0;
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 4; ++i) {
        int start = i * rango + 1;
        int end = (i == 4 - 1) ? numero : (start + 4 - 1);
        std::thread t(contarPrimosEnRango, start, end, std::ref(count), std::ref(primeList));
        threads.push_back(std::move(t));
    }
    for (auto& t : threads) {
        t.join();
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Cantidad de números primos hasta " << numero << ": " << count << std::endl;
    std::cout << "Los 10 números primos más grandes son: ";

    // Mostrar los 10 últimos primos
    for (int i = count - 10; i < count; ++i) {
        std::cout << primeList[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Tiempo: " << elapsed.count() << " segundos"<< std::endl;

    return 0;
}
