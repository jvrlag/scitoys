// Initial version, 2002
// Last update: 101115
// Golden spiral visualization
#include"easyx.h"

int main()
{
     int xsize=600;
     long Nt=1900;
     EXStart(100,100,xsize,xsize);
     palette rojo=EXAllocRGBColor(1.0,0.4,0.4); // red
     palette rojo_osc=EXAllocRGBColor(0.5,0.2,0.2); // dark red
     EXEnableBuffer();
     for (double ca=3.8; ca<=3.9; ca+=0.0001) // angle multiplication factor
     {
	  double cr=1.003;   // radius multiplication factor
	  double cs=1.0005;  // size multiplication factor
	  double a=0.0;   // initial angle
	  double r=5.0;   // initial radius
	  double s=1.5;	  // initial size
	  EXClear();
	  for (long int t=1;t<=Nt;t++)
	  {
	       // coordinates of the next point in the spiral
	       int x=xsize/2+(int)(r*cos(a));
	       int y=xsize/2+(int)(r*sin(a));
	       // Poor man's anti-aliasing for circles
	       EXSetColor(rojo_osc);
	       EXFillCircle(x,y,(int)ceil(s)+1);
	       EXSetColor(rojo);
	       EXFillCircle(x,y,(int)floor(s));
	       // Next point!
	       r*=cr;
	       a+=ca;
	       s*=cs;
	  }
	  EXFlush();
	  EXReadKey();
     }
     EXClose();
    
}
