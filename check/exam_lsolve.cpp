/** @file exam_lsolve.cpp
 *
 *  These exams test solving small linear systems of symbolic equations. */

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

#include "exams.h"

static unsigned exam_lsolve1(void)
{
    // A trivial example.
    unsigned result = 0;
    symbol x("x");
    ex eq, aux;
    
    eq = (3*x+5 == numeric(8));
    aux = lsolve(eq, x);
    if (aux != 1) {
        result++;
        clog << "solution of 3*x+5==8 erroneously returned "
             << aux << endl;
    }
    
    return result;
}

static unsigned exam_lsolve2a(void)
{
    // An example from the Maple online help.
    unsigned result = 0;
    symbol a("a"), b("b"), x("x"), y("y");
    lst eqns, vars;
    ex sol;
    
    // Create the linear system [a*x+b*y==3,x-y==b]...
    eqns.append(a*x+b*y==3).append(x-y==b);
    // ...to be solved for [x,y]...
    vars.append(x).append(y);
    // ...and solve it:
    sol = lsolve(eqns, vars);
    ex sol_x = sol.op(0).rhs();  // rhs of solution for first variable (x)
    ex sol_y = sol.op(1).rhs();  // rhs of solution for second variable (y)
    
    // It should have returned [x==(3+b^2)/(a+b),y==(3-a*b)/(a+b)]
    if (!normal(sol_x - (3+pow(b,2))/(a+b)).is_zero() ||
        !normal(sol_y - (3-a*b)/(a+b)).is_zero()) {
        result++;
        clog << "solution of the system " << eqns << " for " << vars
             << " erroneously returned " << sol << endl;
    }
    
    return result;
}

static unsigned exam_lsolve2b(void)
{
    // A boring example from Mathematica's online help.
    unsigned result = 0;
    symbol x("x"), y("y");
    lst eqns, vars;
    ex sol;
    
    // Create the linear system [3*x+y==7,2*x-5*y==8]...
    eqns.append(3*x+y==7).append(2*x-5*y==8);
    // ...to be solved for [x,y]...
    vars.append(x).append(y);
    // ...and solve it:
    sol = lsolve(eqns, vars);
    ex sol_x = sol.op(0).rhs();  // rhs of solution for first variable (x)
    ex sol_y = sol.op(1).rhs();  // rhs of solution for second variable (y)
    
    // It should have returned [x==43/17,y==-10/17]
    if (!(sol_x - numeric(43,17)).is_zero() ||
        !(sol_y - numeric(-10,17)).is_zero()) {
        result++;
        clog << "solution of the system " << eqns << " for " << vars
             << " erroneously returned " << sol << endl;
    }
    
    return result;
}

static unsigned exam_lsolve2c(void)
{
    // A more interesting example from the Maple online help.
    unsigned result = 0;
    symbol x("x"), y("y");
    lst eqns, vars;
    ex sol;
    
    // Create the linear system [I*x+y==1,I*x-y==2]...
    eqns.append(I*x+y==1).append(I*x-y==2);
    // ...to be solved for [x,y]...
    vars.append(x).append(y);
    // ...and solve it:
    sol = lsolve(eqns, vars);
    ex sol_x = sol.op(0).rhs();  // rhs of solution for first variable (x)
    ex sol_y = sol.op(1).rhs();  // rhs of solution for second variable (y)
    
    // It should have returned [x==-3/2*I,y==-1/2]
    if (!(sol_x - numeric(-3,2)*I).is_zero() ||
        !(sol_y - numeric(-1,2)).is_zero()) {
        result++;
        clog << "solution of the system " << eqns << " for " << vars
             << " erroneously returned " << sol << endl;
    }
    
    return result;
}

unsigned exam_lsolve(void)
{
    unsigned result = 0;
    
    cout << "examining linear solve" << flush;
    clog << "----------linear solve:" << endl;
    
    result += exam_lsolve1();  cout << '.' << flush;
    result += exam_lsolve2a();  cout << '.' << flush;
    result += exam_lsolve2b();  cout << '.' << flush;
    result += exam_lsolve2c();  cout << '.' << flush;
    
    if (!result) {
        cout << " passed " << endl;
        clog << "(no output)" << endl;
    } else {
        cout << " failed " << endl;
    }
    
    return result;
}
