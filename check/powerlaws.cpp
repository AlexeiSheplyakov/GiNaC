/** @file powerlaws.cpp
 *
 *  Tests for power laws.  You shouldn't try to draw much inspiration from
 *  this code, it is a sanity check rather deeply rooted in GiNaC's classes. */

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

static unsigned powerlaws1(void)
{
    // (x^a)^b = x^(a*b)
    
    symbol x("x");
    symbol a("a");
    symbol b("b");
    
    ex e1=power(power(x,a),b);
    if (!(is_ex_exactly_of_type(e1,power) &&
          is_ex_exactly_of_type(e1.op(0),power) &&
          is_ex_exactly_of_type(e1.op(0).op(0),symbol) &&
          is_ex_exactly_of_type(e1.op(0).op(1),symbol) &&
          is_ex_exactly_of_type(e1.op(1),symbol) &&
          e1.is_equal(power(power(x,a),b)) )) {
        clog << "(x^a)^b, x,a,b symbolic wrong" << endl;
        clog << "returned: " << e1 << endl;
        return 1;
    }
    
    ex e2=e1.subs(a==1);
    if (!(is_ex_exactly_of_type(e2,power) &&
          is_ex_exactly_of_type(e2.op(0),symbol) &&
          is_ex_exactly_of_type(e2.op(1),symbol) &&
          e2.is_equal(power(x,b)) )) {
        clog << "(x^a)^b, x,b symbolic, a==1 wrong" << endl;
        clog << "returned: " << e2 << endl;
        return 1;
    }
    
    ex e3=e1.subs(a==-1);
    if (!(is_ex_exactly_of_type(e3,power) &&
          is_ex_exactly_of_type(e3.op(0),power) &&
          is_ex_exactly_of_type(e3.op(0).op(0),symbol) &&
          is_ex_exactly_of_type(e3.op(0).op(1),numeric) &&
          is_ex_exactly_of_type(e3.op(1),symbol) &&
          e3.is_equal(power(power(x,-1),b)) )) {
        clog << "(x^a)^b, x,b symbolic, a==-1 wrong" << endl;
        clog << "returned: " << e3 << endl;
        return 1;
    }
    
    ex e4=e1.subs(lst(a==-1,b==2.5));
    if (!(is_ex_exactly_of_type(e4,power) &&
          is_ex_exactly_of_type(e4.op(0),power) &&
          is_ex_exactly_of_type(e4.op(0).op(0),symbol) &&
          is_ex_exactly_of_type(e4.op(0).op(1),numeric) &&
          is_ex_exactly_of_type(e4.op(1),numeric) &&
          e4.is_equal(power(power(x,-1),2.5)) )) {
        clog << "(x^a)^b, x symbolic, a==-1, b==2.5 wrong" << endl;
        clog << "returned: " << e4 << endl;
        return 1;
    }
    
    ex e5=e1.subs(lst(a==-0.9,b==2.5));
    if (!(is_ex_exactly_of_type(e5,power) &&
          is_ex_exactly_of_type(e5.op(0),symbol) &&
          is_ex_exactly_of_type(e5.op(1),numeric) &&
          e5.is_equal(power(x,numeric(-0.9)*numeric(2.5))) )) {
        clog << "(x^a)^b, x symbolic, a==-0.9, b==2.5 wrong" << endl;
        clog << "returned: " << e5 << endl;
        return 1;
    }
    
    ex e6=e1.subs(lst(a==numeric(3)+numeric(5.3)*I,b==-5));
    if (!(is_ex_exactly_of_type(e6,power) &&
          is_ex_exactly_of_type(e6.op(0),symbol) &&
          is_ex_exactly_of_type(e6.op(1),numeric) &&
          e6.is_equal(power(x,numeric(-15)+numeric(5.3)*numeric(-5)*I)) )) {
        clog << "(x^a)^b, x symbolic, a==3+5.3*I, b==-5 wrong" << endl;
        clog << "returned: " << e6 << endl;
        return 1;
    }
    return 0;
}

