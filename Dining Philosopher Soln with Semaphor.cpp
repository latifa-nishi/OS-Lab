#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>

using namespace std;

#define N 5  // Number of philosophers

sem_t forks[N];      // Renamed from fork to forks
mutex print_mutex;

void print_message(string message) {
    print_mutex.lock();
    cout << message << endl;
    print_mutex.unlock();
}

void think() {
    usleep((rand() % 1000 + 500) * 1000);
}

void eat() {
    usleep((rand() % 1000 + 500) * 1000);
}

void pick_up_forks(int i) {
    if (i % 2 == 0) {
        sem_wait(&forks[i]);
        print_message("Philosopher " + to_string(i) + " picked up fork " + to_string(i) + ".");
        usleep(100000);
        
        sem_wait(&forks[(i + 1) % N]);
        print_message("Philosopher " + to_string(i) + " picked up fork " + to_string((i + 1) % N) + ".");
    }
    else {
        sem_wait(&forks[(i + 1) % N]);
        print_message("Philosopher " + to_string(i) + " picked up fork " + to_string((i + 1) % N) + ".");
        
        sem_wait(&forks[i]);
        print_message("Philosopher " + to_string(i) + " picked up fork " + to_string(i) + ".");
    }
}

void put_down_forks(int i) {
    sem_post(&forks[i]);
    print_message("Philosopher " + to_string(i) + " put down fork " + to_string(i) + ".");
    
    sem_post(&forks[(i + 1) % N]);
    print_message("Philosopher " + to_string(i) + " put down fork " + to_string((i + 1) % N) + ".");
}

void philosopher(int i) {
    print_message("Philosopher " + to_string(i) + " is thinking.");
    think();

    pick_up_forks(i);
    print_message("Philosopher " + to_string(i) + " is eating.");
    eat();

    put_down_forks(i);
}


int main() {
    srand(time(NULL));
    thread philosophers[N];
    
    for(int i = 0; i < N; i++) {
        sem_init(&forks[i], 0, 1);
    }
    
    for(int i = 0; i < N; i++) {
        philosophers[i] = thread(philosopher, i);
    }
    
    for(int i = 0; i < N; i++) {
        philosophers[i].join();
    }
    
    for(int i = 0; i < N; i++) {
        sem_destroy(&forks[i]);
    }
    
    return 0;
}