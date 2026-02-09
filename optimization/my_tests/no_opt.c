extern void print(int);
extern int read();

int func() {
  int x;
  int y;
  int alive;

  alive = 1;
  y = alive + 1;
  x = y + 3;
  y = 4;
  x = y + 3;

  return x;
}
