/** @file time_lw_O.cpp
 *
 *  Test O1 from the paper "Comparison of Polynomial-Oriented CAS" by Robert H.
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

static const bool do_test = true;  // set to true in order to run this beast

static unsigned test1(void)
{
	symbol a1("a1"), a2("a2"), a3("a3"), a4("a4"), a5("a5"), a6("a6");
	symbol b1("b1"), b2("b2"), b3("b3"), b4("b4"), b5("b5"), b6("b6");
	symbol c1("c1"), c2("c2"), c3("c3"), c4("c4"), c5("c5"), c6("c6");
	ex w1[15][15] = {
		{a6, a5, a4, a3, a2, a1, 0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{0,  0,  a6, 0,  a5, a4, 0,  a3, a2, a1, 0,  0,  0,  0,  0 },
		{0,  a6, 0,  a5, a4, 0,  a3, a2, a1, 0,  0,  0,  0,  0,  0 },
		{0,  0,  0,  a6, 0,  0,  a5, a4, 0,  0,  a3, a2, a1, 0,  0 },
		{0,  0,  0,  0,  a6, 0,  0,  a5, a4, 0,  0,  a3, a2, a1, 0 },
		{0,  0,  0,  0,  0,  a6, 0,  0,  a5, a4, 0,  0,  a3, a2, a1},
		{0,  0,  0,  b6, 0,  0,  b5, b4, 0,  0,  b3, b2, b1, 0,  0 },
		{0,  0,  0,  0,  b6, 0,  0,  b5, b4, 0,  0,  b3, b2, b1, 0 },
		{0,  b6, 0,  b5, b4, 0,  b3, b2, b1, 0,  0,  0,  0,  0,  0 },
		{0,  0,  b6, 0,  b5, b4, 0,  b3, b2, b1, 0,  0,  0,  0,  0 },
		{0,  0,  0,  0,  0,  b6, 0,  0,  b5, b4, 0,  0,  b3, b2, b1},
		{0,  0,  0,  0,  0,  c6, 0,  0,  c5, c4, 0,  0,  c3, c2, c1},
		{0,  0,  c6, 0,  c5, c4, 0,  c3, c2, c1, 0,  0,  0,  0,  0 },
		{0,  c6, 0,  c5, c4, 0,  c3, c2, c1, 0,  0,  0,  0,  0,  0 },
		{0,  0,  0,  0,  c6, 0,  0,  c5, c4, 0,  0,  c3, c2, c1, 0 }
	};
	ex w2[15][15] = {
		{b6, b5, b4, b3, b2, b1, 0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{0,  0,  b6, 0,  b5, b4, 0,  b3, b2, b1, 0,  0,  0,  0,  0 },
		{0,  b6, 0,  b5, b4, 0,  b3, b2, b1, 0,  0,  0,  0,  0,  0 },
		{0,  0,  0,  b6, 0,  0,  b5, b4, 0,  0,  b3, b2, b1, 0,  0 },
		{0,  0,  0,  0,  b6, 0,  0,  b5, b4, 0,  0,  b3, b2, b1, 0 },
		{0,  0,  0,  0,  0,  b6, 0,  0,  b5, b4, 0,  0,  b3, b2, b1},
		{0,  0,  0,  c6, 0,  0,  c5, c4, 0,  0,  c3, c2, c1, 0,  0 },
		{0,  0,  0,  0,  c6, 0,  0,  c5, c4, 0,  0,  c3, c2, c1, 0 },
		{0,  c6, 0,  c5, c4, 0,  c3, c2, c1, 0,  0,  0,  0,  0,  0 },
		{0,  0,  c6, 0,  c5, c4, 0,  c3, c2, c1, 0,  0,  0,  0,  0 },
		{0,  0,  0,  0,  0,  c6, 0,  0,  c5, c4, 0,  0,  c3, c2, c1},
		{0,  0,  0,  0,  0,  a6, 0,  0,  a5, a4, 0,  0,  a3, a2, a1},
		{0,  0,  a6, 0,  a5, a4, 0,  a3, a2, a1, 0,  0,  0,  0,  0 },
		{0,  a6, 0,  a5, a4, 0,  a3, a2, a1, 0,  0,  0,  0,  0,  0 },
		{0,  0,  0,  0,  a6, 0,  0,  a5, a4, 0,  0,  a3, a2, a1, 0 }
	};
	ex w3[15][15] = {
		{c6, c5, c4, c3, c2, c1, 0,  0,  0,  0,  0,  0,  0,  0,  0 },
		{0,  0,  c6, 0,  c5, c4, 0,  c3, c2, c1, 0,  0,  0,  0,  0 },
		{0,  c6, 0,  c5, c4, 0,  c3, c2, c1, 0,  0,  0,  0,  0,  0 },
		{0,  0,  0,  c6, 0,  0,  c5, c4, 0,  0,  c3, c2, c1, 0,  0 },
		{0,  0,  0,  0,  c6, 0,  0,  c5, c4, 0,  0,  c3, c2, c1, 0 },
		{0,  0,  0,  0,  0,  c6, 0,  0,  c5, c4, 0,  0,  c3, c2, c1},
		{0,  0,  0,  a6, 0,  0,  a5, a4, 0,  0,  a3, a2, a1, 0,  0 },
		{0,  0,  0,  0,  a6, 0,  0,  a5, a4, 0,  0,  a3, a2, a1, 0 },
		{0,  a6, 0,  a5, a4, 0,  a3, a2, a1, 0,  0,  0,  0,  0,  0 },
		{0,  0,  a6, 0,  a5, a4, 0,  a3, a2, a1, 0,  0,  0,  0,  0 },
		{0,  0,  0,  0,  0,  a6, 0,  0,  a5, a4, 0,  0,  a3, a2, a1},
		{0,  0,  0,  0,  0,  b6, 0,  0,  b5, b4, 0,  0,  b3, b2, b1},
		{0,  0,  b6, 0,  b5, b4, 0,  b3, b2, b1, 0,  0,  0,  0,  0 },
		{0,  b6, 0,  b5, b4, 0,  b3, b2, b1, 0,  0,  0,  0,  0,  0 },
		{0,  0,  0,  0,  b6, 0,  0,  b5, b4, 0,  0,  b3, b2, b1, 0 }
	};
	matrix d1(15,15), d2(15,15), d3(15,15);
	for (unsigned r=0; r<15; ++r) {
		for (unsigned c=0; c<15; ++c) {
			d1.set(r,c,w1[r][c]);
			d2.set(r,c,w2[r][c]);
			d3.set(r,c,w3[r][c]);
		}
	}
	unsigned nops1 = nops(d1.determinant());  cout << '.' << flush;
	unsigned nops2 = nops(d2.determinant());  cout << '.' << flush;
	unsigned nops3 = nops(d3.determinant());  cout << '.' << flush;
	
	if ((nops1 != 37490) || (nops2 != 37490) || (nops3 != 37490)) {
		clog << "Determinants were miscalculated" << endl;
		return 1;
	}
	return 0;
}

unsigned time_lw_O(void)
{
	unsigned result = 0;
	unsigned count = 0;
	timer rolex;
	double time = .0;
	
	cout << "timing Lewis-Wester test O1 (three 15x15 dets)" << flush;
	clog << "-------Lewis-Wester test O1 (three 15x15 dets):" << endl;
	
	if (do_test) {
		rolex.start();
		// correct for very small times:
		do {
			result = test1();
			++count;
		} while ((time=rolex.read())<0.1 && !result);
		
		if (!result) {
			cout << " passed ";
			clog << "(no output)" << endl;
		} else {
			cout << " failed ";
		}
		cout << int(1000*(time/(3*count)))*0.001 << "s (average)" << endl;
	} else {
		cout << " disabled" << endl;
		clog << "(no output)" << endl;
	}
	
	return result;
}
