#include <math.h>
#include <stdio.h>

double f_naive(double x) {
    return (1 - cos(x)) / pow(x, 2);
}

double f_improved(double x) {
    return 2 * pow(sin(x/2), 2) / pow(x, 2);
}

int main (int argc, char *argv[]) {
    double xs[] = {1e-01, 1e-02, 1e-04, 1e-08, 1e-16};
    int length = sizeof(xs) / sizeof(xs[0]);

    for (int i = 0; i < length; i++) {
        printf("f(%f) = %f\n", xs[i], f_naive(xs[i]));
    }

    for (int i = 0; i < length; i++) {
        printf("f(%f) = %f\n", xs[i], f_improved(xs[i]));
    }

	return 0;
}
