extern int bispev_(real *tx, integer *nx, real *ty, integer *ny, real *c, 
		   integer *kx, integer *ky, real *x, integer *mx, real *y,
		   integer *my, real *z, real *wrk, integer *lwrk, 
		   integer *iwrk, integer *kwrk, integer *ier);
/*:ref: fpbisp_ 14 16 6 4 6 4 6 4 4 6 4 6 4 6 6 6 4 4 */

extern int clocur_(integer *iopt, integer *ipar, integer *idim, integer *m, 
		   real *u, integer *mx, real *x, real *w, integer *k,
		   real *s, integer *nest, integer *n, real *t, integer *nc,
		   real *c, real *fp, real *wrk, integer *lwrk, 
		   integer *iwrk, integer *ier);
/*:ref: fpchep_ 14 6 6 4 6 4 4 4 */
/*:ref: fpclos_ 14 29 4 4 4 6 4 6 6 4 6 4 6 4 4 4 4 6 4 6 6 6 6 6 6 6 \
  6 6 6 4 4 */

extern int cocosp_(integer *m, real *x, real *y, real *w, integer *n, real *t,
		   real *e, integer *maxtr, integer *maxbin, real *c,
		   real *sq, real *sx, logical *bind, real *wrk,
		   integer *lwrk, integer *iwrk, integer *kwrk, integer *ier);
/*:ref: fpchec_ 14 6 6 4 6 4 4 4 */
/*:ref: fpcosp_ 14 29 4 6 6 6 4 6 6 4 4 6 6 6 12 4 4 6 6 6 6 6 6 6 4 \
  4 4 4 4 4 4 */

extern int concon_(integer *iopt, integer *m, real *x, real *y, real *w,
		   real *v, real *s, integer *nest, integer *maxtr,
		   integer *maxbin, integer *n, real *t, real *c, real *sq,
		   real *sx, logical *bind, real *wrk, integer *lwrk,
		   integer *iwrk, integer *kwrk, integer *ier);
/*:ref: fpcoco_ 14 22 4 4 6 6 6 6 6 4 4 4 4 6 6 6 6 12 6 6 4 4 4 4 */

extern int concur_(integer *iopt, integer *idim, integer *m, real *u, 
		   integer *mx, real *x, real *xx, real *w, integer *ib, 
		   real *db, integer *nb, integer *ie, real *de,
		   integer *ne, integer *k, real *s, integer *nest, 
		   integer *n, real *t, integer *nc, real *c, integer *np, 
		   real *cp, real *fp, real *wrk, integer *lwrk, 
		   integer *iwrk, integer *ier);
/*:ref: fpched_ 14 8 6 4 6 4 4 4 4 4 */
/*:ref: fppocu_ 14 12 4 4 6 6 4 6 4 4 6 4 6 4 */
/*:ref: curev_ 14 11 4 6 4 6 4 4 6 4 6 4 4 */
/*:ref: fpcons_ 14 29 4 4 4 6 4 6 6 4 4 4 6 4 6 4 4 4 4 6 4 6 6 6 6 6 \
  6 6 6 4 4 */
/*:ref: fpadpo_ 14 11 4 6 4 6 4 4 6 4 6 6 6 */

extern int cualde_(integer *idim, real *t, integer *n, real *c, integer *nc, 
		   integer *k1, real *u, real *d, integer *nd, integer *ier);
/*:ref: fpader_ 14 7 6 4 6 4 6 4 6 */

extern int curev_(integer *idim, real *t, integer *n, real *c, integer *nc,
		  integer *k, real *u, integer *m, real *x, integer *mx,
		  integer *ier);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */

extern int curfit_(integer *iopt, integer *m, real *x, real *y, real *w,
		   real *xb, real *xe, integer *k, real *s, integer *nest,
		   integer *n, real *t, real *c, real *fp, real *wrk,
		   integer *lwrk, integer *iwrk, integer *ier);
/*:ref: fpchec_ 14 6 6 4 6 4 4 4 */
/*:ref: fpcurf_ 14 26 4 6 6 6 4 6 6 4 6 4 6 4 4 4 4 6 6 6 6 6 6 6 6 6 4 4 */

extern E_f dblint_(real *tx, integer *nx, real *ty, integer *ny, real *c, 
		   integer *kx, integer *ky, real *xb, real *xe, real *yb, 
		   real *ye, real *wrk);
/*:ref: fpintb_ 14 6 6 4 6 4 6 6 */

