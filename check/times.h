/** @file times.h
 *
 *  Prototypes for all individual timings. */

/*
 *  GiNaC Copyright (C) 1999-2002 Johannes Gutenberg University Mainz, Germany
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

#include <sys/resource.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include "config.h"
#include "ginac.h"
using namespace std;
using namespace GiNaC;

class timer {
public:
	timer();
	void start(void);
	void stop(void);
	void reset(void);
	double read(void);
	bool running(void);
private:
	bool on;
	struct rusage used1, used2;
};

// prototypes for all individual timings should be unsigned fcn():
unsigned time_dennyfliegner();
unsigned time_gammaseries();
unsigned time_vandermonde();
unsigned time_toeplitz();
unsigned time_lw_A();
unsigned time_lw_B();
unsigned time_lw_C();
unsigned time_lw_D();
unsigned time_lw_E();
unsigned time_lw_F();
unsigned time_lw_G();
unsigned time_lw_H();
unsigned time_lw_IJKL();
unsigned time_lw_M1();
unsigned time_lw_M2();
unsigned time_lw_N();
unsigned time_lw_O();
unsigned time_lw_P();
unsigned time_lw_Pprime();
unsigned time_lw_Q();
unsigned time_lw_Qprime();
unsigned time_antipode();
unsigned time_fateman_expand();

#endif // ndef CHECKS_H
