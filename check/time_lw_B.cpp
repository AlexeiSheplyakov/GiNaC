/** @file time_lw_B.cpp
 *
 *  Test B from the paper "Comparison of Polynomial-Oriented CAS" by Robert H.
 *  Lewis and Michael Wester. */

/*
 *  GiNaC Copyright (C) 1999-2005 Johannes Gutenberg University Mainz, Germany
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "times.h"

static unsigned test()
{
	numeric s;
	
	for (int i=1; i<=1000; ++i)
		s += numeric(i).inverse();
	
	if (abs(s.evalf()-numeric("7.4854708605503449"))>numeric("2.0E-16")) {
		clog << "sum(1/i,i=1..1000) erroneously returned " << s << endl;
		return 1;
	}
	return 0;
}

unsigned time_lw_B()
{
	unsigned result = 0;
	unsigned count = 0;
	timer rolex;
	double time = .0;
	
	cout << "timing Lewis-Wester test B (sum of rational numbers)" << flush;
	clog << "-------Lewis-Wester test B (sum of rational numbers):" << endl;
	
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
