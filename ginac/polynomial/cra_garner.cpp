#include <cln/integer.h>
#include <cln/modinteger.h>
#include <vector>
#include <cstddef>
#include "cra_garner.hpp"

namespace cln
{
using std::vector;
using std::size_t;

static cl_I 
retract_symm(const cl_MI& x, const cl_modint_ring& R,
	     const cl_I& modulus)
{
	cl_I result = R->retract(x);
	if (result > (modulus >> 1))
		result = result - modulus;
	return result;
}

static void
compute_recips(vector<cl_MI>& dst, 
	       const vector<cl_I>& moduli)
{
	for (size_t k = 1; k < moduli.size(); ++k) {
		cl_modint_ring R = find_modint_ring(moduli[k]);
		cl_MI product = R->canonhom(moduli[0]);
		for (size_t i = 1; i < k; ++i)
			product = product*moduli[i];
		dst[k-1] = recip(product);
	}
}

static void
compute_mix_radix_coeffs(vector<cl_I>& dst,
	                 const vector<cl_I>& residues,
	                 const vector<cl_I>& moduli,
			 const vector<cl_MI>& recips)
{
	dst[0] = residues[0];

	do {
		cl_modint_ring R = find_modint_ring(moduli[1]);
		cl_MI tmp = R->canonhom(residues[0]);
		cl_MI next = (R->canonhom(residues[1]) - tmp)*recips[0];
		dst[1] = retract_symm(next, R, moduli[1]);
	} while (0);

	for (size_t k = 2; k < residues.size(); ++k) {
		cl_modint_ring R = find_modint_ring(moduli[k]);
		cl_MI tmp = R->canonhom(dst[k-1]);

		for (size_t j = k - 1 /* NOT k - 2 */; j-- != 0; )
			tmp = tmp*moduli[j] + R->canonhom(dst[j]);

		cl_MI next = (R->canonhom(residues[k]) - tmp)*recips[k-1];
		dst[k] = retract_symm(next, R, moduli[k]);
	}
}

static cl_I
mixed_radix_2_ordinary(const vector<cl_I>& mixed_radix_coeffs,
	               const vector<cl_I>& moduli)
{
	size_t k = mixed_radix_coeffs.size() - 1;
	cl_I u = mixed_radix_coeffs[k];
	for (; k-- != 0; )
		u = u*moduli[k] + mixed_radix_coeffs[k];
	return u;
}

cl_I integer_cra(const vector<cl_I>& residues,
	         const vector<cl_I>& moduli)
{

	vector<cl_MI> recips(moduli.size() - 1);
	compute_recips(recips, moduli);

	vector<cl_I> coeffs(moduli.size());
	compute_mix_radix_coeffs(coeffs, residues, moduli, recips);
	cl_I result = mixed_radix_2_ordinary(coeffs, moduli);

	return result;
}

} // namespace cln

