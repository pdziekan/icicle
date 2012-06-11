/** @file
 *  @author Sylwester Arabas <slayoo@igf.fuw.edu.pl>
 *  @copyright University of Warsaw
 *  @date January - March 2012
 *  @section LICENSE
 *    GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
 */
#pragma once

#  include "opt.hpp"
#  include "eqs_scalar_advection.hpp"
#  include "eqs_shallow_water.hpp"
#  include "eqs_isentropic.hpp"
#  include "eqs_harmonic_oscillator.hpp"
#  include "eqs_todo_bulk_ode.hpp"
#  include "eqs_todo_sdm.hpp"

inline void opt_eqs_desc(po::options_description &desc)
{
  desc.add_options()
    ("eqs", po::value<string>()->default_value("scalar_advection"), "equation system: shallow_water, isentropic, ...")

    ("eqs.isentropic.nlev", po::value<int>(), "number of fluid layers")
    ("eqs.isentropic.p_top", po::value<string>()->default_value("0"), "pressure at the uppermost surface [Pa]")
    ("eqs.isentropic.theta_frst", po::value<string>(), "mid-layer potential temperature of the first layer [K]")
    ("eqs.isentropic.abslev", po::value<int>(), "absorber lowermost level")
    ("eqs.isentropic.absamp", po::value<string>()->default_value("1"), "absorber amplitude [1]") 

    ("eqs.harmonic_oscillator.omega", po::value<string>(), "omega [Hz]") 
    
    ("eqs.todo_bulk.cond", po::value<bool>()->default_value(true), "cloud water condensation [on/off]")
    ("eqs.todo_bulk.cevp", po::value<bool>()->default_value(true), "cloud water evaporation [on/off]")
    ("eqs.todo_bulk.conv", po::value<bool>()->default_value(true), "conversion of cloud water into rain [on/off]")
    ("eqs.todo_bulk.clct", po::value<bool>()->default_value(true), "collection of cloud water by rain [on/off]")
    ("eqs.todo_bulk.sedi", po::value<bool>()->default_value(true), "rain water sedimentation [on/off]")
    ("eqs.todo_bulk.revp", po::value<bool>()->default_value(true), "rain water evaporation [on/off]")

    ("eqs.todo_sdm.cond", po::value<bool>()->default_value(true), "condensation/evaporation [on/off]")
    ("eqs.todo_sdm.coal", po::value<bool>()->default_value(true), "coalescence [on/off]")
    ("eqs.todo_sdm.sedi", po::value<bool>()->default_value(true), "sedimentation [on/off]")
    ("eqs.todo_sdm.xi", po::value<string>()->default_value("ln"), "definition of xi (id, ln, p2, p3)")
    ("eqs.todo_sdm.ode_algo_xy", po::value<string>()->default_value("rk4"), "advection ODE solver type (euler, rk4)")
    ("eqs.todo_sdm.ode_algo_xi", po::value<string>()->default_value("rk4"), "drop-growth ODE solver type (euler, rk4)")
    ("eqs.todo_sdm.sd_conc_mean", po::value<string>()->default_value("64"), "mean super-droplet density per cell") // TODO: why 64? :)
    ("eqs.todo_sdm.min_rd", po::value<string>(), "minimum possible dry aerosol radius [m]") 
    ("eqs.todo_sdm.max_rd", po::value<string>(), "maximum posiible dry aerosol radius [m]")
    // parameters for dry aerosol two-mode lognormal distribution
    ("eqs.todo_sdm.mean_rd1", po::value<string>(), "first mode dry aerosol mean radii [m]") 
    ("eqs.todo_sdm.mean_rd2", po::value<string>(), "second mode dry aerosol mean radii[m]")
    ("eqs.todo_sdm.sdev_rd1", po::value<string>(), "first mode geometric standard deviation [1]") 
    ("eqs.todo_sdm.sdev_rd2", po::value<string>(), "second mode geometric standard deviation [1]")
    ("eqs.todo_sdm.n1_tot", po::value<string>(), "first mode total concentration [m-3]") 
    ("eqs.todo_sdm.n2_tot", po::value<string>(), "second mode total concentration [m-3]")
    ("eqs.todo_sdm.kappa", po::value<string>(), "dolubility parameter kappa [1]")
    ;
}

