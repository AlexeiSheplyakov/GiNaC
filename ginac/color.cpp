/** @file color.cpp
 *
 *  Implementation of GiNaC's color objects.
 *  No real implementation yet, to be done.     */

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

#include <string>
#include <list>
#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "color.h"
#include "ex.h"
#include "coloridx.h"
#include "ncmul.h"
#include "numeric.h"
#include "relational.h"
#include "archive.h"
#include "debugmsg.h"
#include "utils.h"

#ifndef NO_GINAC_NAMESPACE
namespace GiNaC {
#endif // ndef NO_GINAC_NAMESPACE

GINAC_IMPLEMENT_REGISTERED_CLASS(color, indexed)

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

color::color() : type(invalid), representation_label(0)
{
    debugmsg("color default constructor",LOGLEVEL_CONSTRUCT);
    tinfo_key=TINFO_color;
}

color::~color()
{
    debugmsg("color destructor",LOGLEVEL_DESTRUCT);
    destroy(0);
}

color::color(const color & other)
{
    debugmsg("color copy constructor",LOGLEVEL_CONSTRUCT);
    copy (other);
}

const color & color::operator=(const color & other)
{
    debugmsg("color operator=",LOGLEVEL_ASSIGNMENT);
    if (this != &other) {
        destroy(1);
       copy(other);
    }
    return *this;
}

// protected

void color::copy(const color & other)
{
    inherited::copy(other);
    type=other.type;
    representation_label=other.representation_label;
}

void color::destroy(bool call_parent)
{
    if (call_parent) {
        inherited::destroy(call_parent);
    }
}

//////////
// other constructors
//////////

// protected

color::color(color_types const t, unsigned rl) : type(t), representation_label(rl)
{
    debugmsg("color constructor from color_types,unsigned",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(representation_label<MAX_REPRESENTATION_LABELS);
    tinfo_key=TINFO_color;
    GINAC_ASSERT(all_of_type_coloridx());
}

color::color(color_types const t, const ex & i1, unsigned rl)
    : inherited(i1), type(t), representation_label(rl)
{
    debugmsg("color constructor from color_types,ex,unsigned",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(representation_label<MAX_REPRESENTATION_LABELS);
    tinfo_key=TINFO_color;
    GINAC_ASSERT(all_of_type_coloridx());
}

color::color(color_types const t, const ex & i1, const ex & i2, unsigned rl)
    : inherited(i1,i2), type(t), representation_label(rl)
{
    debugmsg("color constructor from color_types,ex,ex,unsigned",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(representation_label<MAX_REPRESENTATION_LABELS);
    tinfo_key=TINFO_color;
    GINAC_ASSERT(all_of_type_coloridx());
}

color::color(color_types const t, const ex & i1, const ex & i2, const ex & i3,
             unsigned rl) : inherited(i1,i2,i3), type(t), representation_label(rl)
{
    debugmsg("color constructor from color_types,ex,ex,ex,unsigned",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(representation_label<MAX_REPRESENTATION_LABELS);
    tinfo_key=TINFO_color;
    GINAC_ASSERT(all_of_type_coloridx());
}

color::color(color_types const t, const exvector & iv, unsigned rl)
    : inherited(iv), type(t), representation_label(rl)
{
    debugmsg("color constructor from color_types,exvector,unsigned",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(representation_label<MAX_REPRESENTATION_LABELS);
    tinfo_key=TINFO_color;
    GINAC_ASSERT(all_of_type_coloridx());
}

color::color(color_types const t, exvector * ivp, unsigned rl)
    : inherited(ivp), type(t), representation_label(rl)
{
    debugmsg("color constructor from color_types,exvector *,unsigned",LOGLEVEL_CONSTRUCT);
    GINAC_ASSERT(representation_label<MAX_REPRESENTATION_LABELS);
    tinfo_key=TINFO_color;
    GINAC_ASSERT(all_of_type_coloridx());
}

//////////
// archiving
//////////

/** Construct object from archive_node. */
color::color(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
    debugmsg("color constructor from archive_node", LOGLEVEL_CONSTRUCT);
    unsigned int ty;
    if (!(n.find_unsigned("type", ty)))
        throw (std::runtime_error("unknown color type in archive"));
    type = (color_types)ty;
    if (!(n.find_unsigned("representation", representation_label)))
        throw (std::runtime_error("unknown color representation label in archive"));
}

/** Unarchive the object. */
ex color::unarchive(const archive_node &n, const lst &sym_lst)
{
    return (new color(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void color::archive(archive_node &n) const
{
    inherited::archive(n);
    n.add_unsigned("type", type);
    n.add_unsigned("representation", representation_label);
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * color::duplicate() const
{
    debugmsg("color duplicate",LOGLEVEL_DUPLICATE);
    return new color(*this);
}

void color::printraw(ostream & os) const
{
    debugmsg("color printraw",LOGLEVEL_PRINT);
    os << "color(type=" << (unsigned)type
       << ",representation_label=" << representation_label
       << ",indices=";
    printrawindices(os);
    os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void color::printtree(ostream & os, unsigned indent) const
{
    debugmsg("color printtree",LOGLEVEL_PRINT);
    os << string(indent,' ') << "color object: "
       << "type=" << (unsigned)type
       << ",representation_label=" << representation_label << ", ";
    os << seq.size() << " indices" << endl;
    printtreeindices(os,indent);
    os << string(indent,' ') << "hash=" << hashvalue
       << " (0x" << hex << hashvalue << dec << ")"
       << ", flags=" << flags << endl;
}

void color::print(ostream & os, unsigned upper_precedence) const
{
    debugmsg("color print",LOGLEVEL_PRINT);
    switch (type) {
    case color_T:
        os << "T";
        if (representation_label!=0) {
            os << "^(" << representation_label << ")";
        }
        break;
    case color_f:
        os << "f";
        break;
    case color_d:
        os << "d";
        break;
    case color_delta8:
        os << "delta8";
        break;
    case color_ONE:
        os << "color_ONE";
        break;
    case invalid:
    default:
        os << "INVALID_COLOR_OBJECT";
        break;
    }
    printindices(os);
}

void color::printcsrc(ostream & os, unsigned type, unsigned upper_precedence) const
{
    debugmsg("color print csrc",LOGLEVEL_PRINT);
    print(os,upper_precedence);
}

bool color::info(unsigned inf) const
{
    return inherited::info(inf);
}

#define CMPINDICES(A,B,C) ((idx1.get_value()==(A))&&(idx2.get_value()==(B))&&(idx3.get_value()==(C)))

ex color::eval(int level) const
{
    // canonicalize indices
    
    bool antisymmetric=false;
    
    switch (type) {
    case color_f:
        antisymmetric=true; // no break here!
    case color_d:
    case color_delta8:
        {
            exvector iv=seq;
            int sig=canonicalize_indices(iv,antisymmetric);
            if (sig!=INT_MAX) {
                // something has changed while sorting indices, more evaluations later
                if (sig==0) return _ex0();
                return ex(sig)*color(type,iv,representation_label);
            }
        }
        break;
    default:
        // nothing to canonicalize
        break;
    }

    switch (type) {
    case color_delta8:
        {
            GINAC_ASSERT(seq.size()==2);
            const coloridx & idx1=ex_to_coloridx(seq[0]);
            const coloridx & idx2=ex_to_coloridx(seq[1]);
            
            // check for delta8_{a,a} where a is a symbolic index, replace by 8
            if ((idx1.is_symbolic())&&(idx1.is_equal_same_type(idx2))) {
                return ex(COLOR_EIGHT);
            }

            // check for delta8_{a,b} where a and b are numeric indices, replace by 0 or 1
            if ((!idx1.is_symbolic())&&(!idx2.is_symbolic())) {
                if ((idx1.get_value()!=idx2.get_value())) {
                    return _ex1();
                } else {
                    return _ex0();
                }
            }
	}
        break;
    case color_d:
        // check for d_{a,a,c} (=0) when a is symbolic
        {
            GINAC_ASSERT(seq.size()==3);
            const coloridx & idx1=ex_to_coloridx(seq[0]);
            const coloridx & idx2=ex_to_coloridx(seq[1]);
            const coloridx & idx3=ex_to_coloridx(seq[2]);
            
            if (idx1.is_equal_same_type(idx2) && idx1.is_symbolic()) {
                return _ex0();
            } else if (idx2.is_equal_same_type(idx3) && idx2.is_symbolic()) {
                return _ex0();
            }
            
            // check for three numeric indices
            if (!(idx1.is_symbolic()||idx2.is_symbolic()||idx3.is_symbolic())) {
                GINAC_ASSERT(idx1.get_value()<=idx2.get_value());
                GINAC_ASSERT(idx2.get_value()<=idx3.get_value());
                if (CMPINDICES(1,4,6)||CMPINDICES(1,5,7)||CMPINDICES(2,5,6)||
                    CMPINDICES(3,4,4)||CMPINDICES(3,5,5)) {
                    return _ex1_2();
                } else if (CMPINDICES(2,4,7)||CMPINDICES(3,6,6)||CMPINDICES(3,7,7)) {
                    return -_ex1_2();
                } else if (CMPINDICES(1,1,8)||CMPINDICES(2,2,8)||CMPINDICES(3,3,8)) {
                    return 1/sqrt(numeric(3));
                } else if (CMPINDICES(8,8,8)) {
                    return -1/sqrt(numeric(3));
                } else if (CMPINDICES(4,4,8)||CMPINDICES(5,5,8)||CMPINDICES(6,6,8)||CMPINDICES(7,7,8)) {
                    return -1/(2*sqrt(numeric(3)));
                }
                return _ex0();
            }
        }
        break;
    case color_f:
        {
            GINAC_ASSERT(seq.size()==3);
            const coloridx & idx1=ex_to_coloridx(seq[0]);
            const coloridx & idx2=ex_to_coloridx(seq[1]);
            const coloridx & idx3=ex_to_coloridx(seq[2]);
            
            // check for three numeric indices
            if (!(idx1.is_symbolic()||idx2.is_symbolic()||idx3.is_symbolic())) {
                GINAC_ASSERT(idx1.get_value()<=idx2.get_value());
                GINAC_ASSERT(idx2.get_value()<=idx3.get_value());
                if (CMPINDICES(1,2,3)) {
                    return _ex1();
                } else if (CMPINDICES(1,4,7)||CMPINDICES(2,4,6)||
                           CMPINDICES(2,5,7)||CMPINDICES(3,4,5)) {
                    return _ex1_2();
                } else if (CMPINDICES(1,5,6)||CMPINDICES(3,6,7)) {
                    return -_ex1_2();
                } else if (CMPINDICES(4,5,8)||CMPINDICES(6,7,8)) {
                    return sqrt(numeric(3))/2;
                } else if (CMPINDICES(8,8,8)) {
                    return -1/sqrt(numeric(3));
                } else if (CMPINDICES(4,4,8)||CMPINDICES(5,5,8)||CMPINDICES(6,6,8)||CMPINDICES(7,7,8)) {
                    return -1/(2*sqrt(numeric(3)));
                }
                return _ex0();
            }
            break;
        }
    default:
        // nothing to evaluate
        break;
    }
    
    return this->hold();
}
    
// protected

int color::compare_same_type(const basic & other) const
{
    GINAC_ASSERT(other.tinfo() == TINFO_color);
    const color *o = static_cast<const color *>(&other);
    if (type==o->type) {
        if (representation_label==o->representation_label) {
            return inherited::compare_same_type(other);
        }
        return representation_label < o->representation_label ? -1 : 1;
    }
    return type < o->type ? -1 : 1;
}

bool color::is_equal_same_type(const basic & other) const
{
    GINAC_ASSERT(other.tinfo() == TINFO_color);
    const color *o = static_cast<const color *>(&other);
    if (type!=o->type) return false;
    if (representation_label!=o->representation_label) return false;
    return inherited::is_equal_same_type(other);
}

#include <iostream>

ex color::simplify_ncmul(const exvector & v) const
{
    // simplifications: contract delta8_{a,b} where possible
    //                  sort delta8,f,d,T(rl=0),T(rl=1),...,ONE(rl=0),ONE(rl=1),...
    //                  remove superfluous ONEs
    
    // contract indices of delta8_{a,b} if they are different and symbolic

    exvector v_contracted=v;
    unsigned replacements;
    bool something_changed=false;

    exvector::iterator it=v_contracted.begin();
    while (it!=v_contracted.end()) {
        // process only delta8 objects
        if (is_ex_exactly_of_type(*it,color) && (ex_to_color(*it).type==color_delta8)) {
            color & d8=ex_to_nonconst_color(*it);
            GINAC_ASSERT(d8.seq.size()==2);
            const coloridx & first_idx=ex_to_coloridx(d8.seq[0]);
            const coloridx & second_idx=ex_to_coloridx(d8.seq[1]);
            // delta8_{a,a} should have been contracted in color::eval()
            GINAC_ASSERT((!first_idx.is_equal(second_idx))||(!first_idx.is_symbolic()));
            ex saved_delta8=*it; // save to restore it later

            // try to contract first index
            replacements=1;
            if (first_idx.is_symbolic()) {
                replacements = subs_index_in_exvector(v_contracted,first_idx,second_idx);
                if (replacements==1) {
                    // not contracted except in itself, restore delta8 object
                    *it=saved_delta8;
                } else {
                    // a contracted index should occur exactly twice
                    GINAC_ASSERT(replacements==2);
                    *it=_ex1();
                    something_changed=true;
                }
            }

            // try second index only if first was not contracted
            if ((replacements==1)&&(second_idx.is_symbolic())) {
                // first index not contracted, *it is guaranteed to be the original delta8 object
                replacements = subs_index_in_exvector(v_contracted,second_idx,first_idx);
                if (replacements==1) {
                    // not contracted except in itself, restore delta8 object
                    *it=saved_delta8;
                } else {
                    // a contracted index should occur exactly twice
                    GINAC_ASSERT(replacements==2);
                    *it=_ex1();
                    something_changed=true;
                }
            }
        }
        ++it;
    }

    if (something_changed) {
        // do more simplifications later
        return nonsimplified_ncmul(v_contracted);
    }

    // there were no indices to contract
    // sort delta8,f,d,T(rl=0),T(rl=1),...,ONE(rl=0),ONE(rl=1),...,unknown
    // (if there is at least one unknown object, all Ts will be unknown to not change the order)
    
    exvector delta8vec;
    exvector fvec;
    exvector dvec;
    exvectorvector Tvecs;
    Tvecs.resize(MAX_REPRESENTATION_LABELS);
    exvectorvector ONEvecs;
    ONEvecs.resize(MAX_REPRESENTATION_LABELS);
    exvector unknownvec;
    
    split_color_string_in_parts(v,delta8vec,fvec,dvec,Tvecs,ONEvecs,unknownvec);

    // d_{a,k,l} f_{b,k,l}=0 (includes case a=b)
    if ((dvec.size()>=1)&&(fvec.size()>=1)) {
        for (exvector::iterator it1=dvec.begin(); it1!=dvec.end(); ++it1) {
            for (exvector::iterator it2=fvec.begin(); it2!=fvec.end(); ++it2) {
                GINAC_ASSERT(is_ex_exactly_of_type(*it1,color));
                GINAC_ASSERT(is_ex_exactly_of_type(*it2,color));
                const color & col1=ex_to_color(*it1);
                const color & col2=ex_to_color(*it2);
                exvector iv_intersect=idx_intersect(col1.seq,col2.seq);
                if (iv_intersect.size()>=2) return _ex0();
            }
        }
    }
    
    // d_{a,k,l} d_{b,k,l}=5/3 delta8_{a,b} (includes case a=b)
    if (dvec.size()>=2) {
        for (exvector::iterator it1=dvec.begin(); it1!=dvec.end()-1; ++it1) {
            for (exvector::iterator it2=it1+1; it2!=dvec.end(); ++it2) {
                GINAC_ASSERT(is_ex_exactly_of_type(*it1,color));
                GINAC_ASSERT(is_ex_exactly_of_type(*it2,color));
                const color & col1=ex_to_color(*it1);
                const color & col2=ex_to_color(*it2);
                exvector iv_intersect=idx_intersect(col1.seq,col2.seq);
                if (iv_intersect.size()>=2) {
                    if (iv_intersect.size()==3) {
                        *it1=numeric(40)/numeric(3);
                        *it2=_ex1();
                    } else {
                        int sig1, sig2; // unimportant, since symmetric
                        ex idx1=permute_free_index_to_front(col1.seq,iv_intersect,false,&sig1);
                        ex idx2=permute_free_index_to_front(col2.seq,iv_intersect,false,&sig2);
                        *it1=numeric(5)/numeric(3)*color(color_delta8,idx1,idx2);
                        *it2=_ex1();
                    }
                    return nonsimplified_ncmul(recombine_color_string(
                           delta8vec,fvec,dvec,Tvecs,ONEvecs,unknownvec));
                }
            }
        }
    }

    // f_{a,k,l} f_{b,k,l}=3 delta8_{a,b} (includes case a=b)
    if (fvec.size()>=2) {
        for (exvector::iterator it1=fvec.begin(); it1!=fvec.end()-1; ++it1) {
            for (exvector::iterator it2=it1+1; it2!=fvec.end(); ++it2) {
                GINAC_ASSERT(is_ex_exactly_of_type(*it1,color));
                GINAC_ASSERT(is_ex_exactly_of_type(*it2,color));
                const color & col1=ex_to_color(*it1);
                const color & col2=ex_to_color(*it2);
                exvector iv_intersect=idx_intersect(col1.seq,col2.seq);
                if (iv_intersect.size()>=2) {
                    if (iv_intersect.size()==3) {
                        *it1=numeric(24);
                        *it2=_ex1();
                    } else {
                        int sig1, sig2;
                        ex idx1=permute_free_index_to_front(col1.seq,iv_intersect,true,&sig1);
                        ex idx2=permute_free_index_to_front(col2.seq,iv_intersect,true,&sig2);
                        *it1=numeric(sig1*sig2*5)/numeric(3)*color(color_delta8,idx1,idx2);
                        *it2=_ex1();
                    }
                    return nonsimplified_ncmul(recombine_color_string(
                           delta8vec,fvec,dvec,Tvecs,ONEvecs,unknownvec));
                }
            }
        }
    }

    // d_{a,b,c} T_b T_c = 5/6 T_a
    // f_{a,b,c} T_b T_c = 3/2 I T_a
    for (unsigned rl=0; rl<MAX_REPRESENTATION_LABELS; ++rl) {
        if ((Tvecs[rl].size()>=2)&&((dvec.size()>=1)||(fvec.size()>=1))) {
            for (exvector::iterator it1=Tvecs[rl].begin(); it1!=Tvecs[rl].end()-1; ++it1) {
                exvector iv;
                GINAC_ASSERT(is_ex_exactly_of_type(*it1,color)&&ex_to_color(*it1).type==color_T);
                GINAC_ASSERT(is_ex_exactly_of_type(*(it1+1),color)&&ex_to_color(*(it1+1)).type==color_T);
                iv.push_back(ex_to_color(*it1).seq[0]);
                iv.push_back(ex_to_color(*(it1+1)).seq[0]);
                
                // d_{a,b,c} T_b T_c = 5/6 T_a
                for (exvector::iterator it2=dvec.begin(); it2!=dvec.end(); ++it2) {
                    GINAC_ASSERT(is_ex_exactly_of_type(*it2,color)&&ex_to_color(*it2).type==color_d);
                    const color & dref=ex_to_color(*it2);
                    exvector iv_intersect=idx_intersect(dref.seq,iv);
                    if (iv_intersect.size()==2) {
                        int sig; // unimportant, since symmetric
                        ex free_idx=permute_free_index_to_front(dref.seq,iv,false,&sig);
                        *it1=color(color_T,free_idx,rl);
                        *(it1+1)=color(color_ONE,rl);
                        *it2=numeric(5)/numeric(6);
                        return nonsimplified_ncmul(recombine_color_string(
                               delta8vec,fvec,dvec,Tvecs,ONEvecs,unknownvec));
                    }
                }

                // f_{a,b,c} T_b T_c = 3/2 I T_a
                for (exvector::iterator it2=fvec.begin(); it2!=fvec.end(); ++it2) {
                    GINAC_ASSERT(is_ex_exactly_of_type(*it2,color)&&ex_to_color(*it2).type==color_f);
                    const color & fref=ex_to_color(*it2);
                    exvector iv_intersect=idx_intersect(fref.seq,iv);
                    if (iv_intersect.size()==2) {
                        int sig;
                        ex free_idx=permute_free_index_to_front(fref.seq,iv,true,&sig);
                        *it1=color(color_T,free_idx,rl);
                        *(it1+1)=color(color_ONE,rl);
                        *it2=numeric(sig*3)/numeric(2)*I;
                        return nonsimplified_ncmul(recombine_color_string(
                               delta8vec,fvec,dvec,Tvecs,ONEvecs,unknownvec));
                    }
                }
            }
        }
    }
    
    // clear all ONEs when there is at least one corresponding color_T
    // in this representation, retain one ONE otherwise
    for (unsigned rl=0; rl<MAX_REPRESENTATION_LABELS; ++rl) {
        if (Tvecs[rl].size()!=0) {
            ONEvecs[rl].clear();
        } else if (ONEvecs[rl].size()!=0) {
            ONEvecs[rl].clear();
            ONEvecs[rl].push_back(color(color_ONE,rl));
        }
    }

    // return a sorted vector
    return simplified_ncmul(recombine_color_string(delta8vec,fvec,dvec,Tvecs,
                                                   ONEvecs,unknownvec));
}

ex color::thisexprseq(const exvector & v) const
{
    return color(type,v,representation_label);
}

ex color::thisexprseq(exvector * vp) const
{
    return color(type,vp,representation_label);
}

bool color::all_of_type_coloridx(void) const
{
    // used only inside of ASSERTs
    for (exvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        if (!is_ex_of_type(*cit,coloridx)) return false;
    }
    return true;
}

//////////
// virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

//////////
// static member variables
//////////

// none

//////////
// global constants
//////////

const color some_color;
const type_info & typeid_color=typeid(some_color);

//////////
// friend functions
//////////

color color_ONE(unsigned rl)
{
    return color(color::color_ONE,rl);
}

color color_T(const ex & a, unsigned rl)
{
    return color(color::color_T,a,rl);
}

color color_f(const ex & a, const ex & b, const ex & c)
{
    return color(color::color_f,a,b,c);
}

color color_d(const ex & a, const ex & b, const ex & c)
{
    return color(color::color_d,a,b,c);
}

ex color_h(const ex & a, const ex & b, const ex & c)
{
    return color(color::color_d,a,b,c)+I*color(color::color_f,a,b,c);
}

color color_delta8(const ex & a, const ex & b)
{
    return color(color::color_delta8,a,b);
}

void split_color_string_in_parts(const exvector & v, exvector & delta8vec,
                                 exvector & fvec, exvector & dvec,
                                 exvectorvector & Tvecs,
                                 exvectorvector & ONEvecs,
                                 exvector & unknownvec)
{
    // if not all elements are of type color, put all Ts in unknownvec to
    // retain the ordering
    bool all_color=true;
    for (exvector::const_iterator cit=v.begin(); cit!=v.end(); ++cit) {
        if (!is_ex_exactly_of_type(*cit,color)) {
            all_color=false;
            break;
        }
    }
    
    for (exvector::const_iterator cit=v.begin(); cit!=v.end(); ++cit) {
        if (is_ex_exactly_of_type(*cit,color)) {
            switch (ex_to_color(*cit).type) {
            case color::color_delta8:
                delta8vec.push_back(*cit);
                break;
            case color::color_f:
                fvec.push_back(*cit);
                break;
            case color::color_d:
                dvec.push_back(*cit);
                break;
            case color::color_T:
                GINAC_ASSERT(ex_to_color(*cit).representation_label<MAX_REPRESENTATION_LABELS);
                if (all_color) {
                    Tvecs[ex_to_color(*cit).representation_label].push_back(*cit);
                } else {
                    unknownvec.push_back(*cit);
                }
                break;
            case color::color_ONE:
                GINAC_ASSERT(ex_to_color(*cit).representation_label<MAX_REPRESENTATION_LABELS);
                ONEvecs[ex_to_color(*cit).representation_label].push_back(*cit);
                break;
            default:
                throw(std::logic_error("invalid type in split_color_string_in_parts()"));
            }
        } else {
            unknownvec.push_back(*cit);
        }
    }
}    

exvector recombine_color_string(exvector & delta8vec, exvector & fvec,
                                exvector & dvec, exvectorvector & Tvecs,
                                exvectorvector & ONEvecs, exvector & unknownvec)
{
    unsigned sz=delta8vec.size()+fvec.size()+dvec.size()+unknownvec.size();
    for (unsigned rl=0; rl<MAX_REPRESENTATION_LABELS; ++rl) {
        sz += Tvecs[rl].size();
        sz += ONEvecs[rl].size();
    }
    exvector v;
    v.reserve(sz);
    
    append_exvector_to_exvector(v,delta8vec);
    append_exvector_to_exvector(v,fvec);
    append_exvector_to_exvector(v,dvec);
    for (unsigned rl=0; rl<MAX_REPRESENTATION_LABELS; ++rl) {
        append_exvector_to_exvector(v,Tvecs[rl]);
        append_exvector_to_exvector(v,ONEvecs[rl]);
    }
    append_exvector_to_exvector(v,unknownvec);
    return v;
}

ex color_trace_of_one_representation_label(const exvector & v)
{
    if (v.size()==0) {
        return numeric(COLOR_THREE);
    } else if (v.size()==1) {
        GINAC_ASSERT(is_ex_exactly_of_type(*(v.begin()),color));
        return _ex0();
    }
    exvector v1=v;
    ex last_element=v1.back();
    GINAC_ASSERT(is_ex_exactly_of_type(last_element,color));
    GINAC_ASSERT(ex_to_color(last_element).type==color::color_T);
    v1.pop_back();
    ex next_to_last_element=v1.back();
    GINAC_ASSERT(is_ex_exactly_of_type(next_to_last_element,color));
    GINAC_ASSERT(ex_to_color(next_to_last_element).type==color::color_T);
    v1.pop_back();
    exvector v2=v1;

    const ex & last_index=ex_to_color(last_element).seq[0];
    const ex & next_to_last_index=ex_to_color(next_to_last_element).seq[0];
    ex summation_index=coloridx();

    v2.push_back(color_T(summation_index)); // don't care about the representation_label
    
    // FIXME: check this formula for SU(N) with N!=3
    return numeric(1)/numeric(2*COLOR_THREE)*color_delta8(next_to_last_index,last_index)
           % color_trace_of_one_representation_label(v1)
          +numeric(1)/numeric(2)*color_h(next_to_last_index,last_index,summation_index)
           % color_trace_of_one_representation_label(v2);
    /*
    ex term1=numeric(1)/numeric(2*COLOR_THREE)*color_delta8(next_to_last_index,last_index)
           % color_trace_of_one_representation_label(v1);
    cout << "term 1 of trace of " << v.size() << " ts=" << term1 << endl;
    ex term2=numeric(1)/numeric(2)*color_h(next_to_last_index,last_index,summation_index)
           % color_trace_of_one_representation_label(v2);
    cout << "term 2 of trace of " << v.size() << " ts=" << term2 << endl;
    return term1+term2;
    */
}

ex color_trace(const exvector & v, unsigned rl)
{
    GINAC_ASSERT(rl<MAX_REPRESENTATION_LABELS);
    
    exvector v_rest;
    v_rest.reserve(v.size()+1); // max size if trace is empty
    
    exvector delta8vec;
    exvector fvec;
    exvector dvec;
    exvectorvector Tvecs;
    Tvecs.resize(MAX_REPRESENTATION_LABELS);
    exvectorvector ONEvecs;
    ONEvecs.resize(MAX_REPRESENTATION_LABELS);
    exvector unknownvec;

    split_color_string_in_parts(v,delta8vec,fvec,dvec,Tvecs,ONEvecs,unknownvec);

    if (unknownvec.size()!=0) {
        throw(std::invalid_argument("color_trace(): expression must be expanded"));
    }

    append_exvector_to_exvector(v_rest,delta8vec);
    append_exvector_to_exvector(v_rest,fvec);
    append_exvector_to_exvector(v_rest,dvec);
    for (unsigned i=0; i<MAX_REPRESENTATION_LABELS; ++i) {
        if (i!=rl) {
            append_exvector_to_exvector(v_rest,Tvecs[i]);
            append_exvector_to_exvector(v_rest,ONEvecs[i]);
        } else {
            if (Tvecs[i].size()!=0) {
                v_rest.push_back(color_trace_of_one_representation_label(Tvecs[i]));
            } else if (ONEvecs[i].size()!=0) {
                v_rest.push_back(numeric(COLOR_THREE));
            } else {
                throw(std::logic_error("color_trace(): representation_label not in color string"));
            }
        }
    }

    return nonsimplified_ncmul(v_rest);
}

ex simplify_pure_color_string(const ex & e)
{
    GINAC_ASSERT(is_ex_exactly_of_type(e,ncmul));

    exvector delta8vec;
    exvector fvec;
    exvector dvec;
    exvectorvector Tvecs;
    Tvecs.resize(MAX_REPRESENTATION_LABELS);
    exvectorvector ONEvecs;
    ONEvecs.resize(MAX_REPRESENTATION_LABELS);
    exvector unknownvec;

    split_color_string_in_parts(ex_to_ncmul(e).get_factors(),delta8vec,fvec,dvec,Tvecs,ONEvecs,unknownvec);

    // search for T_k S T_k (=1/2 Tr(S) - 1/6 S)
    for (unsigned rl=0; rl<MAX_REPRESENTATION_LABELS; ++rl) {
        if (Tvecs[rl].size()>=2) {
            for (unsigned i=0; i<Tvecs[rl].size()-1; ++i) {
                for (unsigned j=i+1; j<Tvecs[rl].size(); ++j) {
                    ex & t1=Tvecs[rl][i];
                    ex & t2=Tvecs[rl][j];
                    GINAC_ASSERT(is_ex_exactly_of_type(t1,color)&&
                           (ex_to_color(t1).type==color::color_T)&&
                           (ex_to_color(t1).seq.size()==1));
                    GINAC_ASSERT(is_ex_exactly_of_type(t2,color)&&
                           (ex_to_color(t2).type==color::color_T)&&
                           (ex_to_color(t2).seq.size()==1));
                    const coloridx & idx1=ex_to_coloridx(ex_to_color(t1).seq[0]);
                    const coloridx & idx2=ex_to_coloridx(ex_to_color(t2).seq[0]);
                    
                    if (idx1.is_equal(idx2) && idx1.is_symbolic()) {
                        exvector S;
                        for (unsigned k=i+1; k<j; ++k) {
                            S.push_back(Tvecs[rl][k]);
                        }
                        t1=_ex1();
                        t2=_ex1();
                        ex term1=numeric(-1)/numeric(6)*nonsimplified_ncmul(recombine_color_string(
                                 delta8vec,fvec,dvec,Tvecs,ONEvecs,unknownvec));
                        for (unsigned k=i+1; k<j; ++k) {
                            S.push_back(_ex1());
                        }
                        t1=color_trace_of_one_representation_label(S);
                        ex term2=numeric(1)/numeric(2)*nonsimplified_ncmul(recombine_color_string(
                                 delta8vec,fvec,dvec,Tvecs,ONEvecs,unknownvec));
                        return simplify_color(term1+term2);
                    }
                }
            }
        }
    }
    
    // FIXME: higher contractions
    
    return e;
}
    
ex simplify_color(const ex & e)
{
    // all simplification is done on expanded objects
    ex e_expanded=e.expand();

    // simplification of sum=sum of simplifications
    if (is_ex_exactly_of_type(e_expanded,add)) {
        ex sum=_ex0();
        for (unsigned i=0; i<e_expanded.nops(); ++i)
            sum += simplify_color(e_expanded.op(i));
        
        return sum;
    }

    // simplification of commutative product=commutative product of simplifications
    if (is_ex_exactly_of_type(e_expanded,mul)) {
        ex prod=_ex1();
        for (unsigned i=0; i<e_expanded.nops(); ++i)
            prod *= simplify_color(e_expanded.op(i));
        
        return prod;
    }

    // simplification of noncommutative product: test if everything is color
    if (is_ex_exactly_of_type(e_expanded,ncmul)) {
        bool all_color=true;
        for (unsigned i=0; i<e_expanded.nops(); ++i) {
            if (!is_ex_exactly_of_type(e_expanded.op(i),color)) {
                all_color=false;
                break;
            }
        }
        if (all_color) {
            return simplify_pure_color_string(e_expanded);
        }
    }

    // cannot do anything
    return e_expanded;
}

ex brute_force_sum_color_indices(const ex & e)
{
    exvector iv_all=e.get_indices();
    exvector iv_double;
    
    // find double symbolic indices
    if (iv_all.size()<2) return e;
    for (exvector::const_iterator cit1=iv_all.begin(); cit1!=iv_all.end()-1; ++cit1) {
        GINAC_ASSERT(is_ex_of_type(*cit1,coloridx));
        for (exvector::const_iterator cit2=cit1+1; cit2!=iv_all.end(); ++cit2) {
            GINAC_ASSERT(is_ex_of_type(*cit2,coloridx));
            if (ex_to_coloridx(*cit1).is_symbolic() && 
                ex_to_coloridx(*cit1).is_equal(ex_to_coloridx(*cit2))) {
                iv_double.push_back(*cit1);
                break;
            }
        }
    }

    vector<int> counter;
    counter.resize(iv_double.size());
    int l;
    for (l=0; unsigned(l)<iv_double.size(); ++l) {
        counter[l]=1;
    }

    ex sum=_ex0();
    
    while (1) {
        ex term=e;
        for (l=0; unsigned(l)<iv_double.size(); ++l) {
            term=term.subs(iv_double[l]==coloridx((unsigned)(counter[l])));
            //iv_double[l].print(cout);
            //cout << " " << counter[l] << " ";
        }
        //cout << endl;
        sum += term;
        
        // increment counter[]
        l=iv_double.size()-1;
        while ((l>=0)&&((++counter[l])>(int)COLOR_EIGHT)) {
            counter[l]=1;    
            l--;
        }
        if (l<2) { cout << counter[0] << counter[1] << endl; }
        if (l<0) break;
    }
    
    return sum;
}

void append_exvector_to_exvector(exvector & dest, const exvector & source)
{
    for (exvector::const_iterator cit=source.begin(); cit!=source.end(); ++cit) {
        dest.push_back(*cit);
    }
}

#ifndef NO_GINAC_NAMESPACE
} // namespace GiNaC
#endif // ndef NO_GINAC_NAMESPACE

