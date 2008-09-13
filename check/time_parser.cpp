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

void benchmark_and_cmp(const string& srep, double& t_new, double& t_old)
{
	parser the_parser;
	timer RSD10;
	RSD10.start();
	ex e = the_parser(srep);
	t_new = RSD10.read();
	RSD10.stop();
	if (t_new > 2.0)
		cout << '.' << flush;

	symtab syms = the_parser.get_syms();
	const symbol x = find_or_insert_symbol("x", syms, true);
	lst sl;
	sl = x;
	RSD10.start();
	ex e2(srep, sl);
	t_old = RSD10.read();
	
	if (t_old > 2.0)
		cout << '.' << flush;

	ex dif = (e - e2).expand();
	if (!dif.is_zero()) {
		cerr << "Got a difference: " << dif << endl;
		throw std::logic_error("New and old parser give different results");
	}
}

int main(int argc, char** argv)
{
	cout << "timing GiNaC parser..." << flush;
	randomify_symbol_serials();
	unsigned n_min = 1024;
	unsigned n_max = 32768;
	if (argc > 1)
		n_max = atoi(argv[1]);

	vector<double> times_new, times_old;
	vector<unsigned> ns;
	for (unsigned n = n_min; n <= n_max; n = n << 1) {
		double t_new, t_old;
		string srep = prepare_str(n);
		benchmark_and_cmp(srep, t_new, t_old);
		times_new.push_back(t_new);
		times_old.push_back(t_old);
		ns.push_back(n);
	}

	cout << "OK" << endl;
	cout << "# terms  new parser, s  old parser, s" << endl;
	for (size_t i = 0; i < times_new.size(); i++)
		cout << " " << ns[i] << '\t' << times_new[i] << '\t' << times_old[i] << endl;
	return 0;
}
