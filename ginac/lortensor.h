/** @file lortensor.h
 *
 *  Interface to GiNaC's Lorentz tensors. */

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

#ifndef __GINAC_LORTENSOR_H__
#define __GINAC_LORTENSOR_H__

#include <string>
#include <vector>
#include <iostream>
#include "indexed.h"
#include "lorentzidx.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC


/** This class holds an object carrying Lorentz indices (of class
 *  lorentzidx). It can represent a general (symbolic) tensor of type
 *  (p,q), or one of the constant tensors g (the metric), delta (unity
 *  matrix) or epsilon (4-dimensional totally antisymmetric tensor). */
class lortensor : public indexed
{
	GINAC_DECLARE_REGISTERED_CLASS(lortensor, indexed)

// friends

	friend lortensor lortensor_g(const ex & mu, const ex & nu);
	friend lortensor lortensor_delta(const ex & mu, const ex & nu);
	friend lortensor lortensor_epsilon(const ex & mu, const ex & nu,
									   const ex & rho, const ex & sigma);
	friend lortensor lortensor_vector(const string & n, const ex & mu);
	friend lortensor lortensor_symbolic(const string & name, const exvector & iv);

	friend ex simplify_lortensor_mul(const ex & m);
	friend ex simplify_lortensor(const ex & e);
	
// types

public:
	typedef enum {
		invalid,           /**< not properly constructed */
		lortensor_g,       /**< metric tensor */
		lortensor_delta,   /**< unity matrix */
		lortensor_epsilon, /**< four-dimensional totally antisymmetric tensor */
		lortensor_symbolic /**< general symbolic Lorentz tensor */
	} lortensor_types;

// member functions

	// default constructor, destructor, copy constructor assignment operator and helpers
public:
	lortensor();
	~lortensor();
	lortensor(const lortensor & other);
	const lortensor & operator=(const lortensor & other);
protected:
	void copy(const lortensor & other);
	void destroy(bool call_parent);

	// other constructors
protected:
	lortensor(lortensor_types const lt, const std::string & n);
	lortensor(lortensor_types const lt, const std::string & n, const ex & mu);
	lortensor(lortensor_types const lt, const std::string & n, const ex & mu, const ex & nu);
	lortensor(lortensor_types const lt, const std::string & n, const ex & mu, const ex & nu, const ex & rho);
	lortensor(lortensor_types const lt, const std::string & n, const ex & mu, const ex & nu, const ex & rho, const ex & sigma);
	lortensor(lortensor_types const lt, const std::string & n, const exvector & iv);
	lortensor(lortensor_types const lt, const std::string & n, unsigned s, const exvector & iv);
	lortensor(lortensor_types const lt, const std::string & n, unsigned s, exvector * ivp);

	//functions overriding virtual functions from base classes
public:
	basic * duplicate() const;
	void printraw(std::ostream & os) const;
	void printtree(std::ostream & os, unsigned indent) const;
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	bool info(unsigned inf) const;
	ex eval(int level=0) const;
protected:
	int compare_same_type(const basic & other) const;
	bool is_equal_same_type(const basic & other) const;
	unsigned return_type(void) const;
	unsigned return_type_tinfo(void) const;
	ex thisexprseq(const exvector & v) const;
	ex thisexprseq(exvector * vp) const;

	// new virtual functions which can be overridden by derived classes
	// none

	//non virtual functions in this class
public:
	void setname(const std::string & n) {name = n;}
	std::string getname(void) const {return name;}
protected:
	bool all_of_type_lorentzidx(void) const;
private:
	std::string & autoname_prefix(void);

	//member variables

protected:
	lortensor_types type; /**< Type of object */
	std::string name;     /**< Name of symbolic tensor */
	unsigned serial;      /**< Unique serial number for comparing symbolic tensors */
private:
	static unsigned next_serial;
};

// utility functions
	
inline const lortensor & ex_to_lortensor(const ex &e)
{
	return static_cast<const lortensor &>(*e.bp);
}

inline lortensor &ex_to_nonconst_lortensor(const ex &e)
{
	return static_cast<lortensor &>(*e.bp);
}

lortensor lortensor_g(const ex & mu, const ex & nu);
lortensor lortensor_delta(const ex & mu, const ex & nu);
lortensor lortensor_epsilon(const ex & mu, const ex & nu,
                            const ex & rho, const ex & sigma);
lortensor lortensor_vector(const string & n, const ex & mu);
lortensor lortensor_symbolic(const string & name, const exvector & iv);

ex simplify_lortensor_mul(const ex & m);
ex simplify_lortensor(const ex & e);

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_LORTENSOR_H__
