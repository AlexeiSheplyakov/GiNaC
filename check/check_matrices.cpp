/** @file check_matrices.cpp
 *
 *  Here we test manipulations on GiNaC's symbolic matrices. */

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

#include "checks.h"

// determinants of some sparse symbolic size x size matrices
static unsigned matrix_determinants(void)
{
    unsigned result = 0;
    symbol a("a");

    for (int size=3; size<17; ++size) {
        matrix A(size,size);
        for (int r=0; r<size-1; ++r) {
            // populate one element in each row:
            A.set(r,unsigned(rand()%size),dense_univariate_poly(a,5));
        }
        for (int c=0; c<size; ++c) {
            // set the last line to a linear combination of two other lines
            // to guarantee that the determinant vanishes:
            A.set(size-1,c,A(0,c)-A(size-2,c));
        }
        if (!A.determinant().is_zero()) {
            clog << "Determinant of " << size << "x" << size << " matrix "
                 << endl << A << endl
                 << "was not found to vanish!" << endl;
            ++result;
        }
    }
    
    return result;
}

unsigned check_matrices(void)
{
    unsigned result = 0;
    
    cout << "checking symbolic matrix manipulations" << flush;
    clog << "---------symbolic matrix manipulations:" << endl;
    
    result += matrix_determinants();  cout << '.' << flush;
    
    if (!result) {
        cout << " passed " << endl;
        clog << "(no output)" << endl;
    } else {
        cout << " failed " << endl;
    }
    
    return result;
}
