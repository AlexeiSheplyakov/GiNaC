/** @file printcsrc.cpp
 *
 *  The methods .printcsrc() are responsible for C-source output of
 *  objects.  All related helper-functions go in here as well. */

/*
 *  GiNaC Copyright (C) 1999 Johannes Gutenberg University Mainz, Germany
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

#include <iostream>

#include "basic.h"
#include "ex.h"
#include "add.h"
#include "constant.h"
#include "expairseq.h"
#include "indexed.h"
#include "inifcns.h"
#include "mul.h"
#include "ncmul.h"
#include "numeric.h"
#include "power.h"
#include "relational.h"
#include "series.h"
#include "symbol.h"
#include "debugmsg.h"

namespace GiNaC {

/** Print expression as a C++ statement. The output looks like
 *  "<type> <var_name> = <expression>;". The "type" parameter has an effect
 *  on how number literals are printed.
 *
 *  @param os output stream
 *  @param type variable type (one of the csrc_types)
 *  @param var_name variable name to be printed */
void ex::printcsrc(ostream & os, unsigned type, const char *var_name) const
{
    debugmsg("ex print csrc", LOGLEVEL_PRINT);
    GINAC_ASSERT(bp!=0);
	switch (type) {
		case csrc_types::ctype_float:
			os << "float ";
			break;
		case csrc_types::ctype_double:
			os << "double ";
			break;
		case csrc_types::ctype_cl_N:
			os << "cl_N ";
			break;
	}
    os << var_name << " = ";
    bp->printcsrc(os, type, 0);
    os << ";\n";
}

void basic::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("basic print csrc", LOGLEVEL_PRINT);
}

void numeric::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("numeric print csrc", LOGLEVEL_PRINT);
	ios::fmtflags oldflags = os.flags();
	os.setf(ios::scientific);
    if (is_rational() && !is_integer()) {
        if (compare(numZERO()) > 0) {
            os << "(";
			if (type == csrc_types::ctype_cl_N)
				os << "cl_F(\"" << numer().evalf() << "\")";
			else
	            os << numer().to_double();
        } else {
            os << "-(";
			if (type == csrc_types::ctype_cl_N)
				os << "cl_F(\"" << -numer().evalf() << "\")";
			else
	            os << -numer().to_double();
        }
        os << "/";
		if (type == csrc_types::ctype_cl_N)
			os << "cl_F(\"" << denom().evalf() << "\")";
		else
	        os << denom().to_double();
        os << ")";
    } else {
		if (type == csrc_types::ctype_cl_N)
			os << "cl_F(\"" << evalf() << "\")";
		else
	        os << to_double();
	}
	os.flags(oldflags);
}

void symbol::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("symbol print csrc", LOGLEVEL_PRINT);
    os << name;
}

void constant::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("constant print csrc",LOGLEVEL_PRINT);
    os << name;
}

static void print_sym_pow(ostream & os, unsigned type, const symbol &x, int exp)
{
    // Optimal output of integer powers of symbols to aid compiler CSE
    if (exp == 1) {
        x.printcsrc(os, type, 0);
    } else if (exp == 2) {
        x.printcsrc(os, type, 0);
        os << "*";
        x.printcsrc(os, type, 0);
    } else if (exp & 1) {
        x.printcsrc(os, 0);
        os << "*";
        print_sym_pow(os, type, x, exp-1);
    } else {
        os << "(";
        print_sym_pow(os, type, x, exp >> 1);
        os << ")*(";
        print_sym_pow(os, type, x, exp >> 1);
        os << ")";
    }
}

void power::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("power print csrc", LOGLEVEL_PRINT);

	// Integer powers of symbols are printed in a special, optimized way
    if (exponent.info(info_flags::integer) &&
        (is_ex_exactly_of_type(basis, symbol) ||
         is_ex_exactly_of_type(basis, constant))) {
        int exp = ex_to_numeric(exponent).to_int();
        if (exp > 0)
            os << "(";
        else {
            exp = -exp;
			if (type == csrc_types::ctype_cl_N)
				os << "recip(";
			else
	            os << "1.0/(";
        }
        print_sym_pow(os, type, static_cast<const symbol &>(*basis.bp), exp);
        os << ")";

	// <expr>^-1 is printed as "1.0/<expr>" or with the recip() function of CLN
    } else if (exponent.compare(numMINUSONE()) == 0) {
		if (type == csrc_types::ctype_cl_N)
			os << "recip(";
		else
	        os << "1.0/(";
        basis.bp->printcsrc(os, type, 0);
		os << ")";

	// Otherwise, use the pow() or expt() (CLN) functions
    } else {
		if (type == csrc_types::ctype_cl_N)
			os << "expt(";
		else
	        os << "pow(";
        basis.bp->printcsrc(os, type, 0);
        os << ",";
        exponent.bp->printcsrc(os, type, 0);
        os << ")";
    }
}

