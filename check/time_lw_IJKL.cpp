/** @file time_lw_IJKL.cpp
 *
 *  Tests I, J, K and L from the paper "Comparison of Polynomial-Oriented CAS"
 *  by Robert H. Lewis and Michael Wester. */

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

static unsigned test(unsigned n)
{
	unsigned result = 0;
	timer cartier;
	char name = (n==40?'I':(n==70?'K':'?'));
	
	cout << "timing Lewis-Wester test " << name
	     << " (invert rank " << n << " Hilbert)" << flush;
	clog << "-------Lewis-Wester test " << name
	     << " (invert rank " << n << " Hilbert):" << endl;
	
	// Create a rank n Hilbert matrix:
	matrix H(n,n);
	for (unsigned r=0; r<n; ++r)
		for (unsigned c=0; c<n; ++c)
			H.set(r,c,numeric(1,r+c+1));
	// invert it:
	cartier.start();
	matrix Hinv(n,n);
	Hinv = H.inverse();
	cout << ". passed ";
	clog << "(no output)" << endl;
	cout << int(1000*cartier.read())*0.001 << 's' << endl;
	
	// check result:
	name = (n==40?'J':(n==70?'L':'?'));
	
	cout << "timing Lewis-Wester test " << name
	     << " (check rank " << n << " Hilbert)" << flush;
	clog << "-------Lewis-Wester test " << name
	     << " (check rank " << n << " Hilbert):" << endl;
	
	cartier.reset();
	matrix identity = H.mul(Hinv);
	bool correct = true;
	for (unsigned r=0; r<n; ++r)
		for (unsigned c=0; c<n; ++c) {
			if (r==c) {
				if (identity(r,c)!=1)
					correct = false;
			} else {
				if (identity(r,c)!=0)
					correct = false;
			}
		}
	if (correct) {
		cout << ". passed ";
		clog << "(no output)" << endl;
	} else {
		cout << ". failed ";
		++result;
	}
	cout << int(1000*cartier.read())*0.001 << 's' << endl;
	return result;
}

unsigned time_lw_IJKL(void)
{
	unsigned result = 0;
	
	// Tests I and J:
	result += test(40);
	// Tests K and L:
	result += test(70);
	
	return result;
}
