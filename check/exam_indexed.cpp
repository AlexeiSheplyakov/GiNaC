/** @file exam_indexed.cpp
 *
 *  Here we test manipulations on GiNaC's indexed objects. */

/*
 *  GiNaC Copyright (C) 1999-2001 Johannes Gutenberg University Mainz, Germany
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

#include "exams.h"

static unsigned check_equal(const ex &e1, const ex &e2)
{
	ex e = e1 - e2;
	if (!e.is_zero()) {
		clog << e1 << "-" << e2 << " erroneously returned "
		     << e << " instead of 0" << endl;
		return 1;
	}
	return 0;
}

static unsigned check_equal_simplify(const ex &e1, const ex &e2)
{
	ex e = simplify_indexed(e1) - e2;
	if (!e.is_zero()) {
		clog << "simplify_indexed(" << e1 << ")-" << e2 << " erroneously returned "
		     << e << " instead of 0" << endl;
		return 1;
	}
	return 0;
}

static unsigned delta_check(void)
{
	// checks identities of the delta tensor

	unsigned result = 0;

	symbol s_i("i"), s_j("j"), s_k("k");
	idx i(s_i, 3), j(s_j, 3), k(s_k, 3);
	symbol A("A");

	// symmetry
	result += check_equal(delta_tensor(i, j), delta_tensor(j, i));

	// trace = dimension of index space
	result += check_equal(delta_tensor(i, i), 3);
	result += check_equal_simplify(delta_tensor(i, j) * delta_tensor(i, j), 3);

	// contraction with delta tensor
	result += check_equal_simplify(delta_tensor(i, j) * indexed(A, k), delta_tensor(i, j) * indexed(A, k));
	result += check_equal_simplify(delta_tensor(i, j) * indexed(A, j), indexed(A, i));
	result += check_equal_simplify(delta_tensor(i, j) * indexed(A, i), indexed(A, j));
	result += check_equal_simplify(delta_tensor(i, j) * delta_tensor(j, k) * indexed(A, i), indexed(A, k));

	return result;
}

static unsigned metric_check(void)
{
	// checks identities of the metric tensor

	unsigned result = 0;

	symbol s_mu("mu"), s_nu("nu"), s_rho("rho"), s_sigma("sigma");
	varidx mu(s_mu, 4), nu(s_nu, 4), rho(s_rho, 4), sigma(s_sigma, 4);
	symbol A("A");

	// becomes delta tensor if indices have opposite variance
	result += check_equal(metric_tensor(mu, nu.toggle_variance()), delta_tensor(mu, nu.toggle_variance()));

	// scalar contraction = dimension of index space
	result += check_equal(metric_tensor(mu, mu.toggle_variance()), 4);
	result += check_equal_simplify(metric_tensor(mu, nu) * metric_tensor(mu.toggle_variance(), nu.toggle_variance()), 4);

	// contraction with metric tensor
	result += check_equal_simplify(metric_tensor(mu, nu) * indexed(A, nu), metric_tensor(mu, nu) * indexed(A, nu));
	result += check_equal_simplify(metric_tensor(mu, nu) * indexed(A, nu.toggle_variance()), indexed(A, mu));
	result += check_equal_simplify(metric_tensor(mu, nu) * indexed(A, mu.toggle_variance()), indexed(A, nu));
	result += check_equal_simplify(metric_tensor(mu, nu) * metric_tensor(mu.toggle_variance(), rho.toggle_variance()) * indexed(A, nu.toggle_variance()), indexed(A, rho.toggle_variance()));
	result += check_equal_simplify(metric_tensor(mu, rho) * metric_tensor(nu, sigma) * indexed(A, rho.toggle_variance(), sigma.toggle_variance()), indexed(A, mu, nu));
	result += check_equal_simplify(indexed(A, mu.toggle_variance()) * metric_tensor(mu, nu) - indexed(A, mu.toggle_variance()) * metric_tensor(nu, mu), 0);
	result += check_equal_simplify(indexed(A, mu.toggle_variance(), nu.toggle_variance()) * metric_tensor(nu, rho), indexed(A, mu.toggle_variance(), rho));

	// contraction with delta tensor yields a metric tensor
	result += check_equal_simplify(delta_tensor(mu, nu.toggle_variance()) * metric_tensor(nu, rho), metric_tensor(mu, rho));
	result += check_equal_simplify(metric_tensor(mu, nu) * indexed(A, nu.toggle_variance()) * delta_tensor(mu.toggle_variance(), rho), indexed(A, rho));

	return result;
}

static unsigned epsilon_check(void)
{
	// checks identities of the epsilon tensor

	unsigned result = 0;

	symbol s_mu("mu"), s_nu("nu"), s_rho("rho"), s_sigma("sigma");
	varidx mu(s_mu, 4), nu(s_nu, 4), rho(s_rho, 4), sigma(s_sigma, 4);

	// antisymmetry
	result += check_equal(lorentz_eps(mu, nu, rho, sigma) + lorentz_eps(sigma, rho, mu, nu), 0);

	// convolution is zero
	result += check_equal(lorentz_eps(mu, nu, rho, nu.toggle_variance()), 0);
	result += check_equal(lorentz_eps(mu, nu, mu.toggle_variance(), nu.toggle_variance()), 0);
	result += check_equal_simplify(lorentz_g(mu.toggle_variance(), nu.toggle_variance()) * lorentz_eps(mu, nu, rho, sigma), 0);

	return result;
}

static unsigned symmetry_check(void)
{
	// check symmetric/antisymmetric objects

	unsigned result = 0;

	symbol s_i("i"), s_j("j"), s_k("k");
	idx i(s_i, 3), j(s_j, 3), k(s_k, 3);
	symbol A("A");
	ex e, e1, e2;

	result += check_equal(indexed(A, indexed::symmetric, i, j), indexed(A, indexed::symmetric, j, i));
	result += check_equal(indexed(A, indexed::antisymmetric, i, j) + indexed(A, indexed::antisymmetric, j, i), 0);
	result += check_equal(indexed(A, indexed::antisymmetric, i, j, k) - indexed(A, indexed::antisymmetric, j, k, i), 0);

	return result;
}

static unsigned edyn_check(void)
{
	// relativistic electrodynamics: check transformation laws of electric
	// and magnetic fields by applying a Lorentz boost to the field tensor

	unsigned result = 0;

	symbol beta("beta");
	ex gamma = 1 / sqrt(1 - pow(beta, 2));
	symbol Ex("Ex"), Ey("Ey"), Ez("Ez");
	symbol Bx("Bx"), By("By"), Bz("Bz");

	// Lorentz transformation matrix (boost along x axis)
	matrix L(4, 4);
	L.set(0, 0, gamma);
	L.set(0, 1, -beta*gamma);
	L.set(1, 0, -beta*gamma);
	L.set(1, 1, gamma);
	L.set(2, 2, 1);
	L.set(3, 3, 1);

	// Electromagnetic field tensor
	matrix F(4, 4);
	F.set(0, 1, -Ex);
	F.set(1, 0, Ex);
	F.set(0, 2, -Ey);
	F.set(2, 0, Ey);
	F.set(0, 3, -Ez);
	F.set(3, 0, Ez);
	F.set(1, 2, -Bz);
	F.set(2, 1, Bz);
	F.set(1, 3, By);
	F.set(3, 1, -By);
	F.set(2, 3, -Bx);
	F.set(3, 2, Bx);

	// Indices
	symbol s_mu("mu"), s_nu("nu"), s_rho("rho"), s_sigma("sigma");
	varidx mu(s_mu, 4), nu(s_nu, 4), rho(s_rho, 4), sigma(s_sigma, 4);

	// Apply transformation law of second rank tensor
	ex e = (indexed(L, mu, rho.toggle_variance())
	   * indexed(L, nu, sigma.toggle_variance())
	   * indexed(F, rho, sigma)).simplify_indexed();

	// Extract transformed electric and magnetic fields
	ex Ex_p = e.subs(lst(mu == 1, nu == 0)).normal();
	ex Ey_p = e.subs(lst(mu == 2, nu == 0)).normal();
	ex Ez_p = e.subs(lst(mu == 3, nu == 0)).normal();
	ex Bx_p = e.subs(lst(mu == 3, nu == 2)).normal();
	ex By_p = e.subs(lst(mu == 1, nu == 3)).normal();
	ex Bz_p = e.subs(lst(mu == 2, nu == 1)).normal();

	// Check results
	result += check_equal(Ex_p, Ex);
	result += check_equal(Ey_p, gamma * (Ey - beta * Bz));
	result += check_equal(Ez_p, gamma * (Ez + beta * By));
	result += check_equal(Bx_p, Bx);
	result += check_equal(By_p, gamma * (By + beta * Ez));
	result += check_equal(Bz_p, gamma * (Bz - beta * Ey));

	return result;
}

unsigned exam_indexed(void)
{
	unsigned result = 0;
	
	cout << "examining indexed objects" << flush;
	clog << "----------indexed objects:" << endl;

	result += delta_check();  cout << '.' << flush;
	result += metric_check();  cout << '.' << flush;
	result += epsilon_check();  cout << '.' << flush;
	result += symmetry_check();  cout << '.' << flush;
	result += edyn_check();  cout << '.' << flush;
	
	if (!result) {
		cout << " passed " << endl;
		clog << "(no output)" << endl;
	} else {
		cout << " failed " << endl;
	}
	
	return result;
}
