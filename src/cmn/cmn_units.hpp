/** @file
 *  @author Sylwester Arabas <slayoo@igf.fuw.edu.pl>
 *  @copyright University of Warsaw
 *  @date August 2012
 *  @section LICENSE
 *    GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
 *  @brief Boost.units includes
 */

#pragma once

#  include <boost/units/systems/si.hpp>
#  include <boost/units/cmath.hpp> 
#  include <boost/units/io.hpp>
namespace si = boost::units::si;
using boost::units::quantity; 
using boost::units::pow;
using boost::units::multiply_typeof_helper;
using boost::units::divide_typeof_helper;
using boost::units::power_typeof_helper;

#include <boost/units/static_rational.hpp>
using boost::units::static_rational;
