extern void print(int);
extern int read();

int func() {
  int x;
  int y;
  int alive;

  alive = read();
  y = alive + 1;
  x = y + 3;
  y = read();
  x = y + 3;

  return x;
}
