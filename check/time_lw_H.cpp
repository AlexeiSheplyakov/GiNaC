/** @file time_lw_H.cpp
 *
 *  Test H from the paper "Comparison of Polynomial-Oriented CAS" by Robert H.
 *  Lewis and Michael Wester. */

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

#include "times.h"

static unsigned test(void)
{
    matrix h80(80,80);
    
    for (unsigned r=0; r<80; ++r)
        for (unsigned c=0; c<80; ++c)
            h80.set(r,c,numeric(1,r+c+1));
    ex det = h80.determinant();
    
    if (abs(det.evalf()-numeric(".10097939769690107E-3789"))>numeric("1.E-3800")) {
        clog << "determinant of 80x80 erroneously returned " << det << endl;
        return 1;
    }
    return 0;
}

unsigned time_lw_H(void)
{
    unsigned result = 0;
    unsigned count = 0;
    timer rolex;
    double time = .0;
    
    cout << "timing Lewis-Wester test H (det of 80x80 Hilbert)" << flush;
    clog << "-------Lewis-Wester test H (det of 80x80 Hilbert)" << endl;
    
    rolex.start();
    // correct for very small times:
    do {
        result = test();
        ++count;
    } while ((time=rolex.read())<0.1 && !result);
    cout << '.' << flush;
    
    if (!result) {
        cout << " passed ";
        clog << "(no output)" << endl;
    } else {
        cout << " failed ";
    }
    cout << int(1000*(time/count))*0.001 << 's' << endl;
    
    return result;
}
