/** @file time_antipode.cpp
 *
 *  This is a beautiful example that calculates the counterterm for the
 *  overall divergence of some special sorts of Feynman diagrams in a massless
 *  Yukawa theory.  For this end it computes the antipode of the corresponding
 *  decorated rooted tree using dimensional regularization in the parameter
 *  x==-(D-4)/2, which leads to a Laurent series in x.  The renormalization
 *  scheme used is the minimal subtraction scheme (MS).  From an efficiency
 *  point of view it boils down to power series expansion.  It also has quite
 *  an intriguing check for consistency, which is why we include it here.
 *
 *  This program is based on work by Isabella Bierenbaum and Dirk Kreimer.
 *  For details, please see the diploma theses of Isabella Bierenbaum.
 */

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

#include "times.h"
#include <utility>
#include <list>

// whether to run this beast or not:
static const bool do_test = true;

typedef pair<unsigned, unsigned> ijpair;
typedef pair<class node, bool> child;

const constant TrOne("Tr[One]", numeric(4));

/* Extract only the divergent part of a series and discard the rest. */
static ex div_part(const ex &exarg, const symbol &x, unsigned grad)
{
	unsigned order = grad;
	ex exser;
	// maybe we have to generate more terms on the series (obnoxious):
	do {
		exser = exarg.series(x==0, order);
		++order;
	} while (exser.degree(x) < 0);
	ex exser_trunc;
	for (int i=exser.ldegree(x); i<0; ++i)
		exser_trunc += exser.coeff(x,i)*pow(x,i);
	// NB: exser_trunc is by construction collected in x.
	return exser_trunc;
}

/* F_ab(a, i, b, j, "x") is a common pattern in all vertex evaluators. */
static ex F_ab(int a, int i, int b, int j, const symbol &x)
{
	if ((i==0 && a<=0) || (j==0 && b<=0))
		return 0;
	else
		return (tgamma(2-a-(i+1)*x)*
		        tgamma(2-b-(1+j)*x)*
		        tgamma(a+b-2+(1+i+j)*x)/
		        tgamma(a+i*x)/
		        tgamma(b+j*x)/tgamma(4-a-b-(2+i+j)*x));
}

/* Abstract base class (ABC) for all types of vertices. */
class vertex {
public:
	vertex(ijpair ij = ijpair(0,0)) : indices(ij) { }
	void increment_indices(const ijpair &ind) { indices.first += ind.first; indices.second += ind.second; }
	virtual ~vertex() { }
	virtual vertex* copy(void) const = 0;
	virtual ijpair get_increment(void) const { return indices; }
	virtual ex evaluate(const symbol &x) const = 0; 
protected:
	ijpair indices;
};


/*
 * Class of vertices of type Sigma.
 */
class Sigma : public vertex {
public:
	Sigma(ijpair ij = ijpair(0,0), bool f = true) : vertex(ij), flag(f) { }
	vertex* copy(void) const { return new Sigma(*this); }
	ijpair get_increment(void) const;
	ex evaluate(const symbol &x) const;
private:
	bool flag;
};

ijpair Sigma::get_increment(void) const
{
	if (flag == true)
	    return  ijpair(indices.first+1, indices.second);
	else
	    return  ijpair(indices.first, indices.second+1);
}

ex Sigma::evaluate(const symbol &x) const
{
	int i = indices.first;
	int j = indices.second;
	return (F_ab(0,i,1,j,x)+F_ab(1,i,1,j,x)-F_ab(1,i,0,j,x))/2;
}


/*
 *Class of vertices of type Gamma.
 */
class Gamma : public vertex {
public:
	Gamma(ijpair ij = ijpair(0,0)) : vertex(ij) { }
	vertex* copy(void) const { return new Gamma(*this); }
  	ijpair get_increment(void) const { return ijpair(indices.first+indices.second+1, 0); }
	ex evaluate(const symbol &x) const;
private:
};

ex Gamma::evaluate(const symbol &x) const
{
	int i = indices.first;
	int j = indices.second;
	return F_ab(1,i,1,j,x);
}


/*
 * Class of vertices of type Vacuum.
 */
class Vacuum : public vertex {
public:
	Vacuum(ijpair ij = ijpair(0,0)) : vertex(ij) { }
	vertex* copy(void) const { return new Vacuum(*this); }
	ijpair get_increment() const { return ijpair(0, indices.first+indices.second+1); }
	ex evaluate(const symbol &x) const;
private:
};

ex Vacuum::evaluate(const symbol &x) const
{
	int i = indices.first;
	int j = indices.second;
	return (-TrOne*(F_ab(0,i,1,j,x)-F_ab(1,i,1,j,x)+F_ab(1,i,0,j,x)))/2;
}


