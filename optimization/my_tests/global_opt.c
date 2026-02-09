extern void print(int);
extern int read();

int func() {
  int x;
  int y;

  x = 1;
  y = read();

  if (y == 1) {
    print(x);
    x = 1;
    y = 0;
  }
  else {
    x = 1;
    y = x;
  }

  if (y == 1) {
    x = 1;
    y = x + 1;
  }

  return x;
}
