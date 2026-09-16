extern void print(int);
extern int read();

int func(int n) {
  int a;
  int b;
  int x;
  int y;
  int z;
  int w;

  a = n;
  b = n;
  x = n + 1;
  y = n + 2;
  z = n + 3;
  w = n + 4;

  a = 99;
  b = 100;
  w = 104;

  print(n + 1);
  print(n + 2);
  print(n + 3);
  print(n + 4);
  print(a);
  print(b);
  print(w);
  return n;
}
