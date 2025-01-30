#include <iostream>
#include <thread>
#include <semaphore.h>
#include <unistd.h>
#include <vector>
#include <mutex>

using namespace std;

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE]; // Fixed-size buffer
int in = 0, out = 0, item = 0;     // Buffer indexes

sem_t empty, full;       // Counting semaphores
mutex mtx;               // Mutex lock for mutual exclusion

void producer() {
    for (int i = 0; i < 5; i++) {
        sem_wait(&empty);  // Wait if buffer is full
        mtx.lock();        // Lock critical section

        // Add item to buffer
        buffer[in] = ++item;
        cout << "Produced item: " << item << " at index " << in << endl;
        in = (in + 1) % BUFFER_SIZE; // Move producer index

        mtx.unlock();      // Release lock
        sem_post(&full);   // Increment full slots

        usleep(1000000);   // Simulate production delay (1 second)
    }
}

void consumer() {
    for (int i = 0; i < 5; i++) {
        sem_wait(&full);   // Wait if buffer is empty
        mtx.lock();        // Lock critical section

        // Remove item from buffer
        int item = buffer[out];
        cout << "Consumed item: " << item << " from index " << out << endl;
        out = (out + 1) % BUFFER_SIZE; // Move consumer index

        mtx.unlock();      // Release lock
        sem_post(&empty);  // Increment empty slots

        usleep(2000000);   // Simulate consumption delay (2 seconds)
    }
}

int main() {
    // Initialize semaphores
    sem_init(&empty, 0, BUFFER_SIZE); // BUFFER_SIZE empty slots
    sem_init(&full, 0, 0);            // Initially no full slots

    // Create producer and consumer threads
    thread producer_thread(producer);
    thread consumer_thread(consumer);

    // Join threads (this will keep running indefinitely)
    producer_thread.join();
    consumer_thread.join();

    // Destroy semaphores (not reached in infinite loop)
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}