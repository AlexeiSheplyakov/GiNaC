/** @file tinfos.h
 *
 *  Values for a do-it-yourself typeinfo scheme. */

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

#ifndef __GINAC_TINFOS_H__
#define __GINAC_TINFOS_H__

namespace GiNaC {

const unsigned TINFO_basic         = 0x00000001U;

const unsigned TINFO_expairseq     = 0x00010001U;
const unsigned TINFO_add           = 0x00011001U;
const unsigned TINFO_mul           = 0x00011002U;

const unsigned TINFO_symbol        = 0x00020001U;
const unsigned TINFO_constant      = 0x00021001U;

const unsigned TINFO_exprseq       = 0x00030001U;
const unsigned TINFO_function      = 0x00031001U;
const unsigned TINFO_fderivative   = 0x00032001U;
const unsigned TINFO_ncmul         = 0x00031002U;

const unsigned TINFO_lst           = 0x00040001U;

const unsigned TINFO_matrix        = 0x00050001U;

const unsigned TINFO_power         = 0x00060001U;

const unsigned TINFO_relational    = 0x00070001U;

const unsigned TINFO_fail          = 0x00080001U;

const unsigned TINFO_numeric       = 0x00090001U;

const unsigned TINFO_pseries       = 0x000a0001U;

const unsigned TINFO_indexed       = 0x000b0001U;
const unsigned TINFO_color         = 0x000b1000U;
// reserved up to                    0x000b10ffU
// for color algebras (only used for return_type_tinfo())
const unsigned TINFO_clifford      = 0x000b1100U;
// reserved up to                    0x000b11ffU
// for clifford algebras (only used for return_type_tinfo())

const unsigned TINFO_structure     = 0x000c0001U;
// reserved up to                    0x000cffffU
// for user defined structures

const unsigned TINFO_idx           = 0x000d0001U;
const unsigned TINFO_varidx        = 0x000d1001U;
const unsigned TINFO_spinidx       = 0x000d2001U;

const unsigned TINFO_tensor        = 0x000e0001U;
const unsigned TINFO_tensdelta     = 0x000e1001U;
const unsigned TINFO_tensmetric    = 0x000e1002U;
const unsigned TINFO_minkmetric    = 0x000e2001U;
const unsigned TINFO_spinmetric    = 0x000e2002U;
const unsigned TINFO_tensepsilon   = 0x000e1003U;
const unsigned TINFO_su3one        = 0x000e1008U;
const unsigned TINFO_su3t          = 0x000e1009U;
const unsigned TINFO_su3f          = 0x000e100aU;
const unsigned TINFO_su3d          = 0x000e100bU;
const unsigned TINFO_diracone      = 0x000e100cU;
const unsigned TINFO_diracgamma    = 0x000e100dU;
const unsigned TINFO_diracgamma5   = 0x000e100eU;
const unsigned TINFO_diracgammaL   = 0x000e100fU;
const unsigned TINFO_diracgammaR   = 0x000e1010U;

const unsigned TINFO_wildcard      = 0x000f0001U;

const unsigned TINFO_symmetry      = 0x00100001U;

} // namespace GiNaC

#endif // ndef __GINAC_TINFOS_H__
