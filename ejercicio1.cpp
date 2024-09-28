/*---------------------------------------
* UNIVERSIDAD DEL VALLE DE GUATEMALA
* Autor: Eliazar Jose Pablo Canastuj Matias
*----------------------------------------
*/


#include <iostream>
#include <thread>
#include <vector>
#include <pthread.h>
#include <chrono>
#include <cstdio>

// Definir pthread_mutex_t
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

struct Producto {
    const char* nombre;
    double precioUnitario;
    double costoFijo;
    int unidadesVendidas;
    double ventas;
    double utilidad;
};

struct ThreadData {
    int id;            // Identificador del hilo
    Producto* producto; // Puntero al producto que el hilo debe procesar
};

void* calcularProducto(void* arg) {
    // Obtener los datos del hilo (ID y producto)
    ThreadData* data = (ThreadData*)arg;
    int id = data->id;
    Producto* producto = data->producto;

    // Bloqueo del mutex para imprimir
    pthread_mutex_lock(&mtx);
    printf("Calculo hilo %d iniciado\n", id);
    pthread_mutex_unlock(&mtx);

    // Simulacion del tiempo de calculo
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Cálculo de ventas y utilidad
    producto->ventas = producto->unidadesVendidas * producto->precioUnitario;
    producto->utilidad = producto->ventas - (producto->unidadesVendidas * producto->costoFijo);

    // Bloqueo del mutex para imprimir
    pthread_mutex_lock(&mtx);
    printf("Calculo hilo %d terminado\n", id);
    pthread_mutex_unlock(&mtx);

    return nullptr;
}

void generarReporte(const std::vector<Producto>& productos, const char* mes) {
    double totalVentas = 0, totalUtilidad = 0;

    // Calculo total de ventas y utilidades del mes
    for (const auto& producto : productos) {
        totalVentas += producto.ventas;
        totalUtilidad += producto.utilidad;
    }

    // Imprimir reporte del mes
    printf("------------------------------------------\n");
    printf("REPORTE DEL MES DE %s\n", mes);
    printf("--- Monto Ventas por Producto ---\n");
    for (const auto& producto : productos) {
        printf("- %s: Q%.2f\n", producto.nombre, producto.ventas);
    }

    printf("--- Utilidad por Producto ---\n");
    for (const auto& producto : productos) {
        printf("%s: Q%.2f\n", producto.nombre, producto.utilidad);
    }

    printf("--- Monto Total Ventas del Mes: Q%.2f\n", totalVentas);
    printf("--- Utilidad del mes: Q%.2f\n", totalUtilidad);
    printf("------------------------------------------\n");
}

int main() {
    // Definir los productos del mes de julio
    std::vector<Producto> productosJulio = {
        {"Porcion pastel de chocolate", 60, 20, 300},
        {"White mocha", 32, 19.2, 400},
        {"Cafe americano 8onz", 22, 13.2, 1590},
        {"Latte 8onz", 24, 17.2, 200},
        {"Toffee coffee", 28, 20.1, 390},
        {"Cappuccino 8onz", 24, 17.2, 1455},
        {"S'mores Latte", 32, 23, 800},
        {"Cafe tostado molido", 60, 20, 60}
    };

    // Definir los productos del mes de agosto
    std::vector<Producto> productosAgosto = {
        {"Porcion pastel de chocolate", 60, 20, 250},
        {"White mocha", 32, 19.2, 380},
        {"Cafe americano 8onz", 22, 13.2, 800},
        {"Latte 8onz", 24, 17.2, 250},
        {"Toffee coffee", 28, 20.1, 600},
        {"Cappuccino 8onz", 24, 17.2, 1200},
        {"S'mores Latte", 32, 23, 1540},
        {"Cafe tostado molido", 60, 20, 15}
    };

    // Definir los hilos para julio
    std::vector<pthread_t> hilosJulio(productosJulio.size());
    std::vector<ThreadData> dataJulio(productosJulio.size()); // Arreglo para datos de hilos

    // Lanzar los hilos para procesar los productos de julio
    for (int i = 0; i < productosJulio.size(); ++i) {
        dataJulio[i].id = i;
        dataJulio[i].producto = &productosJulio[i];
        pthread_create(&hilosJulio[i], nullptr, calcularProducto, &dataJulio[i]);
    }

    // Esperar a que todos los hilos de julio terminen
    for (auto& hilo : hilosJulio) {
        pthread_join(hilo, nullptr);
    }

    // Generar el reporte para el mes de julio
    generarReporte(productosJulio, "JULIO");

    // Definir los hilos para agosto
    std::vector<pthread_t> hilosAgosto(productosAgosto.size());
    std::vector<ThreadData> dataAgosto(productosAgosto.size()); // Arreglo para datos de hilos

    // Lanzar los hilos para procesar los productos de agosto
    for (int i = 0; i < productosAgosto.size(); ++i) {
        dataAgosto[i].id = i;
        dataAgosto[i].producto = &productosAgosto[i];
        pthread_create(&hilosAgosto[i], nullptr, calcularProducto, &dataAgosto[i]);
    }

    // Esperar a que todos los hilos de agosto terminen
    for (auto& hilo : hilosAgosto) {
        pthread_join(hilo, nullptr);
    }

    // Generar el reporte para el mes de agosto
    generarReporte(productosAgosto, "AGOSTO");

    // Destruir el mutex
    pthread_mutex_destroy(&mtx);

    return 0;
}
