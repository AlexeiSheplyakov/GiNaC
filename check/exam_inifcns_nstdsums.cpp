/** @file exam_inifcns_nstdsums.cpp
 *
 *  This test routine applies assorted tests on initially known higher level
 *  functions. */

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

#include "exams.h"

#include <fstream>


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  S exam
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/*
 * The data in the following include file has been produced by the following
 * Mathematica (V4.1) script:
 *
 *
 *    x={2/10,1,14/10,30/10}
 *    y={0,3/10,-14/10}
 *    st = OpenAppend["exam_inifcns_nstdsums_data.raw"]
 *    $NumberMarks = False
 *    Do[
 *      Do[
 *        Do[Write[st, i]; Write[st,j]; Write[st,x[[k]]+I*y[[l]]];
 *          Write[st,Chop[N[PolyLog[i,j,x[[k]]+I*y[[l]]],25]]],{i,3},{j,3}], {k,4}],{l,3}]
 *    Do[
 *      Do[
 *        Do[Write[st, i]; Write[st,j]; Write[st,-x[[k]]+I*y[[l]]];
 *          Write[st,Chop[N[PolyLog[i,j,-x[[k]]+I*y[[l]]],25]]],{i,3},{j,3}], {k,4}], {l,3}]
 *    Close[st]
 *
 *    
 * and postprocessed by the following shell script
 *
 *
 *    #/bin/sh
 *    IFS=$'\n'
 *    cat exam_inifcns_nstdsums_data.raw | sed -e 's/\*\^/E/g' > exam_inifcns_nstdsums_data.raw2
 *    echo 'const char *data[] = {' > exam_inifcns_nstdsums_data.raw3
 *    for i in `cat exam_inifcns_nstdsums_data.raw2`; do echo \"$i\",; done >> exam_inifcns_nstdsums_data.raw3
 *    echo '"-999"};' >> exam_inifcns_nstdsums.h
 *
 *
 */
#include "exam_inifcns_nstdsums.h"


// signals end of data
const int ENDMARK = -999;