extern E_f evapol_(real *tu, integer *nu, real *tv, integer *nv, real *c, 
		   E_fp rad, real *x, real *y);
/*:ref: bispev_ 14 17 6 4 6 4 6 4 4 6 4 6 4 6 6 4 4 4 4 */

extern int fourco_(real *t, integer *n, real *c, real *alfa, integer *m, 
		   real *ress, real *resc, real *wrk1, real *wrk2, 
		   integer *ier);
/*:ref: fpbfou_ 14 5 6 4 6 6 6 */

extern int fpader_(real *t, integer *n, real *c, integer *k1, real *x, 
		   integer *l, real *d);
extern int fpadno_(integer *maxtr, integer *up, integer *left, 
		   integer *right, integer *info, integer *count, 
		   integer *merk, integer *jbind, integer *n1, integer *ier);
/*:ref: fpfrno_ 14 10 4 4 4 4 4 4 4 4 4 4 */

extern int fpadpo_(integer *idim, real *t, integer *n, real *c,
		   integer *nc, integer *k, real *cp, integer *np,
		   real *cc, real *t1, real *t2);
/*:ref: fpinst_ 14 11 4 6 4 6 4 6 4 6 4 6 4 */

extern int fpback_(real *a, real *z, integer *n, integer *k, real *c, 
		   integer *nest);
extern int fpbacp_(real *a, real *b, real *z, integer *n, integer *k,
		   real *c, integer *k1, integer *nest);
extern int fpbfou_(real *t, integer *n, real *par, real *ress, real *resc);
/*:ref: fpcsin_ 14 9 6 6 6 6 6 6 6 6 6 */

extern int fpbisp_(real *tx, integer *nx, real *ty, integer *ny, real *c,
		   integer *kx, integer *ky, real *x, integer *mx, real *y, 
		   integer *my, real *z, real *wx, real *wy, integer *lx,
		   integer *ly);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */

extern int fpbspl_(real *t, integer *n, integer *k, real *x, integer *l, 
		   real *h);
extern int fpchec_(real *x, integer *m, real *t, integer *n, integer *k,
		   integer *ier);
extern int fpched_(real *x, integer *m, real *t, integer *n, integer *k,
		   integer *ib, integer *ie, integer *ier);
extern int fpchep_(real *x, integer *m, real *t, integer *n, integer *k,
		   integer *ier);
extern int fpclos_(integer *iopt, integer *idim, integer *m, real *u,
		   integer *mx, real *x, real *w, integer *k, real *s,
		   integer *nest, real *tol, integer *maxit, integer *k1,
		   integer *k2, integer *n, real *t, integer *nc, real *c,
		   real *fp, real *fpint, real *z, real *a1, real *a2,
		   real *b, real *g1, real *g2, real *q, integer *nrdata,
		   integer *ier);
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpbacp_ 14 8 6 6 6 4 4 6 4 4 */
/*:ref: fpknot_ 14 9 6 4 6 4 6 4 4 4 4 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fpcoco_(integer *iopt, integer *m, real *x, real *y, real *w, 
		   real *v, real *s, integer *nest, integer *maxtr,
		   integer *maxbin, integer *n, real *t, real *c, real *sq,
		   real *sx, logical *bind, real *e, real *wrk, integer *lwrk,
		   integer *iwrk, integer *kwrk, integer *ier);
/*:ref: fpcosp_ 14 29 4 6 6 6 4 6 6 4 4 6 6 6 12 4 4 6 6 6 6 6 6 6 4 \
  4 4 4 4 4 4 */

extern int fpcons_(integer *iopt, integer *idim, integer *m, real *u,
		   integer *mx, real *x, real *w, integer *ib, integer *ie,
		   integer *k, real *s, integer *nest, real *tol,
		   integer *maxit, integer *k1, integer *k2, integer *n,
		   real *t, integer *nc, real *c, real *fp, real *fpint,
		   real *z, real *a, real *b, real *g, real *q,
		   integer *nrdata, integer *ier);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */
/*:ref: fpback_ 14 6 6 6 4 4 6 4 */
/*:ref: fpknot_ 14 9 6 4 6 4 6 4 4 4 4 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fpcosp_(integer *m, real *x, real *y, real *w, integer *n, real *t,
		   real *e, integer *maxtr, integer *maxbin, real *c,
		   real *sq, real *sx, logical *bind, integer *nm,
		   integer *mb, real *a, real *b, real *const__, real *z,
		   real *zz, real *u, real *q, integer *info, integer *up,
		   integer *left, integer *right, integer *jbind,
		   integer *ibind, integer *ier);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpadno_ 14 10 4 4 4 4 4 4 4 4 4 4 */
