if (($#ARGV!=0) and ($#ARGV!=1)) {
	die 'usage: container.pl type [maxargs] (type=lst or exprseq)';
}

if ($ARGV[0] eq 'lst') {
	$type='lst';
} elsif ($ARGV[0] eq 'exprseq') {
	$type='exprseq';
} else {
	die 'only lst and exprseq supported';
}

if ($#ARGV==1) {
	$maxargs=$ARGV[1];
} else {
	$maxargs=16; # must be greater or equal than the value used in function.pl
}

if ($type eq 'exprseq') {

	# settings for exprseq
	$CONTAINER="exprseq";
	$STLHEADER="vector";
	$reserve=1;
	$prepend=0;
	$sort=0;
	$let_op=0;
	$open_bracket='(';
	$close_bracket=')';
	
} elsif ($type eq 'lst') {
 
	# settings for lst
	$CONTAINER="lst";
	$STLHEADER="list";
	$reserve=0;
	$prepend=1;
	$sort=1;
	$let_op=1;
	$open_bracket='{';
	$close_bracket='}';

} else {
	die "invalid type $type";
}

$CONTAINER_UC=uc(${CONTAINER});
$STLT="ex".$STLHEADER;

if ($reserve) {
	$RESERVE_IMPLEMENTATION="#define RESERVE(s,size) (s).reserve(size)";
} else {
	$RESERVE_IMPLEMENTATION="#define RESERVE(s,size) // no reserve needed for ${STLHEADER}";
}

if ($prepend) {
	$PREPEND_INTERFACE=<<END_OF_PREPEND_INTERFACE;
	virtual ${CONTAINER} & prepend(const ex & b);
	virtual ${CONTAINER} & remove_first(void);
END_OF_PREPEND_INTERFACE

	$PREPEND_IMPLEMENTATION=<<END_OF_PREPEND_IMPLEMENTATION;
${CONTAINER} & ${CONTAINER}::prepend(const ex & b)
{
	ensure_if_modifiable();
	seq.push_front(b);
	return *this;
}

${CONTAINER} & ${CONTAINER}::remove_first(void)
{
	ensure_if_modifiable();
	seq.pop_front();
	return *this;
}
END_OF_PREPEND_IMPLEMENTATION
} else {
	$PREPEND_INTERFACE="    // no prepend possible for ${CONTAINER}";
	$PREPEND_IMPLEMENTATION="";
}

if ($sort) {
	$SORT_INTERFACE=<<END_OF_SORT_INTERFACE;
	virtual ${CONTAINER} & sort(void);
	virtual ${CONTAINER} & unique(void);
END_OF_SORT_INTERFACE

	$SORT_IMPLEMENTATION=<<END_OF_SORT_IMPLEMENTATION;
${CONTAINER} & ${CONTAINER}::sort(void)
{
	ensure_if_modifiable();
	seq.sort(ex_is_less());
	return *this;
}

${CONTAINER} & ${CONTAINER}::unique(void)
{
	ensure_if_modifiable();
	seq.unique(ex_is_equal());
	return *this;
}
END_OF_SORT_IMPLEMENTATION
} else {
	$SORT_INTERFACE="    // no sort possible for ${CONTAINER}";
	$SORT_IMPLEMENTATION="";
}

if ($let_op) {
	$LET_OP_IMPLEMENTATION=<<END_OF_LET_OP_IMPLEMENTATION
ex & ${CONTAINER}::let_op(int i)
{
	GINAC_ASSERT(i>=0);
	GINAC_ASSERT(i<nops());

	${STLT}::iterator it=seq.begin();
	for (int j=0; j<i; j++) {
		++it;
	}
	return *it;
}
END_OF_LET_OP_IMPLEMENTATION
} else {
	$LET_OP_IMPLEMENTATION="// ${CONTAINER}::let_op() will be implemented by user elsewhere";
}

sub generate_seq {
	my ($seq_template,$n,$separator)=@_;
	my ($res,$N);
	
	$res='';
	for ($N=1; $N<=$n; $N++) {
		$res .= eval('"' . $seq_template . '"');
		if ($N!=$n) {
			$res .= $separator;
		}
	}
	return $res;
}

sub generate_from_to {
	my ($template,$seq_template1,$seq_separator1,$seq_template2,
	    $seq_separator2,$from,$to)=@_;
	my ($res,$N,$SEQ);

	$res='';
	for ($N=$from; $N<=$to; $N++) {
		$SEQ1=generate_seq($seq_template1,$N,$seq_separator1);
		$SEQ2=generate_seq($seq_template2,$N,$seq_separator2);
		$res .= eval('"' . $template . '"');
		$SEQ1=''; # to avoid main::SEQ1 used only once warning
		$SEQ2=''; # same as above
	}
	return $res;
}

sub generate {
	my ($template,$seq_template1,$seq_separator1,$seq_template2,
	    $seq_separator2)=@_;
	return generate_from_to($template,$seq_template1,$seq_separator1,
							$seq_template2,$seq_separator2,1,$maxargs);
}

$constructors_interface=generate(
'	explicit ${CONTAINER}(${SEQ1});'."\n",
'const ex & param${N}',', ','','');

$constructors_implementation=generate(
	<<'END_OF_CONSTRUCTORS_IMPLEMENTATION','const ex & param${N}',', ','	seq.push_back(param${N});',"\n");
${CONTAINER}::${CONTAINER}(${SEQ1}) : basic(TINFO_${CONTAINER})
{
	RESERVE(seq,${N});
${SEQ2}
}
END_OF_CONSTRUCTORS_IMPLEMENTATION

$interface=<<END_OF_INTERFACE;
/** \@file ${CONTAINER}.h
 *
 *  Definition of GiNaC's ${CONTAINER}. */

/*
 *  This file was generated automatically by container.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  container.pl options: \$CONTAINER=${CONTAINER}
 *                        \$STLHEADER=${STLHEADER}
 *                        \$reserve=${reserve}
 *                        \$prepend=${prepend}
 *                        \$sort=${sort}
 *                        \$let_op=${let_op}
 *                        \$open_bracket=${open_bracket}
 *                        \$close_bracket=${close_bracket}
 *                        \$maxargs=${maxargs}
 *
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

#ifndef __GINAC_${CONTAINER_UC}_H__
#define __GINAC_${CONTAINER_UC}_H__

#include <${STLHEADER}>

// CINT needs <algorithm> to work properly with <vector> and <list> 
#include <algorithm>

#include "basic.h"
#include "ex.h"

namespace GiNaC {


typedef std::${STLHEADER}<ex> ${STLT};

class ${CONTAINER} : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(${CONTAINER}, basic)

public:
	${CONTAINER}(${STLT} const & s, bool discardable = false);
	${CONTAINER}(${STLT} * vp); // vp will be deleted
${constructors_interface}

public:
	void print(const print_context & c, unsigned level = 0) const;
	unsigned precedence(void) const {return 10;}
	bool info(unsigned inf) const;
	unsigned nops() const;
	ex & let_op(int i);
	ex map(map_function & f) const;
	ex eval(int level=0) const;
	ex subs(const lst & ls, const lst & lr, bool no_pattern = false) const;
protected:
	bool is_equal_same_type(const basic & other) const;
	unsigned return_type(void) const;

	// new virtual functions which can be overridden by derived classes
public:
	virtual ${CONTAINER} & append(const ex & b);
	virtual ${CONTAINER} & remove_last(void);
${PREPEND_INTERFACE}
${SORT_INTERFACE}
protected:
	virtual void printseq(const print_context & c, char openbracket, char delim,
	                      char closebracket, unsigned this_precedence,
	                      unsigned upper_precedence = 0) const;
	virtual ex this${CONTAINER}(${STLT} const & v) const;
	virtual ex this${CONTAINER}(${STLT} * vp) const;

protected:
	bool is_canonical() const;
	${STLT} evalchildren(int level) const;
	${STLT} * subschildren(const lst & ls, const lst & lr, bool no_pattern = false) const;

protected:
	${STLT} seq;
};

// utility functions

/** Specialization of is_exactly_a<${CONTAINER}>(obj) for ${CONTAINER} objects. */
template<> inline bool is_exactly_a<${CONTAINER}>(const basic & obj)
{
	return obj.tinfo()==TINFO_${CONTAINER};
}

} // namespace GiNaC

