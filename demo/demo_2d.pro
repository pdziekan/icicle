;; @file
;  @author Sylwester Arabas (University of Warsaw) <slayoo@igf.fuw.edu.pl>
;  @date November 2011
;  @section LICENSE
;    GPL v3 (see the COPYING file or http://www.gnu.org/licenses/)
;  @section DESCRIPTION
;    example usage: 
;    GDL> demo_2D, nx=20, nz=30, nt=100, adv='mpdata', vel='uniform --vel.uniform.u .2 --vel.uniform.v 0 --vel.uniform.w .3'
;    GDL> demo_2D, nx=20, nz=30, nt=100, adv='mpdata', vel='rasinski --vel.rasinski.A 1 --vel.rasinski.Z_top 30 --vel.rasinski.Z_clb 20'

pro demo_2D, nx=nx, nz=nz, nt=nt, adv=adv, vel=vel
  spawn, 'rm -f demo_2d.nc'
  cmd = '../icicle' + $
    ' --bits 32 --dt 1 --dx 1 --dy 1 --dz 1 --ny 1' + $
    ' --nt ' +  strmid(nt, 2) + $
    ' --nx ' +  strmid(nx, 2) + $
    ' --nz ' +  strmid(nz, 2) + $
    ' --vel ' + vel + $
    ' --adv ' + adv + $
    ' --dom serial --nsd 2' + $
    ' --out netcdf --out.netcdf.file demo_2d.nc'
  spawn, cmd
  a = ncdf_open('demo_2d.nc') 
  scl = 20
  window, xsize=scl*nx, ysize=scl*nz
  loadct, 1
  i=0l
  while i lt 100 * nt do begin ; repeat 100 times...
    ncdf_varget, a, 'psi', p, offset=[0,0,0,i++ mod nt], count=[nz, 1, nx, 1]
    p = reform(p[*,0,*,0]) ; removing dimensions of size one
    p = rotate(p, 4)
    tvscl, rebin(p, scl*nx, scl*nz, /sample)
  endwhile
end