/*:ref: fpdeno_ 14 6 4 4 4 4 4 4 */
/*:ref: fpseno_ 14 8 4 4 4 4 4 4 4 4 */

extern int fpcsin_(real *a, real *b, real *par, real *sia, real *coa,
		   real *sib, real *cob, real *ress, real *resc);
extern int fpcurf_(integer *iopt, real *x, real *y, real *w, integer *m,
		   real *xb, real *xe, integer *k, real *s, integer *nest,
		   real *tol, integer *maxit, integer *k1, integer *k2,
		   integer *n, real *t, real *c, real *fp, real *fpint,
		   real *z, real *a, real *b, real *g, real *q,
		   integer *nrdata, integer *ier);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */
/*:ref: fpback_ 14 6 6 6 4 4 6 4 */
/*:ref: fpknot_ 14 9 6 4 6 4 6 4 4 4 4 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fpcuro_(real *a, real *b, real *c, real *d, real *x, integer *n);
extern int fpcyt1_(real *a, integer *n, integer *nn);
extern int fpcyt2_(real *a, integer *n, real *b, real *c, integer *nn);
extern int fpdeno_(integer *maxtr, integer *up, integer *left, integer *right,
		   integer *nbind, integer *merk);
extern int fpdisc_(real *t, integer *n, integer *k2, real *b, integer *nest);
extern int fpfrno_(integer *maxtr, integer *up, integer *left, integer *right,
		   integer *info, integer *point, integer *merk, integer *n1,
		   integer *count, integer *ier);
extern int fpgivs_(real *piv, real *ww, real *cos__, real *sin__);
extern int fpgrdi_(integer *ifsu, integer *ifsv, integer *ifbu, integer *ifbv,
		   integer *iback, real *u, integer *mu, real *v, integer *mv,
		   real *z, integer *mz, real *dz, integer *iop0,
		   integer *iop1, real *tu, integer *nu, real *tv,
		   integer *nv, real *p, real *c, integer *nc, real *sq,
		   real *fp, real *fpu, real *fpv, integer *mm, integer *mvnu,
		   real *spu, real *spv, real *right, real *q, real *au,
		   real *av1, real *av2, real *bu, real *bv, real *aa,
		   real *bb, real *cc, real *cosi, integer *nru, integer *nrv);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpcyt1_ 14 3 6 4 4 */
/*:ref: fpcyt2_ 14 5 6 4 6 6 4 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */
/*:ref: fpbacp_ 14 8 6 6 6 4 4 6 4 4 */
/*:ref: fpback_ 14 6 6 6 4 4 6 4 */

extern int fpgrpa_(integer *ifsu, integer *ifsv, integer *ifbu, integer *ifbv,
		   integer *idim, integer *ipar, real *u, integer *mu,
		   real *v, integer *mv, real *z, integer *mz, real *tu,
		   integer *nu, real *tv, integer *nv, real *p, real *c,
		   integer *nc, real *fp, real *fpu, real *fpv, integer *mm,
		   integer *mvnu, real *spu, real *spv, real *right, real *q,
		   real *au, real *au1, real *av, real *av1, real *bu,
		   real *bv, integer *nru, integer *nrv);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fptrnp_ 14 12 4 4 4 4 4 6 6 6 6 6 6 6 */
/*:ref: fptrpe_ 14 13 4 4 4 4 4 6 6 6 6 6 6 6 6 */
/*:ref: fpback_ 14 6 6 6 4 4 6 4 */
/*:ref: fpbacp_ 14 8 6 6 6 4 4 6 4 4 */

extern int fpgrre_(integer *ifsx, integer *ifsy, integer *ifbx, integer *ifby,
		   real *x, integer *mx, real *y, integer *my, real *z,
		   integer *mz, integer *kx, integer *ky, real *tx,
		   integer *nx, real *ty, integer *ny, real *p, real *c,
		   integer *nc, real *fp, real *fpx, real *fpy, integer *mm,
		   integer *mynx, integer *kx1, integer *kx2, integer *ky1,
		   integer *ky2, real *spx, real *spy, real *right, real *q,
		   real *ax, real *ay, real *bx, real *by, integer *nrx,
		   integer *nry);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */
/*:ref: fpback_ 14 6 6 6 4 4 6 4 */

