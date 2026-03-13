extern void print(int);
extern int read();

int func(int n){
        int i;
        int j;
        int sum;
        int prod;
        int acc;
        int x;
        int y;
        int z;
        int flag;
        int limit;
        int tmp;

        i = 0;
        j = 0;
        sum = 0;
        prod = 1;
        acc = 0;
        x = 0;
        y = 1;
        z = 2;
        flag = 0;
        limit = n + 5;
        tmp = 0;

        while (i < limit){
                x = read();
                y = read();
                z = x + y;
                sum = sum + z;

                if (z > n){
                        prod = prod * 2;
                        acc = acc + z;
                } else {
                        prod = prod + 3;
                        acc = acc - z;
                }

                if (x == y){
                        flag = flag + 1;
                } else {
                        flag = flag - 1;
                }

                {
                        int a;
                        int b;
                        int c;
                        int d;

                        a = z + i;
                        b = a - y;
                        c = b * x;
                        d = c - a;

                        if (d < 0){
                                d = -d;
                        } else {
                                d = d + 1;
                        }

                        sum = sum + d;
                        prod = prod + a;
                        acc = acc + b;
                        tmp = d + c;
                }

                if (tmp > 1000){
                        print(tmp);
                        return tmp - n;
                }

                j = 0;
                while (j < 3){
                        {
                                int p;
                                int q;
                                int r;

                                p = x + j;
                                q = y + p;
                                r = q - z;

                                if (r >= 0){
                                        sum = sum + r;
                                        acc = acc + p;
                                } else {
                                        sum = sum - r;
                                        acc = acc - q;
                                }

                                if (p != q){
                                        prod = prod + 1;
                                } else {
                                        prod = prod - 1;
                                }
                        }

                        j = j + 1;
                }

                if (flag > 5){
                        print(flag);
                } else {
                        print(sum);
                }

                if (prod > 5000){
                        return prod - sum;
                }

                i = i + 1;
        }

        if (acc < 0){
                acc = -acc;
        } else {
                acc = acc + 2;
        }

        tmp = sum + prod;
        tmp = tmp - acc;
        tmp = tmp + flag;

        if (tmp > n){
                return tmp;
        } else {
                return n - tmp;
        }
}
