#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int func(int);

int read() {
  assert(((uintptr_t)__builtin_frame_address(0) + 8) % 16 == 0);
  int x;
  scanf("%d", &x);
  return x;
}

void print(int x) {
  assert(((uintptr_t)__builtin_frame_address(0) + 8) % 16 == 0);
  printf("%d\n", x);
}

int main(int argc, char *argv[]) {
  int n = argc > 1 ? atoi(argv[1]) : 4;
  int i = func(n);
  printf("In main printing return value of test: %d\n", i);
  return 0;
}
