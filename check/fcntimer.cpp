/** @file fcntimer.cpp
 *
 *  Function execution timer.
 *
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


#include <stdio.h>
#include <sys/resource.h>


// fcntimer() is a little wrapper around GiNaC's automated checks.  All those
// functions are passed void and return unsigned.  fcntimer() accepts one such
// function fcn(), returns its result and as a side-effect prints to stdout how
// much CPU time was consumed by fcn's execution in the fashion "(0.07s)\n".
unsigned fcntimer(unsigned fcn())
{
    unsigned fcnresult;
    struct rusage used1, used2;
    double elapsed;

    // time the execution of the function:
    getrusage(RUSAGE_SELF, &used1);
    fcnresult = fcn();
    getrusage(RUSAGE_SELF, &used2);

    // add elapsed user and system time in microseconds:
    elapsed = ((used2.ru_utime.tv_sec - used1.ru_utime.tv_sec) +
               (used2.ru_stime.tv_sec - used1.ru_stime.tv_sec) +
               (used2.ru_utime.tv_usec - used1.ru_utime.tv_usec) / 1e6 +
               (used2.ru_stime.tv_usec - used1.ru_stime.tv_usec) / 1e6);

    printf("(%.2fs)\n", elapsed);

    return fcnresult;
}
