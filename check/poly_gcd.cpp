// check/poly_gcd.cpp

/* Some test with polynomial GCD calculations. See also the checks for
 * rational function normalization in normalization.cpp. */

#include <ginac/ginac.h>

const int MAX_VARIABLES = 5;

static symbol x("x"), z("z");
static symbol y[MAX_VARIABLES];

// GCD = 1
static unsigned poly_gcd1(void)
{
	for (int v=1; v<=MAX_VARIABLES; v++) {
		ex e1 = x;
		ex e2 = pow(x, 2);
		for (int i=0; i<v; i++) {
			e1 += y[i];
			e2 += pow(y[i], 2);
		}

		ex f = (e1 + 1) * (e1 + 2);
		ex g = e2 * (-pow(x, 2) * y[0] * 3 + pow(y[0], 2) - 1);
		ex r = gcd(f, g);
		if (r != exONE()) {
			clog << "case 1, gcd(" << f << "," << g << ") = " << r << " (should be 1)" << endl;
			return 1;
		}
	}
	return 0;
}

// Linearly dense quartic inputs with quadratic GCDs
static unsigned poly_gcd2(void)
{
	for (int v=1; v<=MAX_VARIABLES; v++) {
		ex e1 = x;
		ex e2 = x;
		for (int i=0; i<v; i++) {
			e1 += y[i];
			e2 -= y[i];
		}

		ex d = pow(e1 + 1, 2);
		ex f = d * pow(e2 - 2, 2);
		ex g = d * pow(e1 + 2, 2);
		ex r = gcd(f, g);
        ex re=r.expand();
        ex df1=r-d;
        ex df=(r-d).expand();
		if ((r - d).expand().compare(exZERO()) != 0) {
			clog << "case 2, gcd(" << f << "," << g << ") = " << r << " (should be " << d << ")" << endl;
			return 1;
		}
	}
	return 0;
}

// Sparse GCD and inputs where degrees are proportional to the number of variables
static unsigned poly_gcd3(void)
{
	for (int v=1; v<=MAX_VARIABLES; v++) {
		ex e1 = pow(x, v + 1);
		for (int i=0; i<v; i++)
			e1 += pow(y[i], v + 1);

		ex d = e1 + 1;
		ex f = d * (e1 - 2);
		ex g = d * (e1 + 2);
		ex r = gcd(f, g);
		if ((r - d).expand().compare(exZERO()) != 0) {
			clog << "case 3, gcd(" << f << "," << g << ") = " << r << " (should be " << d << ")" << endl;
			return 1;
		}
	}
	return 0;
}

// Variation of case 3; major performance degradation with PRS
static unsigned poly_gcd3p(void)
{
	for (int v=1; v<=MAX_VARIABLES; v++) {
		ex e1 = pow(x, v + 1);
		ex e2 = pow(x, v);
		for (int i=0; i<v; i++) {
			e1 += pow(y[i], v + 1);
			e2 += pow(y[i], v);
		}

		ex d = e1 + 1;
		ex f = d * (e1 - 2);
		ex g = d * (e2 + 2);
		ex r = gcd(f, g);
		if ((r - d).expand().compare(exZERO()) != 0) {
			clog << "case 3p, gcd(" << f << "," << g << ") = " << r << " (should be " << d << ")" << endl;
			return 1;
		}
	}
	return 0;
}

// Quadratic non-monic GCD; f and g have other quadratic factors
static unsigned poly_gcd4(void)
{
	for (int v=1; v<=MAX_VARIABLES; v++) {
		ex e1 = pow(x, 2) * pow(y[0], 2);
		ex e2 = pow(x, 2) - pow(y[0], 2);
		ex e3 = x * y[0];
		for (int i=1; i<v; i++) {
			e1 += pow(y[i], 2);
			e2 += pow(y[i], 2);
			e3 += y[i];
		}

		ex d = e1 + 1;
		ex f = d * (e2 - 1);
		ex g = d * pow(e3 + 2, 2);
		ex r = gcd(f, g);
		if ((r - d).expand().compare(exZERO()) != 0) {
			clog << "case 4, gcd(" << f << "," << g << ") = " << r << " (should be " << d << ")" << endl;
			return 1;
		}
	}
	return 0;
}

