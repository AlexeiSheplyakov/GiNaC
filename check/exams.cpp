/** @file exams.cpp
 *
 *  Main program that calls all individual exams. */

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

#include "exams.h"

int main()
{
	unsigned result = 0;
	
	try {
		result += exam_paranoia();
	} catch (const exception &e) {
		cout << "Error: caught exception " << e.what() << endl;
		++result;
	}
	
	try {
		result += exam_numeric();
	} catch (const exception &e) {
		cout << "Error: caught exception " << e.what() << endl;
		++result;
	}
	
	try {
		result += exam_powerlaws();
	} catch (const exception &e) {
		cout << "Error: caught exception " << e.what() << endl;
		++result;
	}
	
	try {
		result += exam_inifcns();
	} catch (const exception &e) {
		cout << "Error: caught exception " << e.what() << endl;
		++result;
	}
	
	try {
		result += exam_differentiation();
	} catch (const exception &e) {
		cout << "Error: caught exception " << e.what() << endl;
		++result;
	}
	
	try {
		result += exam_polygcd();
	} catch (const exception &e) {
		cout << "Error: caught exception " << e.what() << endl;
		++result;
	}
	
	try {
		result += exam_normalization();
	} catch (const exception &e) {
		cout << "Error: caught exception " << e.what() << endl;
		++result;
	}
	
	try {
		result += exam_pseries();
	} catch (const exception &e) {
		cout << "Error: caught exception " << e.what() << endl;
		++result;
	}
	
	try {
		result += exam_matrices();
	} catch (const exception &e) {
		cout << "Error: caught exception " << e.what() << endl;
		++result;
	}
	
	try {
		result += exam_lsolve();
	} catch (const exception &e) {
		cout << "Error: caught exception " << e.what() << endl;
		++result;
	}
	
	try {
		result += exam_noncommut();
	} catch (const exception &e) {
		cout << "Error: caught exception " << e.what() << endl;
		++result;
	}
	
	try {
		result += exam_misc();
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
		cout << "please check exams.out against exams.ref for more details."
		     << endl << "happy debugging!" << endl;
	}
	
	return result;
}
