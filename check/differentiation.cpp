/** @file differentiation.cpp
 *
 *  Tests for symbolic differentiation, including various functions. */

/*
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
using namespace GiNaC;

static unsigned check_diff(const ex &e, const symbol &x,
                           const ex &d, unsigned nth=1)
{
    ex ed = e.diff(x, nth);
    if ((ed - d).compare(exZERO()) != 0) {
        switch (nth) {
        case 0:
            clog << "zeroth ";
            break;
        case 1:
            break;
        case 2:
            clog << "second ";
            break;
        case 3:
            clog << "third ";
            break;
        default:
            clog << nth << "th ";
        }
        clog << "derivative of " << e << " by " << x << " returned "
             << ed << " instead of " << d << endl;
        clog << "returned:" << endl;
        ed.printtree(clog);
        clog << endl << "instead of" << endl;
        d.printtree(clog);

        return 1;
    }
    return 0;
}

// Simple (expanded) polynomials
static unsigned differentiation1(void)
{
    unsigned result = 0;
    symbol x("x"), y("y");
    ex e1, e2, e, d;
    
    // construct bivariate polynomial e to be diff'ed:
    e1 = pow(x, -2) * 3 + pow(x, -1) * 5 + 7 + x * 11 + pow(x, 2) * 13;
    e2 = pow(y, -2) * 5 + pow(y, -1) * 7 + 11 + y * 13 + pow(y, 2) * 17;
    e = (e1 * e2).expand();
    
    // d e / dx:
    d = 121 - 55*pow(x,-2) - 66*pow(x,-3) - 30*pow(x,-3)*pow(y,-2)
        - 42*pow(x,-3)*pow(y,-1) - 78*pow(x,-3)*y
        - 102*pow(x,-3)*pow(y,2) - 25*pow(x,-2) * pow(y,-2)
        - 35*pow(x,-2)*pow(y,-1) - 65*pow(x,-2)*y
        - 85*pow(x,-2)*pow(y,2) + 77*pow(y,-1) + 143*y + 187*pow(y,2)
        + 130*x*pow(y,-2) + 182*pow(y,-1)*x + 338*x*y + 442*x*pow(y,2)
        + 55*pow(y,-2) + 286*x;
    result += check_diff(e, x, d);
    
    // d e / dy:
    d = 91 - 30*pow(x,-2)*pow(y,-3) - 21*pow(x,-2)*pow(y,-2)
        + 39*pow(x,-2) + 102*pow(x,-2)*y - 50*pow(x,-1)*pow(y,-3)
        - 35*pow(x,-1)*pow(y,-2) + 65*pow(x,-1) + 170*pow(x,-1)*y
        - 77*pow(y,-2)*x + 143*x + 374*x*y - 130*pow(y,-3)*pow(x,2)
        - 91*pow(y,-2)*pow(x,2) + 169*pow(x,2) + 442*pow(x,2)*y
        - 110*pow(y,-3)*x - 70*pow(y,-3) + 238*y - 49*pow(y,-2);
    result += check_diff(e, y, d);
    
    // d^2 e / dx^2:
    d = 286 + 90*pow(x,-4)*pow(y,-2) + 126*pow(x,-4)*pow(y,-1)
        + 234*pow(x,-4)*y + 306*pow(x,-4)*pow(y,2)
        + 50*pow(x,-3)*pow(y,-2) + 70*pow(x,-3)*pow(y,-1)
        + 130*pow(x,-3)*y + 170*pow(x,-3)*pow(y,2)
        + 130*pow(y,-2) + 182*pow(y,-1) + 338*y + 442*pow(y,2)
        + 198*pow(x,-4) + 110*pow(x,-3);
    result += check_diff(e, x, d, 2);
    
    // d^2 e / dy^2:
    d = 238 + 90*pow(x,-2)*pow(y,-4) + 42*pow(x,-2)*pow(y,-3)
        + 102*pow(x,-2) + 150*pow(x,-1)*pow(y,-4)
        + 70*pow(x,-1)*pow(y,-3) + 170*pow(x,-1) + 330*x*pow(y,-4)
        + 154*x*pow(y,-3) + 374*x + 390*pow(x,2)*pow(y,-4)
        + 182*pow(x,2)*pow(y,-3) + 442*pow(x,2) + 210*pow(y,-4)
        + 98*pow(y,-3);
    result += check_diff(e, y, d, 2);
    
    return result;
}

// Trigonometric functions
static unsigned differentiation2(void)
{
    unsigned result = 0;
    symbol x("x"), y("y"), a("a"), b("b");
    ex e1, e2, e, d;
    
    // construct expression e to be diff'ed:
    e1 = y*pow(x, 2) + a*x + b;
    e2 = sin(e1);
    e = b*pow(e2, 2) + y*e2 + a;
    
    d = 2*b*e2*cos(e1)*(2*x*y + a) + y*cos(e1)*(2*x*y + a);
    result += check_diff(e, x, d);
    
    d = 2*b*pow(cos(e1),2)*pow(2*x*y + a, 2) + 4*b*y*e2*cos(e1)
        - 2*b*pow(e2,2)*pow(2*x*y + a, 2) - y*e2*pow(2*x*y + a, 2)
        + 2*pow(y,2)*cos(e1);
    result += check_diff(e, x, d, 2);
    
    d = 2*b*e2*cos(e1)*pow(x, 2) + e2 + y*cos(e1)*pow(x, 2);
    result += check_diff(e, y, d);

    d = 2*b*pow(cos(e1),2)*pow(x,4) - 2*b*pow(e2,2)*pow(x,4)
        + 2*cos(e1)*pow(x,2) - y*e2*pow(x,4);
    result += check_diff(e, y, d, 2);
    
    // construct expression e to be diff'ed:
    e2 = cos(e1);
    e = b*pow(e2, 2) + y*e2 + a;
    
    d = -2*b*e2*sin(e1)*(2*x*y + a) - y*sin(e1)*(2*x*y + a);
    result += check_diff(e, x, d);
    
    d = 2*b*pow(sin(e1),2)*pow(2*y*x + a,2) - 4*b*e2*sin(e1)*y 
        - 2*b*pow(e2,2)*pow(2*y*x + a,2) - y*e2*pow(2*y*x + a,2)
        - 2*pow(y,2)*sin(e1);
    result += check_diff(e, x, d, 2);
    
    d = -2*b*e2*sin(e1)*pow(x,2) + e2 - y*sin(e1)*pow(x, 2);
    result += check_diff(e, y, d);
    
    d = -2*b*pow(e2,2)*pow(x,4) + 2*b*pow(sin(e1),2)*pow(x,4)
        - 2*sin(e1)*pow(x,2) - y*e2*pow(x,4);
    result += check_diff(e, y, d, 2);

	return result;
}
    
// exp function
static unsigned differentiation3(void)
{
    unsigned result = 0;
    symbol x("x"), y("y"), a("a"), b("b");
    ex e1, e2, e, d;

    // construct expression e to be diff'ed:
    e1 = y*pow(x, 2) + a*x + b;
    e2 = exp(e1);
    e = b*pow(e2, 2) + y*e2 + a;
    
    d = 2*b*pow(e2, 2)*(2*x*y + a) + y*e2*(2*x*y + a);
    result += check_diff(e, x, d);
    
    d = 4*b*pow(e2,2)*pow(2*y*x + a,2) + 4*b*pow(e2,2)*y
        + 2*pow(y,2)*e2 + y*e2*pow(2*y*x + a,2);
    result += check_diff(e, x, d, 2);
    
    d = 2*b*pow(e2,2)*pow(x,2) + e2 + y*e2*pow(x,2);
    result += check_diff(e, y, d);
    
    d = 4*b*pow(e2,2)*pow(x,4) + 2*e2*pow(x,2) + y*e2*pow(x,4);
    result += check_diff(e, y, d, 2);

	return result;
}

// log functions
static unsigned differentiation4(void)
{
    unsigned result = 0;
    symbol x("x"), y("y"), a("a"), b("b");
    ex e1, e2, e, d;
    
    // construct expression e to be diff'ed:
    e1 = y*pow(x, 2) + a*x + b;
    e2 = log(e1);
    e = b*pow(e2, 2) + y*e2 + a;
    
    d = 2*b*e2*(2*x*y + a)/e1 + y*(2*x*y + a)/e1;
    result += check_diff(e, x, d);
    
    d = 2*b*pow((2*x*y + a),2)*pow(e1,-2) + 4*b*y*e2/e1
        - 2*b*e2*pow(2*x*y + a,2)*pow(e1,-2) + 2*pow(y,2)/e1
        - y*pow(2*x*y + a,2)*pow(e1,-2);
    result += check_diff(e, x, d, 2);
    
    d = 2*b*e2*pow(x,2)/e1 + e2 + y*pow(x,2)/e1;
    result += check_diff(e, y, d);
    
    d = 2*b*pow(x,4)*pow(e1,-2) - 2*b*e2*pow(e1,-2)*pow(x,4)
        + 2*pow(x,2)/e1 - y*pow(x,4)*pow(e1,-2);
    result += check_diff(e, y, d, 2);

	return result;
}

// Functions with two variables
static unsigned differentiation5(void)
{
    unsigned result = 0;
    symbol x("x"), y("y"), a("a"), b("b");
    ex e1, e2, e, d;
    
    // test atan2
    e1 = y*pow(x, 2) + a*x + b;
    e2 = x*pow(y, 2) + b*y + a;
    e = atan2(e1,e2);
    /*
    d = pow(y,2)*(-b-y*pow(x,2)-a*x)/(pow(b+y*pow(x,2)+a*x,2)+pow(x*pow(y,2)+b*y+a,2))
        +(2*y*x+a)/((x*pow(y,2)+b*y+a)*(1+pow(b*y*pow(x,2)+a*x,2)/pow(x*pow(y,2)+b*y+a,2)));
    */
    /*
    d = ((a+2*y*x)*pow(y*b+pow(y,2)*x+a,-1)-(a*x+b+y*pow(x,2))*
         pow(y*b+pow(y,2)*x+a,-2)*pow(y,2))*
        pow(1+pow(a*x+b+y*pow(x,2),2)*pow(y*b+pow(y,2)*x+a,-2),-1);
    */
    /*
    d = pow(1+pow(a*x+b+y*pow(x,2),2)*pow(y*b+pow(y,2)*x+a,-2),-1)
        *pow(y*b+pow(y,2)*x+a,-1)*(a+2*y*x)
        +pow(y,2)*(-a*x-b-y*pow(x,2))*
        pow(pow(y*b+pow(y,2)*x+a,2)+pow(a*x+b+y*pow(x,2),2),-1);
    */
    d = pow(y,2)*pow(pow(b+y*pow(x,2)+x*a,2)+pow(y*b+pow(y,2)*x+a,2),-1)*
        (-b-y*pow(x,2)-x*a)+
        pow(pow(b+y*pow(x,2)+x*a,2)+pow(y*b+pow(y,2)*x+a,2),-1)*
        (y*b+pow(y,2)*x+a)*(2*y*x+a);
    result += check_diff(e, x, d);
    
    return result;
}

// Series
static unsigned differentiation6(void)
{
    symbol x("x");
    ex e, d, ed;
    
    e = sin(x).series(x, exZERO(), 8);
    d = cos(x).series(x, exZERO(), 7);
    ed = e.diff(x);
    ed = static_cast<series *>(ed.bp)->convert_to_poly();
    d = static_cast<series *>(d.bp)->convert_to_poly();
    
    if ((ed - d).compare(exZERO()) != 0) {
        clog << "derivative of " << e << " by " << x << " returned "
             << ed << " instead of " << d << ")" << endl;
        return 1;
    }
    return 0;
}

unsigned differentiation(void)
{
    unsigned result = 0;
    
    cout << "checking symbolic differentiation..." << flush;
    clog << "---------symbolic differentiation:" << endl;
    
    result += differentiation1();
    result += differentiation2();
    result += differentiation3();
    result += differentiation4();
    result += differentiation5();
    result += differentiation6();
    
    if (!result) {
        cout << " passed ";
        clog << "(no output)" << endl;
    } else {
        cout << " failed ";
    }
    return result;
}
