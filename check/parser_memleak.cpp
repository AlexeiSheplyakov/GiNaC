/**
 * This small program exhibits the memory leak in the ginac_yylex().
 * Run it as
 *
 * valgrind --leak-check=yes  ./parser_memleak
 *
 * or simply
 *
 * ulimit -v `expr 64 \* 1024` ./parser_memleak
 */
#include <iostream>
#include <stdexcept>
#include <ginac/ginac.h>
using namespace std;
using namespace GiNaC;

int main(int argc, char** argv) {
	const symbol x("x"), y("y");
	const lst syms(x, y);
	// parser-generated symbol => memory leak.
	static const char* str[] = { "x²+2*x*y + cos(x)", "Li2(x/y) + log(y/x)" };
	
	// depends on the amount of the available VM, compiler options, etc.
	const unsigned N_max = 500000;
	unsigned N=0;
	ex e;
	try {
		for (; N < N_max; N++) {
			e = ex(str[N & 1], syms);
		}
	} catch (std::bad_alloc) {
		cerr << "N = " << N << endl;
		return 1;
	}
	return 0;
}
