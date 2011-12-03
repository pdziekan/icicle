/** @file
 *  @author Sylwester Arabas <slayoo@igf.fuw.edu.pl>
 *  @copyright University of Warsaw
 *  @date November 2011
 *  @section LICENSE
 *    GPL v3 (see the COPYING file or http://www.gnu.org/licenses/)
 */
#ifndef DOM_SERIAL_HPP
#  define DOM_SERIAL_HPP

#  include "slv.hpp"
#  include "stp.hpp"

template <typename real_t>
class slv_serial : public slv<real_t>
{
  private: auto_ptr<arr<real_t> > *psi;
  private: Array<real_t, 3> **psi_ijk, **psi_jki, **psi_kij;
  private: adv<real_t> *fllbck, *advsch;
  private: auto_ptr<Range> i, j, k;
  private: out<real_t> *output;
  private: int nx, ny, nz, xhalo, yhalo, zhalo;
  private: auto_ptr<arr<real_t> > Cx, Cy, Cz;

  public: slv_serial(stp<real_t> *setup,
    int i_min, int i_max, int nx,
    int j_min, int j_max, int ny,
    int k_min, int k_max, int nz, 
    quantity<si::time, real_t> dt // TODO: dt should not be needed here!
  )
    : fllbck(setup->fllbck), advsch(setup->advsch), output(setup->output), nx(nx), ny(ny), nz(nz)
  {
    // memory allocation
    {
      int halo = (advsch->stencil_extent() - 1) / 2;
      xhalo = halo; // in extreme cases paralellisaion may make i_max = i_min
      yhalo = (j_max != j_min ? halo : 0);
      zhalo = (k_max != k_min ? halo : 0);
    }
    psi = new auto_ptr<arr<real_t> >[advsch->time_levels()];
    psi_ijk = new Array<real_t, 3>*[advsch->time_levels()]; 
    psi_jki = new Array<real_t, 3>*[advsch->time_levels()]; 
    psi_kij = new Array<real_t, 3>*[advsch->time_levels()]; 
    for (int n=0; n < advsch->time_levels(); ++n) 
    {
      psi[n].reset(new arr<real_t>(
        Range(i_min - xhalo, i_max + xhalo),
        Range(j_min - yhalo, j_max + yhalo),
        Range(k_min - zhalo, k_max + zhalo)
      ));
      psi_ijk[n] = &psi[n]->ijk();
      psi_jki[n] = &psi[n]->jki();
      psi_kij[n] = &psi[n]->kij();
    }
    i.reset(new Range(i_min, i_max));
    j.reset(new Range(j_min, j_max));
    k.reset(new Range(k_min, k_max));

    // initial condition
    setup->grid->populate_scalar_field(*i, *j, *k, psi_ijk[0], setup->intcond);

    // periodic boundary in all directions
    for (enum slv<real_t>::side s=this->first; s <= this->last; ++s) 
      this->hook_neighbour(s, this);
 
    // velocity fields
    {
      Range 
        ii = setup->grid->rng_vctr(i->first(), i->last()),
        jj = (j->first() != j->last())
          ? setup->grid->rng_vctr(j->first(), j->last())
          : Range(j->first(), j->last()),
        kk = (k->first() != k->last())
          ? setup->grid->rng_vctr(k->first(), k->last())
          : Range(k->first(), k->last());

      Cx.reset(new arr<real_t>(ii, jj, kk));
      Cy.reset(new arr<real_t>(ii, jj, kk));
      Cz.reset(new arr<real_t>(ii, jj, kk));

      setup->grid->populate_courant_fields(ii, jj, kk, &Cx->ijk(), &Cy->ijk(), &Cz->ijk(), setup->velocity, dt);
    }
  }

  public: ~slv_serial()
  {
    delete[] psi_jki;
    delete[] psi_kij;
    delete[] psi_ijk;
    delete[] psi;
  }

  public: void record(const int n, const unsigned long t)
  {
    output->record(psi_ijk, n, *i, *j, *k, t);
  }

  public: Array<real_t, 3> data(int n, 
    const Range &i, const Range &j, const Range &k) 
  { 
    return (*psi_ijk[n])(i, j, k);
  }

