# Create files containing dummies, wrappers and so on to overcome certain
# deficiencies in Cint.  In an ideal world it would be unnecessary.

$header='dummies.h';
$source='dummies.cpp';

# Generate a header file which is additionally included in cint
# to work around the broken overloading resolution of cint for
# C library functions and other problems.
# E.g: if a function declared as ex sin(ex const & x) and called
# with sin(y) where y is a symbol, cint favours a conversion from
# symbol to void * to double over symbol to ex and thus calls the
# C math library function double sin(double x) (sigh!)

# types which need help to be converted to ex
@types=('symbol','function','constant','idx','lorentzidx','coloridx');
@moretypes=('numeric','int','double');
@extype=('ex');

# C math library functions with one parameter and return type 'function'
@cfunc_1p_function=('sin','cos','tan','asin','acos','atan','exp','log',
                    'sinh','cosh','tanh','abs');

# C math library functions with one parameter and return type 'ex'
@cfunc_1p_ex=('sqrt');

# C math library functions with two parameters and return type 'function'
@cfunc_2p_function=('atan2');

# C math library functions with two parameters and return type 'ex'
@cfunc_2p_ex=('pow');

@binops_ex=('+','-','*','/','%');
@binops_relational=('==','!=','<','<=','>','>=');
@binops_exconstref=('+=','-=','*=','/=');

open OUT,">$header";

$opening=<<END_OF_OPENING;
/*  dummies.h
 *
 *  Dummies and wrappers to overcome certain deficiencies of Cint.
 *  This file was generated automatically by dummies.pl.
 *  Please do not modify it directly, edit the perl script instead!
 */

END_OF_OPENING

print OUT $opening;

sub inline_cfunc_1p {
    my ($rettype,$funcsref)=@_;
    foreach $f (@$funcsref) {
        print OUT "// fixes for $rettype $f(x)\n";
        foreach $t (@types) {
            print OUT "inline $rettype $f($t const & x) { return $f(ex(x)); }\n";
        }
        print OUT "\n";
    }
}  

inline_cfunc_1p('function',\@cfunc_1p_function);
inline_cfunc_1p('ex',\@cfunc_1p_ex);

sub inline_single_cfunc_2p {
    my ($rettype,$types1ref,$types2ref)=@_;
    foreach $t1 (@$types1ref) {
        foreach $t2 (@$types2ref) {
            print OUT "inline $rettype $f($t1 const & x,$t2 const & y) {\n";
            print OUT "    return $f(ex(x),ex(y));\n";
            print OUT "}\n";
        }
    }
}

sub inline_cfunc_2p {
    my ($rettype,$funcsref)=@_;
    foreach $f (@$funcsref) {
        print OUT "// fixes for $rettype $f(x,y)\n";
        inline_single_cfunc_2p($rettype,\@types,\@types);
        inline_single_cfunc_2p($rettype,\@types,\@moretypes);
        inline_single_cfunc_2p($rettype,\@moretypes,\@types);
        inline_single_cfunc_2p($rettype,\@extype,\@moretypes);
        inline_single_cfunc_2p($rettype,\@moretypes,\@extype);
        print OUT "\n";
    }
}  

inline_cfunc_2p('function',\@cfunc_2p_function);
inline_cfunc_2p('ex',\@cfunc_2p_ex);

sub inline_function_1p {
    my ($rettype,$func)=@_;
    print OUT "inline $rettype $func(basic const & x) {\n";
    print OUT "    return $func(ex(x));\n";
    print OUT "}\n";
}

sub inline_single_function_2p {
    my ($rettype,$func,$t1,$cast1,$t2,$cast2)=@_;
    print OUT "inline $rettype $func($t1 x, $t2 y) {\n";
    print OUT "    return $func($cast1(x),$cast2(y));\n";
    print OUT "}\n";
}

sub inline_single_function_2p_with_defarg {
    my ($rettype,$func,$t1,$defarg)=@_;
    print OUT "inline $rettype $func(basic const & x, $t1 y=$defarg) {\n";
    print OUT "    return $func(ex(x),y);\n";
    print OUT "}\n";
}

sub inline_single_function_3p {
    my ($rettype,$func,$t1,$cast1,$t2,$cast2,$t3,$cast3)=@_;
    print OUT "inline $rettype $func($t1 x, $t2 y, $t3 z) {\n";
    print OUT "    return $func($cast1(x),$cast2(y),$cast3(z));\n";
    print OUT "}\n";
}

