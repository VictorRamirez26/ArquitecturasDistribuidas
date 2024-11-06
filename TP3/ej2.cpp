#include <mpi.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <chrono>

using namespace std;

long double taylor_sin_mpi(long int x , long int n_taylor) {
    long double sum = 0.0;
    long double t2 = (long double)(x-1)/(x+1);

    for (int i = 0; i < n_taylor; i++) {
        long int t1 = (2*i) + 1;
        sum += (1.0/t1) * pow(t2, (2*i) + 1);
    }
    return 2 * sum;
}

// Función que cada proceso MPI utilizará para calcular su parte de la serie de Taylor
long double taylor_mpi_parcial(long int x, long int start, long int end) {
    long double sum = 0.0;
    long double t2 = (long double)(x-1)/(x+1);

    for (int i = start; i < end; i++) {
        long int t1 = (2*i) + 1;
        sum += (1.0/t1) * pow(t2, (2*i) + 1);
    }
    return sum;
}

int main(int argc, char** argv) {
    int rank, size;

    // Inicialización de MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtengo el numero que identifica el proceso y lo guardo en rank
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtengo el numero total de procesos y lo guardo en size

    long int x = 1500000;
    long int n_taylor = 10000000;
    long double total_sum = 0.0;  // Variable para almacenar el resultado total

    // Solo el proceso 0 mide el tiempo de ejecución
    chrono::time_point<chrono::high_resolution_clock> start_time, end_time;
    if (rank == 0) {
        cout << "Comienzo de Taylor con MPI..." << endl;
        start_time = chrono::high_resolution_clock::now();
    }

    // División del trabajo entre los procesos
    long int range = n_taylor / size;
    long int start = rank * range;
    long int end = (rank == size - 1) ? n_taylor : start + range;

    // Cada proceso calcula su suma parcial
    long double local_sum = taylor_mpi_parcial(x, start, end);

    // Reducción: el proceso 0 reúne y suma todas las sumas parciales
    // MPI_Reduce(sendbuf, recvbuf, count, MPI_Datatype datatype, MPI_Op op, root, MPI_Comm comm);
    // sendbuf: Datos a enviar desde cada proceso
    // recvbuf: Buffer en el proceso raiz donde se realizará la reduccion
    // count: Numero de elementos en el buffer de envio
    // datatype: Tipo de dato de los elementos en el buffer
    // op: Operacion de reduccion que se realizará
    // root: Número de proceso que recibira el resultado de la reduccion
    // comm: Comunicador MPI
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // El proceso 0 multiplica el resultado total por 2 y muestra el tiempo
    if (rank == 0) {
        total_sum *= 2;
        end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> duracion = end_time - start_time;

        cout << "Resultado con " << size << " proceso" << (size > 1 ? "s" : "") << ": " << setprecision(15) << total_sum << endl;
        cout << "Tiempo de ejecucion con " << size << " proceso" << (size > 1 ? "s" : "") << ": " << duracion.count() << " segundos" << endl;
        cout << "*******************************************************" << endl;
    }

    // Finalización de MPI
    MPI_Finalize();
    return 0;
}
