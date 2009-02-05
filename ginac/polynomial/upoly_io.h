#ifndef GINAC_UPOLY_IO_HPP
#define GINAC_UPOLY_IO_HPP
#include <iostream>
#include "upoly.h"

namespace GiNaC
{
extern std::ostream& operator<<(std::ostream&, const upoly& );
extern std::ostream& operator<<(std::ostream&, const umodpoly& );
}

#endif // GINAC_UPOLY_IO_HPP
