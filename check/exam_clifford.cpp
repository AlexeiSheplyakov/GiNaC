/** @file exam_clifford.cpp
 *
 *  Here we test GiNaC's Clifford algebra objects. */

/*
 *  GiNaC Copyright (C) 1999-2005 Johannes Gutenberg University Mainz, Germany
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

static unsigned clifford_check1()
{
	// checks general identities and contractions

	unsigned result = 0;

	symbol dim("D");
	varidx mu(symbol("mu"), dim), nu(symbol("nu"), dim), rho(symbol("rho"), dim);
	ex e;

	e = dirac_ONE() * dirac_ONE();
	result += check_equal(e, dirac_ONE());

	e = dirac_ONE() * dirac_gamma(mu) * dirac_ONE();
	result += check_equal(e, dirac_gamma(mu));

	e = dirac_gamma(varidx(2, dim)) * dirac_gamma(varidx(1, dim)) *
	    dirac_gamma(varidx(1, dim)) * dirac_gamma(varidx(2, dim));
	result += check_equal(e, dirac_ONE());

	e = dirac_gamma(mu) * dirac_gamma(nu) *
	    dirac_gamma(nu.toggle_variance()) * dirac_gamma(mu.toggle_variance());
	result += check_equal_simplify(e, pow(dim, 2) * dirac_ONE());

	e = dirac_gamma(mu) * dirac_gamma(nu) *
	    dirac_gamma(mu.toggle_variance()) * dirac_gamma(nu.toggle_variance());
	result += check_equal_simplify(e, 2*dim*dirac_ONE()-pow(dim, 2)*dirac_ONE());

	e = dirac_gamma(nu.toggle_variance()) * dirac_gamma(rho.toggle_variance()) *
	    dirac_gamma(mu) * dirac_gamma(rho) * dirac_gamma(nu);
	e = e.simplify_indexed().collect(dirac_gamma(mu));
	result += check_equal(e, pow(2 - dim, 2).expand() * dirac_gamma(mu));

	return result;
}

static unsigned clifford_check2()
{
	// checks identities relating to gamma5

	unsigned result = 0;

	symbol dim("D");
	varidx mu(symbol("mu"), dim), nu(symbol("nu"), dim);
	ex e;

	e = dirac_gamma(mu) * dirac_gamma5() + dirac_gamma5() * dirac_gamma(mu);
	result += check_equal(e, 0);

	e = dirac_gamma5() * dirac_gamma(mu) * dirac_gamma5() + dirac_gamma(mu);
	result += check_equal(e, 0);

	return result;
}

static unsigned clifford_check3()
{
	// checks traces

	unsigned result = 0;

	symbol dim("D"), m("m"), q("q"), l("l"), ldotq("ldotq");
	varidx mu(symbol("mu"), dim), nu(symbol("nu"), dim), rho(symbol("rho"), dim),
	       sig(symbol("sig"), dim), kap(symbol("kap"), dim), lam(symbol("lam"), dim);
	ex e;

	e = dirac_gamma(mu);
	result += check_equal(dirac_trace(e), 0);

	e = dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(rho);
	result += check_equal(dirac_trace(e), 0);

	e = dirac_gamma5() * dirac_gamma(mu);
	result += check_equal(dirac_trace(e), 0);

	e = dirac_gamma5() * dirac_gamma(mu) * dirac_gamma(nu);
	result += check_equal(dirac_trace(e), 0);

	e = dirac_gamma5() * dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(rho);
	result += check_equal(dirac_trace(e), 0);

	scalar_products sp;
	sp.add(q, q, pow(q, 2));
	sp.add(l, l, pow(l, 2));
	sp.add(l, q, ldotq);

	e = pow(m, 2) * dirac_slash(q, dim) * dirac_slash(q, dim);
	e = dirac_trace(e).simplify_indexed(sp);
	result += check_equal(e, 4*pow(m, 2)*pow(q, 2));

	// cyclicity without gamma5
	e = dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(rho) * dirac_gamma(sig)
	  - dirac_gamma(nu) * dirac_gamma(rho) * dirac_gamma(sig) * dirac_gamma(mu);
	e = dirac_trace(e);
	result += check_equal(e, 0);

	e = dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(rho) * dirac_gamma(sig) * dirac_gamma(kap) * dirac_gamma(lam)
	  - dirac_gamma(nu) * dirac_gamma(rho) * dirac_gamma(sig) * dirac_gamma(kap) * dirac_gamma(lam) * dirac_gamma(mu);
	e = dirac_trace(e).expand();
	result += check_equal(e, 0);

	// cyclicity of gamma5 * S_4
	e = dirac_gamma5() * dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(rho) * dirac_gamma(sig)
	  - dirac_gamma(sig) * dirac_gamma5() * dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(rho);
	e = dirac_trace(e);
	result += check_equal(e, 0);

	// non-cyclicity of order D-4 of gamma5 * S_6
	e = dirac_gamma5() * dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(rho) * dirac_gamma(sig) * dirac_gamma(kap) * dirac_gamma(mu.toggle_variance())
	  + dim * dirac_gamma5() * dirac_gamma(nu) * dirac_gamma(rho) * dirac_gamma(sig) * dirac_gamma(kap);
	e = dirac_trace(e).simplify_indexed();
	e = (e / (dim - 4)).normal();
	result += check_equal(e, 8 * I * lorentz_eps(nu.replace_dim(4), rho.replace_dim(4), sig.replace_dim(4), kap.replace_dim(4)));

	// one-loop vacuum polarization in QED
	e = dirac_gamma(mu) *
	    (dirac_slash(l, dim) + dirac_slash(q, 4) + m * dirac_ONE()) *
	    dirac_gamma(mu.toggle_variance()) *
	    (dirac_slash(l, dim) + m * dirac_ONE());
	e = dirac_trace(e).simplify_indexed(sp);
	result += check_equal(e, 4*((2-dim)*l*l + (2-dim)*ldotq + dim*m*m).expand());

	e = dirac_slash(q, 4) *
	    (dirac_slash(l, dim) + dirac_slash(q, 4) + m * dirac_ONE()) *
	    dirac_slash(q, 4) *
	    (dirac_slash(l, dim) + m * dirac_ONE());
	e = dirac_trace(e).simplify_indexed(sp);
	result += check_equal(e, 4*(2*ldotq*ldotq + q*q*ldotq - q*q*l*l + q*q*m*m).expand());

	// stuff that had problems in the past
	ex prop = dirac_slash(q, dim) - m * dirac_ONE();
	e = dirac_slash(l, dim) * dirac_gamma5() * dirac_slash(l, dim) * prop;
	e = dirac_trace(dirac_slash(q, dim) * e) - dirac_trace(m * e)
	  - dirac_trace(prop * e);
	result += check_equal(e, 0);

	e = (dirac_gamma5() + dirac_ONE()) * dirac_gamma5();
	e = dirac_trace(e);
	result += check_equal(e, 4);

	// traces with multiple representation labels
	e = dirac_ONE(0) * dirac_ONE(1) / 16;
	result += check_equal(dirac_trace(e, 0), dirac_ONE(1) / 4);
	result += check_equal(dirac_trace(e, 1), dirac_ONE(0) / 4);
	result += check_equal(dirac_trace(e, 2), e);
	result += check_equal(dirac_trace(e, lst(0, 1)), 1);

	e = dirac_gamma(mu, 0) * dirac_gamma(mu.toggle_variance(), 1) * dirac_gamma(nu, 0) * dirac_gamma(nu.toggle_variance(), 1);
	result += check_equal_simplify(dirac_trace(e, 0), 4 * dim * dirac_ONE(1));
	result += check_equal_simplify(dirac_trace(e, 1), 4 * dim * dirac_ONE(0));
	result += check_equal_simplify(dirac_trace(e, 2), e);
	result += check_equal_simplify(dirac_trace(e, lst(0, 1)), 16 * dim);

	return result;
}

static unsigned clifford_check4()
{
	// simplify_indexed()/dirac_trace() cross-checks

	unsigned result = 0;

	symbol dim("D");
	varidx mu(symbol("mu"), dim), nu(symbol("nu"), dim), rho(symbol("rho"), dim),
	       sig(symbol("sig"), dim), lam(symbol("lam"), dim);
	ex e, t1, t2;

	e = dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(rho) * dirac_gamma(mu.toggle_variance());
	t1 = dirac_trace(e).simplify_indexed();
	t2 = dirac_trace(e.simplify_indexed());
	result += check_equal((t1 - t2).expand(), 0);

	e = dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(rho) * dirac_gamma(sig) * dirac_gamma(mu.toggle_variance()) * dirac_gamma(lam);
	t1 = dirac_trace(e).simplify_indexed();
	t2 = dirac_trace(e.simplify_indexed());
	result += check_equal((t1 - t2).expand(), 0);

	e = dirac_gamma(sig) * dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(rho) * dirac_gamma(nu.toggle_variance()) * dirac_gamma(mu.toggle_variance());
	t1 = dirac_trace(e).simplify_indexed();
	t2 = dirac_trace(e.simplify_indexed());
	result += check_equal((t1 - t2).expand(), 0);

	e = dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(rho) * dirac_gamma(mu.toggle_variance()) * dirac_gamma(sig) * dirac_gamma(nu.toggle_variance());
	t1 = dirac_trace(e).simplify_indexed();
	t2 = dirac_trace(e.simplify_indexed());
	result += check_equal((t1 - t2).expand(), 0);

	return result;
}

static unsigned clifford_check5()
{
	// canonicalize_clifford() checks

	unsigned result = 0;

	symbol dim("D");
	varidx mu(symbol("mu"), dim), nu(symbol("nu"), dim), lam(symbol("lam"), dim);
	ex e;

	e = dirac_gamma(mu) * dirac_gamma(nu) + dirac_gamma(nu) * dirac_gamma(mu);
	result += check_equal(canonicalize_clifford(e), 2*dirac_ONE()*lorentz_g(mu, nu));

	e = (dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(lam)
	   + dirac_gamma(nu) * dirac_gamma(lam) * dirac_gamma(mu)
	   + dirac_gamma(lam) * dirac_gamma(mu) * dirac_gamma(nu)
	   - dirac_gamma(nu) * dirac_gamma(mu) * dirac_gamma(lam)
	   - dirac_gamma(lam) * dirac_gamma(nu) * dirac_gamma(mu)
	   - dirac_gamma(mu) * dirac_gamma(lam) * dirac_gamma(nu)) / 6
	  + lorentz_g(mu, nu) * dirac_gamma(lam)
	  - lorentz_g(mu, lam) * dirac_gamma(nu)
	  + lorentz_g(nu, lam) * dirac_gamma(mu)
	  - dirac_gamma(mu) * dirac_gamma(nu) * dirac_gamma(lam);
	result += check_equal(canonicalize_clifford(e), 0);

	return result;
}

static unsigned clifford_check6(const matrix & A)
{
	varidx v(symbol("v"), 4), nu(symbol("nu"), 4), mu(symbol("mu"), 4),
	       psi(symbol("psi"),4), lam(symbol("lambda"), 4),
	       xi(symbol("xi"), 4),  rho(symbol("rho"),4);

	ex G = A;

	matrix A2(4, 4);
	A2 = A.mul(A);
	ex e, e1;

	int result = 0;

	// checks general identities and contractions for clifford_unit
	e = dirac_ONE() * clifford_unit(mu, G) * dirac_ONE();
	result += check_equal(e, clifford_unit(mu, G));

	e = clifford_unit(varidx(2, 4), G) * clifford_unit(varidx(1, 4), G)
	  * clifford_unit(varidx(1, 4), G) * clifford_unit(varidx(2, 4), G);
	result += check_equal(e, A(1, 1) * A(2, 2) * dirac_ONE());

	e = clifford_unit(nu, G) * clifford_unit(nu.toggle_variance(), G);
	result += check_equal_simplify(e, A.trace() * dirac_ONE());

	e = clifford_unit(nu, G) * clifford_unit(nu, G);
	result += check_equal_simplify(e, indexed(G, sy_symm(), nu, nu) * dirac_ONE());

	e = clifford_unit(nu, G) * clifford_unit(nu.toggle_variance(), G) * clifford_unit(mu, G);
	result += check_equal_simplify(e, A.trace() * clifford_unit(mu, G));

	e = clifford_unit(nu, G) * clifford_unit(mu, G) * clifford_unit(nu.toggle_variance(), G);
	result += check_equal_simplify(e, 2*indexed(G, sy_symm(), mu, mu)*clifford_unit(mu, G) - A.trace()*clifford_unit(mu, G));

	e = clifford_unit(nu, G) * clifford_unit(nu.toggle_variance(), G)
	  * clifford_unit(mu, G) * clifford_unit(mu.toggle_variance(), G);
	result += check_equal_simplify(e, pow(A.trace(), 2) * dirac_ONE());

	e = clifford_unit(mu, G) * clifford_unit(nu, G)
	  * clifford_unit(nu.toggle_variance(), G) * clifford_unit(mu.toggle_variance(), G);
	result += check_equal_simplify(e, pow(A.trace(), 2)  * dirac_ONE());

	e = clifford_unit(mu, G) * clifford_unit(nu, G)
	  * clifford_unit(mu.toggle_variance(), G) * clifford_unit(nu.toggle_variance(), G);
	result += check_equal_simplify(e, 2*A2.trace()*dirac_ONE() - pow(A.trace(), 2)*dirac_ONE());

	e = clifford_unit(mu.toggle_variance(), G) * clifford_unit(nu, G)
	  * clifford_unit(mu, G) * clifford_unit(nu.toggle_variance(), G);
	result += check_equal_simplify(e, 2*A2.trace()*dirac_ONE() - pow(A.trace(), 2)*dirac_ONE());

	e = clifford_unit(nu.toggle_variance(), G) * clifford_unit(rho.toggle_variance(), G)
	  * clifford_unit(mu, G) * clifford_unit(rho, G) * clifford_unit(nu, G);
	e = e.simplify_indexed().collect(clifford_unit(mu, G));
	result += check_equal(e, (pow(A.trace(), 2)+4-4*A.trace()*indexed(A, mu, mu)) * clifford_unit(mu, G));

	e = clifford_unit(nu.toggle_variance(), G) * clifford_unit(rho, G)
	  * clifford_unit(mu, G) * clifford_unit(rho.toggle_variance(), G) * clifford_unit(nu, G);
	e = e.simplify_indexed().collect(clifford_unit(mu, G));
	result += check_equal(e, (pow(A.trace(), 2)+4-4*A.trace()*indexed(A, mu, mu))* clifford_unit(mu, G));

	// canonicalize_clifford() checks
	e = clifford_unit(mu, G) * clifford_unit(nu, G) + clifford_unit(nu, G) * clifford_unit(mu, G);
	result += check_equal(canonicalize_clifford(e), 2*dirac_ONE()*indexed(G, sy_symm(), mu, nu));

	e = (clifford_unit(mu, G) * clifford_unit(nu, G) * clifford_unit(lam, G)
	   + clifford_unit(nu, G) * clifford_unit(lam, G) * clifford_unit(mu, G)
	   + clifford_unit(lam, G) * clifford_unit(mu, G) * clifford_unit(nu, G)
	   - clifford_unit(nu, G) * clifford_unit(mu, G) * clifford_unit(lam, G)
	   - clifford_unit(lam, G) * clifford_unit(nu, G) * clifford_unit(mu, G)
	   - clifford_unit(mu, G) * clifford_unit(lam, G) * clifford_unit(nu, G)) / 6
	  + indexed(G, sy_symm(), mu, nu) * clifford_unit(lam, G)
	  - indexed(G, sy_symm(), mu, lam) * clifford_unit(nu, G)
	  + indexed(G, sy_symm(), nu, lam) * clifford_unit(mu, G)
	  - clifford_unit(mu, G) * clifford_unit(nu, G) * clifford_unit(lam, G);
	result += check_equal(canonicalize_clifford(e), 0);

	// lst_to_clifford() and clifford_inverse()  check
	symbol x("x"), y("y"), t("t"), z("z");
	
	e = lst_to_clifford(lst(t, x, y, z), mu, G) * lst_to_clifford(lst(1, 2, 3, 4), nu, G);
	e1 = clifford_inverse(e);
	result += check_equal((e*e1).simplify_indexed().normal(), dirac_ONE());

	return result;
}

static unsigned clifford_check7()
{
	// checks general identities and contractions

	unsigned result = 0;

	symbol dim("D");
	varidx mu(symbol("mu"), dim), nu(symbol("nu"), dim), rho(symbol("rho"), dim),
	       psi(symbol("psi"),dim), lam(symbol("lambda"), dim), xi(symbol("xi"), dim);

	ex e;

	ex G = minkmetric();

	e = dirac_ONE() * dirac_ONE();
	result += check_equal(e, dirac_ONE());

	e = dirac_ONE() * clifford_unit(mu, G) * dirac_ONE();
	result += check_equal(e, clifford_unit(mu, G));

	e = clifford_unit(varidx(2, dim), G) * clifford_unit(varidx(1, dim), G)
	  * clifford_unit(varidx(1, dim), G) * clifford_unit(varidx(2, dim), G);
	result += check_equal(e, dirac_ONE());

	e = clifford_unit(mu, G) * clifford_unit(nu, G)
	  * clifford_unit(nu.toggle_variance(), G) * clifford_unit(mu.toggle_variance(), G);
	result += check_equal_simplify(e, pow(dim, 2) * dirac_ONE());

	e = clifford_unit(mu, G) * clifford_unit(nu, G)
	  * clifford_unit(mu.toggle_variance(), G) * clifford_unit(nu.toggle_variance(), G);
	result += check_equal_simplify(e, 2*dim*dirac_ONE() - pow(dim, 2)*dirac_ONE());

	e = clifford_unit(nu.toggle_variance(), G) * clifford_unit(rho.toggle_variance(), G)
	  * clifford_unit(mu, G) * clifford_unit(rho, G) * clifford_unit(nu, G);
	e = e.simplify_indexed().collect(clifford_unit(mu, G));
	result += check_equal(e, pow(2 - dim, 2).expand() * clifford_unit(mu, G));

	// canonicalize_clifford() checks
	e = clifford_unit(mu, G) * clifford_unit(nu, G) + clifford_unit(nu, G) * clifford_unit(mu, G);
	result += check_equal(canonicalize_clifford(e), 2*dirac_ONE()*indexed(G, sy_symm(), mu, nu));

	e = (clifford_unit(mu, G) * clifford_unit(nu, G) * clifford_unit(lam, G)
	   + clifford_unit(nu, G) * clifford_unit(lam, G) * clifford_unit(mu, G)
	   + clifford_unit(lam, G) * clifford_unit(mu, G) * clifford_unit(nu, G)
	   - clifford_unit(nu, G) * clifford_unit(mu, G) * clifford_unit(lam, G)
	   - clifford_unit(lam, G) * clifford_unit(nu, G) * clifford_unit(mu, G)
	   - clifford_unit(mu, G) * clifford_unit(lam, G) * clifford_unit(nu, G)) / 6
	  + indexed(G, sy_symm(), mu, nu) * clifford_unit(lam, G)
	  - indexed(G, sy_symm(), mu, lam) * clifford_unit(nu, G)
	  + indexed(G, sy_symm(), nu, lam) * clifford_unit(mu, G)
	  - clifford_unit(mu, G) * clifford_unit(nu, G) * clifford_unit(lam, G);
	result += check_equal(canonicalize_clifford(e), 0);

	return result;
}

unsigned exam_clifford()
{
	unsigned result = 0;
	
	cout << "examining clifford objects" << flush;
	clog << "----------clifford objects:" << endl;

	result += clifford_check1(); cout << '.' << flush;
	result += clifford_check2(); cout << '.' << flush;
	result += clifford_check3(); cout << '.' << flush;
	result += clifford_check4(); cout << '.' << flush;
	result += clifford_check5(); cout << '.' << flush;

	matrix A(4, 4);
	A = -1, 0, 0, 0,
	     0, 1, 0, 0,
	     0, 0, 1, 0,
	     0, 0, 0, 1;
	result += clifford_check6(A); cout << '.' << flush;

	A = -1, 0, 0, 0,
	     0,-1, 0, 0,
	     0, 0,-1, 0,
	     0, 0, 0,-1;
	result += clifford_check6(A); cout << '.' << flush;
	
	A = -1, 0, 0, 0,
	     0, 1, 0, 0,
	     0, 0, 1, 0,
	     0, 0, 0,-1;
	result += clifford_check6(A); cout << '.' << flush;

	A = -1, 0, 0, 0,
	     0, 0, 0, 0,
	     0, 0, 1, 0,
	     0, 0, 0,-1;
	result += clifford_check6(A); cout << '.' << flush;

	result += clifford_check7(); cout << '.' << flush;

	if (!result) {
		cout << " passed " << endl;
		clog << "(no output)" << endl;
	} else {
		cout << " failed " << endl;
	}

	return result;
}
