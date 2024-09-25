#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex mtx; 
std::condition_variable cv; 
int lbs_in_silo = 0; 
int lbs_in_warehouse = 0; 
bool done = false; 


void tostadora(int id) {
    for (int i = 0; i < 200; ++i) { 
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        std::lock_guard<std::mutex> lock(mtx); 
        lbs_in_silo += 1;
        std::cout << "Tostadora " << id << " produjo: 1 lb de cafe tostada. Lbs de cafe en silo: " << lbs_in_silo << "\n";
        cv.notify_all();
    }
}


void empacadora() {
    while (lbs_in_warehouse < 400) { 
        std::unique_lock<std::mutex> lock(mtx); 
        cv.wait(lock, [] { return lbs_in_silo >= 5 || lbs_in_warehouse >= 400; }); 
        if (lbs_in_silo >= 5) {
            for (int i = 0; i < 5; ++i) {
                std::this_thread::sleep_for(std::chrono::seconds(1)); 
                lbs_in_silo -= 1;
                lbs_in_warehouse += 1;
                std::cout << "Empacadora produjo: 1 bolsa de 1 lb de cafe. Lbs de cafe en bodega: " << lbs_in_warehouse << "\n";
            }
        }
    }
}

int main() {
    std::thread t1(tostadora, 1);
    std::thread t2(tostadora, 2);
    std::thread packager(empacadora);

    t1.join();
    t2.join();
    packager.join();

    std::cout << "Produccion completada. Lbs de cafe en bodega: " << lbs_in_warehouse << "\n";
    return 0;
}