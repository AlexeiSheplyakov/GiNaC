/** @file expand_subs.cpp
 *
 *  The first test routine implements Denny Fliegner's quick consistency check:
 *     e = (a0 + a1 + a2 + a3 + ...)^2
 *     expand e
 *     substitute a0 by (-a2 - a3 - ...) in e
 *     expand e
 *  after which e should be just a1^2.
 *  In addition, a simpler modification is tested in the second test:
 *     e = (a0 + a1)^200
 *     expand e
 *     substitute a0 by -a1 in e
 *  after which e should return 0 (without expanding).
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

#define VECSIZE 100

static unsigned expand_subs1(void)
{
    symbol a1("a1");
    symbol a[VECSIZE];
    ex e, aux;

    a[1] = a1;
    for (unsigned i=0; i<VECSIZE; ++i) {
        e = e + a[i];
    }

    // prepare aux so it will swallow anything but a1^2:
    aux = -e + a[0] + a[1];
    e = expand(subs(expand(pow(e, 2)), a[0] == aux));

    if (e != pow(a1,2)) {
        clog << "Denny Fliegner's quick consistency check erroneously returned "
             << e << "." << endl;
        return 1;
    }
    return 0;
}

static unsigned expand_subs2(void)
{
    symbol a("a"), b("b");
    ex e, f;

    // Here the final expand() should be superflous. For no particular reason
    // at all, we don't use the wrapper-functions but the methods instead:
    e = pow(a+b,200).expand();
    f = e.subs(a == -b);

    if (f != 0) {
        clog << "e = pow(a+b,200).expand(); f = e.subs(a == -b); erroneously returned "
             << f << " instead of simplifying to 0." << endl;
        return 1;
    }
    return 0;
}

unsigned expand_subs(void)
{
    unsigned result = 0;

    cout << "checking commutative expansion and substitution..." << flush;
    clog << "---------commutative expansion and substitution:" << endl;

    result += expand_subs1();
    result += expand_subs2();

    if (! result) {
        cout << " passed ";
        clog << "(no output)" << endl;
    } else {
        cout << " failed ";
    }

    return result;
}