extern int fpgrsp_(integer *ifsu, integer *ifsv, integer *ifbu, integer *ifbv,
		   integer *iback, real *u, integer *mu, real *v, integer *mv,
		   real *r, integer *mr, real *dr, integer *iop0,
		   integer *iop1, real *tu, integer *nu, real *tv,
		   integer *nv, real *p, real *c, integer *nc, real *sq,
		   real *fp, real *fpu, real *fpv, integer *mm, integer *mvnu,
		   real *spu, real *spv, real *right, real *q, real *au,
		   real *av1, real *av2, real *bu, real *bv, real *a0,
		   real *a1, real *b0, real *b1, real *c0, real *c1,
		   real *cosi, integer *nru, integer *nrv);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpcyt1_ 14 3 6 4 4 */
/*:ref: fpcyt2_ 14 5 6 4 6 6 4 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */
/*:ref: fpbacp_ 14 8 6 6 6 4 4 6 4 4 */
/*:ref: fpback_ 14 6 6 6 4 4 6 4 */

extern int fpinst_(integer *iopt, real *t, integer *n, real *c, integer *k,
		   real *x, integer *l, real *tt, integer *nn, real *cc,
		   integer *nest);
extern int fpintb_(real *t, integer *n, real *bint, integer *nk1, real *x,
		   real *y);
extern int fpknot_(real *x, integer *m, real *t, integer *n, real *fpint,
		   integer *nrdata, integer *nrint, integer *nest,
		   integer *istart);
extern int fpopdi_(integer *ifsu, integer *ifsv, integer *ifbu, integer *ifbv,
		   real *u, integer *mu, real *v, integer *mv, real *z,
		   integer *mz, real *z0, real *dz, integer *iopt,
		   integer *ider, real *tu, integer *nu, real *tv,
		   integer *nv, integer *nuest, integer *nvest, real *p,
		   real *step, real *c, integer *nc, real *fp, real *fpu,
		   real *fpv, integer *nru, integer *nrv, real *wrk,
		   integer *lwrk);
/*:ref: fpgrdi_ 14 42 4 4 4 4 4 6 4 6 4 6 4 6 4 4 6 4 6 4 6 6 4 6 6 6 \
  6 4 4 6 6 6 6 6 6 6 6 6 6 6 6 6 4 4 */
/*:ref: fpsysy_ 14 3 6 4 6 */

extern int fpopsp_(integer *ifsu, integer *ifsv, integer *ifbu, integer *ifbv,
		   real *u, integer *mu, real *v, integer *mv, real *r,
		   integer *mr, real *r0, real *r1, real *dr, integer *iopt,
		   integer *ider, real *tu, integer *nu, real *tv,
		   integer *nv, integer *nuest, integer *nvest, real *p,
		   real *step, real *c, integer *nc, real *fp, real *fpu,
		   real *fpv, integer *nru, integer *nrv, real *wrk,
		   integer *lwrk);
/*:ref: fpgrsp_ 14 45 4 4 4 4 4 6 4 6 4 6 4 6 4 4 6 4 6 4 6 6 4 6 6 6 \
  6 4 4 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 4 4 */
/*:ref: fpsysy_ 14 3 6 4 6 */

extern int fporde_(real *x, real *y, integer *m, integer *kx, integer *ky,
		   real *tx, integer *nx, real *ty, integer *ny,
		   integer *nummer, integer *index, integer *nreg);
extern int fppara_(integer *iopt, integer *idim, integer *m, real *u,
		   integer *mx, real *x, real *w, real *ub, real *ue,
		   integer *k, real *s, integer *nest, real *tol,
		   integer *maxit, integer *k1, integer *k2, integer *n,
		   real *t, integer *nc, real *c, real *fp, real *fpint,
		   real *z, real *a, real *b, real *g, real *q,
		   integer *nrdata, integer *ier);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */
/*:ref: fpback_ 14 6 6 6 4 4 6 4 */
/*:ref: fpknot_ 14 9 6 4 6 4 6 4 4 4 4 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fppasu_(integer *iopt, integer *ipar, integer *idim, real *u,
		   integer *mu, real *v, integer *mv, real *z, integer *mz,
		   real *s, integer *nuest, integer *nvest, real *tol,
		   integer *maxit, integer *nc, integer *nu, real *tu,
		   integer *nv, real *tv, real *c, real *fp, real *fp0,
		   real *fpold, real *reducu, real *reducv, real *fpintu,
		   real *fpintv, integer *lastdi, integer *nplusu,
		   integer *nplusv, integer *nru, integer *nrv,
		   integer *nrdatu, integer *nrdatv, real *wrk, integer *lwrk,
		   integer *ier);
/*:ref: fpgrpa_ 14 36 4 4 4 4 4 4 6 4 6 4 6 4 6 4 6 4 6 6 4 6 6 6 4 4 \
  6 6 6 6 6 6 6 6 6 6 4 4 */
