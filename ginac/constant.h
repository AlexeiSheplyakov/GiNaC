/** @file constant.h
 *
 *  Interface to GiNaC's constant types and some special constants. */

/*
 *  GiNaC Copyright (C) 1999-2014 Johannes Gutenberg University Mainz, Germany
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef GINAC_CONSTANT_H
#define GINAC_CONSTANT_H

#include "basic.h"
#include "ex.h"
#include "archive.h"

#include <string>

namespace GiNaC {

typedef ex (*evalffunctype)();
	
/** This class holds constants, symbols with specific numerical value. Each
 *  object of this class must either provide their own function to evaluate it
 *  to class numeric or provide the constant as a numeric (if it's an exact
 *  number). */
class constant : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(constant, basic)
	// other constructors
public:
	constant(const std::string & initname, evalffunctype efun = 0, const std::string & texname = std::string(), unsigned domain = domain::complex);
	constant(const std::string & initname, const numeric & initnumber, const std::string & texname = std::string(), unsigned domain = domain::complex);
	
	// functions overriding virtual functions from base classes
public:
	bool info(unsigned inf) const;
	ex evalf(int level = 0) const;
	bool is_polynomial(const ex & var) const;
	ex conjugate() const;
	ex real_part() const;
	ex imag_part() const;
	void archive(archive_node& n) const;
	void read_archive(const archive_node& n, lst& syms);
protected:
	ex derivative(const symbol & s) const;
	bool is_equal_same_type(const basic & other) const;
	unsigned calchash() const;
	
	// non-virtual functions in this class
protected:
	void do_print(const print_context & c, unsigned level) const;
	void do_print_tree(const print_tree & c, unsigned level) const;
	void do_print_latex(const print_latex & c, unsigned level) const;
	void do_print_python_repr(const print_python_repr & c, unsigned level) const;

// member variables
private:
	std::string name;     ///< printname of this constant
	std::string TeX_name; ///< LaTeX name
	evalffunctype ef;
	ex number;            ///< numerical value this constant evalf()s to
	unsigned serial;      ///< unique serial number for comparison
	static unsigned next_serial;
	unsigned domain;      ///< numerical value this constant evalf()s to
};
GINAC_DECLARE_UNARCHIVER(constant); 

extern const constant Pi;
extern const constant Catalan;
extern const constant Euler;

} // namespace GiNaC

#endif // ndef GINAC_CONSTANT_H
