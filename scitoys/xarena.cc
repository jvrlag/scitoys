// Hourglass program
// March 2000 - Nov 2010
// JaviRL
#include "easyx.h"

// Grain flow issues
double fluidity=0.5;

// X11 issues
long dot=4; // screen size of each sandgrain
palette blanco, rojo, negro;
long Ncolors=15;
palette *paleta;

class World
{
public:
     long Lx, Ly; // world size
     long *A; // sand grains matrix
     bool *L; // constraint matrix
     World(long lx, long ly);
     ~World();
     void Hourglass();
     void Show();
     void Move();
     
     long Occupied_Q(long x, long y);
     bool Obstacle_Q(long x, long y);
     void Put(long x, long y, long b);
     void Put_Obstacle(long x, long y, bool b);

     void Turn_Over();
     void Skew_Line();
     void Reinforce();
     void Symmetrize();
     void Move_Grain(long x, long y);
};



long World::Occupied_Q(long x, long y)
{
     if (y<=0 || y>Ly || x<=0 || x>Lx) return 1;
     return A[(y-1)*Lx + x-1];
}

bool World::Obstacle_Q(long x, long y)
{
     if (y<=0 || y>Ly || x<=0 || x>Lx) return true;
     return L[(y-1)*Lx + x-1];
}

void World::Put(long x, long y, long B)
{
     A[(y-1)*Lx + x-1]=B;
}

void World::Put_Obstacle(long x, long y, bool B)
{
     L[(y-1)*Lx + x-1]=B;
}


void World::Turn_Over() // Turn the hourglass over
{
     long *A2=(long*)malloc(Lx*Ly*sizeof(long));
     for (long y=1;y<=Ly;y++)
	  for (long x=1;x<=Lx;x++)
	       A2[(y-1)*Lx+x-1]=Occupied_Q(x,Ly+1-y);
     free(A);
     A=A2;
}

void World::Show()
{
     for (long x=1;x<=Lx;x++)
         for (long y=1;y<=Ly;y++)
	 {
	     if (Occupied_Q(x,y))
		  EXSetColor(paleta[ Occupied_Q(x,y) ]);
	     if (Obstacle_Q(x,y))
		  EXSetColor(rojo);
	     if (!Obstacle_Q(x,y) && !Occupied_Q(x,y))
		  EXSetColor(negro);
	     EXFillRectangle(dot*(x-1),dot*(Ly-y),dot,dot);
	 }
     if (EXKeyPressed()) 
     {
	  char q=EXReadKey();
	  if (q=='c') 
	       Turn_Over();
	  if (q==' ')
	       EXReadKey();
	  if (q=='q')
	       exit(1);
     }
}
	   
/////////////////////////////////
// Different types of obstacles

void World::Skew_Line()
{
     for (long x=1;x<=Lx;x++)
     {
	  Put_Obstacle(x,x,true);
	  Put_Obstacle(x,x+1,true);
     }
}

void World::Reinforce() // remove holes in the construction
{
     for (long y=1;y<=Ly;y++)
     {
	  long x=1;
	  do
	       x++;
	  while(!Obstacle_Q(x,y));
	  for (long xx=1;xx<x;xx++)
	       Put_Obstacle(xx,y,true);
     }
}

void World::Symmetrize()
{
     for (long y=1;y<=Ly;y++)
	  for (long x=1;x<=Lx;x++)
	       if (Obstacle_Q(x,y)) Put_Obstacle(Lx+1-x,y,true);
}

void World::Hourglass() // Build the real hourglass figure
{
     for (long y=1;y<=Ly;y++)
     {
	  long x1=(long)floor((double)Lx/4.001*
			    (1.0-cos( 2.0*M_PI*(double)(y-1)/(double)Ly) ));
	  if (x1>Lx) x1=Lx; if (x1<=0) x1=1;
	  Put_Obstacle(x1,y,true);
     }
     Reinforce();
     Symmetrize();
}

////////////////////////////////////////////////////////////////////////

World::World(long lx, long ly)
{
     Lx=lx, Ly=ly;
     A=(long*)malloc(Lx*Ly*sizeof(long));
     L=(bool*)malloc(Lx*Ly*sizeof(bool));     
     
     for (long i=0;i<Lx*Ly;i++) { A[i]=0; L[i]=false; }
     Hourglass();
     for (long x=1;x<=Lx;x++)
     	  for (long y=Ly;y>=Ly-Ncolors;y--)
     	       if (!Obstacle_Q(x,y))
     		    Put(x,y,rand_int(1,Ncolors));
}

World::~World()
{
     free(A);
     free(L);
}

void World::Move_Grain(long x, long y)
{
     long i,j, ir=0, jr=0;
     long color=Occupied_Q(x,y);
     // If at the lowest level, done
     if (y==1) return;
     // First attempt: go just below
     if (!Occupied_Q(x,y-1) && !Obstacle_Q(x,y-1))
     {
	  Put(x,y-1,color);
	  Put(x,y,0);
	  return;
     }
     // // Otherwise, we try a diagonal movement
     i=rand_int(0,1)*2-1;
     if (!Occupied_Q(x+i,y-1) && !Obstacle_Q(x+i,y-1)) 
     {
     	  Put(x+i,y-1,color);   
     	  Put(x,y,0);
     	  return;
     }

     // Otherwise, attempt horizontal movement
     if (rand_double()<fluidity) 
     	  if (!Occupied_Q(x+i,y) && !Obstacle_Q(x+i,y))
     	  {
     	       Put(x+i,y,color);
     	       Put(x,y,0);
     	  }

}

// Random update
void World::Move()
{
     for (long y=Ly;y>=1;y--)
	  for (long x=1;x<=Lx;x++)
	       Move_Grain(x,y);
     // for (long t=1;t<=Lx*Ly;t++)
     // {
     // 	  long x=rand_int(1,Lx);
     // 	  long y=rand_int(1,Ly);
     // 	  Move_Grain(x,y);
     // }
}

void Wait(double dt)
{
     double inicio=uhr();
     do {}
     while(uhr()-inicio<dt);
}
     

int main()
{
     long Lx=80, Ly=120;
     open_rng(0);
     EXStart(100,100,dot*Lx,dot*Ly);
     blanco=EXAllocNamedColor("yellow");
     rojo=EXAllocRGBColor(0,0,0.5);
     negro=EXAllocNamedColor("black");
     paleta = EXPalette(0.5,0.5,0,1.0,1.0,0,Ncolors);

     double dt=0.05; // real time interval
     World W(Lx,Ly);
     W.Hourglass();
     W.Show();
     do
     {
         W.Move();
     	 W.Show();
     	 Wait(dt);
     }while(true);

     EXClose();
}
