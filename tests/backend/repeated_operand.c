extern void print(int);
extern int read();

int func(int n) {
  int x;
  x = read();

  if (n + n < x + 1)
    print(1);
  else
    print(0);

  if (n * n > x - 1)
    print(1);
  else
    print(0);

  if (n - n < x * 2)
    print(1);
  else
    print(0);

  return n;
}
