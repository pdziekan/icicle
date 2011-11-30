/** @file
 *  @author Sylwester Arabas <slayoo@igf.fuw.edu.pl>
 *  @copyright University of Warsaw
 *  @date November 2011
 *  @section LICENSE
 *    GPL v3 (see the COPYING file or http://www.gnu.org/licenses/)
 */
#ifndef SLV_HPP
#  define SLV_HPP

#  include "config.hpp" // USE_* defines
#  include "common.hpp" // root class, error reporting
#  include "adv.hpp"

template <class unit, typename real_t>
class slv : root
{
  private: slv *left, *rght;
  public: virtual void hook_neighbours(slv *l, slv *r) 
  { 
    if (l != NULL) left = l; 
    if (r != NULL) rght = r; 
  }

  public: virtual Array<quantity<unit, real_t>, 3> data(int n, 
    const Range &i, const Range &j, const Range &k
  ) = 0;

  public: Array<quantity<unit, real_t>, 3> left_nghbr_data(int n, 
    const Range &i, const Range &j, const Range &k) 
  { 
    left->sync(n);
    return left->data(n, i, j, k); 
  }
  public: Array<quantity<unit, real_t>, 3> rght_nghbr_data(int n, 
    const Range &i, const Range &j, const Range &k) 
  { 
    rght->sync(n); 
    return rght->data(n, i, j, k); 
  }

  public: bool choose_an(adv<unit, real_t> **a, int *n, int t, adv<unit, real_t> *advsch, adv<unit, real_t> *fllbck)
  {
    assert(advsch->time_levels() <= 3); // FIXME: support for other values
    bool fallback = (t == 0 && advsch->time_levels() == 3); 
    *a = fallback ? fllbck : advsch;
    *n = (*a)->time_levels() - 2;
    return fallback;
  }

  public: virtual void integ_loop(unsigned long nt, quantity<si::time, real_t> dt) = 0;
  public: virtual void sync(int n) {};
};

#endif
