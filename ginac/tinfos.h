/** @file tinfos.h
 *
 *  Values for a do-it-yourself typeinfo scheme.
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

#ifndef __GINAC_TINFOS_H__
#define __GINAC_TINFOS_H__

#define TINFO_basic        0x00000001U

#define TINFO_expairseq    0x00010001U
#define TINFO_add          0x00011001U
#define TINFO_mul          0x00011002U

#define TINFO_symbol       0x00020001U
#define TINFO_constant     0x00021001U

#define TINFO_exprseq      0x00030001U
#define TINFO_function     0x00031001U
#define TINFO_ncmul        0x00031002U

#define TINFO_lst          0x00040001U

#define TINFO_matrix       0x00050001U

#define TINFO_power        0x00060001U

#define TINFO_relational   0x00070001U

#define TINFO_fail         0x00080001U

#define TINFO_numeric      0x00090001U

#define TINFO_series       0x000a0001U

#define TINFO_indexed      0x000b0001U
#define TINFO_algebra      0x000b1001U
#define TINFO_clifford     0x000b1101U
#define TINFO_color        0x000b1201U
#define TINFO_isospin      0x000b1301U
#define TINFO_simp_lor     0x000b1401U

#define TINFO_structure    0x000c0001U
// reserved up to          0x000cffffU
// for user defined structures

#define TINFO_idx          0x000d0001U
#define TINFO_coloridx     0x000d1001U
#define TINFO_lorentzidx   0x000d1002U

#endif // ndef __GINAC_TINFOS_H__
