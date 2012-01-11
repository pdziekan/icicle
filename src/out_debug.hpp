/** @file
 *  @author Sylwester Arabas <slayoo@igf.fuw.edu.pl>
 *  @copyright University of Warsaw
 *  @date November 2011
 *  @section LICENSE
 *    GPL v3 (see the COPYING file or http://www.gnu.org/licenses/)
 */
#ifndef OUT_DEBUG_HPP
#  define OUT_DEBUG_HPP

#  include "out.hpp"

extern "C" {
#  include <unistd.h>
}

template <typename real_t>
class out_debug : public out<real_t>
{
  public: virtual void record(
    arr<real_t> *psi,
    const rng &i, const rng &j, const rng &k, const unsigned long t
  ) 
  {
    std::ostringstream tmp;
    tmp
      << "{pid: " << getpid() << "} :" 
      << "[" << i << "," << j << "," << k << "] @ t/dt=" << t
      << endl
      << (*psi)(i, j, k) 
      << endl;
    std::cerr << tmp.str(); // non-buffered?
  }
};

#endif