static unsigned powerlaws2(void)
{
    // (a*x)^b = a^b * x^b
    
    symbol x("x");
    symbol a("a");
    symbol b("b");
    
    ex e1=power(a*x,b);
    if (!(is_ex_exactly_of_type(e1,power) &&
          is_ex_exactly_of_type(e1.op(0),mul) &&
          (e1.op(0).nops()==2) &&
          is_ex_exactly_of_type(e1.op(0).op(0),symbol) &&
          is_ex_exactly_of_type(e1.op(0).op(1),symbol) &&
          is_ex_exactly_of_type(e1.op(1),symbol) &&
          e1.is_equal(power(a*x,b)) )) {
        clog << "(a*x)^b, x,a,b symbolic wrong" << endl;
        clog << "returned: " << e1 << endl;
        return 1;
    }
    
    ex e2=e1.subs(a==3);
    if (!(is_ex_exactly_of_type(e2,power) &&
          is_ex_exactly_of_type(e2.op(0),mul) &&
          (e2.op(0).nops()==2) &&
          is_ex_exactly_of_type(e2.op(0).op(0),symbol) &&
          is_ex_exactly_of_type(e2.op(0).op(1),numeric) &&
          is_ex_exactly_of_type(e2.op(1),symbol) &&
          e2.is_equal(power(3*x,b)) )) {
        clog << "(a*x)^b, x,b symbolic, a==3 wrong" << endl;
        clog << "returned: " << e2 << endl;
        return 1;
    }
    
    ex e3=e1.subs(b==-3);
    if (!(is_ex_exactly_of_type(e3,mul) &&
          (e3.nops()==2) &&
          is_ex_exactly_of_type(e3.op(0),power) &&
          is_ex_exactly_of_type(e3.op(1),power) &&
          e3.is_equal(power(a,-3)*power(x,-3)) )) {
        clog << "(a*x)^b, x,a symbolic, b==-3 wrong" << endl;
        clog << "returned: " << e3 << endl;
        return 1;
    }
    
    ex e4=e1.subs(b==4.5);
    if (!(is_ex_exactly_of_type(e4,power) &&
          is_ex_exactly_of_type(e4.op(0),mul) &&
          (e4.op(0).nops()==2) &&
          is_ex_exactly_of_type(e4.op(0).op(0),symbol) &&
          is_ex_exactly_of_type(e4.op(0).op(1),symbol) &&
          is_ex_exactly_of_type(e4.op(1),numeric) &&
          e4.is_equal(power(a*x,4.5)) )) {
        clog << "(a*x)^b, x,a symbolic, b==4.5 wrong" << endl;
        clog << "returned: " << e4 << endl;
        return 1;
    }
    
    ex e5=e1.subs(lst(a==3.2,b==3+numeric(5)*I));
    if (!(is_ex_exactly_of_type(e5,mul) &&
          (e5.nops()==2) &&
          is_ex_exactly_of_type(e5.op(0),power) &&
          is_ex_exactly_of_type(e5.op(1),numeric) &&
          e5.is_equal(power(x,3+numeric(5)*I)*
                      power(numeric(3.2),3+numeric(5)*I)) )) {
        clog << "(a*x)^b, x symbolic, a==3.2, b==3+5*I wrong" << endl;
        clog << "returned: " << e5 << endl;
        return 1;
    }
    
    ex e6=e1.subs(lst(a==-3.2,b==3+numeric(5)*I));
    if (!(is_ex_exactly_of_type(e6,mul) &&
          (e6.nops()==2) &&
          is_ex_exactly_of_type(e6.op(0),power) &&
          is_ex_exactly_of_type(e6.op(1),numeric) &&
          e6.is_equal(power(-x,3+numeric(5)*I)*
                      power(numeric(3.2),3+numeric(5)*I)) )) {
        clog << "(a*x)^b, x symbolic, a==-3.2, b==3+5*I wrong" << endl;
        clog << "returned: " << e6 << endl;
        return 1;
    }
    
    ex e7=e1.subs(lst(a==3+numeric(5)*I,b==3.2));
    if (!(is_ex_exactly_of_type(e7,power) &&
          is_ex_exactly_of_type(e7.op(0),mul) &&
          (e7.op(0).nops()==2) &&
          is_ex_exactly_of_type(e7.op(0).op(0),symbol) &&
          is_ex_exactly_of_type(e7.op(0).op(1),numeric) &&
          is_ex_exactly_of_type(e7.op(1),numeric) &&
          e7.is_equal(power((3+numeric(5)*I)*x,3.2)) )) {
        clog << "(a*x)^b, x symbolic, a==3+5*I, b==3.2 wrong" << endl;
        clog << "returned: " << e7 << endl;
        return 1;
    }
    
    return 0;
}

static unsigned powerlaws3(void)
{
    // numeric evaluation

    ex e1=power(numeric(4),numeric(1)/numeric(2));
    if (e1 != 2) {
        clog << "4^(1/2) wrongly returned " << e1 << endl;
        return 1;
    }
    
    ex e2=power(numeric(27),numeric(2)/numeric(3));
    if (e2 != 9) {
        clog << "27^(2/3) wrongly returned " << e2 << endl;
        return 1;
    }

    ex e3=power(numeric(5),numeric(1)/numeric(2));
    if (!(is_ex_exactly_of_type(e3,power) &&
          e3.op(0).is_equal(numeric(5)) &&
          e3.op(1).is_equal(numeric(1)/numeric(2)) )) {
        clog << "5^(1/2) wrongly returned " << e3 << endl;
        return 1;
    }
    
    ex e4=power(numeric(5),evalf(numeric(1)/numeric(2)));
    if (!(is_ex_exactly_of_type(e4,numeric))) {
        clog << "5^(0.5) wrongly returned " << e4 << endl;
        return 1;
    }
    
    ex e5=power(evalf(numeric(5)),numeric(1)/numeric(2));
    if (!(is_ex_exactly_of_type(e5,numeric))) {
        clog << "5.0^(1/2) wrongly returned " << e5 << endl;
        return 1;
    }
    
    return 0;
}

static unsigned powerlaws4(void)
{
    // test for mul::eval()

    symbol a("a");
    symbol b("b");
    symbol c("c");
    
    ex f1=power(a*b,ex(1)/ex(2));
    ex f2=power(a*b,ex(3)/ex(2));
    ex f3=c;

    exvector v;
    v.push_back(f1);
    v.push_back(f2);
    v.push_back(f3);
    ex e1=mul(v);
    if (e1!=a*a*b*b*c) {
        clog << "(a*b)^(1/2)*(a*b)^(3/2)*c wrongly returned " << e1 << endl;
        return 1;
    }
    return 0;
}

unsigned powerlaws(void)
{
    unsigned result = 0;
    
    cout << "checking power laws..." << flush;
    clog << "---------power laws:" << endl;
    
    result += powerlaws1();
    result += powerlaws2();
    result += powerlaws3();
    result += powerlaws4();
    
    if (!result) {
        cout << " passed ";
        clog << "(no output)" << endl;
    } else {
        cout << " failed ";
    }
    return result;
}