sub inline_single_function_3p_with_defarg {
    my ($rettype,$func,$t1,$cast1,$t2,$cast2,$t3,$defarg)=@_;
    print OUT "inline $rettype $func($t1 x, $t2 y, $t3 z=$defarg) {\n";
    print OUT "    return $func($cast1(x),$cast2(y),z);\n";
    print OUT "}\n";
}

sub inline_single_function_4p_with_defarg {
    my ($rettype,$func,$t1,$cast1,$t2,$cast2,$t3,$cast3,$t4,$defarg)=@_;
    print OUT "inline $rettype $func($t1 x, $t2 y, $t3 z, $t4 zz=$defarg) {\n";
    print OUT "    return $func($cast1(x),$cast2(y),$cast3(z),zz);\n";
    print OUT "}\n";
}

sub inline_single_binop {
    my ($rettype,$op,$t1,$cast1,$t2,$cast2)=@_;
    inline_single_function_2p($rettype,'operator'.$op,$t1,$cast1,$t2,$cast2);
}

sub inline_single_unaryop {
    my ($rettype,$op)=@_;
    print OUT "inline $rettype operator$op(basic const & x) {\n";
    print OUT "    return operator$op(ex(x));\n";
    print OUT "}\n";
}

sub inline_function_2p {
    my ($rettype,$func)=@_;
    print OUT "// fixes for $rettype $func(x,y)\n";
    inline_single_function_2p($rettype,$func,'ex const &','','basic const &','ex');
    inline_single_function_2p($rettype,$func,'basic const &','ex','ex const &','');
    inline_single_function_2p($rettype,$func,'basic const &','ex','basic const &','ex');
    print OUT "\n";
}

sub inline_binops {
    my ($rettype,$opsref)=@_;
    foreach $op (@$opsref) {
        inline_function_2p($rettype,'operator'.$op);
    }
}

inline_binops('ex',\@binops_ex);
inline_binops('relational',\@binops_relational);
foreach $op (@binops_exconstref) {
    print OUT "// fixes for ex const & operator$op(x,y)\n";
    inline_single_binop('ex const &',$op,'ex &','','basic const &','ex');
}

print OUT "// fixes for other operators\n";
inline_single_unaryop('ex','+');
inline_single_unaryop('ex','-');
print OUT "inline ostream & operator<<(ostream & os, basic const & x) {\n";
print OUT "    return operator<<(os,ex(x));\n";
print OUT "}\n";

print OUT "// fixes for functions\n";
inline_function_2p('bool','are_ex_trivially_equal');
inline_function_1p('unsigned','nops');
inline_function_1p('ex','expand');
inline_function_2p('bool','has');
inline_single_function_2p('int','degree','basic const &','ex','symbol const &','');
inline_single_function_2p('int','ldegree','basic const &','ex','symbol const &','');
inline_single_function_3p_with_defarg('ex','coeff','basic const &','ex','symbol const &','','int','1');
inline_function_1p('ex','numer');
inline_function_1p('ex','denom');
inline_single_function_2p_with_defarg('ex','normal','int','0');
inline_single_function_2p('ex','collect','basic const &','ex','symbol const &','');
inline_single_function_2p_with_defarg('ex','eval','int','0');
inline_single_function_2p_with_defarg('ex','evalf','int','0');
inline_single_function_3p_with_defarg('ex','diff','basic const &','ex','symbol const &','','int','1');
inline_single_function_3p('ex','series','const basic &','ex','const relational &','ex','int','');
inline_single_function_3p('ex','series','const basic &','ex','const symbol &','ex','int','');
inline_function_2p('ex','subs');
inline_single_function_3p('ex','subs','basic const &','ex','lst const &','','lst const &','');
inline_single_function_2p('ex','op','basic const &','ex','int','');
inline_function_1p('ex','lhs');
inline_function_1p('ex','rhs');
inline_function_1p('bool','is_zero');

# fixes for simp_lor.h
inline_function_2p('simp_lor','lor_g');
inline_single_function_2p('simp_lor','lor_vec','const string &','','const basic &','ex');

close OUT;

# Create a file containing stubs that may be necessary because Cint always
# wants to link against anything that was ever declared:

open OUT,">$source";

$opening=<<END_OF_OPENING;
/*  dummies.cpp
 *
 *  Dummies and stubs to overcome certain deficiencies of Cint.
 *  This file was generated automatically by dummies.pl.
 *  Please do not modify it directly, edit the perl script instead!
 */

#include <ginac/function.h>

#ifndef NO_NAMESPACE_GINAC
using namespace GiNaC;
#endif // ndef NO_NAMESPACE_GINAC

END_OF_OPENING

print OUT $opening;
print OUT "void ginsh_get_ginac_functions(void) { }\n";

close OUT;

# Create dummies 
