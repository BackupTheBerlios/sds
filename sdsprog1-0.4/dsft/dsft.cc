/* wrapper to use the fft routines of Ron Mayer with sds
 *  Copyright 1999; Roland Schäfer
 *  Copyright 1988, 1993; Ron Mayer
 */

/*
** FFT and FHT routines
**  Copyright 1988, 1993; Ron Mayer
**  
**  fht(fz,n);
**      Does a hartley transform of "n" points in the array "fz".
**  fft(n,real,imag)
**      Does a fourier transform of "n" points of the "real" and
**      "imag" arrays.
**  ifft(n,real,imag)
**      Does an inverse fourier transform of "n" points of the "real"
**      and "imag" arrays.
**  realfft(n,real)
**      Does a real-valued fourier transform of "n" points of the
**      "real" and "imag" arrays.  The real part of the transform ends
**      up in the first half of the array and the imaginary part of the
**      transform ends up in the second half of the array.
**  realifft(n,real)
**      The inverse of the realfft() routine above.
**      
**      
** NOTE: This routine uses at least 2 patented algorithms, and may be
**       under the restrictions of a bunch of different organizations.
**       Although I wrote it completely myself; it is kind of a derivative
**       of a routine I once authored and released under the GPL, so it
**       may fall under the free software foundation's restrictions;
**       it was worked on as a Stanford Univ project, so they claim
**       some rights to it; it was further optimized at work here, so
**       I think this company claims parts of it.  The patents are
**       held by R. Bracewell (the FHT algorithm) and O. Buneman (the
**       trig generator), both at Stanford Univ.
**       If it were up to me, I'd say go do whatever you want with it;
**       but it would be polite to give credit to the following people
**       if you use this anywhere:
**           Euler     - probable inventor of the fourier transform.
**           Gauss     - probable inventor of the FFT.
**           Hartley   - probable inventor of the hartley transform.
**           Buneman   - for a really cool trig generator
**           Mayer(me) - for authoring this particular version and
**                       including all the optimizations in one package.
**       Thanks,
**       Ron Mayer; mayer@acuson.com
**
*/

#define GOOD_TRIG
#include "trigtbl.h"
char fht_version[] = "Brcwl-Hrtly-Ron-dbld";

#define SQRT2_2   0.70710678118654752440084436210484
#define SQRT2   2*0.70710678118654752440084436210484
void fht(double *fz, int n)
{
	int i, k, k1, k2, k3, k4, kx;
	double *fi, *fn, *gi;
	TRIG_VARS;

	for (k1 = 1, k2 = 0; k1 < n; k1++) {
		double a;
		for (k = n >> 1; (!((k2 ^= k) & k)); k >>= 1);
		if (k1 > k2) {
			a = fz[k1];
			fz[k1] = fz[k2];
			fz[k2] = a;
		}
	}
	for (k = 0; (1 << k) < n; k++);
	k &= 1;
	if (k == 0) {
		for (fi = fz, fn = fz + n; fi < fn; fi += 4) {
			double f0, f1, f2, f3;
			f1 = fi[0] - fi[1];
			f0 = fi[0] + fi[1];
			f3 = fi[2] - fi[3];
			f2 = fi[2] + fi[3];
			fi[2] = (f0 - f2);
			fi[0] = (f0 + f2);
			fi[3] = (f1 - f3);
			fi[1] = (f1 + f3);
		}
	} else {
		for (fi = fz, fn = fz + n, gi = fi + 1; fi < fn;
		     fi += 8, gi += 8) {
			double s1, c1, s2, c2, s3, c3, s4, c4, g0, f0, f1,
			    g1, f2, g2, f3, g3;
			c1 = fi[0] - gi[0];
			s1 = fi[0] + gi[0];
			c2 = fi[2] - gi[2];
			s2 = fi[2] + gi[2];
			c3 = fi[4] - gi[4];
			s3 = fi[4] + gi[4];
			c4 = fi[6] - gi[6];
			s4 = fi[6] + gi[6];
			f1 = (s1 - s2);
			f0 = (s1 + s2);
			g1 = (c1 - c2);
			g0 = (c1 + c2);
			f3 = (s3 - s4);
			f2 = (s3 + s4);
			g3 = SQRT2 * c4;
			g2 = SQRT2 * c3;
			fi[4] = f0 - f2;
			fi[0] = f0 + f2;
			fi[6] = f1 - f3;
			fi[2] = f1 + f3;
			gi[4] = g0 - g2;
			gi[0] = g0 + g2;
			gi[6] = g1 - g3;
			gi[2] = g1 + g3;
		}
	}
	if (n < 16)
		return;

	do {
		double s1, c1;
		k += 2;
		k1 = 1 << k;
		k2 = k1 << 1;
		k4 = k2 << 1;
		k3 = k2 + k1;
		kx = k1 >> 1;
		fi = fz;
		gi = fi + kx;
		fn = fz + n;
		do {
			double g0, f0, f1, g1, f2, g2, f3, g3;
			f1 = fi[0] - fi[k1];
			f0 = fi[0] + fi[k1];
			f3 = fi[k2] - fi[k3];
			f2 = fi[k2] + fi[k3];
			fi[k2] = f0 - f2;
			fi[0] = f0 + f2;
			fi[k3] = f1 - f3;
			fi[k1] = f1 + f3;
			g1 = gi[0] - gi[k1];
			g0 = gi[0] + gi[k1];
			g3 = SQRT2 * gi[k3];
			g2 = SQRT2 * gi[k2];
			gi[k2] = g0 - g2;
			gi[0] = g0 + g2;
			gi[k3] = g1 - g3;
			gi[k1] = g1 + g3;
			gi += k4;
			fi += k4;
		} while (fi < fn);
		TRIG_INIT(k, c1, s1);
		for (i = 1; i < kx; i++) {
			double c2, s2;
			TRIG_NEXT(k, c1, s1);
			c2 = c1 * c1 - s1 * s1;
			s2 = 2 * (c1 * s1);
			fn = fz + n;
			fi = fz + i;
			gi = fz + k1 - i;
			do {
				double a, b, g0, f0, f1, g1, f2, g2, f3,
				    g3;
				b = s2 * fi[k1] - c2 * gi[k1];
				a = c2 * fi[k1] + s2 * gi[k1];
				f1 = fi[0] - a;
				f0 = fi[0] + a;
				g1 = gi[0] - b;
				g0 = gi[0] + b;
				b = s2 * fi[k3] - c2 * gi[k3];
				a = c2 * fi[k3] + s2 * gi[k3];
				f3 = fi[k2] - a;
				f2 = fi[k2] + a;
				g3 = gi[k2] - b;
				g2 = gi[k2] + b;
				b = s1 * f2 - c1 * g3;
				a = c1 * f2 + s1 * g3;
				fi[k2] = f0 - a;
				fi[0] = f0 + a;
				gi[k3] = g1 - b;
				gi[k1] = g1 + b;
				b = c1 * g2 - s1 * f3;
				a = s1 * g2 + c1 * f3;
				gi[k2] = g0 - a;
				gi[0] = g0 + a;
				fi[k3] = f1 - b;
				fi[k1] = f1 + b;
				gi += k4;
				fi += k4;
			} while (fi < fn);
		}
		TRIG_RESET(k, c1, s1);
	} while (k4 < n);
}

