/** @file checks.cpp
 *
 *  Main program that calls the individual tests. */

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
#include <iostream>
#include <time.h>

#include "checks.h"

int main()
{
    unsigned result = 0;
    
    srand((unsigned)time(NULL));
    
    try {
        for (int i=0; i<1; ++i)
            result += check_numeric();
    } catch (const exception &e) {
        cout << "Error: caught exception " << e.what() << endl;
        ++result;
    }
    
    try {
        for (int i=0; i<1; ++i)
            result += check_inifcns();
    } catch (const exception &e) {
        cout << "Error: caught exception " << e.what() << endl;
        ++result;
    }
    
    try {
        for (int i=0; i<1; ++i)
            result += check_matrices();
    } catch (const exception &e) {
        cout << "Error: caught exception " << e.what() << endl;
        ++result;
    }
    
    try {
        for (int i=0; i<1; ++i)
            result += check_lsolve();
    } catch (const exception &e) {
        cout << "Error: caught exception " << e.what() << endl;
        ++result;
    }
    
    if (result) {
        cout << "Error: something went wrong. ";
        if (result == 1) {
            cout << "(one failure)" << endl;
        } else {
            cout << "(" << result << " individual failures)" << endl;
        }
        cout << "please check check.out against check.ref for more details."
             << endl << "happy debugging!" << endl;
    }
    
    return result;
}