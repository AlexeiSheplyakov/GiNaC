/** @file checks.h
 *
 *  Prototypes for all individual checks. */

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

#ifndef CHECKS_H
#define CHECKS_H

#include <iostream>

#include "config.h"
#include "ginac.h"
using namespace std;
using namespace GiNaC;

// prototypes for the expression generating functions in:
const ex dense_univariate_poly(const symbol & x, unsigned degree);
const ex dense_bivariate_poly(const symbol & x1, const symbol & x2,
                              unsigned degree);
const ex sparse_tree(const symbol & x, const symbol & y, const symbol & z,
                     int level,
                     bool trig = false, bool rational = true, bool complex = false);

// prototypes for all individual checks should be unsigned fcn():
unsigned check_numeric();
unsigned check_inifcns();
unsigned check_matrices();
unsigned check_lsolve();

#endif // ndef CHECKS_H
