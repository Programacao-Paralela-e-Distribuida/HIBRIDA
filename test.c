
#include <stdio.h>
#include <omp.h>

int main() {
  int numdevices = omp_get_num_devices();
  int device= omp_get_device_num();
  int max_threads = omp_get_max_threads();
  printf("number of devices= %d *** device= %d\n", numdevices, device);
  printf("max number of threads: %d\n", max_threads);
}
