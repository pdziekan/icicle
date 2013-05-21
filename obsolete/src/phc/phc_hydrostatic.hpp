/** @file
 *  @author Sylwester Arabas <slayoo@igf.fuw.edu.pl>
 *  @author Anna Jaruga <ajaruga@igf.fuw.edu.pl>
 *  @copyright University of Warsaw
 *  @date October 2012
 *  @section LICENSE
 *    GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
 *  @brief contains definitions of thermodynamic relations that result
 *    from the assumption of hydrostatic equilibrium
 */
#pragma once
#include "phc.hpp"

namespace phc
{
  namespace hydrostatic
  {
    // pressure profile derived by integrating the hydrostatic eq.
    // assuming constant theta, constant rv and R=R(rv) 
    phc_declare_funct_macro quantity<si::pressure,real_t> p(
      quantity<si::length, real_t> z,
      quantity<si::temperature,real_t> th_0,
      quantity<phc::mixing_ratio, real_t> r_0,
      quantity<si::length, real_t> z_0,
      quantity<si::pressure, real_t> p_0 
    )
    {
      return phc::p_1000<real_t>() * real_t(pow(
        pow(p_0 / phc::p_1000<real_t>(), phc::R_d_over_c_pd<real_t>()) 
        -   
        phc::R_d_over_c_pd<real_t>() * phc::g<real_t>() / th_0 / phc::R<real_t>(r_0) * (z - z_0), 
        phc::c_pd<real_t>() / phc::R_d<real_t>()
      )); 
    }
  };
};
