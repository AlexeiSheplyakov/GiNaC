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
	$maxargs=15; # must be greater or equal than the value used in function.pl
}

if ($type eq 'exprseq') {

	# settings for exprseq
	$CONTAINER="exprseq";
	$STLHEADER="vector";
	$reserve=1;
	$prepend=0;
	$let_op=0;
	$open_bracket='(';
	$close_bracket=')';
	
} elsif ($type eq 'lst') {
 
	# settings for lst
	$CONTAINER="lst";
	$STLHEADER="list";
	$reserve=0;
	$prepend=1;
	$let_op=1;
	$open_bracket='[';
	$close_bracket=']';

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
END_OF_PREPEND_INTERFACE

	$PREPEND_IMPLEMENTATION=<<END_OF_PREPEND_IMPLEMENTATION;
${CONTAINER} & ${CONTAINER}::prepend(const ex & b)
{
	ensure_if_modifiable();
	seq.push_front(b);
	return *this;
}
END_OF_PREPEND_IMPLEMENTATION
} else {
	$PREPEND_INTERFACE="    // no prepend possible for ${CONTAINER}";
	$PREPEND_IMPLEMENTATION="";
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
'    explicit ${CONTAINER}(${SEQ1});'."\n",
'const ex & param${N}',', ','','');

$constructors_implementation=generate(
	<<'END_OF_CONSTRUCTORS_IMPLEMENTATION','const ex & param${N}',', ','    seq.push_back(param${N});',"\n");
