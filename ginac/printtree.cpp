/** @file printtree.cpp
 *
 * print in tree- (indented-) form, so developers can have a look at the
 * underlying structure. */

/*
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

#include <iostream>
#include <math.h>

#include "basic.h"
#include "ex.h"
#include "add.h"
#include "constant.h"
#include "expairseq.h"
#include "indexed.h"
#include "inifcns.h"
#include "mul.h"
#include "ncmul.h"
#include "numeric.h"
#include "power.h"
#include "relational.h"
#include "series.h"
#include "symbol.h"
#include "debugmsg.h"

namespace GiNaC {

void ex::printtree(ostream & os, unsigned indent) const
{
    debugmsg("ex printtree",LOGLEVEL_PRINT);
    ASSERT(bp!=0);
    // os << "refcount=" << bp->refcount << " ";
    bp->printtree(os,indent);
}

void ex::dbgprinttree(void) const
{
    debugmsg("ex dbgprinttree",LOGLEVEL_PRINT);
    ASSERT(bp!=0);
    bp->dbgprinttree();
}

void basic::printtree(ostream & os, unsigned indent) const
{
    debugmsg("basic printtree",LOGLEVEL_PRINT);
    os << string(indent,' ') << "type=" << typeid(*this).name()
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags
       << ", nops=" << nops() << endl;
    for (int i=0; i<nops(); ++i) {
        op(i).printtree(os,indent+delta_indent);
    }
}

void basic::dbgprinttree(void) const
{
    printtree(cerr,0);
}

void numeric::printtree(ostream & os, unsigned indent) const
{
    debugmsg("numeric printtree", LOGLEVEL_PRINT);
    // We are cheating here, because we don't want to include the underlying
    // bignum package's headers again, so we use ostream::operator<<(numeric):
    os << string(indent,' ');
    (*this).print(os);
    os << " (numeric): "
       << "hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void symbol::printtree(ostream & os, unsigned indent) const
{
    debugmsg("symbol printtree",LOGLEVEL_PRINT);
    os << string(indent,' ') << name << " (symbol): "
       << "serial=" << serial
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void power::printtree(ostream & os, unsigned indent) const
{
    debugmsg("power printtree",LOGLEVEL_PRINT);

    os << string(indent,' ') << "power: "
       << "hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
    basis.printtree(os,indent+delta_indent);
    exponent.printtree(os,indent+delta_indent);
}

void expairseq::printtree(ostream & os, unsigned indent) const
{
    debugmsg("expairseq printtree",LOGLEVEL_PRINT);

    os << string(indent,' ') << "type=" << typeid(*this).name()
       << ", hash=" << hashvalue << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags
       << ", nops=" << nops() << endl;
    for (unsigned i=0; i<seq.size(); ++i) {
        seq[i].rest.printtree(os,indent+delta_indent);
        seq[i].coeff.printtree(os,indent+delta_indent);
        if (i!=seq.size()-1) {
            os << string(indent+delta_indent,' ') << "-----" << endl;
        }
    }
    if (!overall_coeff.is_equal(default_overall_coeff())) {
        os << string(indent+delta_indent,' ') << "-----" << endl;
        os << string(indent+delta_indent,' ') << "overall_coeff" << endl;
        overall_coeff.printtree(os,indent+delta_indent);
    }
    os << string(indent+delta_indent,' ') << "=====" << endl;
#ifdef EXPAIRSEQ_USE_HASHTAB
    os << string(indent+delta_indent,' ')
       << "hashtab size " << hashtabsize << endl;
    if (hashtabsize==0) return;
#define MAXCOUNT 5
    unsigned count[MAXCOUNT+1];
    for (int i=0; i<MAXCOUNT+1; ++i) count[i]=0;
    unsigned this_bin_fill;
    unsigned cum_fill_sq=0;
    unsigned cum_fill=0;
    for (unsigned i=0; i<hashtabsize; ++i) {
        this_bin_fill=0;
        if (hashtab[i].size()>0) {
            os << string(indent+delta_indent,' ') 
               << "bin " << i << " with entries ";
            for (epplist::const_iterator it=hashtab[i].begin();
                 it!=hashtab[i].end(); ++it) {
                os << *it-seq.begin() << " ";
                this_bin_fill++;
            }
            os << endl;
            cum_fill += this_bin_fill;
            cum_fill_sq += this_bin_fill*this_bin_fill;
        }
        if (this_bin_fill<MAXCOUNT) {
            ++count[this_bin_fill];
        } else {
            ++count[MAXCOUNT];
        }
    }
    unsigned fact=1;
    double cum_prob=0;
    double lambda=(1.0*seq.size())/hashtabsize;
    for (int k=0; k<MAXCOUNT; ++k) {
        if (k>0) fact *= k;
        double prob=pow(lambda,k)/fact*exp(-lambda);
        cum_prob += prob;
        os << string(indent+delta_indent,' ') << "bins with " << k << " entries: "
           << int(1000.0*count[k]/hashtabsize)/10.0 << "% (expected: "
           << int(prob*1000)/10.0 << ")" << endl;
    }
    os << string(indent+delta_indent,' ') << "bins with more entries: "
       << int(1000.0*count[MAXCOUNT]/hashtabsize)/10.0 << "% (expected: "
       << int((1-cum_prob)*1000)/10.0 << ")" << endl;
    
    os << string(indent+delta_indent,' ') << "variance: "
       << 1.0/hashtabsize*cum_fill_sq-(1.0/hashtabsize*cum_fill)*(1.0/hashtabsize*cum_fill)
       << endl;
    os << string(indent+delta_indent,' ') << "average fill: "
       << (1.0*cum_fill)/hashtabsize
       << " (should be equal to " << (1.0*seq.size())/hashtabsize << ")" << endl;
#endif // def EXPAIRSEQ_USE_HASHTAB
}

} // namespace GiNaC
