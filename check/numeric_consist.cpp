/** @file numeric_consist.cpp
 *
 *  This test routine creates some numbers and check the result of several
 *  boolean tests on these numbers like is_integer() etc... */

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

#include <stdlib.h>
#include <ginac/ginac.h>

#ifndef NO_GINAC_NAMESPACE
using namespace GiNaC;
#endif // ndef NO_GINAC_NAMESPACE

/* Simple and maybe somewhat pointless consistency tests of assorted tests and
 * conversions. */
static unsigned numeric_consist1(void)
{
    unsigned result = 0;
    numeric test_int1(42);
    numeric test_int2(5);
    numeric test_rat1 = test_int1; test_rat1 /= test_int2;
    test_rat1 = -test_rat1;         // -42/5
    numeric test_crat = test_rat1+I*test_int2;  // 5*I-42/5
    symbol a("a");
    ex e1, e2;
    
    if (!test_int1.is_integer()) {
        clog << test_int1
             << " erroneously not recognized as integer" << endl;
        ++result;
    }
    if (!test_int1.is_rational()) {
        clog << test_int1
             << " erroneously not recognized as rational" << endl;
        ++result;
    }
    
    if (!test_rat1.is_rational()) {
        clog << test_rat1
             << " erroneously not recognized as rational" << endl;
        ++result;
    }
    if (test_rat1.is_integer()) {
        clog << test_rat1
             << " erroneously recognized as integer" << endl;
        ++result;
    }
    
    if (!test_crat.is_crational()) {
        clog << test_crat
             << " erroneously not recognized as complex rational" << endl;
        ++result;
    }
    
    int i = numeric(1984).to_int();
    if (i-1984) {
        clog << "conversion of " << i
             << " from numeric to int failed" << endl;
        ++result;
    }
    
    e1 = test_int1;
    if (!e1.info(info_flags::posint)) {
        clog << "expression " << e1
             << " erroneously not recognized as positive integer" << endl;
        ++result;
    }
    
    e2 = test_int1 + a;
    if (ex_to_numeric(e2).is_integer()) {
        clog << "expression " << e2
             << " erroneously recognized as integer" << endl;
        ++result;
    }
    
    // The next two were two actual bugs in CLN till June, 12, 1999:
    test_rat1 = numeric(3)/numeric(2);
    test_rat1 += test_rat1;
    if (!test_rat1.is_integer()) {
        clog << "3/2 + 3/2 erroneously not integer 3 but instead "
             << test_rat1 << endl;
        ++result;
    }
    test_rat1 = numeric(3)/numeric(2);
    numeric test_rat2 = test_rat1 + numeric(1);  // 5/2
    test_rat2 -= test_rat1;  // 1
    if (!test_rat2.is_integer()) {
        clog << "5/2 - 3/2 erroneously not integer 1 but instead "
             << test_rat2 << endl;
        ++result;
    }
    
    // Check some numerator and denominator calculations:
    for (int i=0; i<10; ++i) {
        int re_q, im_q;
        do { re_q = rand(); } while (re_q == 0);
        do { im_q = rand(); } while (im_q == 0);
        numeric r(rand()-RAND_MAX/2, re_q);
        numeric i(rand()-RAND_MAX/2, im_q);
        numeric z = r + I*i;
        numeric p = numer(z);
        numeric q = denom(z);
        numeric res = p/q;
        if (res != z) {
            clog << z << " erroneously transformed into " 
                 << p << "/" << q << " by numer() and denom()" << endl;
            ++result;
        }
    }    
    return result;
}

/* We had some fun with a bug in CLN that caused it to loop forever when
 * calculating expt(a,b) if b is a rational and a a nonnegative integer.
 * Implementing a workaround sadly introduced another bug on May 28th 1999
 * that was fixed on May 31st.  The workaround turned out to be stupid and
 * the original bug in CLN was finally killed on September 2nd. */
static unsigned numeric_consist2(void)
{
    unsigned result = 0;
    
    ex zero = numeric(0);
    ex two = numeric(2);
    ex three = numeric(3);
    
    // The hang in this code was the reason for the original workaround
    if (pow(two,two/three)==42) {
        clog << "pow(2,2/3) erroneously returned 42" << endl;
        ++result;  // cannot happen
    }
    
    // Actually, this used to raise a FPE after introducing the workaround
    if (two*zero!=zero) {
        clog << "2*0 erroneously returned " << two*zero << endl;
        ++result;
    }
    
    // And this returned a cl_F due to the implicit call of numeric::power()
    ex six = two*three;
    if (!six.info(info_flags::integer)) {
        clog << "2*3 erroneously returned the non-integer " << six << endl;
        ++result;
    }
    
    // The fix in the workaround left a whole which was fixed hours later...
    ex another_zero = pow(zero,numeric(1)/numeric(2));
    if (!another_zero.is_zero()) {
        clog << "pow(0,1/2) erroneously returned" << another_zero << endl;
        ++result;
    }
    
    return result;
}

/* Assorted tests to ensure some crucial functions behave exactly as specified
 * in the documentation. */
