#include <iostream>
#include <string>
#include <sstream>
#include <cstddef>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include "ginac.h"
#include "timer.h"
extern void randomify_symbol_serials();
using namespace std;
using namespace GiNaC;

/// make a string "1+x+2*x^2+...+n*x^n"
static string prepare_str(const unsigned n, const char x = 'x')
{
	ostringstream s;
	s << x;
	for (unsigned i = 2; i < n; i++)
		s << '+' << i << '*' << x << '^' << i; 
	return s.str();
}

static double benchmark_and_cmp(const string& srep)
{
	parser the_parser;
	timer RSD10;
	RSD10.start();
	ex e = the_parser(srep);
	const double t = RSD10.read();
	return t;
}

int main(int argc, char** argv)
{
	cout << "timing GiNaC parser..." << flush;
	randomify_symbol_serials();
	unsigned n_min = 1024;
	unsigned n_max = 32768;
	if (argc > 1)
		n_max = atoi(argv[1]);

	vector<double> times;
	vector<unsigned> ns;
	for (unsigned n = n_min; n <= n_max; n = n << 1) {
		string srep = prepare_str(n);
		const double t = benchmark_and_cmp(srep);
		times.push_back(t);
		ns.push_back(n);
	}

	cout << "OK" << endl;
	cout << "# terms  time, s" << endl;
	for (size_t i = 0; i < times.size(); i++)
		cout << " " << ns[i] << '\t' << times[i] << endl;
	return 0;
}