/*:ref: fpknot_ 14 9 6 4 6 4 6 4 4 4 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fpperi_(integer *iopt, real *x, real *y, real *w, integer *m,
		   integer *k, real *s, integer *nest, real *tol,
		   integer *maxit, integer *k1, integer *k2, integer *n,
		   real *t, real *c, real *fp, real *fpint, real *z, real *a1,
		   real *a2, real *b, real *g1, real *g2, real *q,
		   integer *nrdata, integer *ier);
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpbacp_ 14 8 6 6 6 4 4 6 4 4 */
/*:ref: fpknot_ 14 9 6 4 6 4 6 4 4 4 4 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fppocu_(integer *idim, integer *k, real *a, real *b, integer *ib,
		   real *db, integer *nb, integer *ie, real *de, integer *ne,
		   real *cp, integer *np);
extern int fppogr_(integer *iopt, integer *ider, real *u, integer *mu,
		   real *v, integer *mv, real *z, integer *mz, real *z0,
		   real *r, real *s, integer *nuest, integer *nvest,
		   real *tol, integer *maxit, integer *nc, integer *nu,
		   real *tu, integer *nv, real *tv, real *c, real *fp,
		   real *fp0, real *fpold, real *reducu, real *reducv,
		   real *fpintu, real *fpintv, real *dz, real *step,
		   integer *lastdi, integer *nplusu, integer *nplusv,
		   integer *lasttu, integer *nru, integer *nrv,
		   integer *nrdatu, integer *nrdatv, real *wrk, integer *lwrk,
		   integer *ier);
/*:ref: fpopdi_ 14 31 4 4 4 4 6 4 6 4 6 4 6 6 4 4 6 4 6 4 4 4 6 6 6 4 \
  6 6 6 4 4 6 4 */
/*:ref: fpknot_ 14 9 6 4 6 4 6 4 4 4 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fppola_(integer *iopt1, integer *iopt2, integer *iopt3, integer *m,
		   real *u, real *v, real *z, real *w, E_fp rad, real *s,
		   integer *nuest, integer *nvest, real *eta, real *tol,
		   integer *maxit, integer *ib1, integer *ib3, integer *nc,
		   integer *ncc, integer *intest, integer *nrest, integer *nu,
		   real *tu, integer *nv, real *tv, real *c, real *fp,
		   real *sup, real *fpint, real *coord, real *f, real *ff,
		   real *row, real *cs, real *cosi, real *a, real *q,
		   real *bu, real *bv, real *spu, real *spv, real *h,
		   integer *index, integer *nummer, real *wrk, integer *lwrk,
		   integer *ier);
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */
/*:ref: fporde_ 14 12 6 6 4 4 4 6 4 6 4 4 4 4 */
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpback_ 14 6 6 6 4 4 6 4 */
/*:ref: fprank_ 14 12 6 6 4 4 4 6 6 6 4 6 6 6 */
/*:ref: fprppo_ 14 9 4 4 4 4 6 6 6 6 4 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fprank_(real *a, real *f, integer *n, integer *m, integer *na,
		   real *tol, real *c, real *sq, integer *rank, real *aa,
		   real *ff, real *h);
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */

extern E_f fprati_(real *p1, real *f1, real *p2, real *f2, real *p3,
		   real *f3);
extern int fpregr_(integer *iopt, real *x, integer *mx, real *y, integer *my,
		   real *z, integer *mz, real *xb, real *xe, real *yb,
		   real *ye, integer *kx, integer *ky, real *s,
		   integer *nxest, integer *nyest, real *tol, integer *maxit,
		   integer *nc, integer *nx, real *tx, integer *ny, real *ty,
		   real *c, real *fp, real *fp0, real *fpold, real *reducx,
		   real *reducy, real *fpintx, real *fpinty, integer *lastdi,
		   integer *nplusx, integer *nplusy, integer *nrx,
		   integer *nry, integer *nrdatx, integer *nrdaty, real *wrk,
		   integer *lwrk, integer *ier);
/*:ref: fpgrre_ 14 38 4 4 4 4 6 4 6 4 6 4 4 4 6 4 6 4 6 6 4 6 6 6 4 4 \
  4 4 4 4 6 6 6 6 6 6 6 6 4 4 */