  public: void fill_halos(int n)
  { // TODO: make it much shorter!!!
    { // left halo
      int i_min = i->first() - xhalo, i_max = i->first() - 1;
      (*psi_ijk[n])(Range(i_min, i_max), *j, *k) = 
        this->nghbr_data(slv<real_t>::left, n, Range((i_min + nx) % nx, (i_max + nx) % nx), *j, *k);
    }
    { // rght halo
      int i_min = i->last() + 1, i_max = i->last() + xhalo;
      (*psi_ijk[n])(Range(i_min, i_max), *j, *k) =
        this->nghbr_data(slv<real_t>::rght, n, Range((i_min + nx) % nx, (i_max + nx) % nx), *j, *k);
    } 
    if (j->first() != j->last())
    {
      { // fore halo
        int j_min = j->first() - yhalo, j_max = j->first() - 1;
        (*psi_ijk[n])(*i, Range(j_min, j_max), *k) = 
          this->nghbr_data(slv<real_t>::fore, n, *i, Range((j_min + ny) % ny, (j_max + ny) % ny), *k);
      }
      { // hind halo
        int j_min = j->last() + 1, j_max = j->last() + yhalo;
        (*psi_ijk[n])(*i, Range(j_min, j_max), *k) =
          this->nghbr_data(slv<real_t>::hind, n, *i, Range((j_min + ny) % ny, (j_max + ny) % ny), *k);
      } 
    }
    if (k->first() != k->last())
    {
      { // base halo
        int k_min = k->first() - zhalo, k_max = k->first() - 1;
        (*psi_ijk[n])(*i, *j, Range(k_min, k_max)) = 
          this->nghbr_data(slv<real_t>::base, n, *i, *j, Range((k_min + nz) % nz, (k_max + nz) % nz));
      }
      { // apex halo
        int k_min = k->last() + 1, k_max = k->last() + zhalo;
        (*psi_ijk[n])(*i, *j, Range(k_min, k_max)) =
          this->nghbr_data(slv<real_t>::apex, n, *i, *j, Range((k_min + nz) % nz, (k_max + nz) % nz));
      } 
    }
  }

  public: void advect(adv<real_t> *a, int n, int s, quantity<si::time, real_t> dt)
  {
    // op() uses the -= operator so the first assignment happens here
    *psi_ijk[n+1] = *psi_ijk[0]; 

    if (true) // in extreme cases paralellisaion may make i->first() = i->last()
      a->op(psi_ijk, *i, *j, *k, n, s, Cx->ijk(), Cy->ijk(), Cz->ijk()); // X
    if (j->first() != j->last()) 
      a->op(psi_jki, *j, *k, *i, n, s, Cy->jki(), Cz->jki(), Cx->jki()); // Y
    if (k->first() != k->last()) 
      a->op(psi_kij, *k, *i, *j, n, s, Cz->kij(), Cx->kij(), Cy->kij()); // Z
  }

  public: void cycle_arrays(const int n)
  {
    switch (advsch->time_levels())
    {
      case 2: // e.g. upstream, mpdata
        cycleArrays(*psi_ijk[n], *psi_ijk[n+1]); 
        cycleArrays(*psi_jki[n], *psi_jki[n+1]); 
        cycleArrays(*psi_kij[n], *psi_kij[n+1]); 
        break;
      case 3: // e.g. leapfrog
        cycleArrays(*psi_ijk[n-1], *psi_ijk[n], *psi_ijk[n+1]); 
        cycleArrays(*psi_jki[n-1], *psi_jki[n], *psi_jki[n+1]); 
        cycleArrays(*psi_kij[n-1], *psi_kij[n], *psi_kij[n+1]); 
        break;
      default: assert(false);
    }
  }

  public: void integ_loop(unsigned long nt, quantity<si::time, real_t> dt) 
  {
    int n;
    for (unsigned long t = 0; t < nt; ++t)
    {   
      adv<real_t> *a;
      bool fallback = this->choose_an(&a, &n, t, advsch, fllbck);    
      record(n, t);
      for (int s = 1; s <= a->num_steps(); ++s)
      {   
        fill_halos(n);
        advect(a, n, s, dt); 
        if (!fallback) cycle_arrays(n);
      } // s
    } // t
    record(n, nt);
  }
};
#endif
