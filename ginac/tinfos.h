/** @file tinfos.h
 *
 *  Values for a do-it-yourself typeinfo scheme. */

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

#ifndef __GINAC_TINFOS_H__
#define __GINAC_TINFOS_H__

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

const unsigned TINFO_basic        = 0x00000001U;

const unsigned TINFO_expairseq    = 0x00010001U;
const unsigned TINFO_add          = 0x00011001U;
const unsigned TINFO_mul          = 0x00011002U;

const unsigned TINFO_symbol       = 0x00020001U;
const unsigned TINFO_constant     = 0x00021001U;

const unsigned TINFO_exprseq      = 0x00030001U;
const unsigned TINFO_function     = 0x00031001U;
const unsigned TINFO_ncmul        = 0x00031002U;

const unsigned TINFO_lst          = 0x00040001U;

const unsigned TINFO_matrix       = 0x00050001U;

const unsigned TINFO_power        = 0x00060001U;

const unsigned TINFO_relational   = 0x00070001U;

const unsigned TINFO_fail         = 0x00080001U;

const unsigned TINFO_numeric      = 0x00090001U;

const unsigned TINFO_pseries      = 0x000a0001U;

const unsigned TINFO_indexed      = 0x000b0001U;
const unsigned TINFO_algebra      = 0x000b1001U;
const unsigned TINFO_clifford     = 0x000b1101U;
const unsigned TINFO_color        = 0x000b1201U;
const unsigned TINFO_isospin      = 0x000b1301U;
const unsigned TINFO_simp_lor     = 0x000b1401U;
const unsigned TINFO_lortensor    = 0x000b1501U;

const unsigned TINFO_structure    = 0x000c0001U;
// reserved up to                   0x000cffffU
// for user defined structures

const unsigned TINFO_idx          = 0x000d0001U;
const unsigned TINFO_coloridx     = 0x000d1001U;
const unsigned TINFO_lorentzidx   = 0x000d1002U;

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

#endif // ndef __GINAC_TINFOS_H__
