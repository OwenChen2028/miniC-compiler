extern void print(int);
extern int read();

int func(int n) {
  print(n + 1);
  print(n + 2);
  print(n + 3);
  print(n + 4);
  print(n + 1);
  print(n + 2);
  print(n + 3);
  print(n + 4);
  return n;
}