// Completely dense non-monic quadratic inputs with dense non-monic linear GCDs
static unsigned poly_gcd5(void)
{
	for (int v=1; v<=MAX_VARIABLES; v++) {
		ex e1 = x + 1;
		ex e2 = x - 2;
		ex e3 = x + 2;
		for (int i=0; i<v; i++) {
			e1 *= y[i] + 1;
			e2 *= y[i] - 2;
			e3 *= y[i] + 2;
		}

		ex d = e1 - 3;
		ex f = d * (e2 + 3);
		ex g = d * (e3 - 3);
		ex r = gcd(f, g);
		if ((r - d).expand().compare(exZERO()) != 0) {
			clog << "case 5, gcd(" << f << "," << g << ") = " << r << " (should be " << d << ")" << endl;
			return 1;
		}
	}
	return 0;
}

// Sparse non-monic quadratic inputs with linear GCDs
static unsigned poly_gcd5p(void)
{
	for (int v=1; v<=MAX_VARIABLES; v++) {
		ex e1 = x;
		for (int i=0; i<v; i++)
			e1 *= y[i];

		ex d = e1 - 1;
		ex f = d * (e1 + 3);
		ex g = d * (e1 - 3);
		ex r = gcd(f, g);
		if ((r - d).expand().compare(exZERO()) != 0) {
			clog << "case 5p, gcd(" << f << "," << g << ") = " << r << " (should be " << d << ")" << endl;
			return 1;
		}
	}
	return 0;
}

// Trivariate inputs with increasing degrees
static unsigned poly_gcd6(void)
{
	symbol y("y");

	for (int j=1; j<=MAX_VARIABLES; j++) {
		ex d = pow(x, j) * y * (z - 1);
		ex f = d * (pow(x, j) + pow(y, j + 1) * pow(z, j) + 1);
		ex g = d * (pow(x, j + 1) + pow(y, j) * pow(z, j + 1) - 7);
		ex r = gcd(f, g);
		if ((r - d).expand().compare(exZERO()) != 0) {
			clog << "case 6, gcd(" << f << "," << g << ") = " << r << " (should be " << d << ")" << endl;
			return 1;
		}
	}
	return 0;
}

// Trivariate polynomials whose GCD has common factors with its cofactors
static unsigned poly_gcd7(void)
{
	symbol y("y");
	ex p = x - y * z + 1;
	ex q = x - y + z * 3;

	for (int j=1; j<=3; j++) {
		for (int k=j+1; k<=4; k++) {
			ex d = pow(p, j) * pow(q, j);
			ex f = pow(p, j) * pow(q, k);
			ex g = pow(p, k) * pow(q, j); 
			ex r = gcd(f, g);
			if ((r - d).expand().compare(exZERO()) != 0 && (r + d).expand().compare(exZERO()) != 0) {
				clog << "case 7, gcd(" << f << "," << g << ") = " << r << " (should be " << d << ")" << endl;
				return 1;
			}
		}
	}
	return 0;
}

unsigned poly_gcd(void)
{
    unsigned result = 0;

	cout << "checking polynomial GCD computation..." << flush;
	clog << "---------polynomial GCD computation:" << endl;

	result += poly_gcd1();
	result += poly_gcd2();
	result += poly_gcd3();
//	result += poly_gcd3p();	// takes extremely long (PRS "worst" case)
	result += poly_gcd4();
	result += poly_gcd5();
	result += poly_gcd5p();
	result += poly_gcd6();
	result += poly_gcd7();

	if (!result) {
		cout << " passed ";
        clog << "(no output)" << endl;
    } else {
		cout << " failed ";
    }
	return result;
}