void add::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("add print csrc", LOGLEVEL_PRINT);
    if (precedence <= upper_precedence)
        os << "(";

	// Print arguments, separated by "+"
    epvector::const_iterator it = seq.begin();
    epvector::const_iterator itend = seq.end();
    while (it != itend) {

		// If the coefficient is -1, it is replaced by a single minus sign
        if (it->coeff.compare(numONE()) == 0) {
	        it->rest.bp->printcsrc(os, type, precedence);
        } else if (it->coeff.compare(numMINUSONE()) == 0) {
            os << "-";
	        it->rest.bp->printcsrc(os, type, precedence);
		} else if (ex_to_numeric(it->coeff).numer().compare(numONE()) == 0) {
	        it->rest.bp->printcsrc(os, type, precedence);
			os << "/";
            ex_to_numeric(it->coeff).denom().printcsrc(os, type, precedence);
		} else if (ex_to_numeric(it->coeff).numer().compare(numMINUSONE()) == 0) {
			os << "-";
	        it->rest.bp->printcsrc(os, type, precedence);
			os << "/";
            ex_to_numeric(it->coeff).denom().printcsrc(os, type, precedence);
		} else {
            it->coeff.bp->printcsrc(os, type, precedence);
            os << "*";
	        it->rest.bp->printcsrc(os, type, precedence);
        }

		// Separator is "+", except it the following expression would have a leading minus sign
        it++;
        if (it != itend && !(it->coeff.compare(numZERO()) < 0 || (it->coeff.compare(numONE()) == 0 && is_ex_exactly_of_type(it->rest, numeric) && it->rest.compare(numZERO()) < 0)))
            os << "+";
    }
    
    if (!overall_coeff.is_equal(exZERO())) {
        if (overall_coeff > 0) os << '+';
        overall_coeff.bp->printcsrc(os,type,precedence);
    }
    
    if (precedence <= upper_precedence)
        os << ")";
}

void mul::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("mul print csrc", LOGLEVEL_PRINT);
    if (precedence <= upper_precedence)
        os << "(";

    if (!overall_coeff.is_equal(exONE())) {
        overall_coeff.bp->printcsrc(os,type,precedence);
        os << "*";
    }
    
	// Print arguments, separated by "*" or "/"
    epvector::const_iterator it = seq.begin();
    epvector::const_iterator itend = seq.end();
    while (it != itend) {

		// If the first argument is a negative integer power, it gets printed as "1.0/<expr>"
        if (it == seq.begin() && ex_to_numeric(it->coeff).is_integer() && it->coeff.compare(numZERO()) < 0) {
			if (type == csrc_types::ctype_cl_N)
				os << "recip(";
			else
	            os << "1.0/";
		}

		// If the exponent is 1 or -1, it is left out
        if (it->coeff.compare(exONE()) == 0 || it->coeff.compare(numMINUSONE()) == 0)
            it->rest.bp->printcsrc(os, type, precedence);
        else
            // outer parens around ex needed for broken gcc-2.95 parser:
            (ex(power(it->rest, abs(ex_to_numeric(it->coeff))))).bp->printcsrc(os, type, upper_precedence);

		// Separator is "/" for negative integer powers, "*" otherwise
        it++;
        if (it != itend) {
            if (ex_to_numeric(it->coeff).is_integer() && it->coeff.compare(numZERO()) < 0)
                os << "/";
            else
                os << "*";
        }
    }
    if (precedence <= upper_precedence)
        os << ")";
}

void ncmul::printcsrc(ostream & os, unsigned upper_precedence) const
{
    debugmsg("ncmul print csrc",LOGLEVEL_PRINT);
    exvector::const_iterator it;
    exvector::const_iterator itend = seq.end()-1;
    os << "ncmul(";
    for (it=seq.begin(); it!=itend; ++it) {
        (*it).bp->printcsrc(os,precedence);
        os << ",";
    }
    (*it).bp->printcsrc(os,precedence);
    os << ")";
}

void relational::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("relational print csrc", LOGLEVEL_PRINT);
    if (precedence<=upper_precedence)
		os << "(";

	// Print left-hand expression
    lh.bp->printcsrc(os, type, precedence);

	// Print relational operator
    switch (o) {
	    case equal:
	        os << "==";
	        break;
	    case not_equal:
	        os << "!=";
	        break;
	    case less:
	        os << "<";
	        break;
	    case less_or_equal:
	        os << "<=";
	        break;
	    case greater:
	        os << ">";
	        break;
	    case greater_or_equal:
	        os << ">=";
	        break;
	    default:
	        os << "(INVALID RELATIONAL OPERATOR)";
			break;
    }

	// Print right-hand operator
    rh.bp->printcsrc(os, type, precedence);

    if (precedence <= upper_precedence)
		os << ")";
}

} // namespace GiNaC
