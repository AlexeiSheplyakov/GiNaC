/** @file series_expansion.cpp
 *
 *  Series expansion test (Laurent and Taylor series). */

/*
 *  GiNaC Copyright (C) 1999-2000 Johannes Gutenberg University Mainz, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ginac.h"

#ifndef NO_NAMESPACE_GINAC
using namespace GiNaC;
#endif // ndef NO_NAMESPACE_GINAC

static symbol x("x");

static unsigned check_series(const ex &e, const ex &point, const ex &d, int order = 8)
{
    ex es = e.series(x, point, order);
    ex ep = ex_to_pseries(es).convert_to_poly();
    if (!(ep - d).is_zero()) {
        clog << "series expansion of " << e << " at " << point
             << " erroneously returned " << ep << " (instead of " << d
             << ")" << endl;
        (ep-d).printtree(clog);
        return 1;
    }
    return 0;
}

// Series expansion
static unsigned series1(void)
{
    unsigned result = 0;
    ex e, d;
    
    e = sin(x);
    d = x - pow(x, 3) / 6 + pow(x, 5) / 120 - pow(x, 7) / 5040 + Order(pow(x, 8));
    result += check_series(e, 0, d);
    
    e = cos(x);
    d = 1 - pow(x, 2) / 2 + pow(x, 4) / 24 - pow(x, 6) / 720 + Order(pow(x, 8));
    result += check_series(e, 0, d);
    
    e = exp(x);
    d = 1 + x + pow(x, 2) / 2 + pow(x, 3) / 6 + pow(x, 4) / 24 + pow(x, 5) / 120 + pow(x, 6) / 720 + pow(x, 7) / 5040 + Order(pow(x, 8));
    result += check_series(e, 0, d);
    
    e = pow(1 - x, -1);
    d = 1 + x + pow(x, 2) + pow(x, 3) + pow(x, 4) + pow(x, 5) + pow(x, 6) + pow(x, 7) + Order(pow(x, 8));
    result += check_series(e, 0, d);
    
    e = x + pow(x, -1);
    d = x + pow(x, -1);
    result += check_series(e, 0, d);
    
    e = x + pow(x, -1);
    d = 2 + pow(x-1, 2) - pow(x-1, 3) + pow(x-1, 4) - pow(x-1, 5) + pow(x-1, 6) - pow(x-1, 7) + Order(pow(x-1, 8));
    result += check_series(e, 1, d);
    
    e = pow(x + pow(x, 3), -1);
    d = pow(x, -1) - x + pow(x, 3) - pow(x, 5) + Order(pow(x, 7));
    result += check_series(e, 0, d);
    
    e = pow(pow(x, 2) + pow(x, 4), -1);
    d = pow(x, -2) - 1 + pow(x, 2) - pow(x, 4) + Order(pow(x, 6));
    result += check_series(e, 0, d);
    
    e = pow(sin(x), -2);
    d = pow(x, -2) + numeric(1,3) + pow(x, 2) / 15 + pow(x, 4) * 2/189 + Order(pow(x, 5));
    result += check_series(e, 0, d);
    
    e = sin(x) / cos(x);
    d = x + pow(x, 3) / 3 + pow(x, 5) * 2/15 + pow(x, 7) * 17/315 + Order(pow(x, 8));
    result += check_series(e, 0, d);
    
    e = cos(x) / sin(x);
    d = pow(x, -1) - x / 3 - pow(x, 3) / 45 - pow(x, 5) * 2/945 + Order(pow(x, 6));
    result += check_series(e, 0, d);
    
    e = pow(numeric(2), x);
    ex t = log(ex(2)) * x;
    d = 1 + t + pow(t, 2) / 2 + pow(t, 3) / 6 + pow(t, 4) / 24 + pow(t, 5) / 120 + pow(t, 6) / 720 + pow(t, 7) / 5040 + Order(pow(x, 8));
    result += check_series(e, 0, d.expand());
    
    e = pow(Pi, x);
    t = log(Pi) * x;
    d = 1 + t + pow(t, 2) / 2 + pow(t, 3) / 6 + pow(t, 4) / 24 + pow(t, 5) / 120 + pow(t, 6) / 720 + pow(t, 7) / 5040 + Order(pow(x, 8));
    result += check_series(e, 0, d.expand());
    
    return result;
}

// Series addition
static unsigned series2(void)
{
    unsigned result = 0;
    ex e, d;
    
    e = pow(sin(x), -1).series(x, 0, 8) + pow(sin(-x), -1).series(x, 0, 12);
    d = Order(pow(x, 6));
    result += check_series(e, 0, d);
    
    return result;
}

// Series multiplication
static unsigned series3(void)
{
    unsigned result = 0;
    ex e, d;
    
    e = sin(x).series(x, 0, 8) * pow(sin(x), -1).series(x, 0, 12);
    d = 1 + Order(pow(x, 7));
    result += check_series(e, 0, d);
    
    return result;
}

// Order term handling
static unsigned series4(void)
{
    unsigned result = 0;
    ex e, d;

    e = 1 + x + pow(x, 2) + pow(x, 3);
    d = Order(1);
    result += check_series(e, 0, d, 0);
    d = 1 + Order(x);
    result += check_series(e, 0, d, 1);
    d = 1 + x + Order(pow(x, 2));
    result += check_series(e, 0, d, 2);
    d = 1 + x + pow(x, 2) + Order(pow(x, 3));
    result += check_series(e, 0, d, 3);
    d = 1 + x + pow(x, 2) + pow(x, 3);
    result += check_series(e, 0, d, 4);
    return result;
}

// Series of special functions
static unsigned series5(void)
{
    unsigned result = 0;
    ex e, d;
    
    // gamma(-1):
    e = gamma(2*x);
    d = pow(x+1,-1)*numeric(1,4) +
        pow(x+1,0)*(numeric(3,4) -
                    numeric(1,2)*EulerGamma) +
        pow(x+1,1)*(numeric(7,4) -
                    numeric(3,2)*EulerGamma +
                    numeric(1,2)*pow(EulerGamma,2) +
                    numeric(1,12)*pow(Pi,2)) +
        pow(x+1,2)*(numeric(15,4) -
                    numeric(7,2)*EulerGamma -
                    numeric(1,3)*pow(EulerGamma,3) +
                    numeric(1,4)*pow(Pi,2) +
                    numeric(3,2)*pow(EulerGamma,2) -
                    numeric(1,6)*pow(Pi,2)*EulerGamma -
                    numeric(2,3)*zeta(3)) +
        pow(x+1,3)*(numeric(31,4) - pow(EulerGamma,3) -
                    numeric(15,2)*EulerGamma +
                    numeric(1,6)*pow(EulerGamma,4) +
                    numeric(7,2)*pow(EulerGamma,2) +
                    numeric(7,12)*pow(Pi,2) -
                    numeric(1,2)*pow(Pi,2)*EulerGamma -
                    numeric(2)*zeta(3) +
                    numeric(1,6)*pow(EulerGamma,2)*pow(Pi,2) +
                    numeric(1,40)*pow(Pi,4) +
                    numeric(4,3)*zeta(3)*EulerGamma) +
        Order(pow(x+1,4));
    result += check_series(e, -1, d, 4);
    
    // tan(Pi/2)
    e = tan(x*Pi/2);
    d = pow(x-1,-1)/Pi*(-2) +
        pow(x-1,1)*Pi/6 +
        pow(x-1,3)*pow(Pi,3)/360 +
        pow(x-1,5)*pow(Pi,5)/15120 +
        pow(x-1,7)*pow(Pi,7)/604800 +
        Order(pow(x-1,8));
    result += check_series(e,1,d,8);
    
    return result;
}

unsigned series_expansion(void)
{
    unsigned result = 0;
    
    cout << "checking series expansion..." << flush;
    clog << "---------series expansion:" << endl;
    
    result += series1();
    result += series2();
    result += series3();
    result += series4();
    result += series5();
    
    if (!result) {
        cout << " passed ";
        clog << "(no output)" << endl;
    } else {
        cout << " failed ";
    }
    return result;
}