/*:ref: fpknot_ 14 9 6 4 6 4 6 4 4 4 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fprota_(real *cos__, real *sin__, real *a, real *b);
extern int fprppo_(integer *nu, integer *nv, integer *if1, integer *if2,
		   real *cosi, real *ratio, real *c, real *f, integer *ncoff);
extern int fprpsp_(integer *nt, integer *np, real *co, real *si, real *c,
		   real *f, integer *ncoff);
extern int fpseno_(integer *maxtr, integer *up, integer *left, integer *right,
		   integer *info, integer *merk, integer *ibind,
		   integer *nbind);
extern int fpspgr_(integer *iopt, integer *ider, real *u, integer *mu,
		   real *v, integer *mv, real *r, integer *mr, real *r0,
		   real *r1, real *s, integer *nuest, integer *nvest,
		   real *tol, integer *maxit, integer *nc, integer *nu,
		   real *tu, integer *nv, real *tv, real *c, real *fp,
		   real *fp0, real *fpold, real *reducu, real *reducv,
		   real *fpintu, real *fpintv, real *dr, real *step,
		   integer *lastdi, integer *nplusu, integer *nplusv,
		   integer *lastu0, integer *lastu1, integer *nru,
		   integer *nrv, integer *nrdatu, integer *nrdatv, real *wrk,
		   integer *lwrk, integer *ier);
/*:ref: fpopsp_ 14 32 4 4 4 4 6 4 6 4 6 4 6 6 6 4 4 6 4 6 4 4 4 6 6 6 \
  4 6 6 6 4 4 6 4 */
/*:ref: fpknot_ 14 9 6 4 6 4 6 4 4 4 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fpsphe_(integer *iopt, integer *m, real *teta, real *phi, real *r,
		   real *w, real *s, integer *ntest, integer *npest,
		   real *eta, real *tol, integer *maxit, integer *ib1,
		   integer *ib3, integer *nc, integer *ncc, integer *intest,
		   integer *nrest, integer *nt, real *tt, integer *np,
		   real *tp, real *c, real *fp, real *sup, real *fpint,
		   real *coord, real *f, real *ff, real *row, real *coco,
		   real *cosi, real *a, real *q, real *bt, real *bp,
		   real *spt, real *spp, real *h, integer *index,
		   integer *nummer, real *wrk, integer *lwrk, integer *ier);
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */
/*:ref: fporde_ 14 12 6 6 4 4 4 6 4 6 4 4 4 4 */
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpback_ 14 6 6 6 4 4 6 4 */
/*:ref: fprank_ 14 12 6 6 4 4 4 6 6 6 4 6 6 6 */
/*:ref: fprpsp_ 14 7 4 4 6 6 6 6 4 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fpsuev_(integer *idim, real *tu, integer *nu, real *tv,
		   integer *nv, real *c, real *u, integer *mu, real *v,
		   integer *mv, real *f, real *wu, real *wv, integer *lu,
		   integer *lv);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */

extern int fpsurf_(integer *iopt, integer *m, real *x, real *y, real *z,
		   real *w, real *xb, real *xe, real *yb, real *ye,
		   integer *kxx, integer *kyy, real *s, integer *nxest,
		   integer *nyest, real *eta, real *tol, integer *maxit,
		   integer *nmax, integer *km1, integer *km2, integer *ib1,
		   integer *ib3, integer *nc, integer *intest, integer *nrest,
		   integer *nx0, real *tx, integer *ny0, real *ty, real *c,
		   real *fp, real *fp0, real *fpint, real *coord, real *f,
		   real *ff, real *a, real *q, real *bx, real *by, real *spx,
		   real *spy, real *h, integer *index, integer *nummer,
		   real *wrk, integer *lwrk, integer *ier);
/*:ref: fporde_ 14 12 6 6 4 4 4 6 4 6 4 4 4 4 */
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */
/*:ref: fpback_ 14 6 6 6 4 4 6 4 */
/*:ref: fprank_ 14 12 6 6 4 4 4 6 6 6 4 6 6 6 */
/*:ref: fpdisc_ 14 5 6 4 4 6 4 */
/*:ref: fprati_ 6 6 6 6 6 6 6 6 */

extern int fpsysy_(real *a, integer *n, real *g);
extern int fptrnp_(integer *m, integer *mm, integer *idim, integer *n,
		   integer *nr, real *sp, real *p, real *b, real *z, real *a,
		   real *q, real *right);
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */

