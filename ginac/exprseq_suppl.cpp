/** @file exprseq_suppl.cpp
 *
 *  Supplement to exprseq.cpp, contains the parts which were not automatically
 *  generated. */

/*
 *  GiNaC Copyright (C) 1999-2003 Johannes Gutenberg University Mainz, Germany
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

#include "exprseq.h"
#include "ex.h"

namespace GiNaC {

bool exprseq::info(unsigned inf) const
{
	if (inf==info_flags::exprseq)
		return 1;
	return basic::info(inf);
}

ex & exprseq::let_op(int i)
{
	GINAC_ASSERT(i>=0);
	GINAC_ASSERT(i<nops());
	
	return seq[i];
}

} // namespace GiNaC
