#include <stdio.h>

int main(){
	int n,m;
	int vec[256];
	for(int  i= 0;i<256;i++){
		vec[i] = 0;
	} 
	FILE *f = fopen("uneq.pgm","r");
	FILE *fo = fopen("out.txt","w");
	fscanf(f,"%d %d",&n,&m);
	for(int  i = 0;i<n;i++){
		for(int  j =0;j<m;j++){
			int t1,t2;
			fscanf(f,"%d",&t1);
			vec[t1]++;
		}
		printf("\n");
	}
	for(int  i = 0;i<256;i++){
		fprintf(fo,"%d - %d\n",i, vec[i]);
	}
	
}