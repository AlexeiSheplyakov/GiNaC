// check/main.cpp

#include "ginac.h"
#include "check.h"

int main()
{
    unsigned result = 0;
    
    try {
        for (int i=0; i<1; ++i) {
            result += fcntimer(paranoia_check);
            result += fcntimer(numeric_output);
            result += fcntimer(numeric_consist);
            result += fcntimer(powerlaws);
            result += fcntimer(expand_subs);
            result += fcntimer(inifcns_consist);
            result += fcntimer(differentiation);
            result += fcntimer(poly_gcd);
            result += fcntimer(normalization);
            result += fcntimer(matrix_checks);
            result += fcntimer(lsolve_onedim);
            result += fcntimer(series_expansion);
        }
    } catch (exception const & e) {
        cout << "error: caught an exception: " << e.what() << endl;
    }
    
    if (result) {
        cout << "error: something went wrong. ";
        if (result == 1) {
            cout << "(one failure)" << endl;
        } else {
            cout << "(" << result << " individual failures)" << endl;
        }
        cout << "please check result.out against result.ref for more details."
             << endl << "happy debugging!" << endl;
    }

    return result;
}