void ifft(int n, double *real, double *imag)
{
	double a, b, c, d;
	double q, r, s, t;
	int i, j, k;
	fht(real, n);
	fht(imag, n);
	for (i = 1, j = n - 1, k = n / 2; i < k; i++, j--) {
		a = real[i];
		b = real[j];
		q = a + b;
		r = a - b;
		c = imag[i];
		d = imag[j];
		s = c + d;
		t = c - d;
		imag[i] = (s + r) * 0.5;
		imag[j] = (s - r) * 0.5;
		real[i] = (q - t) * 0.5;
		real[j] = (q + t) * 0.5;
	}
}

void realfft(int n, double *real)
{
	double a, b;
	int i, j, k;
	fht(real, n);
	for (i = 1, j = n - 1, k = n / 2; i < k; i++, j--) {
		a = real[i];
		b = real[j];
		real[j] = (a - b) * 0.5;
		real[i] = (a + b) * 0.5;
	}
}

void fft(int n, double *real, double *imag)
{
	double a, b, c, d;
	double q, r, s, t;
	int i, j, k;
	for (i = 1, j = n - 1, k = n / 2; i < k; i++, j--) {
		a = real[i];
		b = real[j];
		q = a + b;
		r = a - b;
		c = imag[i];
		d = imag[j];
		s = c + d;
		t = c - d;
		real[i] = (q + t) * .5;
		real[j] = (q - t) * .5;
		imag[i] = (s - r) * .5;
		imag[j] = (s + r) * .5;
	}
	fht(real, n);
	fht(imag, n);
}

void realifft(int n, double *real)
{
	double a, b;
	int i, j, k;
	for (i = 1, j = n - 1, k = n / 2; i < k; i++, j--) {
		a = real[i];
		b = real[j];
		real[j] = (a - b);
		real[i] = (a + b);
	}
	fht(real, n);
}

#include <SharedData.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <float.h>

