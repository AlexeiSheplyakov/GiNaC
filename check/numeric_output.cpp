/** @file numeric_output.cpp
 *
 *  Test output of numeric types. */

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

#include <ginac/ginac.h>

#ifndef NO_NAMESPACE_GINAC
using namespace GiNaC;
#endif // ndef NO_NAMESPACE_GINAC

unsigned numeric_output(void)
{
    unsigned result = 0;
    
    cout << "checking output of numeric types..." << flush;
    clog << "---------output of numeric types:" << endl;
    
    unsigned long Digits_before = Digits;
    Digits = 222;
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
