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

/* determinants of some sparse symbolic matrices with coefficients in
 * an integral domain. */
static unsigned integdom_matrix_determinants(void)
{
    unsigned result = 0;
    symbol a("a");
    
    for (int size=3; size<20; ++size) {
        matrix A(size,size);
        for (int r=0; r<size-1; ++r) {
            // populate one element in each row:
            A.set(r,unsigned(rand()%size),dense_univariate_poly(a,5));
        }
        for (int c=0; c<size; ++c) {
            // set the last line to a linear combination of two other lines
            // to guarantee that the determinant is zero:
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

/* determinants of some sparse symbolic matrices with multivariate rational
 * function coefficients. */
static unsigned rational_matrix_determinants(void)
{
    unsigned result = 0;
    symbol a("a"), b("b"), c("c");

    for (int size=3; size<8; ++size) {
        matrix A(size,size);
        for (int r=0; r<size-1; ++r) {
            // populate one element in each row:
            ex numer = sparse_tree(a, b, c, 4, false, false, false);
            ex denom;
            do {
                denom = sparse_tree(a, b, c, 1, false, false, false);
            } while (denom.is_zero());
            A.set(r,unsigned(rand()%size),numer/denom);
        }
        for (int c=0; c<size; ++c) {
            // set the last line to a linear combination of two other lines
            // to guarantee that the determinant is zero:
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

/* Some quite wild determinants with functions and stuff like that. */
static unsigned wild_matrix_determinants(void)
{
    unsigned result = 0;
    symbol a("a"), b("b"), c("c");
    
    for (int size=3; size<6; ++size) {
        matrix A(size,size);
        for (int r=0; r<size-1; ++r) {
            // populate one element in each row:
            ex numer = sparse_tree(a, b, c, 3, true, true, false);
            ex denom;
            do {
                denom = sparse_tree(a, b, c, 1, false, true, false);
            } while (denom.is_zero());
            A.set(r,unsigned(rand()%size),numer/denom);
        }
        for (int c=0; c<size; ++c) {
            // set the last line to a linear combination of two other lines
            // to guarantee that the determinant is zero:
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
    
    result += integdom_matrix_determinants();  cout << '.' << flush;
    result += rational_matrix_determinants();  cout << '.' << flush;
    result += wild_matrix_determinants();  cout << '.' << flush;
    
    if (!result) {
        cout << " passed " << endl;
        clog << "(no output)" << endl;
    } else {
        cout << " failed " << endl;
    }
    
    return result;
}
