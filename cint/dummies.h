/*  dummies.h
 *
 *  Dummies and wrappers to overcome certain deficiencies of Cint.
 *  This file was generated automatically by dummies.pl.
 *  Please do not modify it directly, edit the perl script instead!
 */

#if (!defined(G_CINTVERSION)) || (G_CINTVERSION < 501437)
// this dummy file seems to be no longer needed for cint >= 5.14.37

// fixes for function sin(x)
inline function sin(symbol const & x) { return sin(ex(x)); }
inline function sin(function const & x) { return sin(ex(x)); }
inline function sin(constant const & x) { return sin(ex(x)); }
inline function sin(idx const & x) { return sin(ex(x)); }
inline function sin(lorentzidx const & x) { return sin(ex(x)); }
inline function sin(coloridx const & x) { return sin(ex(x)); }

// fixes for function cos(x)
inline function cos(symbol const & x) { return cos(ex(x)); }
inline function cos(function const & x) { return cos(ex(x)); }
inline function cos(constant const & x) { return cos(ex(x)); }
inline function cos(idx const & x) { return cos(ex(x)); }
inline function cos(lorentzidx const & x) { return cos(ex(x)); }
inline function cos(coloridx const & x) { return cos(ex(x)); }

// fixes for function tan(x)
inline function tan(symbol const & x) { return tan(ex(x)); }
inline function tan(function const & x) { return tan(ex(x)); }
inline function tan(constant const & x) { return tan(ex(x)); }
inline function tan(idx const & x) { return tan(ex(x)); }
inline function tan(lorentzidx const & x) { return tan(ex(x)); }
inline function tan(coloridx const & x) { return tan(ex(x)); }

// fixes for function asin(x)
inline function asin(symbol const & x) { return asin(ex(x)); }
inline function asin(function const & x) { return asin(ex(x)); }
inline function asin(constant const & x) { return asin(ex(x)); }
inline function asin(idx const & x) { return asin(ex(x)); }
inline function asin(lorentzidx const & x) { return asin(ex(x)); }
inline function asin(coloridx const & x) { return asin(ex(x)); }

// fixes for function acos(x)
inline function acos(symbol const & x) { return acos(ex(x)); }
inline function acos(function const & x) { return acos(ex(x)); }
inline function acos(constant const & x) { return acos(ex(x)); }
inline function acos(idx const & x) { return acos(ex(x)); }
inline function acos(lorentzidx const & x) { return acos(ex(x)); }
inline function acos(coloridx const & x) { return acos(ex(x)); }

// fixes for function atan(x)
inline function atan(symbol const & x) { return atan(ex(x)); }
inline function atan(function const & x) { return atan(ex(x)); }
inline function atan(constant const & x) { return atan(ex(x)); }
inline function atan(idx const & x) { return atan(ex(x)); }
inline function atan(lorentzidx const & x) { return atan(ex(x)); }
inline function atan(coloridx const & x) { return atan(ex(x)); }

// fixes for function exp(x)
inline function exp(symbol const & x) { return exp(ex(x)); }
inline function exp(function const & x) { return exp(ex(x)); }
inline function exp(constant const & x) { return exp(ex(x)); }
inline function exp(idx const & x) { return exp(ex(x)); }
inline function exp(lorentzidx const & x) { return exp(ex(x)); }
inline function exp(coloridx const & x) { return exp(ex(x)); }

// fixes for function log(x)
inline function log(symbol const & x) { return log(ex(x)); }
inline function log(function const & x) { return log(ex(x)); }
inline function log(constant const & x) { return log(ex(x)); }
inline function log(idx const & x) { return log(ex(x)); }
inline function log(lorentzidx const & x) { return log(ex(x)); }
inline function log(coloridx const & x) { return log(ex(x)); }

// fixes for function sinh(x)
inline function sinh(symbol const & x) { return sinh(ex(x)); }
inline function sinh(function const & x) { return sinh(ex(x)); }
inline function sinh(constant const & x) { return sinh(ex(x)); }
inline function sinh(idx const & x) { return sinh(ex(x)); }
inline function sinh(lorentzidx const & x) { return sinh(ex(x)); }
inline function sinh(coloridx const & x) { return sinh(ex(x)); }

// fixes for function cosh(x)
inline function cosh(symbol const & x) { return cosh(ex(x)); }
inline function cosh(function const & x) { return cosh(ex(x)); }
inline function cosh(constant const & x) { return cosh(ex(x)); }
inline function cosh(idx const & x) { return cosh(ex(x)); }
inline function cosh(lorentzidx const & x) { return cosh(ex(x)); }
inline function cosh(coloridx const & x) { return cosh(ex(x)); }

