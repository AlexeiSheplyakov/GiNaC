/** @file main.cpp
 *
 *  Main program that calls all individual tests. */

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

#include <stdexcept>
#include <iostream>

#include "check.h"

int main()
{
    unsigned result = 0;
    
    try {
        for (int i=0; i<1; ++i) {
            result += fcntimer(paranoia_check);
            result += fcntimer(numeric_output);
            result += fcntimer(numeric_consist);
            result += fcntimer(powerlaws);
            result += fcntimer(expand_subs);
            result += fcntimer(inifcns_consist);
            result += fcntimer(differentiation);
            result += fcntimer(poly_gcd);
            result += fcntimer(normalization);
            result += fcntimer(matrix_checks);
            result += fcntimer(lsolve_onedim);
            result += fcntimer(series_expansion);
        }
    } catch (exception const & e) {
        cout << "error: caught an exception: " << e.what() << endl;
        result++;
    }
    
    if (result) {
        cout << "error: something went wrong. ";
        if (result == 1) {
            cout << "(one failure)" << endl;
        } else {
            cout << "(" << result << " individual failures)" << endl;
        }
        cout << "please check result.out against result.ref for more details."
             << endl << "happy debugging!" << endl;
    }

    return result;
}
