#! ./run_exams --silent
int result = 0;
symbol x("x"), y("y"), z("z");
{
    ex a = pow(x,2)-x-2;
    ex b = pow(x+1,2);
    ex s = a/b;
    if (!(s.diff(x)+2/pow(1+x,3)*(-2-x+pow(x,2))-(-1+2*x)/pow(1+x,2)).is_zero()) {
        ++result;
    }
    if (!(s.normal()-(-2+x)/(1+x)).is_zero()) {
        ++result;
    }
}
//GiNaC-cint.function
ex EulerNumber(unsigned n)
{
    symbol xi;
    const ex generator = pow(cosh(xi),-1);
    return generator.diff(xi,n).subs(xi==0);
}
if (EulerNumber(42) != ex(numeric("-10364622733519612119397957304745185976310201"))) {
    ++result;
}
ex f = expand((x*y*z-1)*(x*y*z+3));
ex g = expand((x*y*z-1)*(x*y*z-3));
if (gcd(f, g) != x*y*z-1) {
    ++result;
}
exit(result);
