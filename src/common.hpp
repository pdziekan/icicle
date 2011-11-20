/** @file
 *  @author Sylwester Arabas (University of Warsaw) <slayoo@igf.fuw.edu.pl>
 *  @date November 2011
 *  @section LICENSE
 *    GPL v3 (see the COPYING file or http://www.gnu.org/licenses/)
 */

#ifndef COMMON_HPP
#  define COMMON_HPP

// most common includes
#  include <boost/units/systems/si.hpp>
using namespace boost::units;

#  include <blitz/array.h>
using namespace blitz;

// overloading the default d-tor with a virtual one (enforces execution of child d-tors)
class root { public: virtual ~root() {} };

// error reporting
#  define error_macro(msg) { cerr << "-- error: " << msg << endl; throw exception(); }
#  define warning_macro(msg) { cerr << "-- warning: " << msg << endl; }

// some non-standard units
typedef multiply_typeof_helper<
    si::velocity,
    si::length
  >::type velocity_times_length;

#endif
