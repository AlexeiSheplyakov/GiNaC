/** @file exam_normalization.cpp
 *
 *  Rational function normalization test suite. */

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

#include "exams.h"

static symbol w("w"), x("x"), y("y"), z("z");

static unsigned check_normal(const ex &e, const ex &d)
{
	ex en = e.normal();
	if (en.compare(d) != 0) {
		clog << "normal form of " << e << " erroneously returned "
		     << en << " (should be " << d << ")" << endl;
		return 1;
	}
	return 0;
}

static unsigned exam_normal1(void)
{
	unsigned result = 0;
	ex e, d;
	
	// Expansion
	e = pow(x, 2) - (x+1)*(x-1) - 1;
	d = 0;
	result += check_normal(e, d);
	
	// Expansion inside functions
	e = sin(x*(x+1)-x) + 1;
	d = sin(pow(x, 2)) + 1;
	result += check_normal(e, d);
	
	// Fraction addition
	e = 2/x + y/3;
	d = (x*y + 6) / (x*3);
	result += check_normal(e, d);
	
	e = pow(x, -1) + x/(x+1);
	d = (pow(x, 2)+x+1)/(x*(x+1));
	result += check_normal(e, d);

	return result;
}

static unsigned exam_normal2(void)
{
	unsigned result = 0;
	ex e, d;
	
	// Fraction cancellation
	e = numeric(1)/2 * z * (2*x + 2*y);
	d = z * (x + y);
	result += check_normal(e, d);
	
	e = numeric(1)/6 * z * (3*x + 3*y) * (2*x + 2*w);
	d = z * (x + y) * (x + w);
	result += check_normal(e, d);
	
	e = (3*x + 3*y) * (w/3 + z/3);
	d = (x + y) * (w + z);
	result += check_normal(e, d);
	
	e = (pow(x, 2) - pow(y, 2)) / pow(x-y, 3);
	d = (x + y) / (pow(x, 2) + pow(y, 2) - x * y * 2);
	result += check_normal(e, d);
	
	e = (pow(x, -1) + x) / (pow(x , 2) * 2 + 2);
	d = pow(x * 2, -1);
	result += check_normal(e, d);
	
	// Fraction cancellation with rational coefficients
	e = (pow(x, 2) - pow(y, 2)) / pow(x/2 - y/2, 3);
	d = (8 * x + 8 * y) / (pow(x, 2) + pow(y, 2) - x * y * 2);
	result += check_normal(e, d);
	
	// Fraction cancellation with rational coefficients
	e = z/5 * (x/7 + y/10) / (x/14 + y/20);
	d = 2*z/5;
	result += check_normal(e, d);
	
	return result;
}

static unsigned exam_normal3(void)
{
	unsigned result = 0;
	ex e, d;
	
	// Distribution of powers
	e = pow(x/y, 2);
	d = pow(x, 2) / pow(y, 2);
	result += check_normal(e, d);
	
	// Distribution of powers (integer, distribute) and fraction addition
	e = pow(pow(x, -1) + x, 2);
	d = pow(pow(x, 2) + 1, 2) / pow(x, 2);
	result += check_normal(e, d);
	
	// Distribution of powers (non-integer, don't distribute) and fraction addition
	e = pow(pow(x, -1) + x, numeric(1)/2);
	d = pow((pow(x, 2) + 1) / x, numeric(1)/2);
	result += check_normal(e, d);
	
	return result;
}

static unsigned exam_normal4(void)
{
	unsigned result = 0;
	ex e, d;
	
	// Replacement of functions with temporary symbols and fraction cancellation
	e = pow(sin(x), 2) - pow(cos(x), 2);
	e /= sin(x) + cos(x);
	d = sin(x) - cos(x);
	result += check_normal(e, d);
	
	// Replacement of non-integer powers with temporary symbols
	e = (pow(numeric(2), numeric(1)/2) * x + x) / x;
	d = pow(numeric(2), numeric(1)/2) + 1;
	result += check_normal(e, d);
	
	// Replacement of complex numbers with temporary symbols
	e = (x + y + x*I + y*I) / (x + y);
	d = 1 + I;
	result += check_normal(e, d);
	
	e = (pow(x, 2) + pow(y, 2)) / (x + y*I);
	d = e;
	result += check_normal(e, d);
	
	// More complex rational function
	e = (pow(x-y*2,4)/pow(pow(x,2)-pow(y,2)*4,2)+1)*(x+y*2)*(y+z)/(pow(x,2)+pow(y,2)*4);
	d = (y*2 + z*2) / (x + y*2);
	result += check_normal(e, d);
	
	return result;
}

unsigned exam_normalization(void)
{
	unsigned result = 0;
	
	cout << "examining rational function normalization" << flush;
	clog << "----------rational function normalization:" << endl;
	
	result += exam_normal1();  cout << '.' << flush;
	result += exam_normal2();  cout << '.' << flush;
	result += exam_normal3();  cout << '.' << flush;
	result += exam_normal4();  cout << '.' << flush;
	
	if (!result) {
		cout << " passed " << endl;
		clog << "(no output)" << endl;
	} else {
		cout << " failed " << endl;
	}
	
	return result;
}
