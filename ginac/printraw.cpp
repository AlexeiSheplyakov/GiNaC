/** @file printraw.cpp
 *
 * print in ugly raw format, so brave developers can have a look at the
 * underlying structure. */

/* We are cheating here, because we don't want to include the underlying
 * bignum package's headers again, so in this file we omit the definition of
 * void numeric::printraw(ostream & os) const; */

#include <iostream>

#include "ginac.h"

void ex::printraw(ostream & os) const
{
    debugmsg("ex printraw",LOGLEVEL_PRINT);
    ASSERT(bp!=0);
    os << "ex(";
    bp->printraw(os);
    os << ")";
}

void basic::printraw(ostream & os) const
{
    debugmsg("basic printraw",LOGLEVEL_PRINT);
    os << "[basic object]";
}

void symbol::printraw(ostream & os) const
{
    debugmsg("symbol printraw",LOGLEVEL_PRINT);
    os << "symbol(" << "name=" << name << ",serial=" << serial
       << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void constant::printraw(ostream & os) const
{
    debugmsg("constant printraw",LOGLEVEL_PRINT);
    os << "constant(" << name << ")";
}

void power::printraw(ostream & os) const
{
    debugmsg("power printraw",LOGLEVEL_PRINT);

    os << "power(";
    basis.printraw(os);
    os << ",";
    exponent.printraw(os);
    os << ",hash=" << hashvalue << ",flags=" << flags << ")";
}

void fail::printraw(ostream & os) const
{
    debugmsg("fail printraw",LOGLEVEL_PRINT);
    os << "FAIL";
}

void expairseq::printraw(ostream & os) const
{
    debugmsg("expairseq printraw",LOGLEVEL_PRINT);

    os << "expairseq(";
    for (epvector::const_iterator cit=seq.begin(); cit!=seq.end(); ++cit) {
        os << "(";
        (*cit).rest.printraw(os);
        os << ",";
        (*cit).coeff.printraw(os);
        os << "),";
    }
    os << ")";
}

void add::printraw(ostream & os) const
{
    debugmsg("add printraw",LOGLEVEL_PRINT);

    os << "+(";
    for (epvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        os << "(";
        (*it).rest.bp->printraw(os);
        os << ",";
        (*it).coeff.bp->printraw(os);        
        os << "),";
    }
    os << ",hash=" << hashvalue << ",flags=" << flags;
    os << ")";
}

void mul::printraw(ostream & os) const
{
    debugmsg("mul printraw",LOGLEVEL_PRINT);

    os << "*(";
    for (epvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        os << "(";
        (*it).rest.bp->printraw(os);
        os << ",";
        (*it).coeff.bp->printraw(os);
        os << "),";
    }
    os << ",hash=" << hashvalue << ",flags=" << flags;
    os << ")";
}

void ncmul::printraw(ostream & os) const
{
    debugmsg("ncmul printraw",LOGLEVEL_PRINT);

    os << "%(";
    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
        (*it).bp->printraw(os);
        os << ",";
    }
    os << ",hash=" << hashvalue << ",flags=" << flags;
    os << ")";
}

/*void function::printraw(ostream & os) const
 *{
 *    debugmsg("function printraw",LOGLEVEL_PRINT);
 *
 *    os << "function." << name << "(";
 *    for (exvector::const_iterator it=seq.begin(); it!=seq.end(); ++it) {
 *        (*it).bp->print(os);
 *        os << ",";
 *    }
 *    os << ")";
 *}*/

void series::printraw(ostream &os) const
{
	debugmsg("symbol printraw", LOGLEVEL_PRINT);
	os << "series(" << var << ";" << point << ";";
	for (epvector::const_iterator i=seq.begin(); i!=seq.end(); i++) {
		os << "(" << (*i).rest << "," << (*i).coeff << "),";
	}
	os << ")";
}

void relational::printraw(ostream & os) const
{
    debugmsg("relational printraw",LOGLEVEL_PRINT);
    os << "RELATIONAL(";
    lh.printraw(os);
    os << ",";
    rh.printraw(os);
    os << ",";
    switch (o) {
    case equal:
        os << "==";
        break;
    case not_equal:
        os << "!=";
        break;
    case less:
        os << "<";
        break;
    case less_or_equal:
        os << "<=";
        break;
    case greater:
        os << ">";
        break;
    case greater_or_equal:
        os << ">=";
        break;
    default:
        os << "(INVALID RELATIONAL OPERATOR)";
    }
    os << ")";
}

void matrix::printraw(ostream & os) const
{
    debugmsg("matrix printraw",LOGLEVEL_PRINT);
    os << "matrix(" << row << "," << col <<",";
    for (int r=0; r<row-1; ++r) {
        os << "(";
        for (int c=0; c<col-1; ++c) {
            os << m[r*col+c] << ",";
        }
        os << m[col*(r-1)-1] << "),";
    }
    os << "(";
    for (int c=0; c<col-1; ++c) {
        os << m[(row-1)*col+c] << ",";
    }
    os << m[row*col-1] << "))";
}
