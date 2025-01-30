#include <iostream>
#include <thread>
#include <unistd.h> // for usleep
#include <mutex>    // for mutex

using namespace std;

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
int item = 0;
mutex mtx; // Mutex for synchronization

void producer_func() {
    for (int i = 0; i < 5; i++) {
        mtx.lock(); // Lock the mutex before accessing the buffer
        buffer[in] = ++item; // Produce an item
        cout << "Produced item = " << item << " at index " << in << endl;
        in = (in + 1) % BUFFER_SIZE; // Move the producer index
        mtx.unlock(); // Unlock the mutex after accessing the buffer
        usleep(1500); // Simulate work (1500 microseconds = 1.5 milliseconds)
    }
}

void consumer_func() {
    for (int i = 0; i < 5; i++) {
        mtx.lock(); // Lock the mutex before accessing the buffer
        int consumed_item = buffer[out]; // Consume an item
        cout << "Consumed item = " << consumed_item << " at index " << out << endl;
        out = (out + 1) % BUFFER_SIZE; // Move the consumer index
        mtx.unlock(); // Unlock the mutex after accessing the buffer
        usleep(30000); // Simulate work (30000 microseconds = 30 milliseconds)
    }
}

int main() {

    
    thread producer_thread(producer_func);
    thread consumer_thread(consumer_func);

    producer_thread.join();
    consumer_thread.join();

    return 0;
}