static unsigned inifcns_test_S()
{
	int digitsbuf = Digits;
	// precision of data
	Digits = 22;
	ex prec = 5 * pow(10, -(int)Digits);
	
	unsigned result = 0;
	
	int i = 0;
	while (true) {
		ex n(data[i++],symbol());
		if (n == ENDMARK) {
			break;
		}
		ex p(data[i++],symbol());
		ex x(data[i++],symbol());
		ex res(data[i++],symbol());
		ex res2 = S(n, p, x).evalf();
		if (abs(res-res2) > prec) {
			clog << "S(" << n << "," << p << "," << x << ") seems to be wrong:" << endl;
			clog << "GiNaC           : " << res2 << endl;
			clog << "Reference       : " << res << endl;
			clog << "Abs. Difference : " << res2-res << endl;
			if (res2 != 0) {
				ex reldiff = abs((res2-res)/res2);
				clog << "Rel. Difference : " << reldiff << endl;
			}
			result++;
		}
		if (i % 80) {
			cout << "." << flush;
		}
	}

	Digits = digitsbuf;

	return result;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  H/Li exam
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static unsigned inifcns_test_HLi()
{
	int digitsbuf = Digits;
	Digits = 17;
	ex prec = 5 * pow(10, -(int)Digits);
	numeric almostone("0.999999999999999999");
	unsigned result = 0;

	lst res;
	
	res.append(H(lst(2,1),numeric(1)/2).hold() - (zeta(3)/8 - pow(log(2),3)/6));
	res.append(H(lst(2,1,3),numeric(1)/3).hold() - Li(lst(2,1,3),lst(numeric(1)/3,1,1)).hold());
	res.append(H(lst(2,1,3),numeric(98)/100).hold() - Li(lst(2,1,3),lst(numeric(98)/100,1,1)).hold());
	res.append(H(lst(2,1,3),numeric(245)/100).hold() - Li(lst(2,1,3),lst(numeric(245)/100,1,1)).hold());
	res.append(H(lst(4,1,1,1),numeric(1)/3).hold() - S(3,4,numeric(1)/3).hold());
	res.append(H(lst(4,1,1,1),numeric(98)/100).hold() - S(3,4,numeric(98)/100).hold());
	res.append(H(lst(4,1,1,1),numeric(245)/100).hold() - S(3,4,numeric(245)/100).hold());
	res.append(H(lst(2,2,3),almostone).hold() - zeta(lst(2,2,3)));
	res.append(H(lst(-3,-1,2,1),almostone).hold() - zeta(lst(3,1,2,1),lst(-1,1,-1,1)));
	res.append(H(lst(-2,1,3),numeric(1)/3).hold() - -Li(lst(2,1,3),lst(-numeric(1)/3,-1,1)).hold());
	res.append(H(lst(-2,1,3),numeric(98)/100).hold() - -Li(lst(2,1,3),lst(-numeric(98)/100,-1,1)).hold());
	res.append(H(lst(-2,1,3),numeric(245)/100).hold() - -Li(lst(2,1,3),lst(-numeric(245)/100,-1,1)).hold());
	res.append(H(lst(-3,1,-2,0,0),numeric(3)/10).hold() - convert_H_to_Li(lst(-3,1,-2,0,0),numeric(3)/10).eval());
	
	for (lst::const_iterator it = res.begin(); it != res.end(); it++) {
		ex diff = abs((*it).evalf());
		if (diff > prec) {
			clog << *it << " seems to be wrong: " << diff << endl;
			result++;
		}
		cout << "." << flush;
	}

	Digits = digitsbuf;

	return result;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  zeta exam
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static unsigned inifcns_test_zeta()
{
	int digitsbuf = Digits;
	
	unsigned result = 0;

	lst res;
	
	res.append(zeta(lst(2,1)) - zeta(3));
	res.append(zeta(lst(2,1,1,1,1)) - zeta(6));
	res.append(zeta(lst(6,3)) - (zeta(9)*83/2 - zeta(2)*zeta(7)*21 - zeta(2)*zeta(2)*zeta(5)*12/5));
	res.append(zeta(lst(4,2,3)) - (-zeta(9)*59 + zeta(2)*zeta(7)*28 + pow(zeta(2),2)*zeta(5)*4 -
	                               pow(zeta(3),3)/3 + pow(zeta(2),3)*zeta(3)*8/21));
	res.append(zeta(lst(3,1,3,1,3,1,3,1)) - (2*pow(Pi,16)/factorial(18)));
	res.append(zeta(lst(2),lst(-1)) - -zeta(2)/2);
	res.append(zeta(lst(1,2),lst(-1,1)) - (-zeta(3)/4 - zeta(lst(1),lst(-1))*zeta(2)/2));
	res.append(zeta(lst(2,1,1),lst(-1,-1,1)) - (-pow(zeta(2),2)*23/40 - pow(zeta(lst(1),lst(-1)),2)*zeta(2)*3/4
	                                            - zeta(lst(3,1),lst(-1,1))*3/2 - zeta(lst(1),lst(-1))*zeta(3)*21/8));
	
	for (lst::const_iterator it = res.begin(); it != res.end(); it++) {
		Digits = 17;
		ex prec = 5 * pow(10, -(int)Digits);
		ex diff = abs((*it).evalf());
		if (diff > prec) {
			clog << *it << " seems to be wrong: " << diff << endl;
			clog << "Digits: " << Digits << endl;
			result++;
		}
		cout << "." << flush;
		Digits = 40;
		prec = 5 * pow(10, -(int)Digits);
		diff = abs((*it).evalf());
		if (diff > prec) {
			clog << *it << " seems to be wrong: " << diff << endl;
			clog << "Digits: " << Digits << endl;
			result++;
		}
		cout << "." << flush;
	}

	Digits = digitsbuf;

	return result;
}


unsigned exam_inifcns_nstdsums(void)
{
	unsigned result = 0;
	
	cout << "examining consistency of nestedsums functions" << flush;
	clog << "----------consistency of nestedsums functions:" << endl;
	
	cout << "zeta" << flush;
	result += inifcns_test_zeta();
	cout << "S" << flush;
	result += inifcns_test_S();
	cout << "H/Li" << flush;
	result += inifcns_test_HLi();
	
	if (!result) {
		cout << " passed " << endl;
		clog << "(no output)" << endl;
	} else {
		cout << " failed " << endl;
	}
	
	return result;
}
