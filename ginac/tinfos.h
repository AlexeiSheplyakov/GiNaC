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

#define TINFO_BASIC        0x00000001U

#define TINFO_EXPAIRSEQ    0x00010001U
#define TINFO_ADD          0x00011001U
#define TINFO_MUL          0x00011002U

#define TINFO_SYMBOL       0x00020001U
#define TINFO_CONSTANT     0x00021001U

#define TINFO_EXPRSEQ      0x00030001U
#define TINFO_FUNCTION     0x00031001U
#define TINFO_NCMUL        0x00031002U
// #define TINFO_NFUNCTION     0x00031003U

#define TINFO_LST          0x00040001U

#define TINFO_MATRIX       0x00050001U

#define TINFO_POWER        0x00060001U

#define TINFO_RELATIONAL   0x00070001U

#define TINFO_FAIL         0x00080001U

#define TINFO_NUMERIC      0x00090001U

#define TINFO_SERIES       0x000a0001U

#define TINFO_INDEXED      0x000b0001U
#define TINFO_ALGEBRA      0x000b1001U
#define TINFO_CLIFFORD     0x000b1101U
#define TINFO_COLOR        0x000b1201U
#define TINFO_ISOSPIN      0x000b1301U
#define TINFO_SIMP_LOR     0x000b1401U

#define TINFO_STRUCTURE    0x000c0001U
// reserved up to          0x000cffffU
// for user defined structures

#define TINFO_IDX          0x000d0001U
#define TINFO_COLORIDX     0x000d1001U
#define TINFO_LORENTZIDX   0x000d1002U

#endif // ndef __GINAC_TINFOS_H__
