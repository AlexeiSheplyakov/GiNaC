// check/fcntimer.cpp

#include <stdio.h>
#include <sys/resource.h>


// fcntimer() is a little wrapper around GiNaC's automated checks.  All those
// functions are passed void and return unsigned.  fcntimer() accepts one such
// function fcn(), returns its result and as a side-effect prints to stdout how
// much CPU time was consumed by fcn's execution in the fashion "(0.07s)\n".
unsigned fcntimer(unsigned fcn())
{
    unsigned fcnresult;
    struct rusage used1, used2;
    double elapsed;

    // time the execution of the function:
    getrusage(RUSAGE_SELF, &used1);
    fcnresult = fcn();
    getrusage(RUSAGE_SELF, &used2);

    // add elapsed user and system time in microseconds:
    elapsed = ((used2.ru_utime.tv_sec - used1.ru_utime.tv_sec) +
               (used2.ru_stime.tv_sec - used1.ru_stime.tv_sec) +
               (used2.ru_utime.tv_usec - used1.ru_utime.tv_usec) / 1e6 +
               (used2.ru_stime.tv_usec - used1.ru_stime.tv_usec) / 1e6);

    printf("(%.2fs)\n", elapsed);

    return fcnresult;
}
