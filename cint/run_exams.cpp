/* run_exams.cpp, a launcher that sets variables to start ginaccint.bin.
 * This is necessary because Cint is not libtoolized and so may need to have 
 * LD_LIBRARY_PATH and CINTSYSDIR set. (This cannot be done by a shell-script
 * because the #!-mechanism works only once and we want to enable the user to
 * write scripts using that mechanism.) */

#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "launch.h"

extern char **environ;

int main(int argc, char * *argv)
{
    // what to start
    std::string binprog = "./ginaccint.bin";
    
    // extend LD_LIBRARY_PATH by ../ginac/.libs, so ginaccint.bin really finds libginac
    const char * LD_LIBRARY_PATH = getenv("LD_LIBRARY_PATH");
    if (LD_LIBRARY_PATH == NULL)
        setenv("LD_LIBRARY_PATH", "../ginac/.libs", 1);
    else
        setenv("LD_LIBRARY_PATH", (std::string(LD_LIBRARY_PATH)+':'+"../ginac/.libs").c_str(), 1);
    
    // hard-wire CINTSYSDIR, inherited from configure
    setenv("CINTSYSDIR", CINTSYSDIR.c_str(), 1);
    
    // execute the real thing
    int error = execve(binprog.c_str(), argv, environ);
    
    // only gets here on error
    std::cerr << argv[0] << ": cannot exec " << binprog << std::endl;
    return error;
}