#endif // ndef __GINAC_${CONTAINER_UC}_H__

END_OF_INTERFACE

$implementation=<<END_OF_IMPLEMENTATION;
/** \@file ${CONTAINER}.cpp
 *
 *  Implementation of GiNaC's ${CONTAINER}. */

/*
 *  This file was generated automatically by container.pl.
 *  Please do not modify it directly, edit the perl script instead!
 *  container.pl options: \$CONTAINER=${CONTAINER}
 *                        \$STLHEADER=${STLHEADER}
 *                        \$reserve=${reserve}
 *                        \$prepend=${prepend}
 *                        \$let_op=${let_op}
 *                        \$open_bracket=${open_bracket}
 *                        \$close_bracket=${close_bracket}
 *                        \$maxargs=${maxargs}
 *
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

#include <iostream>
#include <stdexcept>

#include "${CONTAINER}.h"
#include "ex.h"
#include "print.h"
#include "archive.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(${CONTAINER}, basic)

${RESERVE_IMPLEMENTATION}

//////////
// default ctor, dtor, copy ctor, assignment operator and helpers
//////////

// public

${CONTAINER}::${CONTAINER}() : basic(TINFO_${CONTAINER}) {}

// protected

void ${CONTAINER}::copy(${CONTAINER} const & other)
{
	inherited::copy(other);
	seq=other.seq;
}

void ${CONTAINER}::destroy(bool call_parent)
{
	seq.clear();
	if (call_parent) inherited::destroy(call_parent);
}

//////////
// other ctors
//////////

// public

${CONTAINER}::${CONTAINER}(${STLT} const & s, bool discardable) :  basic(TINFO_${CONTAINER})
{
	if (discardable) {
		seq.swap(const_cast<${STLT} &>(s));
	} else {
		seq=s;
	}
}

${CONTAINER}::${CONTAINER}(${STLT} * vp) : basic(TINFO_${CONTAINER})
{
	GINAC_ASSERT(vp!=0);
	seq.swap(*vp);
	delete vp;
}

${constructors_implementation}

//////////
// archiving
//////////

/** Construct object from archive_node. */
${CONTAINER}::${CONTAINER}(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	for (unsigned int i=0; true; i++) {
		ex e;
		if (n.find_ex("seq", e, sym_lst, i))
			seq.push_back(e);
		else
			break;
	}
}

