/** @file time_lw_Pprime.cpp
 *
 *  Test P' from the paper "Comparison of Polynomial-Oriented CAS" by Robert H.
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
#include "time_lw_w101n.h"

static unsigned test(void)
{
    matrix m(101,101);
    for (unsigned r=0; r<101; ++r) {
        for (unsigned c=0; c<10; ++c) {
            m.set(r,
                  unsigned(ex_to_numeric(w101_numeric[r][2*c+1]).to_int()-1),
                  w101_numeric[r][2*c+2]);
        }
    }
    matrix m2(m);
    ex a;
    for (unsigned r=0; r<101; ++r) {
        a = m2(r,0);
        for (unsigned c=0; c<100; ++c) {
            m2.set(r,c,m2(r,c+1));
        }
        m2.set(r,100,a);
    }
    for (unsigned r=0; r<101; ++r) {
        for (unsigned c=0; c<101; ++c) {
            if (!m(r,c).is_zero())
                m2.set(r,c,m(r,c));
        }
    }
    ex det = m2.determinant();
    
    if (det!=numeric("140816284877507872414776")) {
        clog << "det of less sparse rank 101 matrix erroneously returned " << det << endl;
        return 1;
    }
    return 0;
}

unsigned time_lw_Pprime(void)
{
    unsigned result = 0;
    unsigned count = 1;
    timer rolex;
    double time = .0;
    
    cout << "timing Lewis-Wester test P' (det of less sparse rank 101)" << flush;
    clog << "-------Lewis-Wester test P' (det of less sparse rank 101)" << endl;
    
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
