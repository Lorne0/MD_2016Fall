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

int len=0,state,*test,*ans,*result,**trace;
double **a,**b,*pi,**alpha,**beta,**dp;
vector<int> encode[100];

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
	for(auto &p:encode[test[0]]) dp[p][0] = pi[p]+b[p][test[0]];
	for(int i=0;i<state;i++) trace[i][0] = 0;
	for(t=1;t<len;t++){
		for(auto &q:encode[test[t]]){
			maxv = log(0);
			maxi = 0;
			for(auto &p:encode[test[t-1]]){
				if(dp[p][t-1]+a[p][q]>maxv){
					maxv = dp[p][t-1]+a[p][q];
					maxi = p;
				}
			}
			dp[q][t]=maxv+b[q][test[t]];
			trace[q][t]=maxi;
		}
	}
	maxv = log(0);
	maxi = 0;
	for(auto &p:encode[test[len-1]]){
		if(dp[p][len-1]>maxv){
			maxv = dp[p][len-1];
			maxi = p;
		}
	}

	result = (int*)malloc(sizeof(int)*len);
	result[len-1] = maxi;
	for(t=len-2;t>=0;t--) result[t] = trace[result[t+1]][t+1];


	if(!istest){
		double acc=0.0;
		for(t=0;t<len;t++) if(ans[t]==result[t]) acc++;
		acc = 100*acc/len;
		printf("\tAccuracy: %.2lf\n",acc);
	}
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
	
	for(i=0;i<state;i++)
		for(j=0;j<state;j++) a[i][j]=double(1)/double(state);
	

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
	
	//encode accelerate
	for(j=0;j<state;j++)
		for(i=0;i<state;i++)
			if(b[i][j]!=0) encode[j].push_back(i);

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
	//Viterbi(0,istest);

	alpha = (double**)malloc(state*sizeof(double *));
	for(i=0;i<state;i++) alpha[i] = (double*)malloc(len*sizeof(double));
	
	beta = (double**)malloc(state*sizeof(double *));
	for(i=0;i<state;i++) beta[i] = (double*)malloc(len*sizeof(double));
	


	//Start to do Baum-Welch algorithm	
	int ITER = 50,IT;
	for(IT=0;IT<ITER;IT++){
		
		struct timeval st,et,sst;
		double dt=0.0;
		gettimeofday(&sst,NULL);
		gettimeofday(&st,NULL);

		//Forward
		//initialize alpha
		gettimeofday(&st,NULL);
		for(auto &p:encode[test[0]]) alpha[p][0]=pi[p]+b[p][test[0]];
		for(t=0;t<=len-2;t++){
			//#pragma omp parallel for num_threads(2) private(sum)
			for(auto &q:encode[test[t+1]]){
				sum=log(0);
				for(auto &p:encode[test[t]]) sum = sumlog(sum,alpha[p][t]+a[p][q]);
				alpha[q][t+1] = sum+b[q][test[t+1]];
			}
		}
		gettimeofday(&et,NULL);
		dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
		printf("alpha done: %.3fs\n",dt);

		//Backward
		//initialize beta
		gettimeofday(&st,NULL);
		for(i=0;i<state;i++) beta[i][len-1]=log(1);
		for(t=len-2;t>=0;t--){
			//#pragma omp parallel for num_threads(5) private(sum)
			for(auto &p:encode[test[t]]){
				beta[p][t]=log(0);
				for(auto &q:encode[test[t+1]]) beta[p][t] = sumlog(beta[p][t],a[p][q]+b[q][test[t+1]]+beta[q][t+1]);
			}
		}
		gettimeofday(&et,NULL);
		dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
		printf("beta done: %.3fs\n",dt);



		double acc_ga[state];
		double obs_ga[state][state];
		double acc_eps[state][state];
		for(int i=0;i<state;i++){
			for(int j=0;j<state;j++){
				acc_eps[i][j]=log(0);
				obs_ga[i][j]=log(0);
			}
			acc_ga[i]=log(0);
		}


		//gamma, epsilon
		//initialize gamma
		gettimeofday(&st,NULL);
		//#pragma omp parallel for private(sum) num_threads(5)
		for(t=0;t<=len-2;t++){
			for(auto &p:encode[test[t]]){
				acc_ga[p] = sumlog(acc_ga[p], alpha[p][t]+beta[p][t]);
				obs_ga[p][test[t]] = sumlog(obs_ga[p][test[t]], alpha[p][t]+beta[p][t]);
			}
			/*
			sum=log(0);
			double tmp[state];
			for(auto &p:encode[test[t]]){
				tmp[p]=alpha[p][t]+beta[p][t];
				//acc_ga[i] = sumlog(acc_ga[i],tmp);
				sum = sumlog(sum,tmp[p]);
			}
			for(auto &p:encode[test[t]]){
				double tmp2=dividelog(tmp[p],sum);
				acc_ga[p] = sumlog(acc_ga[p], tmp2);
				obs_ga[p][test[t]] = sumlog(obs_ga[p][test[t]], tmp2);
			}
			*/
		}
		gettimeofday(&et,NULL);
		dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
		printf("gamma done: %.3fs\n",dt);

		//accumulate epsilon
		gettimeofday(&st,NULL);
		for(t=0;t<=len-2;t++){
			for(auto &p:encode[test[t]])
				for(auto &q:encode[test[t+1]]) acc_eps[p][q] = sumlog(acc_eps[p][q], alpha[p][t]+a[p][q]+b[q][test[t+1]]+beta[q][t+1]);
			/*
			sum=log(0);
			double tmp[state][state];
			for(auto &p:encode[test[t]]){
				for(auto &q:encode[test[t+1]]){
					tmp[p][q]=alpha[p][t]+a[p][q]+b[q][test[t+1]]+beta[q][t+1];
					sum = sumlog(sum,tmp[p][q]);
				}
			}
			for(auto &p:encode[test[t]])
				for(auto &q:encode[test[t+1]]) acc_eps[p][q] = sumlog(acc_eps[p][q], dividelog(tmp[p][q],sum));
			*/

		}
		gettimeofday(&et,NULL);
		dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
		printf("epsilon done: %.3fs\n",dt);

		//update parameters
		gettimeofday(&st,NULL);

		//pi_i
		sum=log(0);
		for(auto &p:encode[test[0]]) sum = sumlog(sum,alpha[p][0]+beta[p][0]);
		for(int i=0;i<state;i++) pi[i]=dividelog(alpha[i][0]+beta[i][0],sum);

		//a_ij
		for(int i=0;i<state;i++)
			for(int j=0;j<state;j++) a[i][j] = dividelog(acc_eps[i][j], acc_ga[i]);

		//b_ik
		//#pragma omp parallel for num_threads(5) collapse(2) private(sum)
		for(auto &p:encode[test[len-1]]) acc_ga[p] = sumlog(acc_ga[p], alpha[p][len-1]+beta[p][len-1]);
		/*
		sum=log(0);
		for(auto &p:encode[test[len-1]]) sum = sumlog(sum, alpha[p][len-1]+beta[p][len-1]);
		for(auto &p:encode[test[len-1]]) acc_ga[p] = sumlog(acc_ga[p], dividelog(alpha[p][len-1]+beta[p][len-1], sum));
		*/
		for(int k=0;k<state;k++)
			for(int i=0;i<state;i++) b[i][k] = dividelog(obs_ga[i][k], acc_ga[i]);
			

		gettimeofday(&et,NULL);
		dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
		printf("Update parameters done: %.3fs\n",dt);


		//Viterbi
		if(!istest){
			gettimeofday(&st,NULL);
			Viterbi(istest);
			gettimeofday(&et,NULL);
			dt = ((et.tv_sec  - st.tv_sec) * 1000000u + et.tv_usec - st.tv_usec) / 1.e6;
			printf("\tViterbi done: %.3fs\n",dt);
		}
		
		dt = ((et.tv_sec  - sst.tv_sec) * 1000000u + et.tv_usec - sst.tv_usec) / 1.e6;
		printf("\tITER %d time: %.3fs\n",IT+1,dt);
	}

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
