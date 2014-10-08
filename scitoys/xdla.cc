// Diffusion Limited Aggregation (DLA)
// 070614
#include"easyx.h"
#include"easyim.h"

long Lx=300;
long Ly=300;
int px=2;
long maxt=10000;
palette red;
palette *Pal;
long Ncolors=40;


class Board
{
public:
     long Lx, Ly;
     long *M;
     long t; 
     
     // Graphics data
     int px;
     palette *Pal;
     long Ncolors;

     long maxt; // the maximal time index

     Board(long lx, long ly);
     ~Board();
     long Index(long x, long y) const;
     long Occupied(long x, long y) const;
     void Occupy(long x, long y, long t);
     void Launch();

     palette Get_Color(long x, long y) const;
     void Show() const;
     void Export_Pic() const;
};

Board::Board(long lx, long ly)
{
     Lx=lx, Ly=ly;
     M=(long*)malloc(lx*ly*sizeof(long));
     memset(M,0,lx*ly*sizeof(long));
     px=2;
     Ncolors=40;
     maxt=10000;
     Pal=EXPalette(1,0,0,0,1,0,Ncolors);
     Pal[0]=EXAllocRGBColor(0,0,0);
     t=0;
}

Board::~Board()
{
     free(M);
     free(Pal);
}

long Board::Index(long x, long y) const
{
     return y*Lx+x;
}

long Board::Occupied(long x, long y) const
{
     return M[Index(x,y)];
} 

void Board::Occupy(long x, long y, long t)
{
     M[Index(x,y)]=t;
}

palette Board::Get_Color(long x, long y) const
{
     return Pal[(long)ceil(M[Index(x,y)]*(double)Ncolors/(double)maxt)];
}

void Board::Show() const
{
     for (long x=0;x<Lx;x++)
	  for (long y=0;y<Ly;y++)
	  {
	       palette color=Get_Color(x,y);
	       EXSetColor(color);
	       EXFillRectangle(2*x,2*(Ly-y),px,px);
	  }
     EXFlush();
}

void Board::Launch()
{
     long x=Lx/2; 
     long y=Ly/2; 
     bool done=false;
     while(!done)
     {
	  long dx=rand_int(-1,1);
	  long dy=rand_int(-1,1);
	  long xp=x+dx, yp=y+dy;
	  if (Occupied(xp,yp) || xp==0 || xp==Lx-1 || yp==0 || yp==Ly+1) 
	  {
	       Occupy(x,y,t);
	       done=true;
	  }
	  x=xp; y=yp;
     }
     t++;
}

void Board::Export_Pic() const
{
     char name[60];
     sprintf(name,"%04ld.png",t/100);
     long xsize=EXCW->width;
     long ysize=EXCW->height;
     EI_Capture(0,0,xsize,ysize);
     EI_Save(name);
     EI_Free();
}

int main()
{
     open_rng(0);

     long px=2;
     long Lx=300;
     long Ly=300;
     long xsize=px*Lx;
     long ysize=px*Ly;
     
     EXStart(100,100,xsize,ysize);
     EI_Start(); // start image library
     Board B(Lx,Ly);
     B.px=px;
     long t=0;

     do
     {
	  t++;
	  B.Launch();
	  B.Show();
     }while(t<maxt && !B.Occupied(Lx/2,Ly/2));
     EXReadKey();
     EXClose();
}