${CONTAINER}::${CONTAINER}(${SEQ1}) : basic(TINFO_${CONTAINER})
{
	debugmsg(\"${CONTAINER} constructor from ${N}*ex\",LOGLEVEL_CONSTRUCT);
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
 *                        \$let_op=${let_op}
 *                        \$open_bracket=${open_bracket}
 *                        \$close_bracket=${close_bracket}
 *                        \$maxargs=${maxargs}
 *
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

#ifndef __GINAC_${CONTAINER_UC}_H__
#define __GINAC_${CONTAINER_UC}_H__

#include <${STLHEADER}>

// CINT needs <algorithm> to work properly with <vector> and <list> 
#include <algorithm>

#include "basic.h"
#include "ex.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

// typedef std::${STLHEADER}<ex> ${STLT};
typedef std::${STLHEADER}<ex,malloc_alloc> ${STLT}; // CINT does not like ${STLHEADER}<...,default_alloc>

class ${CONTAINER} : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(${CONTAINER}, basic)

public:
	${CONTAINER}();
	~${CONTAINER}();
	${CONTAINER}(${CONTAINER} const & other);
	${CONTAINER} const & operator=(${CONTAINER} const & other);
protected:
	void copy(${CONTAINER} const & other);
	void destroy(bool call_parent);

public:
	${CONTAINER}(${STLT} const & s, bool discardable=0);
	${CONTAINER}(${STLT} * vp); // vp will be deleted
${constructors_interface}

public:
	basic * duplicate() const;
	void printraw(std::ostream & os) const;
	void print(std::ostream & os, unsigned upper_precedence=0) const;
	void printtree(std::ostream & os, unsigned indent) const;
	bool info(unsigned inf) const;
	unsigned nops() const;
	ex & let_op(int i);
	ex expand(unsigned options=0) const;
	bool has(const ex & other) const;
	ex eval(int level=0) const;
	ex evalf(int level=0) const;
	ex normal(lst &sym_lst, lst &repl_lst, int level=0) const;
	ex derivative(const symbol & s) const;
	ex subs(const lst & ls, const lst & lr) const;
protected:
	int compare_same_type(const basic & other) const;
	bool is_equal_same_type(const basic & other) const;
	unsigned return_type(void) const;

	// new virtual functions which can be overridden by derived classes
public:
	virtual ${CONTAINER} & append(const ex & b);
${PREPEND_INTERFACE}
protected:
	virtual void printseq(std::ostream & os, char openbracket, char delim,
	                      char closebracket, unsigned this_precedence,
	                      unsigned upper_precedence=0) const;
	virtual ex this${CONTAINER}(${STLT} const & v) const;
	virtual ex this${CONTAINER}(${STLT} * vp) const;

protected:
	bool is_canonical() const;
	${STLT} evalchildren(int level) const;
	${STLT} evalfchildren(int level) const;
	${STLT} normalchildren(int level) const;
	${STLT} diffchildren(const symbol & s) const;
	${STLT} * subschildren(const lst & ls, const lst & lr) const;

protected:
	${STLT} seq;
	static unsigned precedence;
};

// global constants

extern const ${CONTAINER} some_${CONTAINER};
extern const type_info & typeid_${CONTAINER};

// utility functions
inline const ${CONTAINER} &ex_to_${CONTAINER}(const ex &e)
{
	return static_cast<const ${CONTAINER} &>(*e.bp);
}

inline ${CONTAINER} &ex_to_nonconst_${CONTAINER}(const ex &e)
{
	return static_cast<${CONTAINER} &>(*e.bp);
}

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

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

#include <iostream>
#include <stdexcept>

#include "${CONTAINER}.h"
#include "ex.h"
#include "archive.h"
#include "debugmsg.h"

#ifndef NO_NAMESPACE_GINAC
namespace GiNaC {
#endif // ndef NO_NAMESPACE_GINAC

GINAC_IMPLEMENT_REGISTERED_CLASS(${CONTAINER}, basic)

${RESERVE_IMPLEMENTATION}

//////////
// default constructor, destructor, copy constructor assignment operator and helpers
//////////

// public

${CONTAINER}::${CONTAINER}() : basic(TINFO_${CONTAINER})
{
	debugmsg("${CONTAINER} default constructor",LOGLEVEL_CONSTRUCT);
}

${CONTAINER}::~${CONTAINER}()
{
	debugmsg("${CONTAINER} destructor",LOGLEVEL_DESTRUCT);
	destroy(0);
}

${CONTAINER}::${CONTAINER}(${CONTAINER} const & other)
{
	debugmsg("${CONTAINER} copy constructor",LOGLEVEL_CONSTRUCT);
	copy(other);
}

${CONTAINER} const & ${CONTAINER}::operator=(${CONTAINER} const & other)
{
	debugmsg("${CONTAINER} operator=",LOGLEVEL_ASSIGNMENT);
	if (this != &other) {
		destroy(1);
		copy(other);
	}
	return *this;
}

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
// other constructors
//////////

// public

${CONTAINER}::${CONTAINER}(${STLT} const & s, bool discardable) :  basic(TINFO_${CONTAINER})
{
	debugmsg("${CONTAINER} constructor from ${STLT}", LOGLEVEL_CONSTRUCT);
	if (discardable) {
		seq.swap(const_cast<${STLT} &>(s));
	} else {
		seq=s;
	}
}

${CONTAINER}::${CONTAINER}(${STLT} * vp) : basic(TINFO_${CONTAINER})
{
	debugmsg("${CONTAINER} constructor from ${STLT} *",LOGLEVEL_CONSTRUCT);
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
	debugmsg("${CONTAINER} constructor from archive_node", LOGLEVEL_CONSTRUCT);
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
	${STLT}::const_iterator i = seq.begin(), iend = seq.end();
	while (i != iend) {
		n.add_ex("seq", *i);
		i++;
	}
}

//////////
// functions overriding virtual functions from bases classes
//////////

// public

basic * ${CONTAINER}::duplicate() const
{
	debugmsg("${CONTAINER} duplicate",LOGLEVEL_DUPLICATE);
	return new ${CONTAINER}(*this);
}

void ${CONTAINER}::printraw(std::ostream & os) const
{
	debugmsg("${CONTAINER} printraw",LOGLEVEL_PRINT);

	os << "${CONTAINER}(";
	for (${STLT}::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		(*cit).bp->printraw(os);
		os << ",";
	}
	os << ")";
}

void ${CONTAINER}::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("${CONTAINER} print",LOGLEVEL_PRINT);
	// always print brackets around seq, ignore upper_precedence
	printseq(os,'${open_bracket}',',','${close_bracket}',precedence,precedence+1);
}

void ${CONTAINER}::printtree(std::ostream & os, unsigned indent) const
{
	debugmsg("${CONTAINER} printtree",LOGLEVEL_PRINT);

	os << std::string(indent,' ') << "type=" << class_name()
	   << ", hash=" << hashvalue 
	   << " (0x" << std::hex << hashvalue << std::dec << ")"
	   << ", flags=" << flags
	   << ", nops=" << nops() << std::endl;
	for (${STLT}::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
		(*cit).printtree(os,indent+delta_indent);
	}
	os << std::string(indent+delta_indent,' ') << "=====" << std::endl;
}

// ${CONTAINER}::info() will be implemented by user elsewhere";

unsigned ${CONTAINER}::nops() const
{
	return seq.size();
}

${LET_OP_IMPLEMENTATION}

ex ${CONTAINER}::expand(unsigned options) const
{
	${STLT} s;
	RESERVE(s,seq.size());
	for (${STLT}::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
		s.push_back((*it).expand(options));
	}

	return this${CONTAINER}(s);
}

// a ${CONTAINER} 'has' an expression if it is this expression itself or a child 'has' it

bool ${CONTAINER}::has(const ex & other) const
{
	GINAC_ASSERT(other.bp!=0);
	if (is_equal(*other.bp)) return true;
	for (${STLT}::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
		if ((*it).has(other)) return true;
	}
	return false;
}

ex ${CONTAINER}::eval(int level) const
{
	if (level==1) {
		return this->hold();
	}
	return this${CONTAINER}(evalchildren(level));
}

ex ${CONTAINER}::evalf(int level) const
{
	return this${CONTAINER}(evalfchildren(level));
}

/** Implementation of ex::normal() for ${CONTAINER}s. It normalizes the arguments
 *  and replaces the ${CONTAINER} by a temporary symbol.
 *  \@see ex::normal */
ex ${CONTAINER}::normal(lst &sym_lst, lst &repl_lst, int level) const
{
	ex n=this${CONTAINER}(normalchildren(level));
	return n.bp->basic::normal(sym_lst,repl_lst,level);
}

ex ${CONTAINER}::derivative(const symbol & s) const
{
	return this${CONTAINER}(diffchildren(s));
}

ex ${CONTAINER}::subs(const lst & ls, const lst & lr) const
{
	${STLT} * vp=subschildren(ls,lr);
	if (vp==0) {
		return *this;
	}
	return this${CONTAINER}(vp);
}

// protected

int ${CONTAINER}::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other,${CONTAINER}));
	${CONTAINER} const & o=static_cast<${CONTAINER} const &>
									(const_cast<basic &>(other));
	int cmpval;
	${STLT}::const_iterator it1=seq.begin();
	${STLT}::const_iterator it2=o.seq.begin();

	for (; (it1!=seq.end())&&(it2!=o.seq.end()); ++it1, ++it2) {
		cmpval=(*it1).compare(*it2);
		if (cmpval!=0) return cmpval;
	}

	if (it1==seq.end()) {
		return (it2==o.seq.end() ? 0 : -1);
	}

	return 1;
}

