/** @file
 *  @example phc_terminal_vel/term_vel_test.cpp
 *  @author Anna Jaruga <ajaruga@igf.fuw.edu.pl>
 *  @copyright University of Warsaw
 *  @date May 2012
 *  @section LICENSE
 *    GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
 *  @section DESCRIPTION
 *    Tests the terminal velocity parametersiation implemented in phc_terminal_vel.hpp
 *    following @copybrief Khvorostyanow_and_Curry_2002 (cf. solid line in Figure 2 therein)
 *
 *    \image html "../../tests/phc_terminal_vel/term_vel_test.svg"
 */

#include <map>
using std::map;

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>
namespace si = boost::units::si;
using boost::units::quantity;
using boost::units::divide_typeof_helper;
using boost::units::detail::get_value;

#define GNUPLOT_ENABLE_BLITZ
#include <gnuplot-iostream/gnuplot-iostream.h>
using std::endl;

#include "../../src/phc_terminal_vel.hpp"

typedef float real_t;

int main()
{
  quantity<si::temperature, real_t> T=real_t(293.)*si::kelvin;
  quantity<si::mass_density, real_t> rhoa=real_t(1.23)*si::kilograms/si::cubic_metres;
  quantity<si::length, real_t> r=real_t(1.*1e-6)*si::metres;

  quantity<si::length, real_t> radius;
  quantity<si::velocity, real_t> term_vel;
 
  map< quantity<si::length, real_t>, quantity<si::velocity, real_t> > plot_data;
  for(int i=0; i<=45; i++){
    radius = r*real_t(i*100) ; 
    term_vel = phc::vt(r * real_t(i * 100),T, rhoa) ;
    plot_data[radius * real_t(2*1e6)] = term_vel * real_t(100);
  }
 
  Gnuplot gp;
  gp << "set xlabel 'drop diameter [um]'" << endl;
  gp << "set ylabel 'terminal velocity [cm/s]'" << endl;
  gp << "set term svg" << endl;
  gp << "set output 'term_vel_test.svg'" << endl; 
  gp << "plot '-' u 1:3" << endl;
  gp.send(plot_data);
}
