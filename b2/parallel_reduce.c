#include <stdio.h>

#include <pthread.h>

typedef struct {int len; int *data; int (*op)(int, int); int res;} threaddat_t;
int Num_Threads = 2;

int max(int a, int b)
{
  return (a>b) ? a : b;
}

int sum(int a, int b)
{
  return a+b;
}

int reduce(int (*op)(int, int),
           int *data,
           int len)
{
  int i;
  int result = data[0];
  for (i = 1; i < len; ++i)
    result = op(result, data[i]);
  return result;
}

void *threadFunction(void *param) {
  threaddat_t *dat = (threaddat_t*)param;
  dat->res = reduce(dat->op, dat->data, dat->len);
  pthread_exit(NULL);
}

int parallel_reduce(int (*op)(int, int),
                    int *data,
                    int len)
{
  pthread_t t[Num_Threads];
  threaddat_t threaddat[Num_Threads];
  int width_per_thread = len / Num_Threads;

  for (int i = 0; i < Num_Threads; ++i) {
    threaddat[i].data = data + i * width_per_thread;
    threaddat[i].len = width_per_thread;
    threaddat[i].op = op;

    pthread_create(&t[i], NULL, threadFunction, &threaddat[i]);
  }
  pthread_join(t[0], NULL);
  int res = threaddat[0].res;
  for (int i = 1; i < Num_Threads; ++i) {
    pthread_join(t[i], NULL);
    res = op(res, threaddat[i].res);
  }
  return res;
}

int main() {
  int data[] = {1,2,3,4,5,6,7,8,9,10};

  int m  = reduce(max, data, 10);
  int s = reduce(sum, data, 10);

  printf("max : %i; sum: %i\n", m, s);

  int pm = parallel_reduce(max, data, 10);
  int ps = parallel_reduce(sum, data, 10);

  printf("parallel max : %i; parallel sum: %i\n", pm, ps);
  return 0;
}
