#! ./run_exams --silent
unsigned result = 0;
/*
 * Old Cint versions messed up unary and binary operators:
 */
{
    symbol x;
    ex a = x-x;
    if (!a.is_zero()) {
        ++result;
    }
}
/*
 * Some problems with objects of class relational when cast to bool:
 */
{
    symbol x;
    ex a = x;
    if (a != a) {
        ++result;
    }
}
exit(result);
