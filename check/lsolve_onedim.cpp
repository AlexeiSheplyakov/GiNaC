// check/lsolve_onedim.cpp

/* This test routine does some simple checks on solving a polynomial for a
 * variable. */

#include "ginac.h"

unsigned lsolve_onedim(void)
{
    unsigned result = 0;
    symbol x("x");
    ex eq, aux;
    
    cout << "checking linear solve..." << flush;
    clog << "---------linear solve:" << endl;
    
    eq = (3*x+5 == numeric(8));
    aux = lsolve(eq,x);
    if (aux != 1) {
        result++;
        clog << "solution of 3*x+5==8 erroneously returned "
             << aux << endl;
    }
    
    if (! result) {
        cout << " passed ";
        clog << "(no output)" << endl;
    } else {
        cout << " failed ";
    }
    
    return result;
}
