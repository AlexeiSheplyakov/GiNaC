/*  dummies.h
 *
 *  Dummies and wrappers to overcome certain deficiencies of Cint.
 *  This file was generated automatically by dummies.pl.
 *  Please do not modify it directly, edit the perl script instead!
 */

// fixes for sin(x)
inline ex sin(symbol const & x) { return sin(ex(x)); }
inline ex sin(function const & x) { return sin(ex(x)); }
inline ex sin(constant const & x) { return sin(ex(x)); }
inline ex sin(idx const & x) { return sin(ex(x)); }
inline ex sin(lorentzidx const & x) { return sin(ex(x)); }
inline ex sin(coloridx const & x) { return sin(ex(x)); }

// fixes for cos(x)
inline ex cos(symbol const & x) { return cos(ex(x)); }
inline ex cos(function const & x) { return cos(ex(x)); }
inline ex cos(constant const & x) { return cos(ex(x)); }
inline ex cos(idx const & x) { return cos(ex(x)); }
inline ex cos(lorentzidx const & x) { return cos(ex(x)); }
inline ex cos(coloridx const & x) { return cos(ex(x)); }

// fixes for tan(x)
inline ex tan(symbol const & x) { return tan(ex(x)); }
inline ex tan(function const & x) { return tan(ex(x)); }
inline ex tan(constant const & x) { return tan(ex(x)); }
inline ex tan(idx const & x) { return tan(ex(x)); }
inline ex tan(lorentzidx const & x) { return tan(ex(x)); }
inline ex tan(coloridx const & x) { return tan(ex(x)); }

// fixes for asin(x)
inline ex asin(symbol const & x) { return asin(ex(x)); }
inline ex asin(function const & x) { return asin(ex(x)); }
inline ex asin(constant const & x) { return asin(ex(x)); }
inline ex asin(idx const & x) { return asin(ex(x)); }
inline ex asin(lorentzidx const & x) { return asin(ex(x)); }
inline ex asin(coloridx const & x) { return asin(ex(x)); }

// fixes for acos(x)
inline ex acos(symbol const & x) { return acos(ex(x)); }
inline ex acos(function const & x) { return acos(ex(x)); }
inline ex acos(constant const & x) { return acos(ex(x)); }
inline ex acos(idx const & x) { return acos(ex(x)); }
inline ex acos(lorentzidx const & x) { return acos(ex(x)); }
inline ex acos(coloridx const & x) { return acos(ex(x)); }

// fixes for atan(x)
inline ex atan(symbol const & x) { return atan(ex(x)); }
inline ex atan(function const & x) { return atan(ex(x)); }
inline ex atan(constant const & x) { return atan(ex(x)); }
inline ex atan(idx const & x) { return atan(ex(x)); }
inline ex atan(lorentzidx const & x) { return atan(ex(x)); }
inline ex atan(coloridx const & x) { return atan(ex(x)); }

// fixes for exp(x)
inline ex exp(symbol const & x) { return exp(ex(x)); }
inline ex exp(function const & x) { return exp(ex(x)); }
inline ex exp(constant const & x) { return exp(ex(x)); }
inline ex exp(idx const & x) { return exp(ex(x)); }
inline ex exp(lorentzidx const & x) { return exp(ex(x)); }
inline ex exp(coloridx const & x) { return exp(ex(x)); }

// fixes for log(x)
inline ex log(symbol const & x) { return log(ex(x)); }
inline ex log(function const & x) { return log(ex(x)); }
inline ex log(constant const & x) { return log(ex(x)); }
inline ex log(idx const & x) { return log(ex(x)); }
inline ex log(lorentzidx const & x) { return log(ex(x)); }
inline ex log(coloridx const & x) { return log(ex(x)); }

// fixes for sqrt(x)
inline ex sqrt(symbol const & x) { return sqrt(ex(x)); }
inline ex sqrt(function const & x) { return sqrt(ex(x)); }
inline ex sqrt(constant const & x) { return sqrt(ex(x)); }
inline ex sqrt(idx const & x) { return sqrt(ex(x)); }
inline ex sqrt(lorentzidx const & x) { return sqrt(ex(x)); }
inline ex sqrt(coloridx const & x) { return sqrt(ex(x)); }

// fixes for sinh(x)
inline ex sinh(symbol const & x) { return sinh(ex(x)); }
inline ex sinh(function const & x) { return sinh(ex(x)); }
inline ex sinh(constant const & x) { return sinh(ex(x)); }
inline ex sinh(idx const & x) { return sinh(ex(x)); }
inline ex sinh(lorentzidx const & x) { return sinh(ex(x)); }
inline ex sinh(coloridx const & x) { return sinh(ex(x)); }

// fixes for cosh(x)
inline ex cosh(symbol const & x) { return cosh(ex(x)); }
inline ex cosh(function const & x) { return cosh(ex(x)); }
inline ex cosh(constant const & x) { return cosh(ex(x)); }
inline ex cosh(idx const & x) { return cosh(ex(x)); }
inline ex cosh(lorentzidx const & x) { return cosh(ex(x)); }
inline ex cosh(coloridx const & x) { return cosh(ex(x)); }

// fixes for tanh(x)
inline ex tanh(symbol const & x) { return tanh(ex(x)); }
inline ex tanh(function const & x) { return tanh(ex(x)); }
inline ex tanh(constant const & x) { return tanh(ex(x)); }
inline ex tanh(idx const & x) { return tanh(ex(x)); }
inline ex tanh(lorentzidx const & x) { return tanh(ex(x)); }
inline ex tanh(coloridx const & x) { return tanh(ex(x)); }

// fixes for abs(x)
inline ex abs(symbol const & x) { return abs(ex(x)); }
inline ex abs(function const & x) { return abs(ex(x)); }
inline ex abs(constant const & x) { return abs(ex(x)); }
inline ex abs(idx const & x) { return abs(ex(x)); }
inline ex abs(lorentzidx const & x) { return abs(ex(x)); }
inline ex abs(coloridx const & x) { return abs(ex(x)); }

// fixes for pow(x,y)
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

// fixes for atan2(x,y)
inline ex atan2(symbol const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(symbol const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(symbol const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(symbol const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(symbol const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(symbol const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(function const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(function const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(function const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(function const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(function const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(function const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(constant const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(constant const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(constant const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(constant const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(constant const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(constant const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(idx const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(idx const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(idx const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(idx const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(idx const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(idx const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(lorentzidx const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(lorentzidx const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(lorentzidx const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(lorentzidx const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(lorentzidx const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(lorentzidx const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(coloridx const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(coloridx const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(coloridx const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(coloridx const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(coloridx const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(coloridx const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(symbol const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(symbol const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(symbol const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(function const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(function const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(function const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(constant const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(constant const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(constant const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(idx const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(idx const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(idx const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(lorentzidx const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(lorentzidx const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(lorentzidx const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(coloridx const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(coloridx const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(coloridx const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(numeric const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(numeric const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(numeric const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(numeric const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(numeric const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(numeric const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(int const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(int const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(int const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(int const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(int const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(int const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(double const & x,symbol const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(double const & x,function const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(double const & x,constant const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(double const & x,idx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(double const & x,lorentzidx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(double const & x,coloridx const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(ex const & x,numeric const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(ex const & x,int const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(ex const & x,double const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(numeric const & x,ex const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(int const & x,ex const & y) {
    return atan2(ex(x),ex(y));
}
inline ex atan2(double const & x,ex const & y) {
    return atan2(ex(x),ex(y));
}

