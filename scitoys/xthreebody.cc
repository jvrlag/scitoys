// 110301
// Numerical solution of the three-body problem
#include"easyx.h" // graphical output
#include"matrix.h" // vectors and matrices

// A simple Euler algorithm, quick and dirty
void Dynamics(const Vector &M, Vector &X, Vector &Y, 
	      Vector &Vx, Vector &Vy, double dt)
{
     long N=M.N;
     for (long i=1;i<=N;i++)
     {
	  double Fx=0.0, Fy=0.0;
	  for (long j=1;j<=N;j++)
	       if (i!=j)
	       {
		    double dist2=sqr(X(i)-X(j))+sqr(Y(i)-Y(j));
		    double dist3=pow(dist2,3.0/2.0);
		    if (dist3<1e-3) dist3=1e-3; // cutoff to find force
		    Fx+=(X(j)-X(i))/dist3;
		    Fy+=(Y(j)-Y(i))/dist3;
	       }
	  X(i)+=Vx(i)*dt;
	  Y(i)+=Vy(i)*dt;
	  Vx(i)+=Fx*dt;
	  Vy(i)+=Fy*dt;
     }
}

// Draw the scene
void Show(const Vector &M, const Vector &X, const Vector &Y, palette *Pal)
{
     long xsize=EXCW->width; // width of the current window
     long N=M.N; // number of particles
     EXClear();  // clear the window
     for (long i=1;i<=N;i++)
     {
	  int px=xsize/2+(int)floor(X(i)/10.0*xsize);
	  int py=xsize/2+(int)floor(Y(i)/10.0*xsize);
	  int r=(int)floor(M(i)*10.0);
	  EXSetColor(Pal[i]);
	  EXFillCircle(px,py,r);
     }
     EXFlush();
}

int main()
{
     long N=3;
     double dt=0.0001;
     long Nt=1000000; // number of time-steps
     long xsize=500; // size of the x-window
    
     Vector X(N), Y(N);
     Vector Vx(N), Vy(N);
     Vector M(N);

     EXStart(100,100,xsize,xsize); 
     EXEnableBuffer();
     open_rng(0);

     palette *Pal=(palette*)malloc((N+1)*sizeof(palette));
     for (long i=1;i<=N;i++)
     {
	  double f=(i-1)/(double)(N-1);
	  Pal[i]=EXAllocRGBColor(f,0.8,1.-f);
     }
     
     M.Set_Value(1.0);
     for (long k=1;k<=N;k++)
     {
	  X(k)=rand_double(-2.,2.);
	  Y(k)=rand_double(-2.,2.);
	  Vx(k)=rand_double(-.2,.2);
	  Vy(k)=rand_double(-.2,.2);
     }
     X(1)=-1.0; Y(1)=0.0; Vx(1)=0.0; Vy(1)=0.11;
     X(2)=0.0;  Y(2)=0.0; Vx(2)=0.0; Vy(2)=0.0;
     X(3)=1.17; Y(3)=0.0; Vx(3)=0.0; Vy(3)=-0.11;
     
     EXReadKey(); // wait for a key pressing to start

     for (long it=0;it<=Nt;it++)
     {
	  Dynamics(M,X,Y,Vx,Vy,dt);
	  if (it%8==0)
	       Show(M,X,Y,Pal);
     }
     
}
