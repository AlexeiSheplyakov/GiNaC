#! ./run_exams --silent
unsigned result = 0;
/*
 * A simple substitution test.  If it fails you are really in trouble:
 */
{
	symbol x("x"), y("y");
	ex e, f;
	e = pow(x+y,200).expand();
	f = e.subs(x == -y);
	if (f != 0)
		++result;
}
/*
 * Denny Fliegner's test using vector<symbol> is rather hard on Cint.  If
 * it fails due to missing <sstream> it's due to your compiler not being
 * fully ANSI-compliant.  Don't worry.
 */
#include <sstream>
{
	vector<symbol> a;
	ex bigsum = 0;
	for (int i=0; i<42; ++i) {
        ostringstream buf;
        buf << "a" << i << ends;
        a.push_back(symbol(buf.str()));
        bigsum += a[i];
	}
	ex sbtrct = -bigsum + a[0] + a[1];
	if (pow(bigsum,2).expand().subs(a[0]==sbtrct).expand() != pow(a[1],2))
		++result;
}
exit(result);
