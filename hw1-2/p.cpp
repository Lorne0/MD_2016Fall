#include<cstdio>
#include<cmath>
#include<cstdlib>
#include<ctime>
#include<sys/time.h>

double ***a;
//double *A;

double foo(double a,double b){
	if(isinf(b)) return a;
	if(isinf(a)) return b;
	return a+log(1+exp(b-a));
}

int main(){
	//clock_t st = clock();
	struct timeval st,et;
	gettimeofday(&st,NULL);

	int i,j,t;
	a = (double***)malloc(37*sizeof(double **));
	for(i=0;i<37;i++){
		a[i] = (double**)malloc(37*sizeof(double*));
		for(j=0;j<37;j++) a[i][j] = (double*)malloc(1000000*sizeof(double));
	}
	printf("Start\n");

/*
	#pragma omp parallel for num_threads(3)
	for(int n=0;n<50;n++) printf("%d ",n);
	printf("\n");
*/

	/*
	#pragma omp parallel for collapse(3) num_threads(30)
	for(t=0;t<1000000;t++){
		for(i=0;i<37;i++){
			for(j=0;j<37;j++){
				a[i][j][t] = j+37*i+37*37*t; 
			}
		}
	}
	printf("%lf\n",a[0][0][0]);
	printf("%lf\n",a[0][1][0]);
	printf("%lf\n",a[2][3][0]);
	printf("%lf\n",a[0][4][5]);
	printf("%lf\n",a[0][100][70]);
	*/

	//A = (double**)malloc(50000*sizeof(double*));
	//for(int i=0;i<50000;i++) A[i] = (double*)malloc(1000*sizeof(double));
	double A[37][37];
	for(int i=0;i<37;i++){
		for(int j=0;j<37;j++)	A[i][j]=-0.3*(i+j);
	}

	#pragma omp declare reduction(sumsum: double: omp_out=foo(omp_out,omp_in)) initializer(omp_priv=log(0))
	double sum;
	for(int t=0;t<100000;t++){
		sum=log(0);
		#pragma omp parallel for reduction(sumsum:sum) num_threads(5)
		for(int i=0;i<37;i++){
			for(int j=0;j<37;j++) sum = foo(sum,A[i][j]);
		}
	}

	printf("sum: %lf\n",sum);

	gettimeofday(&et,NULL);
	double delta = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
	printf("%lfs\n",delta);

}
