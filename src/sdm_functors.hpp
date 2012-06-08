/** @file
 *  @author Sylwester Arabas <slayoo@igf.fuw.edu.pl>
 *  @author Anna Jaruga <ajaruga@igf.fuw.edu.pl>
 *  @copyright University of Warsaw
 *  @date April-June 2012
 *  @section LICENSE
 *    GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
 *  @brief a set of functors used with Thrust routines in the eqs_todo_sdm class
 */
#ifndef SDM_FUNCTORS_HPP
#  define SDM_FUNCTORS_HPP

#  include "sdm_base.hpp"

#  ifdef USE_THRUST

namespace sdm {

  /// @brief a random-number-generator functor
  // TODO: random seed as an option
  // TODO: RNG engine as an option
  template <typename thrust_real_t> 
  class rng 
  {
    private: thrust::random::taus88 engine; 
    private: thrust::uniform_real_distribution<thrust_real_t> dist;
    public: rng(thrust_real_t a, thrust_real_t b, thrust_real_t seed) : dist(a, b), engine(seed) {}
    public: thrust_real_t operator()() { return dist(engine); }
  };

  /// @brief a functor that divides by real constant and cast to int
  template <typename thrust_real_t> 
  class divide_by_constant
  {
    private: thrust_real_t c;
    public: divide_by_constant(thrust_real_t c) : c(c) {}
    public: int operator()(thrust_real_t x) { return x/c; }
  };

  /// @brief a functor that multiplies by a real constant
  template <typename thrust_real_t> 
  class multiply_by_constant
  {
    private: thrust_real_t c;
    public: multiply_by_constant(thrust_real_t c) : c(c) {}
    public: thrust_real_t operator()(thrust_real_t x) { return x*c; }
  };

  /// @brief a functor that ravels (i,j) index pairs into a single index
  class ravel_indices
  {
    private: int n;
    public: ravel_indices(int n) : n(n) {}
    public: int operator()(int i, int j) { return i + j * n; }
  };

  /// @brief a functor interface to fmod()
  template <typename thrust_real_t> 
  class modulo
  {
    private: thrust_real_t mod;
    public: modulo(thrust_real_t mod) : mod(mod) {}
    public: thrust_real_t operator()(thrust_real_t a) { return fmod(a + mod, mod); }
  };

  /// @brief a Thrust-to-Blitz data transfer functor 
  template <typename blitz_real_t> 
  class copy_from_device 
  {
    private: int n;
    private: const thrust::device_vector<int> &idx2ij;
    private: const thrust::device_vector<thrust_size_t> &from;
    private: mtx::arr<blitz_real_t> &to;
    private: blitz_real_t scl;

    // ctor
    public: copy_from_device(int n, 
      const thrust::device_vector<int> &idx2ij,
      const thrust::device_vector<thrust_size_t> &from,
      mtx::arr<blitz_real_t> &to,
      blitz_real_t scl = blitz_real_t(1)
    ) : n(n), idx2ij(idx2ij), from(from), to(to), scl(scl) {}

    public: void operator()(int idx) 
    { 
      to(idx2ij[idx] % n, idx2ij[idx] / n, 0) = scl * from[idx]; 
    }
  };

  /// @brief a Blitz-to-Thrust data transfer functor
  template <typename blitz_real_t, typename thrust_real_t> 
  class copy_to_device
  {
    private: int n;
    private: const mtx::arr<blitz_real_t> &from;
    private: thrust::device_vector<thrust_real_t> &to;
    private: thrust_real_t scl;

    // ctor
    public: copy_to_device(int n,
      const mtx::arr<blitz_real_t> &from,
      thrust::device_vector<thrust_real_t> &to,
      thrust_real_t scl = thrust_real_t(1)
    ) : n(n), from(from), to(to), scl(scl) {}

    public: void operator()(int ij)
    {
      to[ij] = scl * from(ij % n, ij / n, 0);
    }
  };

  /// @brief a functor interface to phc_lognormal.hpp routines
  template <typename thrust_real_t> 
  class lognormal
  {
    private: quantity<si::length, thrust_real_t> mean_rd1, mean_rd2;
    private: quantity<si::dimensionless, thrust_real_t> sdev_rd1, sdev_rd2;
    private: quantity<power_typeof_helper<si::length, static_rational<-3>>::type, thrust_real_t> n1_tot, n2_tot;
    
    public: lognormal(
      const quantity<si::length, thrust_real_t> mean_rd1,
      const quantity<si::dimensionless, thrust_real_t> sdev_rd1,
      const quantity<power_typeof_helper<si::length, static_rational<-3>>::type, thrust_real_t> n1_tot,
      const quantity<si::length, thrust_real_t> mean_rd2,
      const quantity<si::dimensionless, thrust_real_t> sdev_rd2,
      const quantity<power_typeof_helper<si::length, static_rational<-3>>::type, thrust_real_t> n2_tot
    ) : mean_rd1(mean_rd1), sdev_rd1(sdev_rd1), n1_tot(n1_tot), mean_rd2(mean_rd2), sdev_rd2(sdev_rd2), n2_tot(n2_tot) {}

    public: thrust_real_t operator()(thrust_real_t lnrd)
    {
      return thrust_real_t(( //TODO allow more modes of distribution
// TODO: logarith or not: as an option
          phc::log_norm_n_e<thrust_real_t>(mean_rd1, sdev_rd1, n1_tot, lnrd) + 
          phc::log_norm_n_e<thrust_real_t>(mean_rd2, sdev_rd2, n2_tot, lnrd) 
        ) * si::cubic_metres
      );
    }
  };

};
#  endif
#endif
