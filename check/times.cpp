/** @file times.cpp
 *
 *  Main program that calls the individual timings. */

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
#include "times.h"

/** Generate a random amount of symbols and destroy them again immediatly.
 *  This operation effectively makes the serial numbers of all subsequent
 *  symbols unpredictable.  If the serials are unpredictable, then so are
 *  their hash values.  If the hash values are unpredictable, then so are
 *  the canonical orderings.  If the canonical orderings are unpredictable,
 *  all subsequent times are subject to some variation.  This variation,
 *  however is natural and desireable for two reasons: First, we cannot know
 *  how many symbols have been generated before in real world computations.
 *  Second, the following timings are subject to some semi-random variation
 *  anyways because short timings need to be repeated until enough time has
 *  gone by for the measurement to be reliable.  During this process the serial
 *  numbers will be shifted anyways in a semi-random way.  It is better not
 *  to lull the user in a false sense of reproducibility and instead confront
 *  her with the normal variation to be expected.
 */
void randomify_symbol_serials()
{
	srand((unsigned)time(NULL));
	const int m = rand() % 666;
	for (int s=0; s<m; ++s ) {
		symbol* tmp = new symbol;
		delete tmp;
	}
}

int main()
{
	randomify_symbol_serials();

	unsigned result = 0;
	
#define TIME(which) \
try { \
	result += time_ ## which (); \
} catch (const exception &e) { \
	cout << "Error: caught exception " << e.what() << endl; \
	++result; \
}

	TIME(dennyfliegner)
	TIME(gammaseries)
	TIME(vandermonde)
	TIME(toeplitz)
	TIME(hashmap)
	TIME(lw_A)
	TIME(lw_B)
	TIME(lw_C)
	TIME(lw_D)
	TIME(lw_E)
	TIME(lw_F)
	TIME(lw_G)
	TIME(lw_H)
	TIME(lw_IJKL)
	TIME(lw_M1)
	TIME(lw_M2)
	TIME(lw_N)
	TIME(lw_O)
	TIME(lw_P)
	TIME(lw_Pprime)
	TIME(lw_Q)
	TIME(lw_Qprime)
	TIME(antipode)
	TIME(fateman_expand)
	
	if (result) {
		cout << "Error: something went wrong. ";
		if (result == 1) {
			cout << "(one failure)" << endl;
		} else {
			cout << "(" << result << " individual failures)" << endl;
		}
		cout << "please check times.out against times.ref for more details."
		     << endl << "happy debugging!" << endl;
	}
	
	return result;
}
