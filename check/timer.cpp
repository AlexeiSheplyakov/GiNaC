/** @file timer.cpp
 *
 *  A simple stop watch class. */

/*
 *  GiNaC Copyright (C) 1999-2003 Johannes Gutenberg University Mainz, Germany
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

timer::timer(void) : on(false)
{
	getrusage(RUSAGE_SELF, &used1);
	getrusage(RUSAGE_SELF, &used2);
}

void timer::start(void)
{
	on = true;
	getrusage(RUSAGE_SELF, &used1);
	getrusage(RUSAGE_SELF, &used2);
}

void timer::stop(void)
{
	on = false;
	getrusage(RUSAGE_SELF, &used2);
}

void timer::reset(void)
{
	getrusage(RUSAGE_SELF, &used1);
	getrusage(RUSAGE_SELF, &used2);
}

double timer::read(void)
{
	double elapsed;
	if (this->running())
		getrusage(RUSAGE_SELF, &used2);
	elapsed = ((used2.ru_utime.tv_sec - used1.ru_utime.tv_sec) +
			   (used2.ru_stime.tv_sec - used1.ru_stime.tv_sec) +
			   (used2.ru_utime.tv_usec - used1.ru_utime.tv_usec) / 1e6 +
			   (used2.ru_stime.tv_usec - used1.ru_stime.tv_usec) / 1e6);
	// round to 10ms for safety:
	return 0.01*int(elapsed*100+0.5);
}

bool timer::running(void)
{
	return on;
}
