// check/normalization.cpp

/* Rational function normalization test-suite. */

#include <ginac/ginac.h>

static symbol x("x"), y("y"), z("z");

static unsigned check_normal(const ex &e, const ex &d)
{
	ex en = e.normal();
	if (en.compare(d) != 0) {
		clog << "normal form of " << e << " is " << en << " (should be " << d << ")" << endl;
		return 1;
	}
	return 0;
}

static unsigned normal1(void)
{
	unsigned result = 0;
	ex e, d;

	// Expansion
	e = pow(x, 2) - (x+1)*(x-1) - 1;
	d = exZERO();
	result += check_normal(e, d);

	// Expansion inside functions
	e = sin(x*(x+1)-x) + 1;
	d = sin(pow(x, 2)) + 1;
	result += check_normal(e, d);

	// Fraction addition
	e = numeric(2)/x + y/3;
	d = (x*y/3 + 2) / x;
	result += check_normal(e, d);

	// Fraction addition
	e = pow(x, -1) + x/(x+1);
	d = (pow(x, 2)+x+1)/(x*(x+1));
	result += check_normal(e, d);

	// Fraction cancellation
	e = (pow(x, 2) - pow(y, 2)) / pow(x-y, 3);
	d = (x + y) / (pow(x, 2) + pow(y, 2) - x * y * 2);
	result += check_normal(e, d);

	// Fraction cancellation
	e = (pow(x, -1) + x) / (pow(x , 2) * 2 + 2);
	d = pow(x * 2, -1);
	result += check_normal(e, d);

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

unsigned normalization(void)
{
	unsigned result = 0;

	cout << "checking rational function normalization..." << flush;
	clog << "---------rational function normalization:" << endl;

	result += normal1();

	if (!result) {
		cout << " passed ";
		clog << "(no output)" << endl;
	} else {
		cout << " failed ";
	}
	return result;
}