// fixes for function tanh(x)
inline function tanh(symbol const & x) { return tanh(ex(x)); }
inline function tanh(function const & x) { return tanh(ex(x)); }
inline function tanh(constant const & x) { return tanh(ex(x)); }
inline function tanh(idx const & x) { return tanh(ex(x)); }
inline function tanh(lorentzidx const & x) { return tanh(ex(x)); }
inline function tanh(coloridx const & x) { return tanh(ex(x)); }

// fixes for function abs(x)
inline function abs(symbol const & x) { return abs(ex(x)); }
inline function abs(function const & x) { return abs(ex(x)); }
inline function abs(constant const & x) { return abs(ex(x)); }
inline function abs(idx const & x) { return abs(ex(x)); }
inline function abs(lorentzidx const & x) { return abs(ex(x)); }
inline function abs(coloridx const & x) { return abs(ex(x)); }

// fixes for ex sqrt(x)
inline ex sqrt(symbol const & x) { return sqrt(ex(x)); }
inline ex sqrt(function const & x) { return sqrt(ex(x)); }
inline ex sqrt(constant const & x) { return sqrt(ex(x)); }
inline ex sqrt(idx const & x) { return sqrt(ex(x)); }
inline ex sqrt(lorentzidx const & x) { return sqrt(ex(x)); }
inline ex sqrt(coloridx const & x) { return sqrt(ex(x)); }

