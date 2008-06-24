/**
 * @file heur_gcd_oops.cpp Check for a bug in heur_gcd().
 *
 * heur_gcd() did not check if the arguments are integer polynomials
 * (and did not convert them to integer polynomials), which lead to
 * endless loop or (even worse) wrong result.
 */
#include <iostream>
#include "ginac.h"
using namespace GiNaC;
using namespace std;

int main(int argc, char** argv)
{
	cout << "checking if heur_gcd() can cope with rational polynomials. ";
	const symbol x("x");
	const ex _ex1(1);
	ex a1 = x + numeric(5, 4);
	ex a2 = x + numeric(5, 2);
	ex b =  pow(x, 2) + numeric(15, 4)*x + numeric(25, 8);
	// note: both a1 and a2 divide b
	
	// a2 divides b, so cofactor of a2 should be a (rational) number
	ex ca2, cb2;
	ex g2 = gcd(a2, b, &ca2, &cb2);
	if (!is_a<numeric>(ca2)) {
		cerr << "gcd(" << a2 << ", " << b << ") was miscomputed" << endl;
		return 1;
	}
	ex ca1, cb1;
	// a1 divides b, so cofactor of a1 should be a (rational) number
	ex g1 = gcd(a1, b, &ca1, &cb1);
	if (!is_a<numeric>(ca1)) {
		cerr << "gcd(" << a1 << ", " << b << ") was miscomputed" << endl;
		return 1;
	}
	return 0;
}

