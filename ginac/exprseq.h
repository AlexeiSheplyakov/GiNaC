/** @file exprseq.h
 *
 *  Definition of GiNaC's exprseq. */

/*
 *  GiNaC Copyright (C) 1999-2005 Johannes Gutenberg University Mainz, Germany
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

#ifndef __GINAC_EXPRSEQ_H__
#define __GINAC_EXPRSEQ_H__

#include <vector>

#include "container.h"

namespace GiNaC {

typedef container<std::vector> exprseq;

/** Specialization of container::get_tinfo() for exprseq. */
template<> inline unsigned exprseq::get_tinfo() { return TINFO_exprseq; }

// defined in exprseq.cpp
template<> bool exprseq::info(unsigned inf) const;

/** Specialization of is_exactly_a<exprseq>(obj) for exprseq objects. */
template<> inline bool is_exactly_a<exprseq>(const basic & obj)
{
	return obj.tinfo() == TINFO_exprseq;
}

} // namespace GiNaC

#endif // ndef __GINAC_EXPRSEQ_H__
