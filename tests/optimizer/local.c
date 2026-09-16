extern void print(int);
extern int read();

int func() {
  int x;
  int y;

  y = 1 + 2;
  x = y + 3;
  print(y);
  x = y + 3;

  return x;
}
