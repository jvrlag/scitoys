// 051226, sqpirals
#include"easyx.h"

int main()
{
     int xsize=600; // screen size
     long imax=800; // number of segments
     long Ncolors=200;

     EXStart(100,100,xsize,xsize);
     EXEnableBuffer();
     palette *paleta=EXPalette(0.0,1.0,0.0, 1.0,0.0,0.0, Ncolors);

     // range over all these angles
     for (double da=0.0; da<=2*M_PI; da+=0.0008)
     {
	  double x1=xsize/2, y1=xsize/2; // starting point: center
	  double d=5.0; // initial segment length
	  double a=0.0; // initial angle
	  double dd=4.0; // increment in segment length
	  EXClear();
	  for (long i=1;i<=imax;i++)
	  {
	       double x2=x1+d*sin(a); // new point
	       double y2=y1+d*cos(a);
	       long index=(long)(Ncolors*fabs(sin(i/(double)Ncolors)))+1;
	       EXSetColor(paleta[index]);
	       EXLine((int)x1,(int)y1,(int)x2,(int)y2); // draw line
	       x1=x2; y1=y2; // update point
	       d+=dd; // increase segment length
	       a+=da; // increase angle
	       if (fabs(x1)+fabs(y1)>4*xsize) break;
	  }
	  EXFlush();
	  delay(0.01);
     }
     EXReadKey();
     EXClose();


}

