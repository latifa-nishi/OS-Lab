#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <mutex>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
int item = 0;


void *producer_func(void *arg) {
for (int i = 0; i < 5; i++) {
buffer[in] = ++item; // item is produced
printf("Produced item = %d at index %d\n", item, in);
in = (in + 1) % BUFFER_SIZE;
usleep(1500);
}
return NULL;
}

void *consumer_func(void *arg) {
for (int i = 0; i < 5; i++) {
printf("Consumed item = %d at index %d\n", item, out); // consumer consumes the item
out = (out + 1) % BUFFER_SIZE;
usleep(30000);
}
return NULL;

}

int main() {
pthread_t producer_thread, consumer_thread;

pthread_create(&producer_thread, NULL, producer_func, NULL);
pthread_create(&consumer_thread, NULL, consumer_func, NULL);

pthread_join(producer_thread, NULL);
pthread_join(consumer_thread, NULL);

return 0;
}