bool ${CONTAINER}::is_equal_same_type(const basic & other) const
{
	GINAC_ASSERT(is_of_type(other,${CONTAINER}));
	${CONTAINER} const & o=static_cast<${CONTAINER} const &>
									(const_cast<basic &>(other));
	if (seq.size()!=o.seq.size()) return false;

	${STLT}::const_iterator it1=seq.begin();
	${STLT}::const_iterator it2=o.seq.begin();

	for (; it1!=seq.end(); ++it1, ++it2) {
		if (!(*it1).is_equal(*it2)) return false;
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

${PREPEND_IMPLEMENTATION}

// protected

void ${CONTAINER}::printseq(std::ostream & os, char openbracket, char delim,
                            char closebracket, unsigned this_precedence,
                            unsigned upper_precedence) const
{
	if (this_precedence<=upper_precedence) os << openbracket;
	if (seq.size()!=0) {
		${STLT}::const_iterator it,it_last;
		it=seq.begin();
		it_last=seq.end();
		--it_last;
		for (; it!=it_last; ++it) {
			(*it).bp->print(os,this_precedence);
			os << delim;
		}
		(*it).bp->print(os,this_precedence);
	}
	if (this_precedence<=upper_precedence) os << closebracket;
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

	${STLT}::const_iterator it=seq.begin();
	${STLT}::const_iterator it_last=it;
	for (++it; it!=seq.end(); it_last=it, ++it) {
		if ((*it_last).compare(*it)>0) {
			if ((*it_last).compare(*it)>0) {
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
	for (${STLT}::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
		s.push_back((*it).eval(level));
	}
	return s;
}

${STLT} ${CONTAINER}::evalfchildren(int level) const
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
	for (${STLT}::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
		s.push_back((*it).evalf(level));
	}
	return s;
}

${STLT} ${CONTAINER}::normalchildren(int level) const
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
	for (${STLT}::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
		s.push_back((*it).normal(level));
	}
	return s;
}

${STLT} ${CONTAINER}::diffchildren(const symbol & y) const
{
	${STLT} s;
	RESERVE(s,seq.size());
	for (${STLT}::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
		s.push_back((*it).diff(y));
	}
	return s;
}

/* obsolete subschildren
${STLT} ${CONTAINER}::subschildren(const lst & ls, const lst & lr) const
{
	${STLT} s;
	RESERVE(s,seq.size());
	for (${STLT}::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
		s.push_back((*it).subs(ls,lr));
	}
	return s;
}
*/

${STLT} * ${CONTAINER}::subschildren(const lst & ls, const lst & lr) const
{
	// returns a NULL pointer if nothing had to be substituted
	// returns a pointer to a newly created epvector otherwise
	// (which has to be deleted somewhere else)

	${STLT}::const_iterator last=seq.end();
	${STLT}::const_iterator cit=seq.begin();
	while (cit!=last) {
		const ex & subsed_ex=(*cit).subs(ls,lr);
		if (!are_ex_trivially_equal(*cit,subsed_ex)) {

			// something changed, copy seq, subs and return it
			${STLT} *s=new ${STLT};
			RESERVE(*s,seq.size());

			// copy parts of seq which are known not to have changed
			${STLT}::const_iterator cit2=seq.begin();
			while (cit2!=cit) {
				s->push_back(*cit2);
				++cit2;
			}
			// copy first changed element
			s->push_back(subsed_ex);
			++cit2;
			// copy rest
			while (cit2!=last) {
				s->push_back((*cit2).subs(ls,lr));
				++cit2;
			}
			return s;
		}
		++cit;
	}
	
	return 0; // nothing has changed
}

//////////
// static member variables
//////////

// protected

unsigned ${CONTAINER}::precedence=10;

//////////
// global constants
//////////

const ${CONTAINER} some_${CONTAINER};
const type_info & typeid_${CONTAINER}=typeid(some_${CONTAINER});

#ifndef NO_NAMESPACE_GINAC
} // namespace GiNaC
#endif // ndef NO_NAMESPACE_GINAC

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