// fixes for function atan2(x,y)
inline function atan2(symbol const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(symbol const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(symbol const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(symbol const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(symbol const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(symbol const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(function const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(function const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(function const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(function const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(function const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(function const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(constant const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(constant const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(constant const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(constant const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(constant const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(constant const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(idx const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(idx const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(idx const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(idx const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(idx const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(idx const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(lorentzidx const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(lorentzidx const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(lorentzidx const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(lorentzidx const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(lorentzidx const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(lorentzidx const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(coloridx const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(coloridx const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(coloridx const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(coloridx const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(coloridx const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(coloridx const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(symbol const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(symbol const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(symbol const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(function const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(function const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(function const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(constant const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(constant const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(constant const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(idx const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(idx const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(idx const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(lorentzidx const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(lorentzidx const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(lorentzidx const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(coloridx const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(coloridx const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(coloridx const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(numeric const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(numeric const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(numeric const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(numeric const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(numeric const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(numeric const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(int const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(int const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(int const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(int const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(int const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(int const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(double const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(double const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(double const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(double const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(double const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(double const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(ex const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(ex const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(ex const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(numeric const & x,ex const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(int const & x,ex const & y) {
    return atan2(ex(x),ex(y));
}
inline function atan2(double const & x,ex const & y) {
    return atan2(ex(x),ex(y));
}

// fixes for ex pow(x,y)
inline ex pow(symbol const & x,symbol const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(symbol const & x,function const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(symbol const & x,constant const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(symbol const & x,idx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(symbol const & x,lorentzidx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(symbol const & x,coloridx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(function const & x,symbol const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(function const & x,function const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(function const & x,constant const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(function const & x,idx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(function const & x,lorentzidx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(function const & x,coloridx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(constant const & x,symbol const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(constant const & x,function const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(constant const & x,constant const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(constant const & x,idx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(constant const & x,lorentzidx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(constant const & x,coloridx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(idx const & x,symbol const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(idx const & x,function const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(idx const & x,constant const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(idx const & x,idx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(idx const & x,lorentzidx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(idx const & x,coloridx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(lorentzidx const & x,symbol const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(lorentzidx const & x,function const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(lorentzidx const & x,constant const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(lorentzidx const & x,idx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(lorentzidx const & x,lorentzidx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(lorentzidx const & x,coloridx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(coloridx const & x,symbol const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(coloridx const & x,function const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(coloridx const & x,constant const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(coloridx const & x,idx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(coloridx const & x,lorentzidx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(coloridx const & x,coloridx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(symbol const & x,numeric const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(symbol const & x,int const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(symbol const & x,double const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(function const & x,numeric const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(function const & x,int const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(function const & x,double const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(constant const & x,numeric const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(constant const & x,int const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(constant const & x,double const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(idx const & x,numeric const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(idx const & x,int const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(idx const & x,double const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(lorentzidx const & x,numeric const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(lorentzidx const & x,int const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(lorentzidx const & x,double const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(coloridx const & x,numeric const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(coloridx const & x,int const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(coloridx const & x,double const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(numeric const & x,symbol const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(numeric const & x,function const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(numeric const & x,constant const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(numeric const & x,idx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(numeric const & x,lorentzidx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(numeric const & x,coloridx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(int const & x,symbol const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(int const & x,function const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(int const & x,constant const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(int const & x,idx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(int const & x,lorentzidx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(int const & x,coloridx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(double const & x,symbol const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(double const & x,function const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(double const & x,constant const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(double const & x,idx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(double const & x,lorentzidx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(double const & x,coloridx const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(ex const & x,numeric const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(ex const & x,int const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(ex const & x,double const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(numeric const & x,ex const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(int const & x,ex const & y) {
    return pow(ex(x),ex(y));
}
inline ex pow(double const & x,ex const & y) {
    return pow(ex(x),ex(y));
}

// fixes for ex operator+(x,y)
inline ex operator+(ex const & x, basic const & y) {
    return operator+((x),ex(y));
}
inline ex operator+(basic const & x, ex const & y) {
    return operator+(ex(x),(y));
}
inline ex operator+(basic const & x, basic const & y) {
    return operator+(ex(x),ex(y));
}

// fixes for ex operator-(x,y)
inline ex operator-(ex const & x, basic const & y) {
    return operator-((x),ex(y));
}
inline ex operator-(basic const & x, ex const & y) {
    return operator-(ex(x),(y));
}
inline ex operator-(basic const & x, basic const & y) {
    return operator-(ex(x),ex(y));
}

// fixes for ex operator*(x,y)
inline ex operator*(ex const & x, basic const & y) {
    return operator*((x),ex(y));
}
inline ex operator*(basic const & x, ex const & y) {
    return operator*(ex(x),(y));
}
inline ex operator*(basic const & x, basic const & y) {
    return operator*(ex(x),ex(y));
}

// fixes for ex operator/(x,y)
inline ex operator/(ex const & x, basic const & y) {
    return operator/((x),ex(y));
}
inline ex operator/(basic const & x, ex const & y) {
    return operator/(ex(x),(y));
}
inline ex operator/(basic const & x, basic const & y) {
    return operator/(ex(x),ex(y));
}

// fixes for ex operator%(x,y)
inline ex operator%(ex const & x, basic const & y) {
    return operator%((x),ex(y));
}
inline ex operator%(basic const & x, ex const & y) {
    return operator%(ex(x),(y));
}
inline ex operator%(basic const & x, basic const & y) {
    return operator%(ex(x),ex(y));
}

// fixes for relational operator==(x,y)
inline relational operator==(ex const & x, basic const & y) {
    return operator==((x),ex(y));
}
inline relational operator==(basic const & x, ex const & y) {
    return operator==(ex(x),(y));
}
inline relational operator==(basic const & x, basic const & y) {
    return operator==(ex(x),ex(y));
}

// fixes for relational operator!=(x,y)
inline relational operator!=(ex const & x, basic const & y) {
    return operator!=((x),ex(y));
}
inline relational operator!=(basic const & x, ex const & y) {
    return operator!=(ex(x),(y));
}
inline relational operator!=(basic const & x, basic const & y) {
    return operator!=(ex(x),ex(y));
}

// fixes for relational operator<(x,y)
inline relational operator<(ex const & x, basic const & y) {
    return operator<((x),ex(y));
}
inline relational operator<(basic const & x, ex const & y) {
    return operator<(ex(x),(y));
}
inline relational operator<(basic const & x, basic const & y) {
    return operator<(ex(x),ex(y));
}

// fixes for relational operator<=(x,y)
inline relational operator<=(ex const & x, basic const & y) {
    return operator<=((x),ex(y));
}
inline relational operator<=(basic const & x, ex const & y) {
    return operator<=(ex(x),(y));
}
inline relational operator<=(basic const & x, basic const & y) {
    return operator<=(ex(x),ex(y));
}

// fixes for relational operator>(x,y)
inline relational operator>(ex const & x, basic const & y) {
    return operator>((x),ex(y));
}
inline relational operator>(basic const & x, ex const & y) {
    return operator>(ex(x),(y));
}
inline relational operator>(basic const & x, basic const & y) {
    return operator>(ex(x),ex(y));
}

// fixes for relational operator>=(x,y)
inline relational operator>=(ex const & x, basic const & y) {
    return operator>=((x),ex(y));
}
inline relational operator>=(basic const & x, ex const & y) {
    return operator>=(ex(x),(y));
}
inline relational operator>=(basic const & x, basic const & y) {
    return operator>=(ex(x),ex(y));
}

// fixes for ex const & operator+=(x,y)
inline ex const & operator+=(ex & x, basic const & y) {
    return operator+=((x),ex(y));
}
// fixes for ex const & operator-=(x,y)
inline ex const & operator-=(ex & x, basic const & y) {
    return operator-=((x),ex(y));
}
// fixes for ex const & operator*=(x,y)
inline ex const & operator*=(ex & x, basic const & y) {
    return operator*=((x),ex(y));
}
// fixes for ex const & operator/=(x,y)
inline ex const & operator/=(ex & x, basic const & y) {
    return operator/=((x),ex(y));
}
// fixes for other operators
inline ex operator+(basic const & x) {
    return operator+(ex(x));
}
inline ex operator-(basic const & x) {
    return operator-(ex(x));
}
inline ostream & operator<<(ostream & os, basic const & x) {
    return operator<<(os,ex(x));
}
// fixes for functions
// fixes for bool are_ex_trivially_equal(x,y)
inline bool are_ex_trivially_equal(ex const & x, basic const & y) {
    return are_ex_trivially_equal((x),ex(y));
}
inline bool are_ex_trivially_equal(basic const & x, ex const & y) {
    return are_ex_trivially_equal(ex(x),(y));
}
inline bool are_ex_trivially_equal(basic const & x, basic const & y) {
    return are_ex_trivially_equal(ex(x),ex(y));
}

inline unsigned nops(basic const & x) {
    return nops(ex(x));
}
inline ex expand(basic const & x) {
    return expand(ex(x));
}
// fixes for bool has(x,y)
inline bool has(ex const & x, basic const & y) {
    return has((x),ex(y));
}
inline bool has(basic const & x, ex const & y) {
    return has(ex(x),(y));
}
inline bool has(basic const & x, basic const & y) {
    return has(ex(x),ex(y));
}

inline int degree(basic const & x, symbol const & y) {
    return degree(ex(x),(y));
}
inline int ldegree(basic const & x, symbol const & y) {
    return ldegree(ex(x),(y));
}
inline ex coeff(basic const & x, symbol const & y, int z=1) {
    return coeff(ex(x),(y),z);
}
inline ex numer(basic const & x) {
    return numer(ex(x));
}
inline ex denom(basic const & x) {
    return denom(ex(x));
}
inline ex normal(basic const & x, int y=0) {
    return normal(ex(x),y);
}
inline ex collect(basic const & x, symbol const & y) {
    return collect(ex(x),(y));
}
inline ex eval(basic const & x, int y=0) {
    return eval(ex(x),y);
}
inline ex evalf(basic const & x, int y=0) {
    return evalf(ex(x),y);
}
inline ex diff(basic const & x, symbol const & y, int z=1) {
    return diff(ex(x),(y),z);
}
inline ex series(const basic & x, const relational & y, int z) {
    return series(ex(x),ex(y),(z));
}
inline ex series(const basic & x, const symbol & y, int z) {
    return series(ex(x),ex(y),(z));
}
// fixes for ex subs(x,y)
inline ex subs(ex const & x, basic const & y) {
    return subs((x),ex(y));
}
inline ex subs(basic const & x, ex const & y) {
    return subs(ex(x),(y));
}
inline ex subs(basic const & x, basic const & y) {
    return subs(ex(x),ex(y));
}

inline ex subs(basic const & x, lst const & y, lst const & z) {
    return subs(ex(x),(y),(z));
}
inline ex op(basic const & x, int y) {
    return op(ex(x),(y));
}
inline ex lhs(basic const & x) {
    return lhs(ex(x));
}
inline ex rhs(basic const & x) {
    return rhs(ex(x));
}
inline bool is_zero(basic const & x) {
    return is_zero(ex(x));
}
// fixes for simp_lor lor_g(x,y)
inline simp_lor lor_g(ex const & x, basic const & y) {
    return lor_g((x),ex(y));
}
inline simp_lor lor_g(basic const & x, ex const & y) {
    return lor_g(ex(x),(y));
}
inline simp_lor lor_g(basic const & x, basic const & y) {
    return lor_g(ex(x),ex(y));
}

inline simp_lor lor_vec(const string & x, const basic & y) {
    return lor_vec((x),ex(y));
}

#endif // (!defined(G_CINTVERSION)) || (G_CINTVERSION < 501437)