extern int fptrpe_(integer *m, integer *mm, integer *idim, integer *n,
		   integer *nr, real *sp, real *p, real *b, real *z, real *a,
		   real *aa, real *q, real *right);
/*:ref: fpgivs_ 14 4 6 6 6 6 */
/*:ref: fprota_ 14 4 6 6 6 6 */

extern int insert_(integer *iopt, real *t, integer *n, real *c, integer *k,
		   real *x, real *tt, integer *nn, real *cc, integer *nest,
		   integer *ier);
/*:ref: fpinst_ 14 11 4 6 4 6 4 6 4 6 4 6 4 */

extern int parcur_(integer *iopt, integer *ipar, integer *idim, integer *m,
		   real *u, integer *mx, real *x, real *w, real *ub, real *ue,
		   integer *k, real *s, integer *nest, integer *n, real *t,
		   integer *nc, real *c, real *fp, real *wrk, integer *lwrk,
		   integer *iwrk, integer *ier);
/*:ref: fpchec_ 14 6 6 4 6 4 4 4 */
/*:ref: fppara_ 14 29 4 4 4 6 4 6 6 6 6 4 6 4 6 4 4 4 4 6 4 6 6 6 6 6 \
  6 6 6 4 4 */

extern int parder_(real *tx, integer *nx, real *ty, integer *ny, real *c,
		   integer *kx, integer *ky, integer *nux, integer *nuy,
		   real *x, integer *mx, real *y, integer *my, real *z,
		   real *wrk, integer *lwrk, integer *iwrk, integer *kwrk,
		   integer *ier);
/*:ref: fpbisp_ 14 16 6 4 6 4 6 4 4 6 4 6 4 6 6 6 4 4 */

extern int parsur_(integer *iopt, integer *ipar, integer *idim, integer *mu,
		   real *u, integer *mv, real *v, real *f, real *s,
		   integer *nuest, integer *nvest, integer *nu, real *tu,
		   integer *nv, real *tv, real *c, real *fp, real *wrk,
		   integer *lwrk, integer *iwrk, integer *kwrk, integer *ier);
/*:ref: fpchec_ 14 6 6 4 6 4 4 4 */
/*:ref: fpchep_ 14 6 6 4 6 4 4 4 */
/*:ref: fppasu_ 14 37 4 4 4 6 4 6 4 6 4 6 4 4 6 4 4 4 6 4 6 6 6 6 6 6 \
  6 6 6 4 4 4 4 4 4 4 6 4 4 */

extern int percur_(integer *iopt, integer *m, real *x, real *y, real *w,
		   integer *k, real *s, integer *nest, integer *n, real *t,
		   real *c, real *fp, real *wrk, integer *lwrk, integer *iwrk,
		   integer *ier);
/*:ref: fpchep_ 14 6 6 4 6 4 4 4 */
/*:ref: fpperi_ 14 26 4 6 6 6 4 4 6 4 6 4 4 4 4 6 6 6 6 6 6 6 6 6 6 6 4 4 */

extern int pogrid_(integer *iopt, integer *ider, integer *mu, real *u,
		   integer *mv, real *v, real *z, real *z0, real *r, real *s,
		   integer *nuest, integer *nvest, integer *nu, real *tu,
		   integer *nv, real *tv, real *c, real *fp, real *wrk,
		   integer *lwrk, integer *iwrk, integer *kwrk, integer *ier);
/*:ref: fpchec_ 14 6 6 4 6 4 4 4 */
/*:ref: fpchep_ 14 6 6 4 6 4 4 4 */
/*:ref: fppogr_ 14 41 4 4 6 4 6 4 6 4 6 6 6 4 4 6 4 4 4 6 4 6 6 6 6 6 \
  6 6 6 6 6 6 4 4 4 4 4 4 4 4 6 4 4 */

extern int polar_(integer *iopt, integer *m, real *x, real *y, real *z,
		  real *w, E_fp rad, real *s, integer *nuest, integer *nvest,
		  real *eps, integer *nu, real *tu, integer *nv, real *tv,
		  real *u, real *v, real *c, real *fp, real *wrk1,
		  integer *lwrk1, real *wrk2, integer *lwrk2, integer *iwrk,
		  integer *kwrk, integer *ier);
/*:ref: fppola_ 14 47 4 4 4 4 6 6 6 6 206 6 4 4 6 6 4 4 4 4 4 4 4 4 6 \
  4 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 4 4 6 4 4 */