static unsigned numeric_consist3(void)
{
    unsigned result = 0;
    numeric calc_rem, calc_quo;
    numeric a, b;
    
    // check if irem(a, b) and irem(a, b, q) really behave like Maple's 
    // irem(a, b) and irem(a, b, 'q') as advertised in our documentation.
    // These overloaded routines indeed need to be checked separately since
    // internally they might be doing something completely different:
    a = 23; b = 4; calc_rem = irem(a, b);
    if (calc_rem != 3) {
        clog << "irem(" << a << "," << b << ") erroneously returned "
             << calc_rem << endl;
        ++result;
    }
    a = 23; b = -4; calc_rem = irem(a, b);
    if (calc_rem != 3) {
        clog << "irem(" << a << "," << b << ") erroneously returned "
             << calc_rem << endl;
        ++result;
    }
    a = -23; b = 4; calc_rem = irem(a, b);
    if (calc_rem != -3) {
        clog << "irem(" << a << "," << b << ") erroneously returned "
             << calc_rem << endl;
        ++result;
    }
    a = -23; b = -4; calc_rem = irem(a, b);
    if (calc_rem != -3) {
        clog << "irem(" << a << "," << b << ") erroneously returned "
             << calc_rem << endl;
        ++result;
    }
    // and now the overloaded irem(a,b,q):
    a = 23; b = 4; calc_rem = irem(a, b, calc_quo);
    if (calc_rem != 3 || calc_quo != 5) {
        clog << "irem(" << a << "," << b << ",q) erroneously returned "
             << calc_rem << " with q=" << calc_quo << endl;
        ++result;
    }
    a = 23; b = -4; calc_rem = irem(a, b, calc_quo);
    if (calc_rem != 3 || calc_quo != -5) {
        clog << "irem(" << a << "," << b << ",q) erroneously returned "
             << calc_rem << " with q=" << calc_quo << endl;
        ++result;
    }
    a = -23; b = 4; calc_rem = irem(a, b, calc_quo);
    if (calc_rem != -3 || calc_quo != -5) {
        clog << "irem(" << a << "," << b << ",q) erroneously returned "
             << calc_rem << " with q=" << calc_quo << endl;
        ++result;
    }
    a = -23; b = -4; calc_rem = irem(a, b, calc_quo);
    if (calc_rem != -3 || calc_quo != 5) {
        clog << "irem(" << a << "," << b << ",q) erroneously returned "
             << calc_rem << " with q=" << calc_quo << endl;
        ++result;
    }
    // check if iquo(a, b) and iquo(a, b, r) really behave like Maple's 
    // iquo(a, b) and iquo(a, b, 'r') as advertised in our documentation.
    // These overloaded routines indeed need to be checked separately since
    // internally they might be doing something completely different:
    a = 23; b = 4; calc_quo = iquo(a, b);
    if (calc_quo != 5) {
        clog << "iquo(" << a << "," << b << ") erroneously returned "
             << calc_quo << endl;
        ++result;
    }
    a = 23; b = -4; calc_quo = iquo(a, b);
    if (calc_quo != -5) {
        clog << "iquo(" << a << "," << b << ") erroneously returned "
             << calc_quo << endl;
        ++result;
    }
    a = -23; b = 4; calc_quo = iquo(a, b);
    if (calc_quo != -5) {
        clog << "iquo(" << a << "," << b << ") erroneously returned "
             << calc_quo << endl;
        ++result;
    }
    a = -23; b = -4; calc_quo = iquo(a, b);
    if (calc_quo != 5) {
        clog << "iquo(" << a << "," << b << ") erroneously returned "
             << calc_quo << endl;
        ++result;
    }
    // and now the overloaded iquo(a,b,r):
    a = 23; b = 4; calc_quo = iquo(a, b, calc_rem);
    if (calc_quo != 5 || calc_rem != 3) {
        clog << "iquo(" << a << "," << b << ",r) erroneously returned "
             << calc_quo << " with r=" << calc_rem << endl;
        ++result;
    }
    a = 23; b = -4; calc_quo = iquo(a, b, calc_rem);
    if (calc_quo != -5 || calc_rem != 3) {
        clog << "iquo(" << a << "," << b << ",r) erroneously returned "
             << calc_quo << " with r=" << calc_rem << endl;
        ++result;
    }
    a = -23; b = 4; calc_quo = iquo(a, b, calc_rem);
    if (calc_quo != -5 || calc_rem != -3) {
        clog << "iquo(" << a << "," << b << ",r) erroneously returned "
             << calc_quo << " with r=" << calc_rem << endl;
        ++result;
    }
    a = -23; b = -4; calc_quo = iquo(a, b, calc_rem);
    if (calc_quo != 5 || calc_rem != -3) {
        clog << "iquo(" << a << "," << b << ",r) erroneously returned "
             << calc_quo << " with r=" << calc_rem << endl;
        ++result;
    }
    
    return result;
}

/* Now we perform some less trivial checks about several functions which should
 * return exact numbers if possible. */
static unsigned numeric_consist4(void)
{
    unsigned result = 0;
    bool passed;
    
    // square roots of squares of integers:
    passed = true;
    for (int i=0; i<42; ++i) {
        if (!sqrt(numeric(i*i)).is_integer()) {
            passed = false;
        }
    }
    if (!passed) {
        clog << "One or more square roots of squares of integers did not return exact integers" << endl;
        ++result;
    }
    
    // square roots of squares of rationals:
    passed = true;
    for (int num=0; num<41; ++num) {
        for (int den=1; den<42; ++den) {
            if (!sqrt(numeric(num*num)/numeric(den*den)).is_rational()) {
                passed = false;
            }
        }
    }
    if (!passed) {
        clog << "One or more square roots of squares of rationals did not return exact integers" << endl;
        ++result;
    }
    
    return result;
}

unsigned numeric_consist(void)
{
    unsigned result = 0;

    cout << "checking consistency of numeric types..." << flush;
    clog << "---------consistency of numeric types:" << endl;
    
    result += numeric_consist1();
    result += numeric_consist2();
    result += numeric_consist3();
    result += numeric_consist4();

    if (!result) {
        cout << " passed ";
        clog << "(no output)" << endl;
    } else {
        cout << " failed ";
    }
    
    return result;
}