static struct option const long_options[] = {
	{"transform", no_argument, 0, 't'},
	{"back",      no_argument, 0, 'b'},
	{"real",      no_argument, 0, 'r'},
	{"imag",      no_argument, 0, 'i'},
	{"amplitude", no_argument, 0, 'a'},
	{"phase",     no_argument, 0, 'p'},
	{"positiv",   no_argument, 0, '0'},
	{"complex",   no_argument, 0, 'c'},
	{"help",      no_argument, 0, 'h'},
	{"version",   no_argument, 0, 'V'},
	{"warranty",  no_argument, 0, 'W'},
	{"copyright", no_argument, 0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "$Id: dsft.cc,v 1.1 2003/01/23 15:12:06 vstein Exp $";

extern char *program_invocation_short_name;

void warranty(void)
{
	printf("This program is free software; you can redistribute it"
	       " and/or modify it\n"
	       "under the terms of the GNU General Public License as"
	       " published by the \n"
	       "Free Software Foundation; either version 2 of the License,"
	       " or (at your \n"
	       "option) any later version.\n"
	       "\n"
	       "This program is distributed in the hope that it will be"
	       " useful, but \n"
	       "WITHOUT ANY WARRANTY; without even the implied warranty of"
	       " MERCHANTABILITY\n"
	       "or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General"
	       " Public License \n"
	       "for more details.\n"
	       "\n"
	       "You should have received a copy of the GNU General Public"
	       " License along \n"
	       "with this program; if not, write to the Free Software"
	       " Foundation, Inc., \n"
	       "59 Temple Place Suite 330, Boston, MA 02111-1307 USA.\n");
	exit(0);
}

void usage(void)
{
	fprintf(stderr,
		"usage: %s  [-t|-b] [-riap] [-c] id(s)\n"
		"\n"
		"-t     --transform   transform to frequency domain\n"
		"-b     --back        transform from frequency domain\n"
		"-0     --positiv     output just positiv x-values\n"
		"-r     --real        calculate real part of transform\n"
		"-i     --imag        calculate imaginary part of transform\n"
		"-a     --amplitude   calculate amplitude of transform\n"
		"-p     --phase       calculate phase of transform\n"
		"-c     --complex     input in pairs of ids corresponding\n"
		"                     to real and imaginary part"
		" respectively\n"
		"-h,-?, --help        this message\n"
		"-V,    --version     display version information and exit.\n"
		"-W,    --warranty    display licensing terms.\n"
		"-C,    --copyright  \n"
		"\n"
		"option --positiv is accepted for real input only\n" ,
		program_invocation_short_name);
	exit(0);
}

void copyright(void)
{
	printf("%s version %s, Copyright (C) 1999 Roland Schaefer\n"
	       "%s comes with ABSOLUTELY NO WARRANTY;\n"
	       "This is free software, and you are welcome to"
	       " redistribute it\n"
	       "under certain conditions; type `%s -W' for details.\n",
	       program_invocation_short_name, verstring,
	       program_invocation_short_name, program_invocation_short_name);
	exit(0);
}

void version(void)
{
	fprintf(stderr, "%s version %s\n",
		program_invocation_short_name, verstring);

	exit(0);
}

#define OPT_TRANS     1<<0
#define OPT_BACK      1<<1
#define OPT_REAL      1<<2
#define OPT_IMAG      1<<3
#define OPT_AMPLITUDE 1<<4
#define OPT_PHASE     1<<5
#define OPT_POSI      1<<6
#define OPT_COMPLEX   1<<7

void manag_rfft(int os,const char *idstr,SharedData *sd);
void manag_cfft(int os,const char *idrstr,const char *idistr,SharedData *sd);

int main(int argc, char **argv)
{
	SharedData sd;
	int os = 0;
	int i, c;

//      sleep(3600);
	while ((c = getopt_long(argc, argv, "tb0riapch?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 't':
			if (os & OPT_BACK)
				usage();
			os |= OPT_TRANS;
			break;
		case 'b':
			if (os & OPT_TRANS)
				usage();
			os |= OPT_BACK;
			break;
 		case '0':
 			os |= OPT_POSI;
 			break;
		case 'r':
			os |= OPT_REAL;
			break;
		case 'i':
			os |= OPT_IMAG;
			break;
		case 'a':
			os |= OPT_AMPLITUDE;
			break;
		case 'p':
			os |= OPT_PHASE;
			break;
		case 'c':
			os |= OPT_COMPLEX;
			break;
		case 'V':
			version();
			break;
		case 'W':
			warranty();
			break;
		case 'C':
			copyright();
			break;
		case 'h':
		case '?':
		default:
			usage();
			break;
		}
	}

	if (!(os & (OPT_TRANS | OPT_BACK)))
		os |= OPT_TRANS;

	if (!(os & (OPT_REAL|OPT_IMAG|OPT_AMPLITUDE|OPT_PHASE))) {
		os |= OPT_REAL|OPT_IMAG;
	}

	if( os&OPT_COMPLEX ) {
		if((argc-optind)%2) {
			usage();
		} else {
			for(i=optind;i<argc;i+=2)
				manag_cfft(os,argv[i],argv[i+1],&sd);
		}
	} else {
		for(i=optind;i<argc;i++) 
			manag_rfft(os,argv[i],&sd);
	}

	sd.changed();
	return 0;
}

void manag_rfft(int os,const char *idstr,SharedData *sd)
{
	char *logline=NULL;
	char *newids=NULL;
	int ll=0,off=0;
	int i,ii,ir,io,c,n,jj,newn;
	int id,newid;
	double *xr,*yr;
	double *x,*y;
	double delta,del,norm;
	double min,max,xoff;
	double *wrk=NULL;
	double *iwrk=NULL;
	idset *ids=NULL;
	
	errno=0;
	id=strtol(idstr,NULL,0);
	if (errno) {
		fprintf (stderr,"ID Overflow in %s\n",idstr);
		return;
	}
	if(!sd->hasid(id)) {
		fprintf(stderr,"No id %d (%s)\n",id,idstr);
		return;
	}
	
	n=sd->n(id);
	xr=sd->x(id);

	min = max = xr[0];
	for (i = 1; i < n; i++) {
		min = (min < xr[i] ? min : xr[i]);
		max = (max > xr[i] ? max : xr[i]);
	}
	
	delta = ((double) n - 1.0) / (double) n / (max - min);  // = 1/T
	del = (max - min)/ (double) (2*n);

	xoff=-DBL_EPSILON;
	if (min*max>0) // x=0 out of bounds
		xoff=ceil(min*delta)/delta;

	yr=sd->y(id);

	if(os&OPT_TRANS) { // forward transform
		wrk = (double *) xmalloc(sizeof(double) * n);
		/* negativ x-values at the end */
		for (i = 0; i < n && xr[i] < xoff; i++);
		for (c = 0; i < n; i++, c++) 
			wrk[c] = yr[i];
		for (i = 0; c < n; c++, i++)
			wrk[c] = yr[i];
		realfft(n, wrk);
		norm = 1.0 / n;
		if(os & OPT_POSI)
			newn=n/2+1;
		else
			newn=n;
		if(os & OPT_REAL) { // forward, real
			newid=sd->create(newn);
			printf("new id=%d\n",newid);
			x=sd->x(newid);
			y=sd->y(newid);
			sd->set_n(newid,newn);
			sd->add_ident(newid,"%s forward real",
				      program_invocation_short_name);
			ids=add_to_set(ids,newid,0);
			if(os & OPT_POSI) { // forward, real, positiv f
				for ( c = 0 ; c < newn; c++) { // c == ir == io
					x[c] = c * delta;
					y[c] = wrk[c] * norm;
				}
			} else { // forward, real, all f
				for (ir = n/2-1, c = 0, io=-n/2+1; ir > 0; ir--, c++, io++) {
					x[c] = io * delta;
					y[c] = wrk[ir] * norm;
				}
				for (; c < newn ; ir++, c++) { // io==ir
					x[c] = ir * delta;
					y[c] = wrk[ir] * norm;
				}
			}
		}
		if (os & OPT_IMAG) { // forward, imag
			newid=sd->create(newn);
			printf("new id=%d\n",newid);
			x=sd->x(newid);
			y=sd->y(newid);
			sd->set_n(newid,newn);
			sd->add_ident(newid,"%s forward imag",
				      program_invocation_short_name);
			ids=add_to_set(ids,newid,0);
			if(os &OPT_POSI) { // forward, imag, positiv f
				x[0]=y[0]=0.0;
				for (ii = n-1,c=1; c<newn-1; ii--,c++) { // io==c
					x[c] = c * delta;
					y[c] = -wrk[ii] * norm;   // sign by vstein
				}
				x[c]=c*delta;
				y[c]=0.0;
			} else { // forward, imag, all f
				for (ii = n/2+1, c = 0, io= -n/2+1; ii < n; ii++, c++, io++) {
					x[c] = io * delta;
					y[c] = wrk[ii] * norm;    // sign by vstein
				}
				x[c]=y[c++]=0.0;
				for (ii=n-1, io=1; c < newn-1; ii--, c++,io++) {
					x[c] = io * delta;
					y[c] = -wrk[ii] * norm;   // sign by vstein
				}
				x[c]=io*delta;
				y[c]=0.0;
			}
		}
		if (os & OPT_AMPLITUDE) { // forward, ampli
			newid=sd->create(newn);
			printf("new id=%d\n",newid);
			x=sd->x(newid);
			y=sd->y(newid);
			sd->set_n(newid,newn);
			sd->add_ident(newid,"%s forward amplitude",
				      program_invocation_short_name);
			ids=add_to_set(ids,newid,0);
			if(os &OPT_POSI) { // forward, ampli, positiv f
				c=0;
				x[c]=0.0;
				y[c++]=fabs(wrk[0])*norm;
				for (ii =n-1; c < newn-1; ii--, c++) { // c==io==ir
					x[c] = c * delta;
					y[c] = hypot(wrk[c],wrk[ii]) * norm;
				}
				x[c]=c*delta;
				y[c]=fabs(wrk[n/2])*norm;
			} else { // forward, ampli, all f
				for (ir = n/2-1, ii = n/2+1, c = 0, io=-n/2+1; 
				     ii < n; ii++, ir--, c++, io++) {
					x[c] = io * delta;
					y[c] = hypot(wrk[ir],wrk[ii]) * norm;
				}
				x[c]=0.0;
				y[c++]=fabs(wrk[0])*norm;
				for (i= c - 2, io=1 ; c < newn-1; i--, c++, io++) {
					x[c] = io * delta;
					y[c] = y[i];
				}
				x[c]=io*delta;
				y[c]=fabs(wrk[n/2])*norm;
			}
		}
		if (os & OPT_PHASE) { // forward, phase
			newid=sd->create(newn);
			printf("new id=%d\n",newid);
			x=sd->x(newid);
			y=sd->y(newid);
			sd->set_n(newid,newn);
			sd->add_ident(newid,"%s forward phase",
				      program_invocation_short_name);
			ids=add_to_set(ids,newid,0);
			if(os &OPT_POSI) { // forward, phase, positiv f
				x[0]=y[0]=0.0;
				for (ir = 1, c=1, ii=n-1; c < newn-1; ir++, ii--, c++) { // io==c
					x[c] = c * delta;
					y[c] = atan2(-wrk[ii],wrk[ir]);     // sign by vstein
				}
				x[c]=c*delta;
				y[c]=0.0;
			} else { // forward, phase, all f
				for (ir = n/2-1, ii = n/2+1, c = 0, io= -n/2+1; ii<n; ii++, ir--, c++, io++) {
					x[c] = io * delta;
					y[c] = atan2(wrk[ii],wrk[ir]);    // sign by vstein
				}
				x[c]=y[c++]=0.0;
				for (ir = 1,ii = n-1; c < newn-1; ir++, ii--, c++) { // io==ir
					x[c] = ir * delta;
					y[c] = atan2(-wrk[ii],wrk[ir]);   // sign by vstein
				}
				x[c]=ir*delta;
				y[c]=0.0;
			}
		}
		// loging
		sort_set(ids);
		newids = string_set(ids);
 		free_idset(ids);
		off=asprintcat(&logline,&ll,0,"<*%s*:%d>\t%s -t",newids,id,
			       program_invocation_short_name);
		free(newids);
		if(os & OPT_POSI)
			off+=asprintcat(&logline,&ll,off,"0");
		if(os & OPT_REAL)
			off+=asprintcat(&logline,&ll,off,"r");
		if(os & OPT_IMAG)
			off+=asprintcat(&logline,&ll,off,"i");
		if(os & OPT_AMPLITUDE)
			off+=asprintcat(&logline,&ll,off,"a");
		if(os & OPT_PHASE)
			off+=asprintcat(&logline,&ll,off,"p");
		off+=asprintcat(&logline,&ll,off," $%d",id);
		sd->log(logline);
		free(logline);
	} else {  // backward transform
		if(fabs(max+min)<4*del) {
			/* values for positiv and negativ
			 * "frequencies" are given, so it's
			 * very likely, that this is not the
			 * fourier transform of a real
			 * array. Therefore we use ifft for
			 * the transform */
			wrk = (double *) xmalloc(sizeof(double) * n);
			iwrk = (double *) xmalloc(sizeof(double) * n);

			/* put negativ "frequencies" at the end */
			for (i = 0; i < n && xr[i] < -DBL_EPSILON; i++);
			for (c = 0; i < n; i++, c++) {
				wrk[c] = yr[i];
				iwrk[c] = 0.0;
			}
			for (i = 0; c < n; c++, i++) {
				wrk[c] = yr[i];
				iwrk[c] = 0.0;
			}
			ifft(n,wrk,iwrk);
			if(os & OPT_POSI)
				newn=n/2+1;
			else
				newn=n;
			if (os & OPT_REAL) { // back, real
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward real",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				if(os &OPT_POSI) { // back, real, positiv t
					for (c = 0; c < newn; i++) { // c==io==ir
						x[c] = c * delta;
						y[c] = wrk[c];
					}
				} else { // back, real, all t
					for (ir=n/2+1,c=0,io=-n/2+1; io<0 ; ir++,c++,io++) {
						x[c] = io * delta;
						y[c] = wrk[ir];
					}
					for (ir=0; c < newn ;ir++, c++) { // ir==io
						x[c] = ir * delta;
						y[c] = wrk[ir];
					}
				}
			}
			if (os & OPT_IMAG) { // back, imag
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward imag",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				if(os &OPT_POSI) { // back, imag, positiv t
					for (c = 0; c < newn; i++) { // c==io==ii
						x[c] = c * delta;
						y[c] = iwrk[c];
					}
				} else { // back, real, all t
					for (ii=n/2+1,c=0,io=-n/2+1; io<0 ; ii++,c++,io++) {
						x[c] = io * delta; 
						y[c] = iwrk[ii];
					}
					for (ii=0; c < newn ;ii++, c++) { // ii==io
						x[c] = ii * delta;
						y[c] = iwrk[ii];
					}
				}
			}
			if (os & OPT_AMPLITUDE) { // back, ampl
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward amplitude",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				if(os &OPT_POSI) { // back, ampl, positiv t
					for (c = 0; c < newn; c++) { // c==ir==ii==io
						x[c] = c * delta;
						y[c] = hypot(wrk[c],iwrk[c]);
					}
				} else { // back, ampl, all t
					for (i=n/2+1,c=0,io=-n/2+1; io<0; i++,c++,io++) { // i==ir==ii
						x[c] = io * delta;
						y[c] = hypot(wrk[i],iwrk[i]);
					}
					for (i = 0; c < newn ; i++, c++) { // io==i
						x[c] = i * delta;
						y[c] = hypot(wrk[i],iwrk[i]);
					}
				}
			}
			if (os & OPT_PHASE) { // back, phase
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward phase",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				if(os &OPT_POSI) { // back, phase, positiv t
					for (c=0; c<newn; c++) { // c==io==ir==ii
						x[c] = c * delta;
						y[c] = atan2(iwrk[c],wrk[c]);
					}
				} else { // back, phase, all t
					for (i=n/2+1,c=0,io=-n/2+1; io < 0; io++,c++,i++) { // i==ir==ii
						x[c] = io * delta;
						y[c] = atan2(iwrk[i],wrk[i]);
					}
					for (i = 0; c < newn ; i++, c++) { // io==i
						x[c] = i * delta;
						y[c] = atan2(iwrk[i],wrk[i]);
					}
				}
			}
			//next brace is end of: if(fabs(max+min)<2*delta)
		} else if(min > -del) {
			/* only positiv frequencies are given.  We
			 * suppose, that the spectrum is not only real
			 * but even symmetric */
			if(min < del) { // y-value at zero "frequency" given!
				delta*=(double)n/2.0/(double)(n-1);
				for(jj=1,c=n-1;jj<c&&jj<(1<<30);jj<<=1);
				if(jj!=n-1) {
					fprintf(stderr,"nop\n");
					return;
				}
				jj<<=1;
				wrk = (double *) xmalloc(sizeof(double) * jj);
				for(c=0;c<n;c++) // ir==c
					wrk[c]=yr[c];
				for(;c<jj;c++)
					wrk[c]=0.0;
			} else if(fabs(min-2*del)<del) { 
				/* y-value at zero "frequency" not
				 * given! */
				delta/=2.0;   // strange, but probably true!
// 				delta*=(double)(n-2)/2.0/(double)(n-1);
				for(jj=1;jj<n&&jj<(1<<30);jj<<=1);
				if(jj!=n) {
					fprintf(stderr,"nop.\n");
					return;
				}
				jj<<=1;
				wrk = (double *) xmalloc(sizeof(double) * jj);
				wrk[0]=0.0;
				for(ir=0,c=1;ir<n;ir++,c++)
					wrk[c]=yr[ir];
				for(;c<jj;c++)
					wrk[c]=0.0;
			} else {
				fprintf(stderr,"nop..\n");
			        return;
			}
			realifft(jj,wrk);
			norm = 1.0;
			if(os & OPT_POSI)
				newn=jj/2+1;
			else
				newn=jj;
			if(os & OPT_REAL) { // back, real
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward real",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				if(os &OPT_POSI) { // back, real, positiv t
					for (c = 0; c < newn; c++) { // c==io==ir
						x[c] = c * delta;
						y[c] = wrk[c];
					}
				} else { // back, real, all t
					for (ir=jj/2+1,c=0,io=-jj/2+1; io < 0; c++,ir++,io++) {
						x[c] = io * delta;
						y[c] = wrk[ir];
					}
					for (ir=0; c < newn ; ir++, c++) { // io==ir
						x[c] = ir * delta;
						y[c] = wrk[ir];
					}
				}
			}
			if(os & OPT_AMPLITUDE) { // back, ampli
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward amplitude",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				if(os &OPT_POSI) { // back, ampli, positiv t
					for (c = 0; c < newn; c++) { // c==io==ir
						x[c] = c * delta;
						y[c] = fabs(wrk[c]);
					}
				} else { // back, ampli, all t
					for (ir=jj/2+1,c=0,io=-jj/2+1; io < 0; c++,ir++,io++) {
						x[c] = io * delta;
						y[c] = fabs(wrk[ir]);
					}
					for (ir=0; c < newn ; ir++, c++) { // io==ir
						x[c] = ir * delta;
						y[c] = fabs(wrk[ir]);
					}
				}
			}
			if(os & OPT_IMAG) { // back, imag
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward imag",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				if(os &OPT_POSI) { // back, imag, positiv t
					for (c = 0; c < newn; i++) {
						x[c] = c * delta;
						y[c] = 0.0;
					}
				} else { // back, imag, all t
					for (c=0,io=-jj/2+1; c<newn; c++,io++) {
						x[c] = io * delta;
						y[c] = 0.0;
					}
				}
			}
			if(os & OPT_PHASE) {
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward phase",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				if(os &OPT_POSI) { // back, phase, positiv t
					for (c = 0; c < newn; i++) {
						x[c] = c * delta;
						y[c] = 0.0;
					}
				} else { // back, phase, all t
					for (c=0,io=-jj/2+1; c<newn; c++,io++) {
						x[c] = io * delta;
						y[c] = 0.0;
					}
				}
			}
		} else {
			fprintf(stderr,"nop...\n");
			exit(1);
		}
		// loging
		sort_set(ids);
		newids = string_set(ids);
 		free_idset(ids);
		off=asprintcat(&logline,&ll,0,"<*%s*:%d>\t%s -b",newids,id,program_invocation_short_name);
		free(newids);
		if(os & OPT_POSI)
			off+=asprintcat(&logline,&ll,off,"0");
		if(os & OPT_REAL)
			off+=asprintcat(&logline,&ll,off,"r");
		if(os & OPT_IMAG)
			off+=asprintcat(&logline,&ll,off,"i");
		if(os & OPT_AMPLITUDE)
			off+=asprintcat(&logline,&ll,off,"a");
		if(os & OPT_PHASE)
			off+=asprintcat(&logline,&ll,off,"p");
		off+=asprintcat(&logline,&ll,off," $%d",id);
		sd->log(logline);
		free(logline);
	}
	if(wrk)
		free(wrk);
	if(iwrk)
		free(iwrk);
}