extern int profil_(integer *iopt, real *tx, integer *nx, real *ty,
		   integer *ny, real *c, integer *kx, integer *ky, real *u,
		   integer *nu, real *cu, integer *ier);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */

extern int regrid_(integer *iopt, integer *mx, real *x, integer *my, real *y,
		   real *z, real *xb, real *xe, real *yb, real *ye,
		   integer *kx, integer *ky, real *s, integer *nxest,
		   integer *nyest, integer *nx, real *tx, integer *ny,
		   real *ty, real *c, real *fp, real *wrk, integer *lwrk,
		   integer *iwrk, integer *kwrk, integer *ier);
/*:ref: fpchec_ 14 6 6 4 6 4 4 4 */
/*:ref: fpregr_ 14 41 4 6 4 6 4 6 4 6 6 6 6 4 4 6 4 4 6 4 4 4 6 4 6 6 \
  6 6 6 6 6 6 6 4 4 4 4 4 4 4 6 4 4 */

extern int spalde_(real *t, integer *n, real *c, integer *k1, real *x,
		   real *d, integer *ier);
/*:ref: fpader_ 14 7 6 4 6 4 6 4 6 */

extern int spgrid_(integer *iopt, integer *ider, integer *mu, real *u,
		   integer *mv, real *v, real *r, real *r0, real *r1, real *s,
		   integer *nuest, integer *nvest, integer *nu, real *tu,
		   integer *nv, real *tv, real *c, real *fp, real *wrk,
		   integer *lwrk, integer *iwrk, integer *kwrk, integer *ier);
/*:ref: fpchec_ 14 6 6 4 6 4 4 4 */
/*:ref: fpchep_ 14 6 6 4 6 4 4 4 */
/*:ref: fpspgr_ 14 42 4 4 6 4 6 4 6 4 6 6 6 4 4 6 4 4 4 6 4 6 6 6 6 6 \
  6 6 6 6 6 6 4 4 4 4 4 4 4 4 4 6 4 4 */

extern int sphere_(integer *iopt, integer *m, real *teta, real *phi, real *r,
		   real *w, real *s, integer *ntest, integer *npest,
		   real *eps, integer *nt, real *tt, integer *np, real *tp,
		   real *c, real *fp, real *wrk1, integer *lwrk1, real *wrk2,
		   integer *lwrk2, integer *iwrk, integer *kwrk, integer *ier);
/*:ref: fpsphe_ 14 44 4 4 6 6 6 6 6 4 4 6 6 4 4 4 4 4 4 4 4 6 4 6 6 6 \
  6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 4 4 6 4 4 */

extern int splder_(real *t, integer *n, real *c, integer *k, integer *nu,
		   real *x, real *y, integer *m, real *wrk, integer *ier);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */

extern int splev_(real *t, integer *n, real *c, integer *k, real *x, real *y,
		  integer *m, integer *ier);
/*:ref: fpbspl_ 14 6 6 4 4 6 4 6 */

extern E_f splint_(real *t, integer *n, real *c, integer *k, real *a, real *b,
		   real *wrk);
/*:ref: fpintb_ 14 6 6 4 6 4 6 6 */

extern int sproot_(real *t, integer *n, real *c, real *zero, integer *mest,
		   integer *m, integer *ier);
/*:ref: fpcuro_ 14 6 6 6 6 6 6 4 */

extern int surev_(integer *idim, real *tu, integer *nu, real *tv, integer *nv,
		  real *c, real *u, integer *mu, real *v, integer *mv,
		  real *f, integer *mf, real *wrk, integer *lwrk,
		  integer *iwrk, integer *kwrk, integer *ier);
/*:ref: fpsuev_ 14 15 4 6 4 6 4 6 6 4 6 4 6 6 6 4 4 */

extern int surfit_(integer *iopt, integer *m, real *x, real *y, real *z,
		   real *w, real *xb, real *xe, real *yb, real *ye,
		   integer *kx, integer *ky, real *s, integer *nxest,
		   integer *nyest, integer *nmax, real *eps, integer *nx,
		   real *tx, integer *ny, real *ty, real *c, real *fp,
		   real *wrk1, integer *lwrk1, real *wrk2, integer *lwrk2,
		   integer *iwrk, integer *kwrk, integer *ier);
/*:ref: fpsurf_ 14 49 4 4 6 6 6 6 6 6 6 6 4 4 6 4 4 6 6 4 4 4 4 4 4 4 \
  4 4 4 6 4 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 4 4 6 4 4 */
