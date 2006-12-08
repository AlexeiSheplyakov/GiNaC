/*
 * This program can be used to find the coefficients needed to approximate
 * the gamma function using the Lanczos approximation.
 *
 * Usage: lanczos -n order -D digits
 *
 * The order defaults to 10. digits defaults to GiNaCs default.
 * It is recommended to run the program several times with an increasing
 * value for digits until numerically stablilty for the values has been
 * reached. The program will also print the number of digits for which
 * the approximation is still reliable. This will be lower then the
 * number of digits given at command line. It is determined by comparing
 * Gamma(1/2) to sqrt(Pi). Note that the program may crash if the number of
 * digits is unreasonably small for the given order. Another thing that
 * can happen if the number of digits is too small is that the program will
 * print "Forget it, this is waaaaaaay too inaccurate." at the top of the
 * output.
 *
 * The gamma function can be (for real_part(z) > -1/2) calculated using
 *
 *    Gamma(z+1) = sqrt(2*Pi)*power(z+g+ex(1)/2, z+ex(1)/2)*exp(-(z+g+ex(1)/2))
 *                  *A_g(z),
 * where,
 * 
 *    A_g(z) = coeff[0] + coeff[1]/(z+1) + coeff[2]/(z+2) + ...
 *                + coeff[N-1]/(z+N-1).
 *
 * The value of g is taken to be equal to the order N.
 *
 * More details can be found at Wikipedia:
 * http://en.wikipedia.org/wiki/Lanczos_approximation.
 *
 * (C) 2006 Chris Dams
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301 USA
 */

#include <ginac/ginac.h>
#include <iostream>

using namespace std;
using namespace GiNaC;

/*
 * Double factorial function.
 */
ex doublefact(int i)
{	if (i==0 || i==-1)
		return 1;
	if (i>0)
		return i*doublefact(i-2);
	cout << "function doublefact called with wrong argument" << endl;
	exit(1);
}

/*
 * Chebyshev polynomial coefficient matrix as far as is required for
 * the Lanczos approximation.
 */
void initialize_C_vec(vector<exvector> &C, int size)
{	C.reserve(size);
	for (int i=0; i<size; ++i)
		C.push_back(exvector(size, 0));
	C[1][1] = 1;
	C[2][2] = 1;
	for (int i=3; i<size; i+=2)
		C[i][1] = -C[i-2][1];
	for (int i=3; i<size; ++i)
		C[i][i] = 2*C[i-1][i-1];
	for (int j=2; j<size; ++j)
		for (int i=j+2; i<size; i+=2)
			C[i][j] = 2*C[i-1][j-1] - C[i-2][j];
}

/*
 * The coefficients p_n(g) that occur in the Lanczos approximation.
 */
ex p(int k, ex g, const vector<exvector> &C)
{	ex result = 0;
	for (int a=0; a<=k; ++a)
		result += 2*C[2*k+1][2*a+1]/sqrt(Pi)*doublefact(2*a-1)
			*power(2*a+2*g+1, -a-ex(1)/2)*exp(a+g+ex(1)/2);
	return result;
}

/*
 * Check wheter x is of the form 1/(z+n) where z is given by the second
 * argument and n should be a positive integer that is returned as the
 * third argument. If x is not of this form, false is returned.
 */
bool is_z_pole(const ex &x, const ex &z, ex &n)
{	if (!is_a<power>(x))
		return false;
	if (x.op(1) != -1)
		return false;
	ex denom = x.op(0);
	if (!is_a<add>(denom))
		return false;
	if (denom.nops() != 2)
		return false;
	if (denom.op(0) != z)
		return false;
	if (!denom.op(1).info(info_flags::posint))
		return false;
	n = denom.op(1);
	return true;
}

/*
 * Simplify the expression x by applying the rules
 *
 *    1/(z+n)/(z+m) = 1/(n-m)/(z+m) - 1/(n-m)/(z+n);
 *    z^m/(z+n) = z^(m-1) - n*z^(m-1)/(z+n)
 *
 * as often as possible, where z is given as an argument to the function
 * and n and m are arbitrary positive numbers.
 */
ex poles_simplify(const ex &x, const ex &z)
{	if (is_a<mul>(x))
	{	for (int i=0; i<x.nops(); ++i)
		{	ex arg1;
			if (!is_z_pole(x.op(i), z, arg1))
				continue;
			for (int j=i+1; j<x.nops(); ++j)
			{	ex arg2;
				if (!is_z_pole(x.op(j), z, arg2))
					continue;
				ex result = x/x.op(i)/(arg1-arg2)-x/x.op(j)/(arg1-arg2);
				result = poles_simplify(result, z);
				return result;
			}
		}
		ex result = expand(x);
		if (is_a<add>(result))
			return poles_simplify(result, z);
		for (int i=0; i<x.nops(); ++i)
		{	ex arg1;
			if (!is_z_pole(x.op(i), z, arg1))
				continue;
			for (int j=0; j<x.nops(); ++j)
			{	ex expon = 0;
				if (is_a<power>(x.op(j)) && x.op(j).op(0)==z
						&& x.op(j).op(1).info(info_flags::posint))
					expon = x.op(j).op(1);
				if (x.op(j) == z)
					expon = 1;
				if (expon == 0)
					continue;
				ex result = x/x.op(i)/z - arg1*x/z;
				result = poles_simplify(result, z);
				return result;
			}
		}
		return x;
	}
	if (is_a<add>(x))
	{	pointer_to_map_function_1arg<const ex&> mf(poles_simplify, z);
		return x.map(mf);
	}
	return x;
}

