#!/usr/bin/perl -w

# Generate a header file which is additionally included in cint
# to work around the broken overloading resolution of cint for
# C library functions
# e.g. if a function declared as ex sin(ex const & x) and called
# with sin(y) where y is a symbol, cint favours a conversion from
# symbol to void * to double over symbol to ex and thus calls the
# C math library function double sin(double x) (sigh!)

$header='cint_workaround.h';

# types which need help to be converted to ex
@types=('symbol','function','constant','idx','lorentzidx','coloridx');
@moretypes=('numeric','int','double');
@extype=('ex');

# functions with one parameter
@functions1p=('sin','cos','tan','asin','acos','atan','exp','log','sqrt',
              'sinh','cosh','tanh','abs');

# functions with two parameters
@functions2p=('pow','atan2');

open OUT,">$header";

foreach $f (@functions1p) {
    print OUT "// fixes for $f(x)\n";
    foreach $t (@types) {
        print OUT "inline ex $f($t const & x) { return $f(ex(x)); }\n";
    }
    print OUT "\n";
}  

sub inlines2 {
    my ($types1ref,$types2ref)=@_;
    foreach $t1 (@$types1ref) {
        foreach $t2 (@$types2ref) {
            print OUT "inline ex $f($t1 const & x,$t2 const & y) {\n";
            print OUT "    return $f(ex(x),ex(y));\n";
            print OUT "}\n";
        }
    }
}

foreach $f (@functions2p) {
    print OUT "// fixes for $f(x,y)\n";
    inlines2(\@types,\@types);
    inlines2(\@types,\@moretypes);
    inlines2(\@moretypes,\@types);
    inlines2(\@extype,\@moretypes);
    inlines2(\@moretypes,\@extype);
    print OUT "\n";
}  

close OUT;


