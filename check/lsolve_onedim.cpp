/** @file lsolve_onedim.cpp
 *
 * This test routine does some simple checks on solving a polynomial for a
 * variable. */

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

#ifndef NO_GINAC_NAMESPACE
using namespace GiNaC;
#endif // ndef NO_GINAC_NAMESPACE

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
    
    if (!result) {
        cout << " passed ";
        clog << "(no output)" << endl;
    } else {
        cout << " failed ";
    }
    
    return result;
}
