#include<cstdio>
#include<cstring>
#include<cmath>
#include<iostream>
#include<map>
#include<ctype.h>

using namespace std;

int chartoint(char c){
	if(isdigit(c)) return int(c-'0')+26;
	if(isalpha(c)) return int(c-'a');
	else return 36;
}

char inttochar(int i){
	if(i<=25) return char(i+int('a'));
	else if(i<36) return char(i-26+int('0'));
	else return ' ';
}

int length = 2000115;
double dp[37][2000115];
int track[37][2000115];
char pred[2000115];

int main(){

	double encode[37][37];
	double bigram[37][37];

	int i,j;
	for(i=0;i<37;i++){
		for(j=0;j<37;j++){
			encode[i][j] = double(log(0));
			bigram[i][j] = double(log(0));
		}
	}

	//read encode file
	FILE *f = fopen("encode.txt","r");
	char c;
	double v;
	int w;
	while((c=fgetc(f))!=-1){
		i = chartoint(c);
		fgetc(f);
		j = chartoint(fgetc(f));
		w = fscanf(f,"%lf",&v);
		fgetc(f);
		encode[i][j] = log(v);
	}
	fclose(f);

	//read bigram file
	f = fopen("bigram.txt","r");
	while((c=fgetc(f))!=-1){
		i = chartoint(c);
		fgetc(f);
		j = chartoint(fgetc(f));
		w = fscanf(f,"%lf",&v);
		fgetc(f);
		bigram[i][j] = log(v);
	}
	fclose(f);

	//Fill in the dp table
	for(i=0;i<37;i++) dp[i][0] = log(double(1/37.0));
	int cnt=1;
	double maxv,tmp;
	int maxj;
	f = fopen("test.txt","r");
	while((c=fgetc(f))!=-1){
		for(i=0;i<37;i++){
			maxv = log(0);
			maxj = 0;
			for(j=0;j<37;j++){
				tmp = dp[j][cnt-1] + bigram[j][i] + encode[j][chartoint(c)];
				if(tmp>maxv){
					maxv = tmp;
					maxj = j;
				}
			}
			dp[i][cnt] = maxv;
			track[i][cnt] = maxj;
		}
		cnt++;
	}
	
	//back trace the dp table 
	int maxi = 0;
	maxv = dp[0][2000114];
	for(i=1;i<37;i++){
		if(dp[i][2000114]>maxv){
			maxv = dp[i][2000114];
			maxi = i;
		}
	}
	pred[2000114] = inttochar(maxi);
	for(cnt=2000114;cnt>=1;cnt--){
		maxi = track[maxi][cnt];
		pred[cnt-1] = inttochar(maxi);
	}

	fclose(f);

	f = fopen("pred.txt","w+");
	fputs(pred,f);
	fclose(f);
	return 0;
}