/** Unarchive the object. */
ex ${CONTAINER}::unarchive(const archive_node &n, const lst &sym_lst)
{
	return (new ${CONTAINER}(n, sym_lst))->setflag(status_flags::dynallocated);
}

/** Archive the object. */
void ${CONTAINER}::archive(archive_node &n) const
{
	inherited::archive(n);
	${STLT}::const_iterator i = seq.begin(), end = seq.end();
	while (i != end) {
		n.add_ex("seq", *i);
		++i;
	}
}

//////////
// functions overriding virtual functions from base classes
//////////

// public

void ${CONTAINER}::print(const print_context & c, unsigned level) const
{
	if (is_a<print_tree>(c)) {

		c.s << std::string(level, ' ') << class_name()
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << ", nops=" << nops()
		    << std::endl;
		unsigned delta_indent = static_cast<const print_tree &>(c).delta_indent;
		${STLT}::const_iterator i = seq.begin(), end = seq.end();
		while (i != end) {
			i->print(c, level + delta_indent);
			++i;
		}
		c.s << std::string(level + delta_indent,' ') << "=====" << std::endl;
	} else if (is_a<print_python>(c)) {
		printseq(c, '[', ',', ']', precedence(), precedence()+1);
	} else if (is_a<print_python_repr>(c)) {
		c.s << class_name ();
		printseq(c, '(', ',', ')', precedence(), precedence()+1);
	} else {
		// always print brackets around seq, ignore upper_precedence
		printseq(c, '${open_bracket}', ',', '${close_bracket}', precedence(), precedence()+1);
	}
}

// ${CONTAINER}::info() will be implemented by user elsewhere";

unsigned ${CONTAINER}::nops() const
{
	return seq.size();
}

${LET_OP_IMPLEMENTATION}

ex ${CONTAINER}::map(map_function & f) const
{
	// This implementation is here because basic::map() uses let_op()
	// which is not defined for all containers
	${STLT} s;
	RESERVE(s,seq.size());
	${STLT}::const_iterator i = seq.begin(), end = seq.end();
	while (i != end) {
		s.push_back(f(*i));
		++i;
	}

	return this${CONTAINER}(s);
}

ex ${CONTAINER}::eval(int level) const
{
	if (level==1) {
		return this->hold();
	}
	return this${CONTAINER}(evalchildren(level));
}

ex ${CONTAINER}::subs(const lst & ls, const lst & lr, bool no_pattern) const
{
	${STLT} *vp = subschildren(ls, lr, no_pattern);
	if (vp)
		return ex_to<basic>(this${CONTAINER}(vp)).basic::subs(ls, lr, no_pattern);
	else
		return basic::subs(ls, lr, no_pattern);
}

// protected

int ${CONTAINER}::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_a<${CONTAINER}>(other));
	${CONTAINER} const & o = static_cast<const ${CONTAINER} &>(other);

	${STLT}::const_iterator it1 = seq.begin(), it1end = seq.end(),
	                        it2 = o.seq.begin(), it2end = o.seq.end();

	while (it1 != it1end && it2 != it2end) {
		int cmpval = it1->compare(*it2);
		if (cmpval)
			return cmpval;
		++it1; ++it2;
	}

	return (it1 == it1end) ? (it2 == it2end ? 0 : -1) : 1;
}

