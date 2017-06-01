#include<stdio.h>
#include<stdlib.h>

int main(){
     double x,y;
     int i,num_targets,num_stalkers,max_cord;
     FILE* fout=NULL;

     /*num_targets=80; max_cord=500;
     num_stalkers=20;*/
     num_targets=1000; max_cord=50;
     num_stalkers=50;

     x=(time(0) %RAND_MAX ); 
     srandom(x);
     fout=fopen("targets.txt","w+");
     if(fout==NULL){
	  printf("cannot open file for writing\n");
	  return 0;
     }
     fprintf(fout,"%d %d\n",num_targets,num_stalkers);
     for(i=0;i<num_targets;i++){
	  x=max_cord*( random()/(double) RAND_MAX ) ;
	  y=max_cord*( random()/(double) RAND_MAX ) ;
	  fprintf(fout,"%.2f %.2f\n",x,y);
     }
     fclose(fout);
     return 0;
}
