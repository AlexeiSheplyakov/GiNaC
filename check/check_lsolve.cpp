/** @file check_lsolve.cpp
 *
 *  These test routines do some simple checks on solving linear systems of
 *  symbolic equations. */

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

static unsigned lsolve1(int size)
{
    // A dense size x size matrix in dense univariate random polynomials
    // of order 4.
    unsigned result = 0;
    symbol a("a");
    ex sol;
    
    // Create two dense linear matrices A and B where all entries are random
    // univariate polynomials 
    matrix A(size,size), B(size,2), X(size,2);
    for (int ro=0; ro<size; ++ro) {
        for (int co=0; co<size; ++co)
            A.set(ro,co,dense_univariate_poly(a, 5));
        for (int co=0; co<2; ++co)
            B.set(ro,co,dense_univariate_poly(a, 5));
    }
    if (A.determinant().is_zero())
        clog << "lsolve1: singular system!" << endl;
    
    // Solve the system A*X==B:
    X = A.old_solve(B);
    
    // check the result:
    bool errorflag = false;
    matrix Aux(size,2);
    Aux = A.mul(X).sub(B);
    for (int ro=0; ro<size && !errorflag; ++ro)
        for (int co=0; co<2; ++co)
            if (!(Aux(ro,co)).normal().is_zero())
                errorflag = true;
    if (errorflag) {
        clog << "Our solve method claims that A*X==B, with matrices" << endl
             << "A == " << A << endl
             << "X == " << X << endl
             << "B == " << B << endl;
        ++result;
    }
    return result;
}

static unsigned lsolve2(int size)
{
    // A dense size x size matrix in dense bivariate random polynomials
    // of order 2.
    unsigned result = 0;
    symbol a("a"), b("b");
    ex sol;
    
    // Create two dense linear matrices A and B where all entries are dense random
    // bivariate polynomials:
    matrix A(size,size), B(size,2), X(size,2);
    for (int ro=0; ro<size; ++ro) {
        for (int co=0; co<size; ++co)
            A.set(ro,co,dense_bivariate_poly(a,b,2));
        for (int co=0; co<2; ++co)
            B.set(ro,co,dense_bivariate_poly(a,b,2));
    }
    if (A.determinant().is_zero())
        clog << "lsolve2: singular system!" << endl;
    
    // Solve the system A*X==B:
    X = A.old_solve(B);
    
    // check the result:
    bool errorflag = false;
    matrix Aux(size,2);
    Aux = A.mul(X).sub(B);
    for (int ro=0; ro<size && !errorflag; ++ro)
        for (int co=0; co<2; ++co)
            if (!(Aux(ro,co)).normal().is_zero())
                errorflag = true;
    if (errorflag) {
        clog << "Our solve method claims that A*X==B, with matrices" << endl
             << "A == " << A << endl
             << "X == " << X << endl
             << "B == " << B << endl;
        ++result;
    }
    return result;
}

unsigned check_lsolve(void)
{
    unsigned result = 0;
    
    cout << "checking linear solve" << flush;
    clog << "---------linear solve:" << endl;
    
    //result += lsolve1(2);  cout << '.' << flush;
    //result += lsolve1(3);  cout << '.' << flush;
    //result += lsolve2(2);  cout << '.' << flush;
    //result += lsolve2(3);  cout << '.' << flush;
    
    if (!result) {
        cout << " passed " << endl;
        clog << "(no output)" << endl;
    } else {
        cout << " failed " << endl;
    }
    
    return result;
}