bool ${CONTAINER}::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_a<${CONTAINER}>(other));
	${CONTAINER} const &o = static_cast<const ${CONTAINER} &>(other);

	if (seq.size() != o.seq.size())
		return false;

	${STLT}::const_iterator it1 = seq.begin(), it1end = seq.end(),
	                        it2 = o.seq.begin();

	while (it1 != it1end) {
		if (!it1->is_equal(*it2))
			return false;
		++it1; ++it2;
	}

	return true;
}

unsigned ${CONTAINER}::return_type(void) const
{
	return return_types::noncommutative_composite;
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

// public

${CONTAINER} & ${CONTAINER}::append(const ex & b)
{
	ensure_if_modifiable();
	seq.push_back(b);
	return *this;
}

${CONTAINER} & ${CONTAINER}::remove_last(void)
{
	ensure_if_modifiable();
	seq.pop_back();
	return *this;
}

${PREPEND_IMPLEMENTATION}

${SORT_IMPLEMENTATION}

// protected

void ${CONTAINER}::printseq(const print_context & c, char openbracket, char delim,
                            char closebracket, unsigned this_precedence,
                            unsigned upper_precedence) const
{
	if (this_precedence <= upper_precedence)
		c.s << openbracket;

	if (!seq.empty()) {
		${STLT}::const_iterator it = seq.begin(), itend = seq.end();
		--itend;
		while (it != itend) {
			it->print(c, this_precedence);
			c.s << delim;
			++it;
		}
		it->print(c, this_precedence);
	}

	if (this_precedence <= upper_precedence)
		c.s << closebracket;
}

ex ${CONTAINER}::this${CONTAINER}(${STLT} const & v) const
{
	return ${CONTAINER}(v);
}

ex ${CONTAINER}::this${CONTAINER}(${STLT} * vp) const
{
	return ${CONTAINER}(vp);
}

//////////
// non-virtual functions in this class
//////////

// public

// none

// protected

bool ${CONTAINER}::is_canonical() const
{
	if (seq.size()<=1) { return 1; }

	${STLT}::const_iterator it = seq.begin(), itend = seq.end();
	${STLT}::const_iterator it_last=it;
	for (++it; it!=itend; it_last=it, ++it) {
		if (it_last->compare(*it)>0) {
			if (it_last->compare(*it)>0) {
				std::cout << *it_last << ">" << *it << "\\n";
				return 0;
			}
		}
	}
	return 1;
}


${STLT} ${CONTAINER}::evalchildren(int level) const
{
	${STLT} s;
	RESERVE(s,seq.size());

	if (level==1) {
		return seq;
	}
	if (level == -max_recursion_level) {
		throw(std::runtime_error("max recursion level reached"));
	}
	--level;
	${STLT}::const_iterator it = seq.begin(), itend = seq.end();
	while (it != itend) {
		s.push_back(it->eval(level));
		++it;
	}
	return s;
}

${STLT} * ${CONTAINER}::subschildren(const lst & ls, const lst & lr, bool no_pattern) const
{
	// returns a NULL pointer if nothing had to be substituted
	// returns a pointer to a newly created epvector otherwise
	// (which has to be deleted somewhere else)

	${STLT}::const_iterator cit = seq.begin(), end = seq.end();
	while (cit != end) {
		const ex & subsed_ex = cit->subs(ls, lr, no_pattern);
		if (!are_ex_trivially_equal(*cit, subsed_ex)) {

			// something changed, copy seq, subs and return it
			${STLT} *s=new ${STLT};
			RESERVE(*s, seq.size());

			// copy parts of seq which are known not to have changed
			${STLT}::const_iterator cit2 = seq.begin();
			while (cit2 != cit) {
				s->push_back(*cit2);
				++cit2;
			}

			// copy first changed element
			s->push_back(subsed_ex);
			++cit2;

			// copy rest
			while (cit2 != end) {
				s->push_back(cit2->subs(ls, lr, no_pattern));
				++cit2;
			}
			return s;
		}
		++cit;
	}
	
	return 0; // nothing has changed
}

} // namespace GiNaC

END_OF_IMPLEMENTATION

print "Creating interface file ${CONTAINER}.h...";
open OUT,">${CONTAINER}.h" or die "cannot open ${CONTAINER}.h";
print OUT $interface;
close OUT;
print "ok.\n";

print "Creating implementation file ${CONTAINER}.cpp...";
open OUT,">${CONTAINER}.cpp" or die "cannot open ${CONTAINER}.cpp";
print OUT $implementation;
close OUT;
print "ok.\n";

print "done.\n";
