#include<cstdio>
#include<cstdlib>
#include<math.h>
#include<cstring>
#include<string>
#include<iostream>
#include<fstream>
#include<ctime>
#include<sys/time.h>
#include<vector>

using namespace std;

int len=0,state,mstate,*test,*ans,*result,**trace;
double **a,**b,**b1,*pi,**alpha,**beta,**dp,*acc_ga,**obs_ga,**acc_eps;
vector<int> encode[10000];

double sumlog(double loga,double logb){
	if(isinf(loga)) return logb;
	if(isinf(logb)) return loga;
	return loga+log(1+exp(logb-loga));
}

double dividelog(double loga,double logb){
	if(isinf(loga) || isinf(logb)) return log(0);
	return loga-logb;
}

void Viterbi(bool istest){
	double maxv;
	int t,maxi;
	for(auto &p:encode[state*test[0]+test[1]]) dp[p][0] = pi[p]+b[p][state*test[0]+test[1]];
	for(int i=0;i<mstate;i++) trace[i][0] = 0;
	for(t=1;t<=len-2;t++){
		for(auto &q:encode[state*test[t]+test[t+1]]){
			maxv = log(0);
			maxi = 0;
			for(auto &p:encode[state*test[t-1]+test[t]]){
				if(dp[p][t-1]+a[p][q%state]>maxv){
					maxv = dp[p][t-1]+a[p][q%state];
					maxi = p;
				}
			}
			dp[q][t]=maxv+b[q][state*test[t]+test[t+1]];
			trace[q][t]=maxi;
		}
	}
	maxv = log(0);
	maxi = 0;
	for(auto &p:encode[state*test[len-2]+test[len-1]]){
		if(dp[p][len-2]>maxv){
			maxv = dp[p][len-2];
			maxi = p;
		}
	}

	result = (int*)malloc(sizeof(int)*(len-1));
	result[len-2] = maxi;
	for(t=len-3;t>=0;t--) result[t] = trace[result[t+1]][t+1];

	for(int i=0;i<30;i++) printf("%d,%d ",result[i]/37,result[i]%37);
	printf("\n");

	/*
	if(!istest){
		double acc=0.0;
		for(t=0;t<len;t++) if(ans[t]==result[t]) acc++;
		acc = 100*acc/len;
		printf("\tAccuracy: %.2lf\n",acc);
	}
	*/

}

