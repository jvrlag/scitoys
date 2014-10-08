// Program to show the movement of vortices in Chorin's approach to 2D
// fluid mechanics. Periodic boundary conditions.
#include"matrix.h"
#include"easyx.h"

// get "fractional part" of x, into [0,1]
void clamp(double &x)
{
     x=fmod(x,1.0);
     if (x<0.0) x+=1.0;
}

// in a torus of size 1x1, get the separation vector btw two points
void separation_vector(double x0, double y0, double x1, double y1,
		       double &rx, double &ry)
{
     rx=( fabs(x1-x0)<0.5 ? x1-x0 : x0-x1);
     ry=( fabs(y1-y0)<0.5 ? y1-y0 : y0-y1);
}

// in a torus of size 1x1, find the distance between two points
double distance(double x0, double y0, double x1, double y1)
{
     double rx, ry;
     separation_vector(x0,y0,x1,y1,rx,ry);
     return (sqrt(rx*rx+ry*ry));
}

// forward and backward, in a lattice of size Lx
#define f(x) (x==Lx ? 1 : x+1)
#define b(x) (x==1 ? Lx : x-1)

// The velocity field is given just by two matrices
typedef struct
{
     Matrix X, Y;
}Velocity_Field;


//////// CLASS VORTICES

class Vortices
{
public:
     long N; // number of vortices
     double regulator; // minimal distance considered
     double visc;      // viscous motion: random motion of the vortices
     Vector X, Y, G;   // X,Y: positions; G: strengths
     
     Vortices(long n);
     ~Vortices() {};
     void Evolve(double dt);
     void Show() const;
     void Get_Velocity_Field(Velocity_Field &V, long lx) const; 
};

Vortices::Vortices(long n): N(n), X(n), Y(n), G(n)
{
     regulator=0.1;
     visc=1e-3;
     for (long i=1;i<=N;i++)
     {
	  X(i)=rand_double();
	  Y(i)=rand_double();
	  G(i)=300.0*(rand_double()-0.5);
     }
}

// Velocity field updating
void Vortices::Get_Velocity_Field(Velocity_Field &V, long Lx) const
{
     V.X.Create(Lx); V.Y.Create(Lx);
     for (long x=1;x<=Lx;x++)
	  for (long y=1; y<=Lx; y++)  // For all sites in the lattice...
	  {
	       V.X(x,y)=0.0; V.Y(x,y)=0.0;
	       for (long i=0;i<N;i++) // summing for all vortices
	       {
		    double rx, ry;
		    separation_vector((double)x/(double)Lx,
				      (double)y/(double)Lx,X(i),Y(i),rx,ry);
		    double dist=sqr(rx)+sqr(ry)+regulator;
		    V.X(x,y)+=G(i)*ry/dist;
		    V.Y(x,y)-=G(i)*rx/dist;
	       }
	  }
}     

// Show function for the vortices
void Vortices::Show() const
{
     static long xsize=EXCW->width;
     EXColor("white");
     for (long i=1;i<=N;i++)
     {
	  int px=(int)(xsize*X(i));
	  int py=(int)(xsize*Y(i));
	  EXCircle(px,py,(int)(fabs(0.1*G(i))));
     }
}

void Vortices::Evolve(double dt)
{
     // Find the velocity fields for each of them
     Vector vx(N), vy(N);
     for (long i=1;i<=N;i++)
     {
	  for (long j=1;j<=N;j++)
	       if (j!=i)
	       {
		    double rx, ry;
		    separation_vector(X(i),Y(i),X(j),Y(j),rx,ry);
		    double distance=rx*rx+ry*ry+regulator;
		    vx(i)+=G(j)*(ry)/distance;
		    vy(i)-=G(j)*(rx)/distance;
	       }
     }
     for (long i=1;i<=N;i++)
     {
	  X(i)+=dt*vx(i); 
	  Y(i)+=dt*vy(i);
	  // Now adding a little bit of noise to simulate the 
	  // effect of viscosity
	  X(i)+=gaussian_dev(0.0,1.0)*sqrt(dt)*visc;
	  Y(i)+=gaussian_dev(0.0,1.0)*sqrt(dt)*visc; 

	  // Toroidal boundary conditions
	  if (X(i)>1.0) X(i)-=1.0;
	  if (X(i)<0.0) X(i)+=1.0;
	  if (Y(i)>1.0) Y(i)-=1.0;
	  if (Y(i)<0.0) Y(i)+=1.0;
     }
}


////// CLASS PASSIVE FIELD

