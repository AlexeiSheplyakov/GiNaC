/** @file time_lw_F.cpp
 *
 *  Test F from the paper "Comparison of Polynomial-Oriented CAS" by Robert H.
 *  Lewis and Michael Wester. */

/*
 *  GiNaC Copyright (C) 1999-2003 Johannes Gutenberg University Mainz, Germany
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

#include "times.h"

static unsigned test(void)
{
	symbol x("x");
	symbol y("y");

	ex p = expand(pow(pow(x,2)-3*x*y+pow(y,2),4)*pow(3*x-7*y+2,5));
	ex q = expand(pow(pow(x,2)-3*x*y+pow(y,2),3)*pow(3*x-7*y-2,6));
	ex result = gcd(p,q);
	if (result!=expand(pow(pow(x,2)-3*x*y+pow(y,2),3))) {
		clog << "gcd(expand((x^2-3*x*y+y^2)^4*(3*x-7*y+2)^5),expand((x^2-3*x*y+y^2)^3*(3*x-7*y-2)^6)) erroneously returned " << result << endl;
		return 1;
	}
	return 0;
}

unsigned time_lw_F(void)
{
	unsigned result = 0;
	unsigned count = 0;
	timer rolex;
	double time = .0;
	
	cout << "timing Lewis-Wester test F (gcd of 2-var polys)" << flush;
	clog << "-------Lewis-Wester test F (gcd of 2-var polys):" << endl;
	
	rolex.start();
	// correct for very small times:
	do {
		result = test();
		++count;
	} while ((time=rolex.read())<0.1 && !result);
	cout << '.' << flush;
	
	if (!result) {
		cout << " passed ";
		clog << "(no output)" << endl;
	} else {
		cout << " failed ";
	}
	cout << int(1000*(time/count))*0.001 << 's' << endl;
	
	return result;
}
