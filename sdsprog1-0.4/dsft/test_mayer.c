/*
** FFT tester
**      Loads random numbers into complex array, ffts it, and shows result.
**	Beats me what the output should be; but any two different ffts
**      should return the same result.
*/

#define REAL double

 REAL real[140001];
 REAL imag[140001];

main(argc,argv)
int argc;
char **argv;
{
 int i;
 for (i=0;i<256;i++)   {real[i]=random()%10;  imag[i]=random()%10; }
 fft(256, real, imag);
 for (i=0;i<10;i++) {printf("%g,%g ",real[i],imag[i]);}
 printf("\n");
}
