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
        // populate one element in each row:
        for (int r=0; r<size-1; ++r)
            A.set(r,unsigned(rand()%size),dense_univariate_poly(a,5));
        // set the last row to a linear combination of two other lines
        // to guarantee that the determinant is zero:
        for (int c=0; c<size; ++c)
            A.set(size-1,c,A(0,c)-A(size-2,c));
        if (!A.determinant().is_zero()) {
            clog << "Determinant of " << size << "x" << size << " matrix "
                 << endl << A << endl
                 << "was not found to vanish!" << endl;
            ++result;
        }
    }
    
    return result;
}

/* determinants of some symbolic matrices with multivariate rational function
 * coefficients. */
static unsigned rational_matrix_determinants(void)
{
    unsigned result = 0;
    symbol a("a"), b("b"), c("c");
    
    for (int size=3; size<8; ++size) {
        matrix A(size,size);
        for (int r=0; r<size-1; ++r) {
            // populate one or two elements in each row:
            for (int ec=0; ec<2; ++ec) {
                ex numer = sparse_tree(a, b, c, 1+rand()%4, false, false, false);
                ex denom;
                do {
                    denom = sparse_tree(a, b, c, rand()%2, false, false, false);
                } while (denom.is_zero());
                A.set(r,unsigned(rand()%size),numer/denom);
            }
        }
        // set the last row to a linear combination of two other lines
        // to guarantee that the determinant is zero:
        for (int co=0; co<size; ++co)
            A.set(size-1,co,A(0,co)-A(size-2,co));
        if (!A.determinant().is_zero()) {
            clog << "Determinant of " << size << "x" << size << " matrix "
                 << endl << A << endl
                 << "was not found to vanish!" << endl;
            ++result;
        }
    }
    
    return result;
}

/* Some quite funny determinants with functions and stuff like that inside. */
static unsigned funny_matrix_determinants(void)
{
    unsigned result = 0;
    symbol a("a"), b("b"), c("c");
    
    for (int size=3; size<7; ++size) {
        matrix A(size,size);
        for (int co=0; co<size-1; ++co) {
            // populate one or two elements in each row:
            for (int ec=0; ec<2; ++ec) {
                ex numer = sparse_tree(a, b, c, 1+rand()%3, true, true, false);
                ex denom;
                do {
                    denom = sparse_tree(a, b, c, rand()%2, false, true, false);
                } while (denom.is_zero());
                A.set(unsigned(rand()%size),co,numer/denom);
            }
        }
        // set the last column to a linear combination of two other lines
        // to guarantee that the determinant is zero:
        for (int ro=0; ro<size; ++ro)
            A.set(ro,size-1,A(ro,0)-A(ro,size-2));
        if (!A.determinant().is_zero()) {
            clog << "Determinant of " << size << "x" << size << " matrix "
                 << endl << A << endl
                 << "was not found to vanish!" << endl;
            ++result;
        }
    }
    
    return result;
}

/* compare results from different determinant algorithms.*/
static unsigned compare_matrix_determinants(void)
{
    unsigned result = 0;
    symbol a("a");
    
    for (int size=2; size<6; ++size) {
        matrix A(size,size);
        for (int co=0; co<size; ++co) {
            for (int ro=0; ro<size; ++ro) {
                // populate some elements
                ex elem = 0;
                if (rand()%(size-1) == 0)
                    elem = sparse_tree(a, a, a, rand()%3, false, true, false);
                A.set(ro,co,elem);
            }
        }
        ex det_gauss = A.determinant(determinant_algo::gauss);
        ex det_laplace = A.determinant(determinant_algo::laplace);
        ex det_bareiss = A.determinant(determinant_algo::bareiss);
        if ((det_gauss-det_laplace).normal() != 0 ||
            (det_bareiss-det_laplace).normal() != 0) {
            clog << "Determinant of " << size << "x" << size << " matrix "
                 << endl << A << endl
                 << "is inconsistent between different algorithms:" << endl
                 << "Gauss elimination:   " << det_gauss << endl
                 << "Minor elimination:   " << det_laplace << endl
                 << "Fraction-free elim.: " << det_bareiss << endl;
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
    result += funny_matrix_determinants();  cout << '.' << flush;
    result += compare_matrix_determinants();  cout << '.' << flush;
    
    if (!result) {
        cout << " passed " << endl;
        clog << "(no output)" << endl;
    } else {
        cout << " failed " << endl;
    }
    
    return result;
}
