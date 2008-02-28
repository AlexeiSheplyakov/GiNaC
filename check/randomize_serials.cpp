#include <cstdlib>
#include <ctime>
#include "ginac.h"
using namespace std;
using namespace GiNaC;

/** Generate a random amount of symbols and destroy them again immediatly.
 *  This operation effectively makes the serial numbers of all subsequent
 *  symbols unpredictable.  If the serials are unpredictable, then so are
 *  their hash values.  If the hash values are unpredictable, then so are
 *  the canonical orderings.  If the canonical orderings are unpredictable,
 *  all subsequent times are subject to some variation.  This variation,
 *  however is natural and desireable for two reasons: First, we cannot know
 *  how many symbols have been generated before in real world computations.
 *  Second, the following timings are subject to some semi-random variation
 *  anyways because short timings need to be repeated until enough time has
 *  gone by for the measurement to be reliable.  During this process the serial
 *  numbers will be shifted anyways in a semi-random way.  It is better not
 *  to lull the user in a false sense of reproducibility and instead confront
 *  her with the normal variation to be expected.
 */
void randomify_symbol_serials()
{
	srand(time(NULL));
	const int m = rand() % 666;
	for (int s=0; s<m; ++s ) {
		symbol("dummy");
	}
}


