/** @file paranoia_check.cpp
 *
 *  This set of tests checks for some of GiNaC's oopses which showed up during
 *  development.  Things were evaluated wrongly and so.  It should not find such
 *  a sick behaviour again.  But since we are paranoic and we want to exclude
 *  that behaviour for good...
 *
 *  GiNaC Copyright (C) 1999 Johannes Gutenberg University Mainz, Germany
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

#include <ginac/ginac.h>

// The very first pair of historic problems had its roots in power.cpp and was
// finally resolved on April 27th. (Fixing the first on April 23rd actually
// introduced the second.)
static unsigned paranoia_check1(void)
{
    unsigned result = 0;
    symbol x("x"), y("y"), z("z");
    ex e, f, g;

    e = x * y * z;
    f = y * z;
    g = e / f;

    // In the first one expand did not do any job at all:
    if ( !g.expand().is_equal(x) ) {
        clog << "e = x*y*z; f = y*z; expand(e/f) erroneously returned "
             << g.expand() << endl;
        ++result;
    }

    // This one somehow used to return 0:
    e = pow(x + 1, -1);
    if (!e.expand().is_equal(e)) {
        clog << "expand(pow(x + 1, -1)) erroneously returned "
             << e.expand() << endl;
        ++result;
    }

    return result;
}

// And here the second oops which showed up until May 17th 1999.  It had to do
// with lexicographic canonicalization and thus showed up only if the variables
// had the names as given here:
static unsigned paranoia_check2(void)
{
    unsigned result = 0;
    symbol x("x"), y("y"), z("z");
    ex e, f, g;

    e = x + z*x;
    f = e*y;
    g = f - e*y;

    // After .expand(), g should be zero:
    if (!g.expand().is_equal(exZERO())) {
        clog << "e = (x + z*x); f = e*y; expand(f - e*y) erroneously returned "
             << g.expand() << endl;
        ++result;
    }
    // After .eval(), g should be zero:
    if (!g.eval().is_equal(exZERO())) {
        clog << "e = (x + z*x); f = e*y; eval(f - e*y) erroneously returned "
             << g.eval() << endl;
        ++result;
    }
    // This actually worked already back in April.  But we are very paranoic!
    if (!g.expand().eval().is_equal(exZERO())) {
        clog << "e = (x + z*x); f = e*y; eval(expand(f - e*y)) erroneously returned "
             << g.expand().eval() << endl;
        ++result;
    }

    return result;
}

// The third bug was introduced on May 18, discovered on May 19 and fixed that
// same day.  It worked when x was substituted by 1 but not with other numbers:
static unsigned paranoia_check3(void)
{
    unsigned result = 0;
    symbol x("x"), y("y");
    ex e, f;

    e = x*y - y;
    f = e.subs(x == 2);

    if (!f.is_equal(y)) {
        clog << "e = x*y - y; f = e.subs(x == 2) erroneously returned "
             << f << endl;
        ++result;
    }
    if (!f.eval().is_equal(y)) {
        clog << "e = x*y - y; eval(e.subs(x == 2)) erroneously returned "
             << f.eval() << endl;
        ++result;
    }
    if (!f.expand().is_equal(y)) {
        clog << "e = x*y - y; expand(e.subs(x == 2)) erroneously returned "
             << f.expand() << endl;
        ++result;
    }

    return result;
}

// The fourth bug was also discovered on May 19 and fixed immediately:
static unsigned paranoia_check4(void)
{
    unsigned result = 0;
    symbol x("x");
    ex e, f, g;

    e = pow(x, 2) + x + 1;
    f = pow(x, 2) + x + 1;
    g = e - f;

    if (!g.is_equal(exZERO())) {
        clog << "e = pow(x,2) + x + 1; f = pow(x,2) + x + 1; g = e-f; g erroneously returned "
             << g << endl;
        ++result;
    }
    if (!g.is_equal(exZERO())) {
        clog << "e = pow(x,2) + x + 1; f = pow(x,2) + x + 1; g = e-f; g.eval() erroneously returned "
             << g.eval() << endl;
        ++result;
    }

    return result;
}

// The fifth oops was discovered on May 20 and fixed a day later:
static unsigned paranoia_check5(void)
{
    unsigned result = 0;
    symbol x("x"), y("y");

    ex e, f;
    e = pow(x*y + 1, 2);
    f = pow(x, 2) * pow(y, 2) + 2*x*y + 1;

    if (!(e-f).expand().is_equal(exZERO())) {
        clog << "e = pow(x*y+1,2); f = pow(x,2)*pow(y,2) + 2*x*y + 1; (e-f).expand() erroneously returned "
             << (e-f).expand() << endl;
        ++result;
    }

    return result;
}

// This one was discovered on Jun 1 and fixed the same day:
static unsigned paranoia_check6(void)
{
    unsigned result = 0;
    symbol x("x");

    ex e, f;
    e = pow(x, -5);
    f = e.denom();

    if (!f.is_equal(pow(x, 5))) {
        clog << "e = pow(x, -5); f = e.denom(); f was " << f << " (should be x^5)" << endl;
        ++result;
    }
    return result;
}

// This one was introduced on June 1 by some aggressive manual optimization.
// Discovered and fixed on June 2.
static unsigned paranoia_check7(void)
{
    unsigned result = 0;
    symbol x("x"), y("y");

    ex e = y + y*x + 2;
    ex f = expand(pow(e, 2) - (e*y*(x + 1)));

    if (f.nops() > 3) {
        clog << "e=y+y*x+2; f=expand(pow(e,2)-(e*y*(x+1))) has "
             << f.nops() << " arguments instead of 3 ( f=="
             << f << " )" << endl;
        ++result;
    }
    return result;
}

// This one was a result of the rewrite of mul::max_coefficient when we
// introduced the overall_coefficient field in expairseq objects on Oct 1.
// Fixed on Oct 4.
static unsigned paranoia_check8(void)
{
    unsigned result = 0;
    symbol x("x");

    ex e = -x / (x+1);
    ex f = e.normal();

    // The bug caused a division by zero in normal(), so the following
    // check is actually bogus...
    if (!f.is_equal(e)) {
        clog << "normal(-x/(x+1)) returns " << f << " instead of -x/(x+1)\n";
        ++result;
    }
    return result;
}

unsigned paranoia_check(void)
{
    unsigned result = 0;

    cout << "checking several ex-bugs just out of pure paranoia..." << flush;
    clog << "---------several ex-bugs just out of pure paranoia:" << endl;

    result += paranoia_check1();
    result += paranoia_check2();
    result += paranoia_check3();
    result += paranoia_check4();
    result += paranoia_check5();
    result += paranoia_check6();
    result += paranoia_check7();
    result += paranoia_check8();

    if (! result) {
        cout << " passed ";
        clog << "(no output)" << endl;
    } else {
        cout << " failed ";
    }

    return result;
}
