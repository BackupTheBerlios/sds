/* this is a simple version of dsft by Peter vom Stein */

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

static struct option const long_options[] = {
	{"transform", no_argument, 0, 't'},
	{"back",      no_argument, 0, 'b'},
	{"real",      no_argument, 0, 'r'},
	{"complex",   no_argument, 0, 'c'},
	{"help",      no_argument, 0, 'h'},
	{"version",   no_argument, 0, 'V'},
	{"warranty",  no_argument, 0, 'W'},
	{"copyright", no_argument, 0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "$Id: dssft.cc,v 1.1 2003/01/23 15:12:06 vstein Exp $";

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
		"usage: %s  [-t|-b] [-r|c] id(s)\n"
		"\n"
		"-t     --transform   transform to frequency domain\n"
		"-b     --back        transform from frequency domain\n"
		"-r     --real        calculate realfft\n"
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
#define OPT_COMPLEX   1<<3

void manag_rfft(int os,const char *idstr,SharedData *sd);
void manag_cfft(int os,const char *idrstr,const char *idistr,SharedData *sd);

int main(int argc, char **argv)
{
	SharedData sd;
	int os = 0;
	int i, c;

//      sleep(3600);
	while ((c = getopt_long(argc, argv, "tbrch?VWC",
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
		case 'r':
			if (os & OPT_COMPLEX)
				usage();
			os |= OPT_REAL;
			break;
		case 'c':
			if (os & OPT_REAL)
				usage();
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

	if( os&OPT_COMPLEX ) {
		if((argc-optind)%2) {
			usage();
		} else {
			if(os&OPT_COMPLEX) {
				for(i=optind;i<argc;i+=2)
					manag_cfft(os,argv[i],argv[i+1],&sd);
			} 
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
	int id,i,n;
	double *yr;

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
	yr=sd->y(id);

	if (os&OPT_TRANS) { // forward transform
		realfft(n, yr);
		for (i=0; i<n ; i++)
			yr[i] /= n ;
		asprintf(&logline,"<%d>\t%s -t -r $%d",id,
			 program_invocation_short_name,id);
		sd->log(logline);
		free(logline);
	} else {  // backward transform
		realifft(n,yr);
		asprintf(&logline,"<%d>\t%s -b -r $%d",id,
			 program_invocation_short_name,id);
		sd->log(logline);
		free(logline);
	}
}

void manag_cfft(int os,const char *idrstr,const char *idistr,SharedData *sd)
{
	char *logline=NULL;
	int idr,idi,i,n;
	double *yr, *yi;

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
	yr=sd->y(idr);
	if((unsigned int)n!=sd->n(idi)) {
		fprintf(stderr,"size match error id %d and %d\n",idr,idi);
		return;
	}
	yi=sd->y(idi);


	if (os&OPT_TRANS) { // forward transform
		fft(n, yr, yi);
		for (i=0; i<n ; i++) {
			yr[i] /= n ;
			yi[i] /= n ;
		}
		asprintf(&logline,"<%d,%d>\t%s -t -c $%d $%d",idr,idi,
			 program_invocation_short_name,idr,idi);
		sd->log(logline);
		free(logline);
	} else {
		ifft(n, yr, yi);
		asprintf(&logline,"<%d,%d>\t%s -b -c $%d $%d",idr,idi,
			 program_invocation_short_name,idr,idi);
		sd->log(logline);
		free(logline);
	}
}

