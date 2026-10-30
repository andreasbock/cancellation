# Numerical cancellation

Consider the function
$$f(x) = \frac{1 - \cos(x)}{x^2}, \qquad x \neq 0,$$
which satisfies $f(x) \to \tfrac12$ as $x \to 0$ (apply L'Hôpital's rule twice).
## A problematic implementation

Evaluating $f$ in floating-point arithmetic is inaccurate for inputs of small magnitude.

The numerator would be one minus a number very close to one, it will likely get rounded off to zero when $x$ is too small.

Let's see this in action:

```
#include <math.h>
#include <stdio.h>

double f_naive(double x) {
    return (1 - cos(x)) / pow(x, 2);
}

int main (int argc, char *argv[]) {
    double xs[] = {1e-01, 1e-02, 1e-04, 1e-08, 1e-16};
    int length = sizeof(xs) / sizeof(xs[0]);

    for (int i = 0; i < length; i++) {
        printf("f(%f) = %f\n", xs[i], f_naive(xs[i]));
    }
	return 0;
}
```

Running this gives us:

```
$ gcc -c -o cancellation.o cancellation.c
$ gcc cancellation.o -o cancellation
$ ./cancellation
f(0.100000) = 0.499583
f(0.010000) = 0.499996
f(0.000100) = 0.500000
f(0.000000) = 0.000000
f(0.000000) = 0.000000
```

Sad panda.
## The solution

We can rewrite $f$ using the half-angle identity $1-\cos(x) = 2 \sin(x/2)^2$:
$$f(x) = \frac{2 \sin(x/2)^2}{x^2}, \qquad x\neq 0.$$
Huzzah, no subtraction!

Let's see this in action again:

```
#include <math.h>
#include <stdio.h>

double f(double x) {
    return 2 * pow(sin(x/2), 2) / pow(x, 2);
}

int main (int argc, char *argv[]) {
    double xs[] = {1e-01, 1e-02, 1e-04, 1e-08, 1e-16};
    int length = sizeof(xs) / sizeof(xs[0]);

    for (int i = 0; i < length; i++) {
        printf("f(%f) = %f\n", xs[i], f(xs[i]));
    }
	return 0;
}
```

Running this gives us:

```
$ gcc -c -o cancellation.o cancellation.c
$ gcc cancellation.o -o cancellation
$ ./cancellation
f(0.100000) = 0.499583
f(0.010000) = 0.499996
f(0.000100) = 0.500000
f(0.000000) = 0.500000
f(0.000000) = 0.500000
```

Yay!
## The reason

The condition number of $f$ at a point $x\neq 0$ is
$$\kappa_f(x) = \Bigg| \frac{x f'(x)}{f(x)} \Bigg| = \Bigg| \frac{x\sin(x)}{1-\cos(x)} - 2\Bigg|.$$
This really isn't the problem; around $0$, this is well-behaved. 

The issue is the subtraction function $g(y) = 1-y$, which is ill-conditioned around $y \approx 1$, which is the case when $x \approx 0$ since $\cos(0)\approx 1$:
$$\kappa_g(\cos(x)) =\left|\frac{\cos (x)}{1 - \cos (x)}\right|.$$This approaches $\infty$ as $x$ goes to $0$. Ouch.
 
We could be fooled into thinking our implementation of `f_naive` was fine, but the condition number of $f$ does not say anything about the subtraction step!

And it's not even subtraction in and of itself, it's where the subtraction is evaluated.
