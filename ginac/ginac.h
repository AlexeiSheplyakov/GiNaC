/* ginac.h */

#ifndef __GINAC_H__
#define __GINAC_H__

#include <ginac/add.h>
#include <ginac/basic.h>
#include <ginac/constant.h>
#include <ginac/ex.h>
#include <ginac/exprseq.h>
#include <ginac/fail.h>
#include <ginac/function.h>
#include <ginac/idx.h>
#include <ginac/inifcns.h>
#include <ginac/lst.h>
#include <ginac/matrix.h>
#include <ginac/mul.h>
#include <ginac/ncmul.h>
#include <ginac/normal.h>
#include <ginac/numeric.h>
#include <ginac/operators.h>
#include <ginac/power.h>
#include <ginac/series.h>
#include <ginac/symbol.h>
#include <ginac/structure.h>

#ifndef GINAC_BASE_ONLY
#include <ginac/clifford.h>
#include <ginac/coloridx.h>
#include <ginac/color.h>
#include <ginac/isospin.h>
#include <ginac/lorentzidx.h>
#include <ginac/simp_lor.h>
#endif /* ndef GINAC_BASE_ONLY */

#endif /* ndef __GINAC_H__ */
