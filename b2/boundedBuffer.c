#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <pthread.h>

#define MAX_ITER (100)

struct Buffer {
  char *buf;
  int in;
  int out;
  int count;
  int size;
  pthread_cond_t empty;
  pthread_cond_t full;
  pthread_mutex_t m;
};

void put(struct Buffer *b, char c) {
  pthread_mutex_lock(&b->m);
  printf("put\n");
  while (b->count==b->size) {
    printf("waitput\n");
    pthread_cond_wait(&b->full, &b->m);
  }
  b->buf[b->in] = c;
  b->in = ((b->in)+1)%(b->size);
  ++(b->count);
  pthread_cond_signal(&b->empty);
  pthread_mutex_unlock(&b->m);
}

char get(struct Buffer *b) {
  pthread_mutex_lock(&b->m);
  printf("get\n");
  while (b->count==0) {
    printf("waitget\n");
    pthread_cond_wait(&b->empty, &b->m);
  }
  char c = b->buf[b->out];
  b->buf[b->out] = '\0';
  b->out = ((b->out)+1)%(b->size);
  --(b->count);
  pthread_cond_signal(&b->full);
  pthread_mutex_unlock(&b->m);
  return c;
}

void initBuffer(struct Buffer *b, int size) {
  b->in = 0; b->out = 0; b->count = 0;
  b->size = size;
  pthread_mutex_init(&b->m, NULL);
  pthread_cond_init(&b->empty, NULL);
  pthread_cond_init(&b->full, NULL);
  b->buf = (char*)malloc(sizeof(char) * size);
  if (b->buf == NULL)
    exit(-1);
};

void destroyBuffer(struct Buffer* b) {
  free(b->buf);
};

void *producerFunc(void *param) {
  struct Buffer* b = (struct Buffer*)param;
  int i;
  for (i = 0; i < MAX_ITER; ++i) {
    // get random small ascii character
    char c = (char)(26 * (rand() / (RAND_MAX + 1.0))) + 97;
    put(b, c);
    usleep( getRandSleepTime()*2 );
  }
};

int getRandSleepTime() {
  /* return rand() % 100 + 750000; */
  return rand() % 100000;
}

void *consumerFunc(void *param) {
  struct Buffer* b = (struct Buffer*)param;
  int i;
  for (i = 0; i < MAX_ITER; ++i) {
    printf("%c\n", get(b));
    usleep( getRandSleepTime());
  };
};

int main(int argc, char **argv) {
  pthread_t producer, consumer;

  srand(time(NULL));

  struct Buffer b;
  initBuffer(&b, 10);

  pthread_create( &producer, NULL, producerFunc, &b );
  pthread_create( &consumer, NULL, consumerFunc, &b );

  pthread_join( producer, NULL );
  pthread_join( consumer, NULL );

  destroyBuffer(&b);
  return 0;
}
