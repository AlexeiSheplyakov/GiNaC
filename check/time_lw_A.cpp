/** @file time_lw_A.cpp
 *
 *  Test A from the paper "Comparison of Polynomial-Oriented CAS" by Robert H.
 *  Lewis and Michael Wester. */

/*
 *  GiNaC Copyright (C) 1999-2002 Johannes Gutenberg University Mainz, Germany
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
	for (int i=1; i<=99; ++i)
		factorial(1000+i)/factorial(900+i);
	ex rat(factorial(1100)/factorial(1000));
	
	if (abs(evalf(rat)-numeric(".13280014101512E303"))>numeric("1.0E289")) {
		clog << "1100!/1000! erroneously returned " << rat << endl;
		return 1;
	}
	return 0;
}

unsigned time_lw_A(void)
{
	unsigned result = 0;
	unsigned count = 0;
	timer rolex;
	double time = .0;
	
	cout << "timing Lewis-Wester test A (divide factorials)" << flush;
	clog << "-------Lewis-Wester test A (divide factorials):" << endl;
	
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