template <typename real_t>
eqs<real_t> *opt_eqs(
  const po::variables_map& vm, 
  const grd<real_t> &grid, 
  const ini<real_t> &intcond, 
  const vel<real_t> &velocity
) 
{
  string initype= vm.count("eqs") ? vm["eqs"].as<string>() : "<unspecified>";
  if (initype == "scalar_advection")
    return new eqs_scalar_advection<real_t>();
  else 
  if (initype == "shallow_water")
    return new eqs_shallow_water<real_t>(grid);
  else
  if (initype == "isentropic")
  {
    if (!vm.count("eqs.isentropic.nlev")) error_macro("TODO")
    if (!vm.count("eqs.isentropic.abslev")) error_macro("TODO")
    return new eqs_isentropic<real_t>(grid, 
      vm["eqs.isentropic.nlev"].as<int>(),
      real_cast<real_t>(vm, "eqs.isentropic.p_top") * si::pascals,
      real_cast<real_t>(vm, "eqs.isentropic.theta_frst") * si::kelvins,
      vm["eqs.isentropic.abslev"].as<int>(),
      real_cast<real_t>(vm, "eqs.isentropic.absamp")
    );
  }
  else
  if (initype == "harmonic_oscillator")
    return new eqs_harmonic_oscillator<real_t>(
      real_cast<real_t>(vm, "eqs.harmonic_oscillator.omega") / si::seconds
    );
  else 
  if (initype == "todo_bulk")
    return new eqs_todo_bulk_ode<real_t>(grid,
      map<enum eqs_todo_bulk<real_t>::processes, bool>({
        {eqs_todo_bulk<real_t>::cond, vm["eqs.todo_bulk.cond"].as<bool>()},
        {eqs_todo_bulk<real_t>::cevp, vm["eqs.todo_bulk.cevp"].as<bool>()},
        {eqs_todo_bulk<real_t>::conv, vm["eqs.todo_bulk.conv"].as<bool>()},
        {eqs_todo_bulk<real_t>::clct, vm["eqs.todo_bulk.clct"].as<bool>()},
        {eqs_todo_bulk<real_t>::sedi, vm["eqs.todo_bulk.sedi"].as<bool>()},
        {eqs_todo_bulk<real_t>::revp, vm["eqs.todo_bulk.revp"].as<bool>()}
      })
    );
  else 
  if (initype == "todo_sdm")
  {
    map<string, enum eqs_todo_sdm<real_t>::ode_algos> map_algo({
      {"euler", eqs_todo_sdm<real_t>::euler},
      {"rk4", eqs_todo_sdm<real_t>::rk4}
    });
    map<string, enum eqs_todo_sdm<real_t>::xi_dfntns> map_xid({
      {"id", eqs_todo_sdm<real_t>::id},
      {"ln", eqs_todo_sdm<real_t>::ln},
      {"p2", eqs_todo_sdm<real_t>::p2},
      {"p3", eqs_todo_sdm<real_t>::p3}
    });
    return new eqs_todo_sdm<real_t>(grid, velocity,
      map<enum eqs_todo_sdm<real_t>::processes, bool>({
        {eqs_todo_sdm<real_t>::cond, vm["eqs.todo_sdm.cond"].as<bool>()},
        {eqs_todo_sdm<real_t>::sedi, vm["eqs.todo_sdm.sedi"].as<bool>()},
        {eqs_todo_sdm<real_t>::coal, vm["eqs.todo_sdm.coal"].as<bool>()},
      }),
      map_xid[vm["eqs.todo_sdm.xi"].as<string>()],
      map_algo[vm["eqs.todo_sdm.ode_algo_xy"].as<string>()],
      map_algo[vm["eqs.todo_sdm.ode_algo_xi"].as<string>()],
      real_cast<real_t>(vm, "eqs.todo_sdm.sd_conc_mean"),
      real_cast<real_t>(vm, "eqs.todo_sdm.min_rd"),
      real_cast<real_t>(vm, "eqs.todo_sdm.max_rd"),
      real_cast<real_t>(vm, "eqs.todo_sdm.mean_rd1"), 
      real_cast<real_t>(vm, "eqs.todo_sdm.mean_rd2"),
      real_cast<real_t>(vm, "eqs.todo_sdm.sdev_rd1"), 
      real_cast<real_t>(vm, "eqs.todo_sdm.sdev_rd2"),
      real_cast<real_t>(vm, "eqs.todo_sdm.n1_tot"), 
      real_cast<real_t>(vm, "eqs.todo_sdm.n2_tot"),
      real_cast<real_t>(vm, "eqs.todo_sdm.kappa")
    );
  }
  else 
  error_macro("unsupported equation system: " << initype)
}
