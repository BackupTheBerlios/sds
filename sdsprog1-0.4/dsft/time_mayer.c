#define REAL double

 REAL real[140001];
 REAL imag[140001];

main(argc,argv)
int argc;
char **argv;
{
 int num=0,lop,i,k,j;
 long cycles;
 REAL ssq=0;
 REAL scale;
int status;
 if (argc>1) num=atoi(argv[1]);
 if (num==0) num=256;
 for (i=0;i<num;i++)
    {
     real[i]=i;
     imag[i]=0;
    }
 scale = 1.0/num;
 lop = 2+256*256/num;
 printf("FFT-mayer                 (%-6d %6d):",lop,num);
 cycles=clock();
 for (k=0;k<lop;k++)
     {
      fft(num, real, imag);
      ifft(num, real, imag);
      for (j=0;j<num*2;j++)  {real[j]*=scale;imag[j]*=scale;}
     }
 printf("%10d CPU us ;",clock());

 for (ssq=0,i=0;i<num;i++)
    ssq+=(real[i]-i)*(real[i]-i);
 printf("ssq errors %#.2g\n",ssq);

}