void manag_cfft(int os,const char *idrstr,const char *idistr,SharedData *sd)
{
	char *logline=NULL;
	int ll=0,off=0;
	int idr,idi;
	int newid;
	int i,ir,ii,c,io,newn,n;
	
	double min,max,del,delta,norm,xoff;
	
	double *wr=NULL;
	double *wi=NULL;

	double *xr,*xi;
	double *yr,*yi;
	double *x,*y;
	
	idset *ids=NULL;

	char *newids=NULL;

	errno=0;
	idr=strtol(idrstr,NULL,0);
	idi=strtol(idistr,NULL,0);
	if (errno) {
		fprintf (stderr,"ID Overflow in %s or %s\n",
			 idrstr,idistr);
		return;
	}
	if(!sd->hasid(idr)||!sd->hasid(idi)) {
		fprintf(stderr,"No id %d (%s) or %d (%s)\n",
			idr,idrstr,idi,idistr);
		return;
	}
	
	n=sd->n(idr);
	if(!n)
		return;
	xr=sd->x(idr);
	yr=sd->y(idr);
	if((unsigned int)n!=sd->n(idi)) {
		fprintf(stderr,"size match error id %d and %d\n",idr,idi);
		return;
	}
	xi=sd->x(idi);
	yi=sd->y(idi);

	min = max = xr[0];
	for (i = 1; i < n; i++) {
		min = (min < xr[i] ? min : xr[i]);
		max = (max > xr[i] ? max : xr[i]);
	}
	delta = ((double) n - 1.0) / (double) n / (max - min);
	del = (max - min)/ (double) (2*n);

	xoff=-DBL_EPSILON;
	if (min*max>0) // x=0 out of bounds
		xoff=ceil(min*delta)/delta;

	for(i = 0; i < n; i++) {
		if(fabs(xr[i]-xi[i])>del) {
			fprintf(stderr,"x-match error in id %d and %d\n",idr,idi);
			return;
		}
	}
	
	if(os & OPT_TRANS) { // forward transform
		newn=n;
		wr=(double *)xmalloc(sizeof(double)*n);
		wi=(double *)xmalloc(sizeof(double)*n);
		/* negativ x-values at the end */
		for (i = 0; i < n && xr[i] < xoff; i++);
		for (c = 0; i < n; i++, c++) {
			wr[c] = yr[i];
			wi[c] = yi[i];
		}
		for (i = 0; c < n; c++, i++) {
			wr[c] = yr[i];
			wi[c] = yi[i];
		}
		fft(n,wr,wi);
		norm= 1.0 / n;
		if (os & OPT_REAL) {
			newid=sd->create(newn);
			printf("new id=%d\n",newid);
			x=sd->x(newid);
			y=sd->y(newid);
			sd->set_n(newid,newn);
			sd->add_ident(newid,"%s forward real",
				      program_invocation_short_name);
			ids=add_to_set(ids,newid,0);
			for (io=-n/2+1, ir=n/2+1 , c = 0; io < 0; io++, ir++, c++) {
				x[c] = io * delta;
				y[c] = wr[ir] * norm;
			}
			for (ir = 0; c < newn; ir++, c++) { // io==ir
				x[c] = ir * delta;
				y[c] = wr[ir] * norm;
			}
		}
		if (os & OPT_IMAG) {
			newid=sd->create(newn);
			printf("new id=%d\n",newid);
			x=sd->x(newid);
			y=sd->y(newid);
			sd->set_n(newid,newn);
			sd->add_ident(newid,"%s forward imag",
				      program_invocation_short_name);
			ids=add_to_set(ids,newid,0);
			for (io=-n/2+1, ii=n/2+1 , c = 0; io < 0; io++, ii++, c++) {
				x[c] = io * delta;
				y[c] = wi[ii] * norm;
			}
			for (ii = 0; c < newn; ii++, c++) { // io==ii
				x[c] = ii * delta;
				y[c] = wi[ii] * norm;
			}
		}
		if (os & OPT_AMPLITUDE) {
			newid=sd->create(newn);
			printf("new id=%d\n",newid);
			x=sd->x(newid);
			y=sd->y(newid);
			sd->set_n(newid,newn);
			sd->add_ident(newid,"%s forward amplitude",
				      program_invocation_short_name);
			ids=add_to_set(ids,newid,0);
			for (io=-n/2+1, i=n/2+1 , c = 0; io < 0; io++, i++, c++) { // ii==ir==i
				x[c] = io * delta;
				y[c] = hypot(wr[i],wi[i]) * norm;
			}
			for (i = 0; c < newn; i++, c++) {  // ii==ir==i
				x[c] = i * delta;
				y[c] = hypot(wr[i],wi[i]) * norm;
			}
		}
		if (os & OPT_PHASE) {
			newid=sd->create(newn);
			printf("new id=%d\n",newid);
			x=sd->x(newid);
			y=sd->y(newid);
			sd->set_n(newid,newn);
			sd->add_ident(newid,"%s forward phase",
				      program_invocation_short_name);
			ids=add_to_set(ids,newid,0);
			for (io=-n/2+1, i=n/2+1 , c = 0; io < 0; io++, i++, c++) { // ii==ir==i
				x[c] = io * delta;
				y[c] = atan2(wi[i],wr[i]);
			}
			for (i = 0; c < newn; i++, c++) {  // ii==ir==i
				x[c] = i * delta;
				y[c] = atan2(wi[i],wr[i]);
			}
		}
		// loging
		sort_set(ids);
		newids = string_set(ids);
 		free_idset(ids);
		off=asprintcat(&logline,&ll,0,"<*%s*:%d,%d>\t%s -t",
			       newids,idr,idi,program_invocation_short_name);
		free(newids);
		if(os & OPT_POSI)
			off+=asprintcat(&logline,&ll,off,"0");
		if(os & OPT_REAL)
			off+=asprintcat(&logline,&ll,off,"r");
		if(os & OPT_IMAG)
			off+=asprintcat(&logline,&ll,off,"i");
		if(os & OPT_AMPLITUDE)
			off+=asprintcat(&logline,&ll,off,"a");
		if(os & OPT_PHASE)
			off+=asprintcat(&logline,&ll,off,"p");
		off+=asprintcat(&logline,&ll,off," -c $%d $%d",idr,idi);
		sd->log(logline);
		free(logline);
	} else { // backward transform
		if(fabs(max+min)<4*del) {
			/* values for positiv and negativ
			 * "frequencies" are given, so it's very
			 * likely, that this is not the fourier
			 * transform of a real array. Therefore we use
			 * ifft for the transform */
			newn=n;
			wr=(double *)xmalloc(sizeof(double)*n);
			wi=(double *)xmalloc(sizeof(double)*n);

			/* put negativ "frequencies" at the end */
			for (i = 0; i < n && xr[i] < -DBL_EPSILON; i++);
			for (c = 0; i < n; i++, c++) {
				wr[c] = yr[i];
				wi[c] = yi[i];
			}
			for (i = 0; c < newn; c++, i++) {
				wr[c] = yr[i];
				wi[c] = yi[i];
			}

			ifft(newn,wr,wi);
			norm = 1.0;
			if (os & OPT_REAL) {
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward real",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				for (io=-n/2+1, ir=n/2+1 , c = 0; io < 0; io++, ir++, c++) {
					x[c] = io * delta;
					y[c] = wr[ir] * norm;
				}
				for (ir = 0; c < newn; ir++, c++) { // io==ir
					x[c] = ir * delta;
					y[c] = wr[ir] * norm;
				}
			}
			if (os & OPT_IMAG) {
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward imag",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				for (io=-n/2+1, ii=n/2+1 , c = 0; io < 0; io++, ii++, c++) {
					x[c] = io * delta;
					y[c] = wi[ii] * norm;
				}
				for (ii = 0; c < newn; ii++, c++) { // io==ii
					x[c] = ii * delta;
					y[c] = wi[ii] * norm;
				}
			}
			if (os & OPT_AMPLITUDE) {
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward amplitude",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				for (io=-n/2+1, i=n/2+1 , c = 0; io < 0; io++, i++, c++) { // ii==ir==i
					x[c] = io * delta;
					y[c] = hypot(wr[i],wi[i]) * norm;
				}
				for (i = 0; c < newn; i++, c++) {  // ii==ir==i
					x[c] = i * delta;
					y[c] = hypot(wr[i],wi[i]) * norm;
				}
			}
			if (os & OPT_PHASE) {
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward phase",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				for (io=-n/2+1, i=n/2+1 , c = 0; io < 0; io++, i++, c++) { // ii==ir==i
					x[c] = io * delta;
					y[c] = atan2(wi[i],wr[i]);
				}
				for (i = 0; c < newn; i++, c++) {  // ii==ir==i
					x[c] = i * delta;
					y[c] = atan2(wi[i],wr[i]);
				}
			}
		} else if(min > -del) {
			/* only positiv frequencies are given.  We
			 * suppose, that the backtransform is real */
			if(min<del) { // y-value at zero "frequency" given!
				delta*=(double)n/2/(double)(n-1);
				for(newn=1,c=n-1;newn<c&&newn<(1<<30);newn<<=1);
				if(newn!=n-1) {
					fprintf(stderr,"nop.-\n");
					return;
				}
				if(fabs(yi[0])>DBL_EPSILON) {
					fprintf(stderr,"nop-\n");
					return;
				}
				if(fabs(yi[n-1])>DBL_EPSILON) {
					fprintf(stderr,"nop-.\n");
					return;
				}
				newn<<=1;
				wr = (double *) xmalloc(sizeof(double) * newn);
				for(c=0;c<n;c++) // ir==c
					wr[c]=yr[c];
				for(ii=n-2;c<newn;ii--,c++)
					wr[c]=-yi[ii];     // sign by vstein
			} else if(fabs(min-2*del)<del) { 
				/* y-value at zero "frequency" not
				 * given! */
				delta/=2.0; // strange, but probably true!
// 				delta*=(double)(n-2)/2/(double)(n-1);
				for(newn=1;newn<n&&newn<(1<<30);newn<<=1);
				if(newn!=n) {
					fprintf(stderr,"nop-..\n");
					return;
				}
				if(fabs(yi[0])>DBL_EPSILON) {
					fprintf(stderr,"nop-...\n");
					return;
				}
				if(fabs(yi[n-1])>DBL_EPSILON) {
					fprintf(stderr,"nop.+\n");
					return;
				}
				newn<<=1;
				wr = (double *) xmalloc(sizeof(double) * newn);
				wr[0]=0.0;
				for(ir=0,c=1;ir<n;ir++,c++)
					wr[c]=yr[ir];
				for(ii=n-2;c<newn;c++,ii--)
					wr[c]=-yi[ii];     // sign by vstein
			} else {
				fprintf(stderr,"nop+\n");
				return;
			}
			realifft(newn,wr);
			if(os & OPT_REAL) { // back, real
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward real",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				for (ir=newn/2+1,c=0,io=-newn/2+1; io < 0; c++,ir++,io++) {
					x[c] = io * delta;
					y[c] = wr[ir];
				}
				for (ir = 0; c < newn ; ir++, c++) { // io==ir
					x[c] = ir * delta;
					y[c] = wr[ir];
				}
			}
			if(os & OPT_AMPLITUDE) { // back, amplitude
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward amplitude",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				for (ir=newn/2+1,c=0,io=-newn/2+1; io < 0; c++,ir++,io++) {
					x[c] = io * delta;
					y[c] = fabs(wr[ir]);
				}
				for (ir = 0; c < newn ; ir++, c++) { // io==ir
					x[c] = ir * delta;
					y[c] = fabs(wr[ir]);
				}
			}
			if(os & OPT_IMAG) { // back, imag
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward imag",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				for (c=0,io=-newn/2+1; c < newn; c++,io++) {
					x[c] = io * delta;
					y[c] = 0.0;
				}
			}
			if(os & OPT_PHASE) {
				newid=sd->create(newn);
				printf("new id=%d\n",newid);
				x=sd->x(newid);
				y=sd->y(newid);
				sd->set_n(newid,newn);
				sd->add_ident(newid,"%s backward phase",
					      program_invocation_short_name);
				ids=add_to_set(ids,newid,0);
				for (c=0,io=-newn/2+1; c < newn; c++,io++) {
					x[c] = io * delta;
					y[c] = 0.0;
				}
			}
		} else {
			fprintf(stderr,"nop+.\n");
			return;
		}
		// loging
		sort_set(ids);
		newids = string_set(ids);
 		free_idset(ids);
		off=asprintcat(&logline,&ll,0,"<*%s*:%d,%d>\t%s -b",
			       newids,idr,idi,program_invocation_short_name);
		free(newids);
		if(os & OPT_POSI)
			off+=asprintcat(&logline,&ll,off,"0");
		if(os & OPT_REAL)
			off+=asprintcat(&logline,&ll,off,"r");
		if(os & OPT_IMAG)
			off+=asprintcat(&logline,&ll,off,"i");
		if(os & OPT_AMPLITUDE)
			off+=asprintcat(&logline,&ll,off,"a");
		if(os & OPT_PHASE)
			off+=asprintcat(&logline,&ll,off,"p");
		off+=asprintcat(&logline,&ll,off," -c $%d $%d",idr,idi);
		sd->log(logline);
		free(logline);
	}
	 
	if(wr)
		free(wr);
	if(wi)
		free(wi);
}
