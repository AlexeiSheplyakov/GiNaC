/* ginaccint.cpp, a launcher that sets variables to start ginaccint.bin.
 * This is necessary because Cint is not libtoolized and so may need to have 
 * LD_LIBRARY_PATH and CINTSYSDIR set.  This cannot be done by a shell-script
 * because the #!-mechanism works only once and we want to enable the user to
 * write scripts using that mechanism. */

#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "launch.h"

extern char **environ;

int main(int argc, char * *argv)
{
    // manually "expand" autoconf-style variables
    if (exec_prefix=="${prefix}")
        exec_prefix = prefix;
    if (bindir=="${exec_prefix}/bin")
        bindir = exec_prefix + "/bin";
    if (libdir=="${exec_prefix}/lib")
        libdir = exec_prefix + "/lib";
    // now we can guess what to start
    string binprog = bindir + "/ginaccint.bin";
    
    // extend LD_LIBRARY_PATH by libdir, so ginaccint.bin really finds libginac
    const char * LD_LIBRARY_PATH = getenv("LD_LIBRARY_PATH");
    if (LD_LIBRARY_PATH == NULL)
        setenv("LD_LIBRARY_PATH", libdir.c_str(), 1);
    else
        setenv("LD_LIBRARY_PATH", (string(LD_LIBRARY_PATH)+':'+libdir).c_str(), 1);
    
    // hard-wire CINTSYSDIR, inherited from configure, if it is not set
    setenv("CINTSYSDIR", CINTSYSDIR.c_str(), 0);
    
    // execute the real thing
    int error = execve(binprog.c_str(), argv, environ);
    
    // only gets here on error
    cerr << argv[0] << ": cannot exec " << binprog << endl;
    return error;
}
