#include <stdio.h>
#include <omp.h>
//  Use este programa para verificar se as opções de compilação estão corretas 
//  e a(s) GPUS(s) conseguem ser identificadas.
int main() {
  #pragma omp target device(1)
  {
  int numdevices = omp_get_num_devices();
  int device= omp_get_default_device();
  printf("Número de dispositivos = %d *** Dispositivo = %d\n", numdevices, device);
  }
}

