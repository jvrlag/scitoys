// Program to show Pascal's triangle mod m
// Original version: Dec 1999
// Current version, exporting PNG files, may 2005
#include"easyx.h"
#include"easyim.h"

class Pascal
{
public:
     long N; // depth
     long **D; // actual values
     long M; // which modulus

     long Ncolors;
     palette* Pal;

     Pascal(long n);
     ~Pascal();
     void Evaluate(long m);
     void Show() const;
     void Export_Pic() const;
};

Pascal::Pascal(long n)
{
     N=n;
     D=(long**)malloc((N+1)*sizeof(long*));
     for (long i=1;i<=N;i++)
	  D[i]=(long*)malloc((2*i+2)*sizeof(long));
     Ncolors=200;
     Pal=EXPalette(1,1,1,0,0,1,Ncolors);
     Pal[0]=EXAllocNamedColor("black"); 
}

Pascal::~Pascal()
{
     for (long i=1;i<=N;i++)
	     free(D[i]);
     free(D);
}

void Pascal::Evaluate(long m)
{
     M=m;
     for (long i=1;i<=N;i++)
     {
	  D[i][0] = D[i][i] = 1;
	  for (long j=1; j<i; j++)
	       D[i][j] = ( D[i-1][j-1] + D[i-1][j] ) % M;
     }

}

void Pascal::Show() const
{
     for (long i=1;i<=N;i++)
	  for (long j=0;j<=i;j++)
	  {
	       long value=D[i][j];
	       long color=(long)floor(value*Ncolors/(double)M);
	       EXSetColor(Pal[color]);
	       EXPixel(N/2-i/2+j,i);
	  }
     EXFlush();
}

void Pascal::Export_Pic() const
{
     Show();
     char name[60];
     sprintf(name,"pascal%03ld.png",M);
     long xsize=EXCW->width;
     long ysize=EXCW->height;
     EI_Capture(0,0,xsize,ysize);
     EI_Save(name);
     EI_Free();
}


int main()
{
     long N=600;
     EXStart(100,100,N,N);
     EXEnableBuffer();
     EI_Start();   // in case you want to export pictures

     Pascal P(N);
     for (long m=2;m<=20;m++)
     {
	  P.Evaluate(m); // Compute Pascal's triangle mod m
	  P.Show();      // Show it
	  EXFlush();     // Flush buffer
	  EXReadKey();   // wait for a key
     }
     EXClose();
}
