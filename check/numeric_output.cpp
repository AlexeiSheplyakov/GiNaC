// check/numeric_output.cpp

#include <GiNaC/ginac.h>

unsigned numeric_output(void)
{
    unsigned result = 0;
    
    cout << "checking output of numeric types..." << flush;
    clog << "---------output of numeric types:" << endl;
    
    unsigned long Digits_before = Digits;
    Digits = 200;
    clog << "Using " << Digits << " digits" << endl;
    clog << Pi << " evalfs to: " << Pi.evalf() << endl;
    clog << Catalan << " evalfs to: " << Catalan.evalf() << endl;
    clog << EulerGamma << " evalfs to: " << EulerGamma.evalf() << endl;
    clog << "Complex integers: ";
    clog << "{(0,0)=" << ex(0 + 0*I) << "} ";
    clog << "{(1,0)=" << ex(1 + 0*I) << "} ";
    clog << "{(1,1)=" << ex(1 + 1*I) << "} ";
    clog << "{(0,1)=" << ex(0 + 1*I) << "} ";
    clog << "{(-1,1)=" << ex(-1 + 1*I) << "} ";
    clog << "{(-1,0)=" << ex(-1 + 0*I) << "} ";
    clog << "{(-1,-1)=" << ex(-1 - 1*I) << "} ";
    clog << "{(0,-1)=" << ex(0 - 1*I) << "} ";
    clog << "{(1,-1)=" << ex(1 - 1*I) << "} " << endl;
    Digits = Digits_before;
    
    if (! result) {
        cout << " passed ";
    } else {
        cout << " failed ";
    }
    
    return result;
}
