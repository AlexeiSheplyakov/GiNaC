/** @file exams.cpp
 *
 *  Main program that calls all individual exams. */

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

#include <stdexcept>

#include "exams.h"

int main()
{
	unsigned result = 0;

#define EXAM(which) \
try { \
	result += exam_ ## which (); \
} catch (const exception &e) { \
	cout << "Error: caught exception " << e.what() << endl; \
	++result; \
}
	
	EXAM(paranoia)
	EXAM(numeric)
	EXAM(powerlaws)
	EXAM(inifcns)
	EXAM(inifcns_nstdsums)
	EXAM(differentiation)
	EXAM(polygcd)
	EXAM(normalization)
	EXAM(pseries)
	EXAM(matrices)
	EXAM(lsolve)
	EXAM(indexed)
	EXAM(color)
	EXAM(clifford)
	EXAM(archive)
	EXAM(structure)
	EXAM(hashmap)
	EXAM(misc)
	
	if (result) {
		cout << "Error: something went wrong. ";
		if (result == 1) {
			cout << "(one failure)" << endl;
		} else {
			cout << "(" << result << " individual failures)" << endl;
		}
		cout << "please check exams.out against exams.ref for more details."
		     << endl << "happy debugging!" << endl;
	}
	
	return result;
}
