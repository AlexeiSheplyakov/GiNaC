/** @file inifcns_consist.cpp
 *
 *  This test routine applies assorted tests on initially known higher level
 *  functions. */

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

#ifndef NO_GINAC_NAMESPACE
using namespace GiNaC;
#endif // ndef NO_GINAC_NAMESPACE

/* Simple tests on the sine trigonometric function. */
static unsigned inifcns_consist_sin(void)
{
    unsigned result = 0;
    bool errorflag;
    
    // sin(n*Pi) == 0?
    errorflag = false;
    for (int n=-10; n<=10; ++n) {
        if ( sin(n*Pi).eval() != numeric(0) ||
            !sin(n*Pi).eval().info(info_flags::integer))
            errorflag = true;
    }
    if (errorflag) {
        clog << "sin(n*Pi) with integer n does not always return exact 0"
             << endl;
        ++result;
    }
    
    // sin((n+1/2)*Pi) == {+|-}1?
    errorflag = false;
    for (int n=-10; n<=10; ++n) {
        if (! sin((n+numeric(1,2))*Pi).eval().info(info_flags::integer) ||
            !(sin((n+numeric(1,2))*Pi).eval() == numeric(1) ||
              sin((n+numeric(1,2))*Pi).eval() == numeric(-1)))
            errorflag = true;
    }
    if (errorflag) {
        clog << "sin((n+1/2)*Pi) with integer n does not always return exact {+|-}1"
             << endl;
        ++result;
    }
    
    return result;
}

/* Simple tests on the cosine trigonometric function. */
static unsigned inifcns_consist_cos(void)
{
    unsigned result = 0;
    bool errorflag;
    
    // cos((n+1/2)*Pi) == 0?
    errorflag = false;
    for (int n=-10; n<=10; ++n) {
        if ( cos((n+numeric(1,2))*Pi).eval() != numeric(0) ||
            !cos((n+numeric(1,2))*Pi).eval().info(info_flags::integer))
            errorflag = true;
    }
    if (errorflag) {
        clog << "cos((n+1/2)*Pi) with integer n does not always return exact 0"
             << endl;
        ++result;
    }
    
    // cos(n*Pi) == 0?
    errorflag = false;
    for (int n=-10; n<=10; ++n) {
        if (! cos(n*Pi).eval().info(info_flags::integer) ||
            !(cos(n*Pi).eval() == numeric(1) ||
              cos(n*Pi).eval() == numeric(-1)))
            errorflag = true;
    }
    if (errorflag) {
        clog << "cos(n*Pi) with integer n does not always return exact {+|-}1"
             << endl;
        ++result;
    }
    
    return result;
}

/* Assorted tests on other transcendental functions. */
static unsigned inifcns_consist_trans(void)
{
    unsigned result = 0;
    symbol x("x");
    ex chk;
    
    chk = asin(1)-acos(0);
    if (!chk.is_zero()) {
        clog << "asin(1)-acos(0) erroneously returned " << chk
             << " instead of 0" << endl;
        ++result;
    }
    
    // arbitrary check of type sin(f(x)):
    chk = pow(sin(acos(x)),2) + pow(sin(asin(x)),2)
        - (1+pow(x,2))*pow(sin(atan(x)),2);
    if (chk != 1-pow(x,2)) {
        clog << "sin(acos(x))^2 + sin(asin(x))^2 - (1+x^2)*sin(atan(x))^2 "
             << "erroneously returned " << chk << " instead of 1-x^2" << endl;
        ++result;
    }
    
    // arbitrary check of type cos(f(x)):
    chk = pow(cos(acos(x)),2) + pow(cos(asin(x)),2)
        - (1+pow(x,2))*pow(cos(atan(x)),2);
    if (!chk.is_zero()) {
        clog << "cos(acos(x))^2 + cos(asin(x))^2 - (1+x^2)*cos(atan(x))^2 "
             << "erroneously returned " << chk << " instead of 0" << endl;
        ++result;
    }
    
    // arbitrary check of type tan(f(x)):
    chk = tan(acos(x))*tan(asin(x)) - tan(atan(x));
    if (chk != 1-x) {
        clog << "tan(acos(x))*tan(asin(x)) - tan(atan(x)) "
             << "erroneously returned " << chk << " instead of -x+1" << endl;
        ++result;
    }
    
    // arbitrary check of type sinh(f(x)):
    chk = -pow(sinh(acosh(x)),2).expand()*pow(sinh(atanh(x)),2)
        - pow(sinh(asinh(x)),2);
    if (!chk.is_zero()) {
        clog << "expand(-(sinh(acosh(x)))^2)*(sinh(atanh(x))^2) - sinh(asinh(x))^2 "
             << "erroneously returned " << chk << " instead of 0" << endl;
        ++result;
    }
    
    // arbitrary check of type cosh(f(x)):
    chk = (pow(cosh(asinh(x)),2) - 2*pow(cosh(acosh(x)),2))
        * pow(cosh(atanh(x)),2);
    if (chk != 1) {
        clog << "(cosh(asinh(x))^2 - 2*cosh(acosh(x))^2) * cosh(atanh(x))^2 "
             << "erroneously returned " << chk << " instead of 1" << endl;
        ++result;
    }
    
    // arbitrary check of type tanh(f(x)):
    chk = (pow(tanh(asinh(x)),-2) - pow(tanh(acosh(x)),2)).expand()
        * pow(tanh(atanh(x)),2);
    if (chk != 2) {
        clog << "expand(tanh(acosh(x))^2 - tanh(asinh(x))^(-2)) * tanh(atanh(x))^2 "
             << "erroneously returned " << chk << " instead of 2" << endl;
        ++result;
    }
    
    return result;
}

