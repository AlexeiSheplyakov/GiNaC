/** @file tinfos.h
 *
 *  Values for a do-it-yourself typeinfo scheme. */

#ifndef _TINFOS_H_
#define _TINFOS_H_

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

#endif // ndef _TINFOS_H_
