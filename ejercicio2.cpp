/*---------------------------------------
* UNIVERSIDAD DEL VALLE DE GUATEMALA
* Autor: Eliazar Jose Pablo Canastuj Matias
*----------------------------------------
*/

#include <iostream>
#include <thread>              
#include <pthread.h>           
#include <chrono>              

// Variables globales compartidas
pthread_mutex_t candado = PTHREAD_MUTEX_INITIALIZER;  
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;       
int lbs_in_silo = 0;  
int lbs_in_warehouse = 0;  

// Función que simula una tostadora (roaster)
void tostadora(int id) {
    for (int i = 0; i < 200; ++i) {  
        std::this_thread::sleep_for(std::chrono::seconds(1));  
        
        pthread_mutex_lock(&candado);  
        lbs_in_silo += 1; 
        printf("Tostadora %d produjo: 1 lb de cafe tostada. Lbs de cafe en silo: %d\n", id, lbs_in_silo);
        pthread_cond_signal(&cond);  
        pthread_mutex_unlock(&candado);  
    }
}

// Función que simula una empacadora (packager)
void empacadora() {
    while (lbs_in_warehouse < 400) {  
        pthread_mutex_lock(&candado);  
        while (lbs_in_silo < 5 && lbs_in_warehouse < 400) {
            pthread_cond_wait(&cond, &candado);  
        }
        
        if (lbs_in_silo >= 5) {  
            for (int i = 0; i < 5; ++i) {
                std::this_thread::sleep_for(std::chrono::seconds(1));  
                lbs_in_silo -= 1;  
                lbs_in_warehouse += 1;  
                printf("Empacadora produjo: 1 bolsa de 1 lb de cafe. Lbs de cafe en bodega: %d\n", lbs_in_warehouse);
            }
        }
        pthread_mutex_unlock(&candado);  // Desbloquea el mutex
    }
}

// Función principal del programa
int main() {
    // Crea dos hilos para las tostadoras y un hilo para la empacadora
    std::thread t1(tostadora, 1);  
    std::thread t2(tostadora, 2);  
    std::thread packager(empacadora);  

    // Espera a que los tres hilos terminen su ejecución
    t1.join();
    t2.join();
    packager.join();

    // Mensaje final indicando que la producción ha terminado
    printf("Produccion completada. Lbs de cafe en bodega: %d\n", lbs_in_warehouse);

    // Destruir los mutex y condiciones
    pthread_mutex_destroy(&candado);
    pthread_cond_destroy(&cond);

    return 0;
}
