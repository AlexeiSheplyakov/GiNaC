/** @file time_lw_C.cpp
 *
 *  Test C from the paper "Comparison of Polynomial-Oriented CAS" by Robert H.
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
	numeric x(13*17*31);
	numeric y(13*19*29);
	
	for (int i=1; i<200; ++i)
		gcd(pow(x,300+(i%181)),pow(y,200+(i%183)));
	
	ex lastgcd = gcd(pow(x,300+(200%181)),pow(y,200+(200%183)));
	if (lastgcd != numeric("53174994123961114423610399251974962981084780166115806651505844915220196792416194060680805428433601792982500430324916963290494659936522782673704312949880308677990050199363768068005367578752699785180694630122629259539608472261461289805919741933")) {
		clog << "gcd(" << x << "^" << 300+(200%181) << ","
		     << y << "^" << 200+(200%183) << ") erroneously returned "
		     << lastgcd << endl;
		return 1;
	}
	return 0;
}

unsigned time_lw_C(void)
{
	unsigned result = 0;
	unsigned count = 0;
	timer rolex;
	double time = .0;
	
	cout << "timing Lewis-Wester test C (gcd of big integers)" << flush;
	clog << "-------Lewis-Wester test C (gcd of big integers):" << endl;
	
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