/* Simple tests on the Gamma combinatorial function.  We stuff in arguments
 * where the result exists in closed form and check if it's ok. */
static unsigned inifcns_consist_gamma(void)
{
    unsigned result = 0;
    ex e;
    
    e = gamma(ex(1));
    for (int i=2; i<8; ++i)
        e += gamma(ex(i));
    if (e != numeric(874)) {
        clog << "gamma(1)+...+gamma(7) erroneously returned "
             << e << " instead of 874" << endl;
        ++result;
    }
    
    e = gamma(ex(1));
    for (int i=2; i<8; ++i)
        e *= gamma(ex(i));    
    if (e != numeric(24883200)) {
        clog << "gamma(1)*...*gamma(7) erroneously returned "
             << e << " instead of 24883200" << endl;
        ++result;
    }
    
    e = gamma(ex(numeric(5, 2)))*gamma(ex(numeric(9, 2)))*64;
    if (e != 315*Pi) {
        clog << "64*gamma(5/2)*gamma(9/2) erroneously returned "
             << e << " instead of 315*Pi" << endl;
        ++result;
    }
    
    e = gamma(ex(numeric(-13, 2)));
    for (int i=-13; i<7; i=i+2)
        e += gamma(ex(numeric(i, 2)));
    e = (e*gamma(ex(numeric(15, 2)))*numeric(512));
    if (e != numeric(633935)*Pi) {
        clog << "512*(gamma(-13/2)+...+gamma(5/2))*gamma(15/2) erroneously returned "
             << e << " instead of 633935*Pi" << endl;
        ++result;
    }
    
    return result;
}

/* Simple tests on the Riemann Zeta function.  We stuff in arguments where the
 * result exists in closed form and check if it's ok.  Of course, this checks
 * the Bernoulli numbers as a side effect. */
static unsigned inifcns_consist_zeta(void)
{
    unsigned result = 0;
    ex e;
    
    for (int i=0; i<13; i+=2)
        e += zeta(i)/pow(Pi,i);
    if (e!=numeric(-204992279,638512875)) {
        clog << "zeta(0) + zeta(2) + ... + zeta(12) erroneously returned "
             << e << " instead of -204992279/638512875" << endl;
        ++result;
    }
    
    e = 0;
    for (int i=-1; i>-16; i--)
        e += zeta(i);
    if (e!=numeric(487871,1633632)) {
        clog << "zeta(-1) + zeta(-2) + ... + zeta(-15) erroneously returned "
             << e << " instead of 487871/1633632" << endl;
        ++result;
    }
    
    return result;
}

unsigned inifcns_consist(void)
{
    unsigned result = 0;

    cout << "checking consistency of symbolic functions..." << flush;
    clog << "---------consistency of symbolic functions:" << endl;
    
    result += inifcns_consist_sin();
    result += inifcns_consist_cos();
    result += inifcns_consist_trans();
    result += inifcns_consist_gamma();
    result += inifcns_consist_zeta();

    if ( !result ) {
        cout << " passed ";
        clog << "(no output)" << endl;
    } else {
        cout << " failed ";
    }
    
    return result;
}
