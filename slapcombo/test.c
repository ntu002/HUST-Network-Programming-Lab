#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int done = 0;

void *worker_thread(void *arg) {
    // Thực hiện công việc...

    // Báo hiệu công việc hoàn tất
    pthread_mutex_lock(&mutex);
    done = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, &worker_thread, NULL);

    // Chờ đợi công việc hoàn tất
    pthread_mutex_lock(&mutex);
    while (done == 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    // Tiếp tục thực hiện công việc sau khi nhận được tín hiệu
    // ...
}
