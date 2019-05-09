#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<cstring>
#include<string>
#include<iostream>
#include<fstream>
#include<ctime>

using namespace std;

int len=0,state,*test,*ans,*result,**trace;
double **a,**b,*pi,**alpha,**beta,**ga,***eps,**dp;

double sumlog(double loga,double logb){
	if(isinf(loga)) return logb;
	if(isinf(logb)) return loga;
	return loga+log(1+exp(logb-loga));
}

double dividelog(double loga,double logb){
	if(isinf(loga) || isinf(logb)) return log(0);
	return loga-logb;
}

void Viterbi(){
	double maxv;
	int i,j,t,maxi;
	for(i=0;i<state;i++) dp[i][0] = pi[i]+b[i][test[0]];
	for(i=0;i<state;i++) trace[i][0] = 0;
	for(t=1;t<len;t++){
		for(j=0;j<state;j++){
			maxv = log(0);
			maxi = 0;
			for(i=0;i<state;i++){
				if(dp[i][t-1]+a[i][j]>maxv){
					maxv = dp[i][t-1]+a[i][j];
					maxi = i;
				}
			}
			dp[j][t]=maxv+b[j][test[t]];
			trace[j][t]=maxi;
		}
	}
	maxv = log(0);
	maxi = 0;
	for(i=0;i<state;i++){
		if(dp[i][len-1]>maxv){
			maxv = dp[i][len-1];
			maxi = i;
		}
	}

	result = (int*)malloc(sizeof(int)*len);
	result[len-1] = maxi;
	for(t=len-2;t>=0;t--) result[t] = trace[result[t+1]][t+1];

	double acc=0.0;
	for(t=0;t<len;t++) if(ans[t]==result[t]) acc++;
	acc = 100*acc/len;
	printf("\tAccuracy: %.2lf\n",acc);
}

