/** @file ginac.h
 *
 *  This include file includes all other public GiNaC headers. */

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

#ifndef __GINAC_H__
#define __GINAC_H__

#include <ginac/basic.h>

#include <ginac/ex.h>
#include <ginac/normal.h>

#include <ginac/constant.h>
#include <ginac/fail.h>
#include <ginac/idx.h>
#include <ginac/lst.h>
#include <ginac/matrix.h>
#include <ginac/numeric.h>
#include <ginac/power.h>
#include <ginac/relational.h>
#include <ginac/structure.h>
#include <ginac/symbol.h>

#include <ginac/expair.h>
#include <ginac/expairseq.h>
#include <ginac/add.h>
#include <ginac/mul.h>
#include <ginac/series.h>

#include <ginac/exprseq.h>
#include <ginac/function.h>
#include <ginac/ncmul.h>

#include <ginac/inifcns.h>
#include <ginac/operators.h>

#ifndef GINAC_BASE_ONLY
#include <ginac/indexed.h>
#include <ginac/clifford.h>
#include <ginac/coloridx.h>
#include <ginac/color.h>
#include <ginac/isospin.h>
#include <ginac/lorentzidx.h>
#include <ginac/simp_lor.h>
#include <ginac/lortensor.h>
#endif // ndef GINAC_BASE_ONLY

#endif // ndef __GINAC_H__
