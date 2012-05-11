/** @file
 *  @author Anna Jaruga <ajaruga@igf.fuw.edu.pl>
 *  @copyright University of Warsaw
 *  @date March - May 2012
 *  @section LICENSE
 *    GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
 */
#ifndef PHC_KESSLER_HPP
#  define PHC_KESSLER_HPP

#  include "phc.hpp"

namespace phc
{
  // Kessler/Beard terminal velocity
  phc_declare_const_macro(vterm_A, 36.34, si::metre_per_second) 
  phc_declare_const_macro(vterm_B, 1e-3, si::cubic_metres / si::kilograms) 
  phc_declare_funct_macro quantity<si::velocity, real_t> vterm(
    quantity<si::mass_density, real_t> rho_r, 
    quantity<si::mass_density, real_t> rho_d,
    quantity<si::mass_density, real_t> rho_d0
  )
  {
// TODO: separate version for rhod=rhod(z)...
    return vterm_A<real_t>() * real_t(pow(rho_r * vterm_B<real_t>(), .1346) * sqrt(rho_d0 / rho_d));
  }
};

#endif