/*
 * Class of nodes (or trees or subtrees), including list of children.
 */
class node {
public:
	node(const vertex &v) { vert = v.copy(); }
	node(const node &n) { vert = (n.vert)->copy(); children = n.children; }
	const node & operator=(const node &);
	~node() { delete vert; }
	void add_child(const node &, bool = false);
	ex evaluate(const symbol &x, unsigned grad) const;
	unsigned total_edges(void) const;
private:
	vertex *vert;
	list<child> children;
};

const node & node::operator=(const node &n)
{
	delete vert;
	vert = (n.vert)->copy();
	children = n.children;
}

void node::add_child(const node &childnode, bool cut)
{
	children.push_back(child(childnode, cut));
	if(!cut)
		vert->increment_indices(childnode.vert->get_increment());
}

ex node::evaluate(const symbol &x, unsigned grad) const
{
	ex product = 1;
	for (list<child>::const_iterator i=children.begin(); i!=children.end(); ++i) {
		if (!i->second)
			product *= i->first.evaluate(x,grad);
		else
			product *= -div_part(i->first.evaluate(x,grad),x,grad);
	}
	return (product * vert->evaluate(x));
}

unsigned node::total_edges(void) const
{
	unsigned accu = 0;
	for (list<child>::const_iterator i=children.begin(); i!=children.end(); ++i) {
		accu += i->first.total_edges();
		++accu;
	}
	return accu;
}


/*
 * These operators let us write down trees in an intuitive way, by adding
 * arbitrarily complex children to a given vertex.  The eye candy that can be
 * produced with it makes detection of errors much simpler than with code
 * written using calls to node's method add_child() because it allows for
 * editor-assisted indentation.
 */
const node operator+(const node &n, const child &c)
{
	node nn(n);
	nn.add_child(c.first, c.second);
	return nn;
}
void operator+=(node &n, const child &c)
{
	n.add_child(c.first, c.second);
}

/*
 * Build this sample rooted tree characterized by a certain combination of
 * cut or uncut edges as specified by the unsigned parameter:
 *              Gamma
 *              /   \
 *         Sigma     Vacuum
 *        /   \       /   \
 *    Sigma Sigma  Sigma0 Sigma
 */
static node mytree(unsigned cuts=0)
{
	return (Gamma()
	        + child(Sigma()
	                + child(Sigma(), bool(cuts & 1))
	                + child(Sigma(), bool(cuts & 2)),
	                bool(cuts & 4))
	        + child(Vacuum()
	                + child(Sigma(ijpair(0,0),false), bool(cuts & 8))
	                + child(Sigma(), bool(cuts & 16)),
	                bool(cuts & 32)));
}


static unsigned test(void)
{
	const symbol x("x");
	
	const unsigned edges = mytree().total_edges();
   	const unsigned vertices = edges+1;
	
	// fill a vector of all possible 2^edges combinations of cuts...
	vector<node> counter;
	for (unsigned i=0; i<(1U<<edges); ++i)
		counter.push_back(mytree(i));
	
	// ...the sum, when evaluated, is the antipode...
	ex accu = 0;
	for (vector<node>::iterator i=counter.begin(); i!=counter.end(); ++i)
		accu += i->evaluate(x,vertices);
	
	// ...which is only interesting term-wise in the series expansion...
	ex result = accu.series(x==0,vertices).expand().normal();
	
	// ...and has the nice property that in each term all the Eulers cancel:
	if (result.has(Euler)) {
		clog << "The antipode was miscalculated\nAntipode==" << result
		     << "\nshould not have any occurrence of Euler" << endl;
		return 1;
	}
	return 0;
}

unsigned time_antipode(void)
{
	unsigned result = 0;
	unsigned count = 0;
	timer jaeger_le_coultre;
	double time = .0;
	
	cout << "timing computation of an antipode in Yukawa theory" << flush;
	clog << "-------computation of an antipode in Yukawa theory" << endl;
	
	if (do_test) {
		jaeger_le_coultre.start();
		// correct for very small times:
		do {
			result = test();
			++count;
		} while ((time=jaeger_le_coultre.read())<0.1 && !result);
		cout << '.' << flush;
		
		if (!result) {
			cout << " passed ";
			clog << "(no output)" << endl;
		} else {
			cout << " failed ";
		}
		cout << int(1000*(time/count))*0.001 << 's' << endl;
	} else {
		cout << " disabled" << endl;
		clog << "(no output)" << endl;
	}
	
	return result;
}
