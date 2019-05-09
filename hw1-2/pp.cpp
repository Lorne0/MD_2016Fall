#include <stdio.h>
#include <omp.h>
struct m10x1 {int v[10];};
int A [] =       {84, 30, 95, 94, 36, 73, 52, 23, 2, 13};  
struct m10x1 S = {{ 0,  0,  0,  0,  0,  0,  0,  0, 0,  0}};
int n,m=0;

void print_m10x1(struct m10x1 x){
	  int i;
	    for(i=0;i<10;i++) printf("%d ",x.v[i]);
		  printf("\n");
}

struct m10x1 add_m10x1(struct m10x1 x,struct m10x1 y){
	  struct m10x1 r ={{ 0,  0,  0,  0,  0,  0,  0,  0, 0,  0}};
	    int i;
		  for (i=0;i<10;i++) r.v[i]=x.v[i]+y.v[i];
		    return r;
}

#pragma omp declare reduction(m10x1Add: struct m10x1: omp_out=add_m10x1(omp_out, omp_in)) initializer( omp_priv={{ 0,  0,  0,  0,  0,  0,  0,  0, 0,  0}} )

int main ()
{
	  #pragma omp parallel for reduction(m10x1Add: S)
	  for ( n=0 ; n<10 ; ++n )
		      {
				        for (m=0; m<=n; ++m){
							        S.v[n] += A[m];
									      }
						    }
	    print_m10x1(S);
}
