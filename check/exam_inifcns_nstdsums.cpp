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


/*
 * The data in the following include file has been produced by the following
 * Mathematica (V4.1) script:
 *
 *
 *    x={0.2,0.7 ,1,1.4,3.0 }
 *    y={0,0.3,-0.8,3.0}
 *    st = OpenAppend["exam_inifcns_nstdsums_data.raw"]
 *    Do[
 *      Do[
 *        Do[Write[st, i]; Write[st,j]; Write[st,x[[k]]+I*y[[l]]];
 *          Write[ st,N[PolyLog[i,j,x[[k]]+I*y[[l]]]]],{i,3},{j,3}], {k,5}],{l,4}]
 *    Do[
 *      Do[
 *        Do[Write[st, i]; Write[st,j]; Write[st,-x[[k]]+I*y[[l]]];
 *          Write[ st,N[PolyLog[i,j,-x[[k]]+I*y[[l]]]]],{i,3},{j,3}], {k,5}], {l,4}]
 *    Close[st]
 *
 *    
 * and postprocessed by the following shell script
 *
 *
 *    #/bin/sh
 *    IFS=$'\n'
 *    cat exam_inifcns_nstdsums_data.raw | sed -e 's/\*\^/E/g' > exam_inifcns_nstdsums_data.raw2
 *    echo 'string data[] = {' > exam_inifcns_nstdsums_data.raw3
 *    for i in `cat exam_inifcns_nstdsums_data.raw2`; do echo \"$i\",; done >> exam_inifcns_nstdsums_data.raw3
 *    echo '"-999"};' >> exam_inifcns_nstdsums.h
 *
 *
 */
#include "exam_inifcns_nstdsums.h"


// adjust this if you want to process more S(n,p,x) data
const int MAX_NDIM = 5;
const int MAX_PDIM = 5;

// signals end of data
const int ENDMARK = -999;

struct point
{
	ex x;
	ex res;
};

typedef vector<point> vp;

vp pp[MAX_NDIM][MAX_PDIM];



static unsigned inifcns_consist_S(void)
{
	unsigned result = 0;
	
	point ppbuf;
	int i = 0;
	while (true) {
		ex en(data[i++],symbol());
		if (en == ENDMARK) {
			break;
		}
		numeric n = ex_to<numeric>(en);
		ex ep(data[i++],symbol());
		numeric p = ex_to<numeric>(ep);
		ex x(data[i++],symbol());
		ex res(data[i++],symbol());
		
		ppbuf.x = x;
		ppbuf.res = res;

		pp[n.to_int()-1][p.to_int()-1].push_back(ppbuf);
	}
	
	vp::iterator it;
	int error = 0;

	cout << endl << "Calculating ";
	for (int sum=2; sum<=3; sum++) {
		for (int nn=1; nn<sum; nn++) {
			vp& da = pp[nn-1][sum-nn-1];
			for (it = da.begin(); it!=da.end(); it++) {
				cout << "S(" << nn << "," << sum-nn << "," << it->x << ") " << flush;
				ex res = S(nn,sum-nn,it->x).evalf();
				if (!is_a<numeric>(res)) {
					if ((it->x != -1) || ((sum-nn) == 1)) {
						clog << "S(" << nn << "," << sum-nn << "," << it->x << ") didn't give numerical result!" << endl;
						result++;
					}
				} 
				else {
					ex reldiff = abs((it->res-res)/it->res);
					if ((!is_a<numeric>(res)) || (reldiff > numeric("1E-10"))) {
						clog << "S(" << nn << "," << sum-nn << "," << it->x << ") seems to be wrong:" << endl;
						clog << "GiNaC           : " << res << endl;
						clog << "Reference       : " << it->res << endl;
						clog << "Abs. Difference : " << it->res-res << endl;
						clog << "Rel. Difference : " << reldiff << endl;
						result++;
					}
				}
			}
		}

	}
	cout << endl;

	return result;
}


unsigned exam_inifcns_nstdsums(void)
{
	unsigned result = 0;
	
	cout << "examining consistency of nestedsums functions" << flush;
	clog << "----------consistency of nestedsums functions:" << endl;
	
	result += inifcns_consist_S();  cout << '.' << flush;
	
	if (!result) {
		cout << " passed " << endl;
		clog << "(no output)" << endl;
	} else {
		cout << " failed " << endl;
	}
	
	return result;
}

