/** @file exams.h
 *
 *  Prototypes for all individual exams. */

/*
 *  GiNaC Copyright (C) 1999-2000 Johannes Gutenberg University Mainz, Germany
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

#ifndef EXAMS_H
#define EXAMS_H

#include "ginac.h"
using namespace std;

#ifndef NO_NAMESPACE_GINAC
using namespace GiNaC;
#endif // ndef NO_NAMESPACE_GINAC

// prototypes for all individual checks should be unsigned fcn():
unsigned exam_paranoia();
unsigned exam_numeric();
unsigned exam_powerlaws();
unsigned exam_inifcns();
unsigned exam_differentiation();
unsigned exam_polygcd();
unsigned exam_normalization();
unsigned exam_pseries();
unsigned exam_matrices();
unsigned exam_lsolve();
unsigned exam_noncommut();
unsigned exam_misc();

#endif // ndef EXAMS_H