int main(){
	srand(time(NULL));

	//get the state number
	char s[30],ss[30],tmp[30];
	int i,j,k,t;
	i = scanf("%s",s);	//valid or test
	memcpy(&ss,&s,sizeof(s));
	strcat(ss,"/encode.bin");
	FILE *f;
	f = fopen(ss,"r");
	while(fgets(tmp,30,f)!=NULL) state++;
	state = int(sqrt(state));
	printf("%d\n",state);
	fclose(f);

	//count length of the cipher
	char sss[30],c;
	memcpy(&sss,&s,sizeof(s));
	strcat(sss,"/test.num");
	printf("%s\n",sss);
	f = fopen(sss,"r");
	while(1){
		c = fgetc(f);
		if(c==' ') len++;
		if(feof(f)) break;
	}
	printf("%d\n",len);
	fclose(f);
	
	//Read test.num
	f = fopen(sss,"r");
	test = (int*)malloc(sizeof(int)*len);
	for(i=0;i<len;i++){
		j = fscanf(f,"%d",&k);
		test[i]=k;
	}
	fclose(f);

	//Read ans.num
	char ssss[30];
	memcpy(&ssss,&s,sizeof(s));
	strcat(ssss,"/ans.num");
	f = fopen(ssss,"r");
	ans = (int*)malloc(sizeof(int)*len);
	for(i=0;i<len;i++){
		j = fscanf(f,"%d",&k);
		ans[i]=k;
	}
	fclose(f);
	printf("Read file done.\n");


	//initialize the pi_i
	pi = (double*)malloc(state*sizeof(double));
	for(i=0;i<state;i++) pi[i]=1/double(state);

	//initialize the a_ij table
	a = (double**)malloc(state*sizeof(double *));
	for(i=0;i<state;i++) a[i] = (double*)malloc(state*sizeof(double));

	double ran[state];
	double sum=0.0;
	/*
	for(i=0;i<state;i++){
		for(j=0;j<state;j++) ran[j]=rand()%10000+1;
		sum=0.0;
		for(j=0;j<state;j++) sum+=ran[j];
		for(j=0;j<state;j++) a[i][j] = ran[j]/sum;
	}
	*/
	for(i=0;i<state;i++){
		for(j=0;j<state;j++) a[i][j]=double(1)/double(state);
	}

	//initialize the b_ij table with encode.bin
	b = (double**)malloc(state*sizeof(double *));
	for(i=0;i<state;i++) b[i] = (double*)malloc(state*sizeof(double));

	FILE *fp;
	fp = fopen(ss,"r");
	while(fscanf(fp,"%d %d %d",&i,&j,&k)!=EOF) b[i][j]=k;
	fclose(fp);
	/*
	for(i=0;i<state;i++){
		sum = 0.0;
		for(j=0;j<state;j++){
			if(b[i][j]!=0){
				b[i][j]=rand()%10000+1;
				sum+=b[i][j];
			}
		}
		for(j=0;j<state;j++){
			if(b[i][j]!=0) b[i][j] = double(b[i][j])/sum;
		}
	}
	*/
	for(i=0;i<state;i++){
		sum=0.0;
		for(j=0;j<state;j++) if(b[i][j]!=0) sum++;
		for(j=0;j<state;j++) if(b[i][j]!=0) b[i][j] = double(1)/sum;
	}


	//pi,a,b to log
	for(i=0;i<state;i++){
		for(j=0;j<state;j++){
			a[i][j] = log(a[i][j]);
			b[i][j] = log(b[i][j]);
		}
		pi[i] = log(pi[i]);
	}

	printf("Init done.\n");

	//Viterbi initialize
	dp = (double**)malloc(state*sizeof(double *));
	for(i=0;i<state;i++)
		dp[i] = (double*)malloc(len*sizeof(double));
	trace = (int**)malloc(state*sizeof(int *));
	for(i=0;i<state;i++)
		trace[i] = (int*)malloc(len*sizeof(int));
	Viterbi();

	alpha = (double**)malloc(state*sizeof(double *));
	for(i=0;i<state;i++) alpha[i] = (double*)malloc(len*sizeof(double));
	
	beta = (double**)malloc(state*sizeof(double *));
	for(i=0;i<state;i++) beta[i] = (double*)malloc(len*sizeof(double));
	
	ga = (double**)malloc(state*sizeof(double *));
	for(i=0;i<state;i++) ga[i] = (double*)malloc(len*sizeof(double));

	eps = (double***)malloc(state*sizeof(double **));
	for(i=0;i<state;i++){
		eps[i] = (double**)malloc(state*sizeof(double*));
		for(j=0;j<state;j++) eps[i][j] = (double*)malloc(len*sizeof(double));
	}


	double acc_ga[state];
	double acc_ga_T[state];
	double acc_eps[state][state];


	//Start to do Baum-Welch algorithm	
	int ITER = 100,IT;
	for(IT=0;IT<ITER;IT++){
		clock_t st = clock();
		clock_t sst = clock();

		//Forward
		//initialize alpha
		for(i=0;i<state;i++) alpha[i][0]=pi[i]+b[i][test[0]];
		for(t=0;t<=len-2;t++){
			for(j=0;j<state;j++){
				if(isinf(b[j][test[t+1]])){
					alpha[j][t+1] = log(0);
				}
				else{
					sum=alpha[0][t]+a[0][j];
					for(i=1;i<state;i++) sum = sumlog(sum,alpha[i][t]+a[i][j]);
					alpha[j][t+1] = sum+b[j][test[t+1]];
				}
			}
		}
		printf("alpha done: %.2fs\n",(double)(clock()-st)/CLOCKS_PER_SEC);

		//Backward
		//initialize beta
		st = clock();
		for(i=0;i<state;i++) beta[i][len-1]=log(1);
		for(t=len-2;t>=0;t--){
			for(i=0;i<state;i++){
				sum=a[i][0]+b[0][test[t+1]]+beta[0][t+1];
				for(j=1;j<state;j++) sum = sumlog(sum,a[i][j]+b[j][test[t+1]]+beta[j][t+1]);
				beta[i][t] = sum;
			}
		}
		printf("beta done: %.2fs\n",(double)(clock()-st)/CLOCKS_PER_SEC);

		//gamma, epsilon
		//initialize gamma
		st = clock();
		for(t=0;t<=len-1;t++){
			sum=alpha[0][t]+beta[0][t];
			for(i=1;i<state;i++) sum = sumlog(sum,alpha[i][t]+beta[i][t]);
			//if(isinf(sum)) for(i=0;i<state;i++) ga[i][t]=log(0);
			for(i=0;i<state;i++) ga[i][t] = dividelog(alpha[i][t]+beta[i][t],sum);
		}
		printf("gamma done: %.2fs\n",(double)(clock()-st)/CLOCKS_PER_SEC);

		//initialize epsilon
		st = clock();
		for(t=0;t<=len-1;t++){
			sum=log(0);
			for(i=0;i<state;i++){
				for(j=0;j<state;j++){
					sum = sumlog(sum,alpha[i][t]+a[i][j]+b[j][test[t+1]]+beta[j][t+1]);
				}
			}
			for(i=0;i<state;i++){
				for(j=0;j<state;j++){
					eps[i][j][t] = dividelog(alpha[i][t]+a[i][j]+b[j][test[t+1]]+beta[j][t+1], sum);
				}
			}
		}
		printf("epsilon done: %.2fs\n",(double)(clock()-st)/CLOCKS_PER_SEC);

		//update parameters
		st = clock();
		for(i=0;i<state;i++){
			sum=ga[i][0];
			for(t=1;t<len-1;t++) sum=sumlog(sum,ga[i][t]);
			acc_ga[i]=sum;
		}
		for(i=0;i<state;i++){
			acc_ga_T[i] = sumlog(acc_ga[i],ga[i][len-1]);
		}
		for(i=0;i<state;i++){
			for(j=0;j<state;j++){
				sum=eps[i][j][0];
				for(t=1;t<len-1;t++) sum=sumlog(sum,eps[i][j][t]);
				acc_eps[i][j] = sum;
			}
		}

		//pi_i
		for(i=0;i<state;i++) pi[i] = ga[i][0];

		//a_ij
		for(i=0;i<state;i++){
			for(j=0;j<state;j++) a[i][j] = dividelog(acc_eps[i][j], acc_ga[i]);
		}

		//b_ik
		for(i=0;i<state;i++){
			for(k=0;k<state;k++){
				sum = log(0);
				for(t=0;t<len;t++) if(test[t]==k) sum = sumlog(sum,ga[i][t]);
				b[i][k] = dividelog(sum,acc_ga_T[i]);
			}
		}

		printf("Update parameters done: %.2fs\n",(double)(clock()-st)/CLOCKS_PER_SEC);

		//Viterbi
		st = clock();
		Viterbi();
		printf("\tViterbi done: %.2fs\n",(double)(clock()-st)/CLOCKS_PER_SEC);
		
		printf("\tITER %d time: %.2fs\n",IT+1,(double)(clock()-sst)/CLOCKS_PER_SEC);
	}
	return 0;
}
