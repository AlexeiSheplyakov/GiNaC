#! ./run_exams --silent
int result = 0;
symbol x("x"), y("y"), z("z");
{
    ex a = pow(x,2)-x-2;
    ex b = pow(x+1,2);
    ex s = a/b;
    if (!(s.diff(x)-2*pow(1+x,-3)*(-2-x+pow(x,2))-(-1+2*x)*pow(1+x,-2)).is_zero()) {
        ++result;
    }
    if (!(s.normal()-(-2+x)/(1+x)).is_zero()) {
        ++result;
    }
}
exit(result);
