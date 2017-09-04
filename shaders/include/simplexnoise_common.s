#ifndef SIMPLEXNOISE_COMMON_S
#define SIMPLEXNOISE_COMMON_S

float skewF(float n)
{
/*
n∈N
X∈R^n
X'∈R^n
A∈R^n ∧ |A| = 1
s∈R
f is a non-uniform scaling along direction A and s is the scaling factor.
f:R^n → R^n
X' = f(X)
   = X・A*s*A - X・A*A + X
   = X・A*(s-1)*A + X

∀B(B∈R^n ∧ B・A = 0 ⇒ f(X)・B = X・B)

X' = f^-1(X)
   = X・A*(1/s-1)*A + X

f^-1(f(X)) = X
X' = X・A*(s-1)*A + X
X = X'・A*(1/s-1)*A + X'
  = (X・A*(s-1)*A + X)・A*(1/s-1)*A + X・A*(s-1)*A + X
  = X・A*(s-1)*(A・A)*(1/s-1)*A + X・A*(1/s-1)*A + X・A*(s-1)*A + X
  = X・A*(1-s-1/s+1)*A          + X・A*(1/s-1)*A + X・A*(s-1)*A + X
  = -X・A*(s-1)*A - X・A*(1/s-1)*A + X・A*(1/s-1)*A + X・A*(s-1)*A + X
  = X

When creating simplex noise, A is a unit vector parallel to a unit hypercube's longest diagonal.
A = (1/√(n), 1/√(n), ...)
  = 1/√(n)(1, 1, ...)

X' = f(X) = (s-1)/n*X・(1, 1, ...)*(1, 1, ...) + X

In skewed coordinate system, basis is not orthogonal.
Any points P in skewed coordinate system such that P∈Z^n become a vertex of a simplex.
In the Cartesian coordinate, all edge of a simplex should have a same length.
But it is not possible in 3D and higher dimension.
Equilateral triangle alone can fill space, but regular tetrahedra alone do not.
Find 's' such that |f^-1((1, 0, 0, ...))| = |f^-1((1, 1, 1, ...))|
|(1/s-1)/n*(1, 0, 0, ...)・(1, 1, ...)*(1, 1, ...) + (1, 0, 0, ...)| = |(1/s-1)/n*(1, 1, ...)・(1, 1, ...)*(1, 1, ...) + (1, 1, 1)|
|(1/s-1)/n*(1, 1, ...) + (1, 0, 0, ...)| = |(1/s-1)*(1, 1, ...) + (1, 1, 1)|
((1/s-1)/n+1)^2 + (((1/s-1)/n)^2)*(n-1) = ((1/s)^2)*n
(1/s-1)*(1/s-1)/(n*n)+ 2*(1/s-1)/n + 1 + (1/s-1)*(1/s-1)*(n-1)/(n*n) = n/(s*s)
2*(1/s-1)/n + 1 + (1/s-1)*(1/s-1)*n/(n*n) = n/(s*s)
2*(1/s-1)/n + 1 + (1/s-1)*(1/s-1)/n = n/(s*s)
(1/s-1)/n*(2 + (1/s-1)) + 1 = n/(s*s)
(1/s-1)/n*(1 + 1/s) + 1 = n/(s*s)
(1-s)/n*(s + 1) + s*s = n
(1-s)*(s + 1) + s*s*n = n*n
(n-1)*s*s + 1 = n*n
s*s = (n*n - 1)/(n-1) = (n+1)(n-1)/(n-1) = n+1
s = sqrt(n+1)

X' = f(X) = (√(n+1)-1)/n*X・(1, 1, ...)*(1, 1, ...) + X
f^-1(X) = (1/√(n+1)-1)/n*X・(1, 1, ...)*(1, 1, ...) + X

Length of edge of a simplex in Cartesian coordinate system:
 f^-1((1, 0, 0, ...))  = (1/√(n+1)-1)/n*(1, 0, 0, ...)・(1, 1, ...)*(1, 1, ...) + (1, 0, 0, ...)
                       = (1/√(n+1)-1)/n*(1, 1, ...) + (1, 0, 0, ...)
|f^-1((1, 0, 0, ...))| = √( ((1/√(n+1)-1)/n+1)^2 + (((1/√(n+1)-1)/n)^2)*(n-1) )
                       = √( ((1/√(n+1)-1)/n)^2 + 2*(1/√(n+1)-1)/n + 1 + (((1/√(n+1)-1)/n)^2)*(n-1) )
                       = √( (((1/√(n+1)-1)/n)^2)*n + 2*(1/√(n+1)-1)/n + 1 )
                       = √( (1/√(n+1)-1)/n*(1/√(n+1)-1 + 2) + 1 )
                       = √( (1/(n+1) - 1)/n + 1 )
                       = √( -n/(n+1)/n + 1 )
                       = √( -1/(n+1) + 1 )
                       = √( n/(n+1) )
https://www.wolframalpha.com/input/?i=sqrt(+((1%2Fsqrt(n%2B1)-1)%2Fn%2B1)^2+%2B+(((1%2Fsqrt(n%2B1)-1)%2Fn)^2)*(n-1)+)

Length of edges of a simplex in Cartesian coordinate system in 3 or higher dimension are not equal.
Y∈{0,1}^n
m = Y・(1, 1, ...)
f^-1(Y) = m*(1/√(n+1)-1)/n*(1, 1, ...) + Y
|f^-1(Y)| = √( ((m*(1/√(n+1)-1)/n+1)^2)*m + ((m*(1/√(n+1)-1)/n)^2)*(n-m) )
          = √( ((m*(1/√(n+1)-1)/n)^2)*m + 2*m*m*(1/√(n+1)-1)/n + m + ((m*(1/√(n+1)-1)/n)^2)*(n-m) )
          = √( ((m*(1/√(n+1)-1)/n)^2)*n + 2*m*m*(1/√(n+1)-1)/n + m )
          = √( (m*(1/√(n+1)-1)/n)*( (m*(1/√(n+1)-1)) + 2*m ) + m )
          = √( (m*(1/√(n+1)-1)/n)*m*( 1/√(n+1)+1 ) + m )
          = √( m*m*(1/(n+1)-1)/n + m )
          = √( m*m*(-n)/(n+1)/n + m )
          = √( -m*m/(n+1) + m )
d/dm(|f^-1(Y)|) = 0.5*(1 - 2*m/(n+1))/√( -m*m/(n+1) + m )
d/dm(|f^-1(Y)|) = 0 when m = 0.5*(n+1)
d/dm(|f^-1(Y)|) = 0.5*(1 - 2/(n+1))/√( n/(n+1) )
                = 0.5*(n-1)/√( n*(n+1) ) > 0 when m = 1
d/dm(|f^-1(Y)|) = 0.5*(1 - 2*n/(n+1))/√( n/(n+1) )
                = 0.5*(1-n)/√( n*(n+1) ) < 0 when m = n

So length of edge is shortest when m = 1 or m = n and other edge is longer than them.

Shortest distance between a vertex on simplex and the opposite edge:
L = √( n/(n+1) )*√(3)/2

References:
https://en.wikipedia.org/wiki/Simplex_noise
http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
*/
    return (sqrt(n + 1.0) - 1.0)/n;
}

float unskewG(float n)
{
    return (1.0/sqrt(n + 1.0) - 1.0)/n;
}

#endif