int main(int argc, char *argv[]){
	srand(time(NULL));

	//get the state number
	char s[30],ss[30],tmp[30];
	int i,j,k,t;
	//i = scanf("%s",s);	//valid or test
	bool istest;
	if(strcmp(argv[1],"test1")==0 || strcmp(argv[1],"test2")==0) istest=true;
	else istest=false;
	printf("%d\n",istest);
	strcpy(s,argv[1]);

	memcpy(&ss,&s,sizeof(s));
	strcat(ss,"/encode.bin");
	FILE *f;
	f = fopen(ss,"r");
	while(fgets(tmp,30,f)!=NULL) state++;
	//mstate=state*state
	mstate=state;
	state = int(sqrt(state));
	printf("%d %d\n",state,mstate);
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
	if(strcmp(argv[1],"valid")==0 || strcmp(argv[1],"valid2")==0){
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
	}

	printf("Read file done.\n");


	//initialize the pi_i
	pi = (double*)malloc(mstate*sizeof(double));
	for(i=0;i<mstate;i++) pi[i]=1/double(mstate);

	//initialize the a_ij table
	a = (double**)malloc(mstate*sizeof(double *));
	for(i=0;i<mstate;i++) a[i] = (double*)malloc(state*sizeof(double));

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
	
	for(i=0;i<mstate;i++)
		for(j=0;j<state;j++) a[i][j]=double(1)/double(state);
	

	//initialize the b_ij table with encode.bin
	b1 = (double**)malloc(state*sizeof(double *));
	for(i=0;i<state;i++) b1[i] = (double*)malloc(state*sizeof(double));
	b = (double**)malloc(mstate*sizeof(double *));
	for(i=0;i<mstate;i++) b[i] = (double*)malloc(mstate*sizeof(double));

	FILE *fp;
	fp = fopen(ss,"r");
	while(fscanf(fp,"%d %d %d",&i,&j,&k)!=EOF) b1[i][j]=k;
	fclose(fp);
	
	for(int i=0;i<state;i++)
		for(int j=0;j<state;j++)
			for(int m=0;m<state;m++)
				for(int n=0;n<state;n++)
					b[i*state+j][m*state+n] = b1[i][m]*b1[j][n];

	
	for(i=0;i<mstate;i++){
		sum=0.0;
		for(j=0;j<mstate;j++) if(b[i][j]!=0) sum++;
		for(j=0;j<mstate;j++) if(b[i][j]!=0) b[i][j] = double(1)/sum;
	}
	
	//encode accelerate
	for(j=0;j<mstate;j++)
		for(i=0;i<mstate;i++)
			if(b[i][j]!=0) encode[j].push_back(i);

	//pi,a,b to log
	for(i=0;i<mstate;i++){
		for(j=0;j<mstate;j++){
			b[i][j] = log(b[i][j]);
		}
		pi[i] = log(pi[i]);
	}
	for(i=0;i<mstate;i++)
		for(j=0;j<state;j++)
			a[i][j] = log(a[i][j]);

	printf("Init done.\n");




	//Start to do Baum-Welch algorithm	
	int ITER = 10,IT;
	for(IT=0;IT<ITER;IT++){

		alpha = (double**)malloc(mstate*sizeof(double *));
		for(i=0;i<mstate;i++) alpha[i] = (double*)malloc((len-1)*sizeof(double));
		
		beta = (double**)malloc(mstate*sizeof(double *));
		for(i=0;i<mstate;i++) beta[i] = (double*)malloc((len-1)*sizeof(double));
		
		acc_ga = (double*)malloc(mstate*sizeof(double));
		obs_ga = (double**)malloc(mstate*sizeof(double *));
		for(i=0;i<mstate;i++) obs_ga[i] = (double*)malloc(mstate*sizeof(double));
		acc_eps = (double**)malloc(mstate*sizeof(double *));
		for(i=0;i<mstate;i++) acc_eps[i] = (double*)malloc(state*sizeof(double));
		
		struct timeval st,et,sst;
		double dt=0.0;
		gettimeofday(&sst,NULL);
		gettimeofday(&st,NULL);

		//Forward
		//initialize alpha
		gettimeofday(&st,NULL);
		for(auto &p:encode[state*test[0]+test[1]]) alpha[p][0]=pi[p]+b[p][state*test[0]+test[1]];
		for(t=0;t<=len-3;t++){
			for(auto &q:encode[state*test[t+1]+test[t+2]]){
				sum=log(0);
				for(auto &p:encode[state*test[t]+test[t+1]]) sum = sumlog(sum,alpha[p][t]+a[p][q%state]);
				alpha[q][t+1] = sum+b[q][state*test[t+1]+test[t+2]];
			}
		}
		gettimeofday(&et,NULL);
		dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
		printf("alpha done: %.3fs\n",dt);

		//Backward
		//initialize beta
		gettimeofday(&st,NULL);
		for(i=0;i<mstate;i++) beta[i][len-2]=log(1);
		for(t=len-3;t>=0;t--){
			for(auto &p:encode[state*test[t]+test[t+1]]){
				beta[p][t]=log(0);
				for(auto &q:encode[state*test[t+1]+test[t+2]]) beta[p][t] = sumlog(beta[p][t],a[p][q%state]+b[q][state*test[t+1]+test[t+2]]+beta[q][t+1]);
			}
		}
		gettimeofday(&et,NULL);
		dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
		printf("beta done: %.3fs\n",dt);





		//gamma, epsilon
		//initialize gamma
		gettimeofday(&st,NULL);

		for(int i=0;i<mstate;i++){
			for(int j=0;j<mstate;j++) obs_ga[i][j]=log(0);
			acc_ga[i]=log(0);
		}

		for(t=0;t<=len-3;t++){
			for(auto &p:encode[state*test[t]+test[t+1]]){
				acc_ga[p] = sumlog(acc_ga[p], alpha[p][t]+beta[p][t]);
				obs_ga[p][state*test[t]+test[t+1]] = sumlog(obs_ga[p][state*test[t]+test[t+1]], alpha[p][t]+beta[p][t]);
			}
		}

		gettimeofday(&et,NULL);
		dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
		printf("gamma done: %.3fs\n",dt);

		//accumulate epsilon
		gettimeofday(&st,NULL);
		for(int i=0;i<mstate;i++)
			for(int j=0;j<state;j++) acc_eps[i][j]=log(0);
		for(t=0;t<=len-3;t++){
			for(auto &p:encode[state*test[t]+test[t+1]])
				for(auto &q:encode[state*test[t+1]+test[t+2]]) acc_eps[p][q%state] = sumlog(acc_eps[p][q%state], alpha[p][t]+a[p][q%state]+b[q][state*test[t+1]+test[t+2]]+beta[q][t+1]);
		}
		gettimeofday(&et,NULL);
		dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
		printf("epsilon done: %.3fs\n",dt);

		//update parameters
		gettimeofday(&st,NULL);

		//pi_i
		sum=log(0);
		for(auto &p:encode[state*test[0]+test[1]]) sum = sumlog(sum,alpha[p][0]+beta[p][0]);
		for(int i=0;i<mstate;i++) pi[i]=dividelog(alpha[i][0]+beta[i][0],sum);

		//a_ij
		for(int i=0;i<mstate;i++)
			for(int j=0;j<state;j++) a[i][j] = dividelog(acc_eps[i][j], acc_ga[i]);

		//b_ik
		for(auto &p:encode[state*test[len-2]+test[len-1]]) acc_ga[p] = sumlog(acc_ga[p], alpha[p][len-2]+beta[p][len-2]);
		for(int k=0;k<mstate;k++)
			for(int i=0;i<mstate;i++) b[i][k] = dividelog(obs_ga[i][k], acc_ga[i]);
			

		gettimeofday(&et,NULL);
		dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
		printf("Update parameters done: %.3fs\n",dt);


		free(alpha);
		free(beta);
		free(acc_ga);
		free(obs_ga);
		free(acc_eps);



		//Viterbi
		if(!istest){

			gettimeofday(&st,NULL);
			//Viterbi initialize
			dp = (double**)malloc(mstate*sizeof(double *));
			for(i=0;i<mstate;i++) dp[i] = (double*)malloc((len-1)*sizeof(double));
			trace = (int**)malloc(mstate*sizeof(int *));
			for(i=0;i<mstate;i++) trace[i] = (int*)malloc((len-1)*sizeof(int));

			Viterbi(istest);

			free(dp);
			free(trace);

			gettimeofday(&et,NULL);
			dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
			printf("\tViterbi done: %.3fs\n",dt);
		}
		
		dt = ((et.tv_sec  - sst.tv_sec) * 1000000u + et.tv_usec - sst.tv_usec) / 1.e6;
		printf("\tITER %d time: %.3fs\n",IT+1,dt);
	}

	dp = (double**)malloc(mstate*sizeof(double *));
	for(i=0;i<mstate;i++) dp[i] = (double*)malloc((len-1)*sizeof(double));
	trace = (int**)malloc(mstate*sizeof(int *));
	for(i=0;i<mstate;i++) trace[i] = (int*)malloc((len-1)*sizeof(int));
	Viterbi(istest);
	if(istest){
		memcpy(&ss,&s,sizeof(s));
		strcat(ss,"/pred.num");
		fp = fopen(ss,"w+");
		for(int t=0;t<len;t++){
			fprintf(fp,"%d ",result[t]);
		}
		fclose(fp);
	}


	return 0;
}
