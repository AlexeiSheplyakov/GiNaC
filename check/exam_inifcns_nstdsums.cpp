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


struct point
{
	ex x;
	ex res;
};


const int NDIM = 5;
const int PDIM = 5;

typedef vector<point> vp;

vp pp[NDIM][PDIM];


static unsigned inifcns_consist_S(void)
{
	unsigned result = 0;
	
	ifstream in("exam_inifcns_nstdsums_data");

	if (!in) {
		clog << "exam_inifcns_nstdsums_data not readable!" << endl;
		return 666;
	}

	string str;
	point ppbuf;

	while (1) {
		getline(in,str);
		if (!in)
			break;
		ex en(str,symbol());
		getline(in,str);
		if (!in)
			break;
		ex ep(str,symbol());
		getline(in,str);
		if (!in)
			break;
		ex x(str,symbol());
		getline(in,str);
		if (!in)
			break;
		ex res(str,symbol());

		numeric n = ex_to<numeric>(en);
		numeric p = ex_to<numeric>(ep);

		ppbuf.x = x;
		ppbuf.res = res;

		pp[n.to_int()-1][p.to_int()-1].push_back(ppbuf);
	}

	in.close();

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