class Passive_Field
{
public:
     long Lx; // grid size
     Matrix F; // Passive Scalar Data
     double Diffusion; 
     
     Passive_Field(long lx);
     ~Passive_Field() {};
     void Initialize();
     void Show(palette *Pal) const;
     void Evolve(const Velocity_Field &V, double dt);
};

Passive_Field::Passive_Field(long lx) : Lx(lx) , F(lx)
{
     // Some initial configuration
     for (long x=1;x<=Lx;x++)
	  for (long y=1;y<=Lx;y++)
	       F(x,y)=sin(M_PI*(double)y/(double)Lx);
     // Yes, the diffusion constant seems pretty high
     Diffusion=5000.0;
}     

// Show function for the passive scalar field
void Passive_Field::Show(palette *Pal) const
{
     static long xsize=EXCW->width;
     static long c=(int)((double)xsize/(double)Lx); // cell size
     static long Ncolors=(long)Pal[0];
     
     for (long x=1; x<=Lx;x++)
	  for (long y=1; y<=Lx; y++)
	  {
	       long px=(x-1)*c; int py=(y-1)*c;
	       long index=(int)fabs(F(x,y)*Ncolors);
	       if (index<1) index=1; if (index>Ncolors) index=Ncolors;
	       EXSetColor(Pal[index]);
	       EXFillRectangle(px,py,c,c);
	  }
}

// Movement routine for the passive scalar field
void Passive_Field::Evolve(const Velocity_Field &V, double dt)
{
     double dx=1.0/(double)Lx;
     Matrix Fold(F);

     // The advection equation
     for (long x=1;x<=Lx; x++)
	  for (long y=1;y<=Lx;y++)
	  {
	       double gradx=(Fold(f(x),y)-Fold(b(x),y))/(2.0*dx);
	       double grady=(Fold(x,f(y))-Fold(x,b(y)))/(2.0*dx);
	       double advection=V.X(x,y)*gradx + V.Y(x,y)*grady;
	       double laplacian=Fold(f(x),y)+Fold(b(x),y)+
		    Fold(x,f(y))+Fold(x,b(y))   
		    -4.0*Fold(x,y);
	       F(x,y)-=dt*(advection-Diffusion*laplacian);
	  }
     // A random long range forcing is added
     if (rand_double()<0.1) 
     {
     	  double xc=rand_double();
     	  double yc=rand_double();
     	  long R=(long)((rand_double()+0.2)*Lx/10.0);
     	  double value=rand_double(-0.25,0.25);

     	  for (long xi=-R;xi<=R;xi++)
     	       for (long yi=-R;yi<=R;yi++)
     	       {
     		    double x=xc-(double)xi/(double)Lx;
     		    double y=yc-(double)yi/(double)Lx;
     		    clamp(x); clamp(y);			    
     		    double d=distance(x,y,xc,yc);
     		    if (d<(double)R/(double)Lx) 
     		    {
     			 long px=(int)(Lx*x)+1;
     			 long py=(int)(Lx*y)+1;
     			 F(px,py)+=value;
     		    }
     	       }
     }
}


//////////////// CLASS WORLD, container of the rest

class World
{
public:
     long Lx; // lattice size
     double dt;  // dt for the simulation
     Passive_Field F;
     Velocity_Field V;
     Vortices W;

     World(long lx, double difft, long nv);
     ~World() {};
     void Evolve();
     void Show(palette *Pal) const;
};

World::World(long lx, double difft, long Nv): Lx(lx), dt(difft), F(Lx), W(Nv)
{
     Lx=lx; dt=difft;
}

void World::Evolve()
{
     W.Evolve(dt);
     W.Get_Velocity_Field(V,Lx);
     F.Evolve(V,dt);
}

void World::Show(palette *Pal) const
{
     F.Show(Pal);
     W.Show();
     EXFlush();
}

int main()
{
     long Lx=75;
     double dt=5e-6;
     long Nv=15;

     open_rng(0);

     int xsize=600; // size of the X-Window
     int Ncolors=30;
     palette *Pal;
     EXStart(100,100,xsize,xsize);
     EXEnableBuffer();
     Pal=EXPalette(0,0,0,1,0,0,Ncolors);
     Pal[0]=Ncolors;

     // Initialization of the variables
     World W(Lx,dt,Nv);
     
     long totalzeit=10000;
     for(long zeit=0;zeit<totalzeit;zeit++)
     {
	  W.Evolve();
	  W.Show(Pal);

     }
}








