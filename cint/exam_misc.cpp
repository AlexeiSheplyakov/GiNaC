#! ./run_exams --silent
unsigned result = 0;
{
    symbol a("a"), b("b");
    ex e, f;
    e = pow(a+b,200).expand();
    f = e.subs(a == -b);
    if (f != 0)
        ++result;
}
exit(result);