/*
 * Calculate the expression A_g(z) that occurs in the expression for the
 * Lanczos approximation of order n. This is returned as an expression of
 * the form
 *
 *    A_g(z) = coeff[0] + coeff[1]/(z+1) + coeff[2]/(z+2) + ...
 *                + coeff[N-1]/(z+N-1).
 */
ex A(const ex &g, const ex &z, int n)
{	vector<exvector> C;
	initialize_C_vec(C, 2*n+2);
	ex result = evalf(p(0, g, C))/2;
	ex fraction = 1;
	for (int i=1; i<n; ++i)
	{	fraction *= (z-i+1)/(z+i);
		fraction = poles_simplify(fraction, z);
		result += evalf(p(i, g, C))*fraction;
	}
	result = poles_simplify(result, z);
	return result;
}

/*
 * The exvector coeffs should contain order elements and these are set to
 * the coefficients that belong to the Lanczos approximation of the gamma
 * function at the given order.
 */
void calc_lanczos_coeffs(exvector &coeffs, double g_val, int order)
{	symbol g("g"), z("z");
	ex result = A(g, z, order);
	result = result.subs(g==g_val);
	for (int i=0; i<result.nops(); ++i)
	{	if (is_a<numeric>(result.op(i)))
			coeffs[0] = result.op(i);
		else
		{	ex n;
			is_z_pole(result.op(i).op(0), z, n);
			coeffs[ex_to<numeric>(n).to_int()] = result.op(i).op(1);
		}
	}
}

/*
 * Calculate Gamma(z) using the Lanczos approximation with parameter g and
 * coefficients stored in the exvector coeffs.
 */
ex calc_gamma(const ex &z, const ex &g, exvector &coeffs)
{	if (real_part(evalf(z)) < 0.5)
		 return evalf(Pi/sin(Pi*z)/calc_gamma(1-z, g, coeffs));
	ex A = coeffs[0];
	for (int i=1; i<coeffs.size(); ++i)
		A += evalf(coeffs[i]/(z-1+i));
	ex result = sqrt(2*Pi)*power(z+g-ex(1)/2, z-ex(1)/2)*exp(-(z+g-ex(1)/2))*A;
	return evalf(result);
}

void usage(char *progname)
{	cout << "Usage: " << progname << " -n order -D digits" << endl;
	exit(0);
}

void read_options(int argc, char**argv, int &order)
{  int c;
   while((c=getopt(argc,argv,"n:D:"))!=-1)
   {  if(c=='n')
         order = atoi(optarg);
      else if (c=='D')
         Digits = atoi(optarg);
      else
         usage(argv[0]);
   }
   if(optind!=argc)
      usage(argv[0]);
}

/*
 * Do something stupid to the number x in order to round it to the current
 * numer of digits. Does somebody know a better way to do this? In that case,
 * please fix me.
 */
ex round(const ex &x)
{	return ex_to<numeric>(x).add(numeric("0.0"));
}

int main(int argc, char *argv[])
{	
/*
 * Handle command line options.
 */
	int order = 10;
	read_options(argc, argv, order);
/*
 * Calculate coefficients.
 */
	const int g_val = order;
	exvector coeffs(order);
	calc_lanczos_coeffs(coeffs, g_val, order);
/*
 * Determine the accuracy by comparing Gamma(1/2) to sqrt(Pi).
 */
	ex gamma_half = calc_gamma(ex(1)/2, g_val, coeffs);
	ex digits = -log(abs((gamma_half - sqrt(Pi)))/sqrt(Pi))/log(10.0);
	digits = evalf(digits);
	int i_digits = int(ex_to<numeric>(digits).to_double());
	if (digits < 1)
		cout << "Forget it, this is waaaaaaay too inaccurate." << endl;
	else
		cout << "Reliable digits: " << i_digits << endl;
/*
 * Print the coefficients.
 */
	Digits = i_digits + 10; // Don't print too many spurious digits.
	for (int i=0; i<coeffs.size(); ++i)
		cout << "coeffs_" << order << "[" << i << "] = numeric(\""
		     << round(coeffs[i]) << "\");"<< endl;
	return 0;
}
