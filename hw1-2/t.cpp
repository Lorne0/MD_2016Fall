#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<cstring>

double *k;

int main(int argc,char *argv[]){
	k = (double*)malloc(5*sizeof(double));
	for(int i=0;i<5;i++) k[i]=double(i);
	for(int i=0;i<5;i++) printf("%lf\n",k[i]);
	free(k);
	k = (double*)malloc(5*sizeof(double));
	for(int i=0;i<5;i++) k[i]=double(i);
	for(int i;i<5;i++) printf("%lf\n",k[i]);
	return 0;
}
