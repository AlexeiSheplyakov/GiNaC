/** @file lortensor_check.cpp
 *
 *  Here we test manipulations on GiNaC's lortensors. */

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

#include <stdexcept>
#include "ginac.h"

#ifndef NO_NAMESPACE_GINAC
using namespace GiNaC;
#endif // ndef NO_NAMESPACE_GINAC

static unsigned lortensor_check1(void)
{
    // checks simple identities of the metric tensor!
    
    unsigned result = 0;
    lorentzidx mu("mu"), nu("nu");
    ex e1, e2, e3, e4, e5, e6;
    e1 = lortensor_g(mu,nu);
    e2 = lortensor_g(nu,mu);
    e3 = e1 - e2; // g(~mu,~nu) - g(~nu,~mu) = 0 !
    e4 = lortensor_g(mu,mu.toggle_covariant());
    e5 = lortensor_g(mu.toggle_covariant(),mu);
    e6 = e4 - e5; // g(~mu,_mu) - g(_mu,~mu) = 0!
    if (!e3.is_zero()) {
        clog << e1 << "-" << e2 << " erroneously returned "
             << e3 << " instead of 0" << endl;
        ++result;
    }
    if (!e6.is_zero()) {
        clog << e4 << "-" << e5 << " erroneously returned "
             << e6 << " instead of 0" << endl;
        ++result;
    }

    return result;
}

static unsigned lortensor_check2(void)
{
    // checks simple contraction properties of an arbitrary (symmetric!) rankn lortensor!
    
    unsigned result = 0;
    lorentzidx mu("mu"), nu("nu"), rho("rho");
    ex e1, e2, e3, e4, e5, e6, e7, e8, e9, e10;
    e1 = lortensor_g(mu,nu);
    e2 = lortensor_g(nu,mu);
    e3 = lortensor_rank1("p",mu.toggle_covariant());
    e4 = lortensor_rank1("p",nu);
    e5 = e3 * e1 - e3 * e2; // p_mu g(~mu,~nu) - p_mu g(~nu,~mu) = 0!
    e6 = simplify_lortensor(e3 * e1) - e4; // p~nu - p~nu = 0!
    e7 = lortensor_g(nu,rho);
    e8 = lortensor_rank2("F",mu.toggle_covariant(),nu.toggle_covariant());
    e9 = lortensor_rank2("F",mu.toggle_covariant(),rho);
    e10 = simplify_lortensor(e8 * e7) - e9; // F(_mu,_nu) g(~nu,~rho) - F(_mu,~rho) = 0!
    if (!e5.is_zero()) {
        clog << e3 << "*" << e1 << "-" << e3 << "*" << e2 << " erroneously returned "
             << e5 << " instead of 0" << endl;
        ++result;
    }
    if (!e6.is_zero()) {    
        clog << " simplify_lortensor(e3 * e1)" << "-" << e4 << " erroneously returned"
             << e6 << " instead of 0" << endl;
        ++result;
    }
    if (!e10.is_zero()) {        
        clog << " simplify_lortensor(e8 * e7)" << "-" << e9 << " erroneously returned"
             << e10 << " instead of 0" << endl;
        ++result;
    }

    return result;
}

unsigned lortensor_check(void)
{
    unsigned result = 0;
    
    cout << "checking manipulations of lortensor objects..." << flush;
    clog << "---------manipulations of lortensor objects:" << endl;
    
    result += lortensor_check1();
    result += lortensor_check2();
    
    if (!result) {
        cout << " passed ";
        clog << "(no output)" << endl;
    } else {
        cout << " failed ";
    }
    
    return result;    
}
