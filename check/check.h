// check/check.h

#ifndef _CHECK_H_
#define _CHECK_H_

// fcntimer is defined in timer.cpp and used for timing check functions only:
unsigned fcntimer(unsigned fcn());

// prototypes for all individual checks must be unsigned fcn() in order to be
// able to use fcntimer() as a wrapper:
unsigned paranoia_check();
unsigned numeric_output();
unsigned numeric_consist();
unsigned powerlaws();
unsigned expand_subs();
unsigned inifcns_consist();
unsigned differentiation();
unsigned poly_gcd();
unsigned normalization();
unsigned matrix_checks();
unsigned lsolve_onedim();
unsigned series_expansion();

#endif // ndef _CHECK_H_
