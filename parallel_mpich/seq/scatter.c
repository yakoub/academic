#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define NUM_TARGETS 100
#define NUM_STALKERS 25
#define MAX_CORD 1000

struct distance{
     int t_id1;
     int t_id2;
     double d;
};

struct Target{
     int id;
     double x,y;
     int group;
};

#define DIST_TABLE_SIZE (int)( NUM_TARGETS*(NUM_TARGETS-1) )/2
struct distance distances[DIST_TABLE_SIZE];
struct Target targets[NUM_TARGETS];
int *groups,num_groups,group_size;
int num_targets;

void fget_targets( int* );
void set_distances( );
void partition();
void fout_solution();

int main(){
     int i;   
     fget_targets(&num_targets);

     /*group_size=sqrt(num_targets)+1; num_groups=group_size;*/
     num_groups=3;
     group_size=num_targets/num_groups;
     groups=(int*)malloc(sizeof(int)*num_groups);
     for(i=0;i<num_groups;i++) groups[i]=group_size;
     for(i=0;i<num_targets%num_groups;i++) groups[i]++;

     set_distances();
     partition();
     fout_solution();
}

double dist(const struct Target* t1,const struct Target* t2){
     double dx=t1->x - t2->x;
     double dy=t1->y - t2->y;
     return sqrt( (dx*dx) + (dy*dy) );
}

void set_distances(){
     int i,j,base=0,idx;
     for(i=0;i<num_targets;i++){
	  for(j=i+1;j<num_targets;j++){
		    idx=base+j-i-1;
		    distances[idx].t_id1=i;
		    distances[idx].t_id2=j;
		    distances[idx].d=dist(targets+i,targets+j);
	  }
	  base+=num_targets-i-1;
     }
}

int dist_cmp(const void* d1,const void* d2){
     double x=((struct distance*)d1)->d;
     double y=((struct distance*)d2)->d;

     return x > y;
}
void partition(){
     int i,t1,t2,dist_table_size,next_group=0;
     dist_table_size=(int)(num_targets*(num_targets-1)/2);
     
     qsort(distances,dist_table_size,sizeof(struct distance),&dist_cmp);
     for(i=0;i<dist_table_size;i++){
	  t1=distances[i].t_id1; 
	  t2=distances[i].t_id2; 
	  if(targets[t1].group!=targets[t2].group){
	       if(targets[t2].group==-1 &&
			      groups[targets[t1].group ]>0 ){
			 targets[t2].group=targets[t1].group;
			 groups[targets[t2].group]--;
	       }else {
		    if(targets[t1].group==-1 &&
			      groups[targets[t2].group ]>0 ){
			 targets[t1].group=targets[t2].group;
			 groups[targets[t1].group]--;
		    }
	       }
	  }else{ 
	       if(targets[t1].group==-1 && next_group<num_groups){
		    targets[t1].group=targets[t2].group=next_group;
		    groups[next_group++]-=2;
	       }
	  }
     }
     for(i=0;i<num_targets;i++)
	  if(targets[i].group==-1){
	       targets[i].group=next_group;
	       printf("%d single ?\n",i);
	  }
			 
}
     
void fout_solution(){
     FILE* fout=fopen("solution.txt","w");
     int i,j;
     if(fout==NULL) exit(0);

     for(i=0;i<num_targets;i++){
	  fprintf(fout,"%d",targets[i].group);
	  if(i<num_targets-1)
	       fprintf(fout,"-");
     }
     fprintf(fout,"#\n");
}
void fget_targets(int *num_targets){
     FILE *fin=fopen("targets.txt","r");
     double x,y;
     int i=0;

     if(fin==NULL) exit(0);
     fscanf(fin,"%d ",num_targets);
     if(*num_targets>NUM_TARGETS ){
	  printf("input range error\n");
	  exit(0);
     }
     while( fscanf(fin,"%lf %lf",&x,&y) !=EOF && i< *num_targets){
	  targets[i].x=x;
	  targets[i].y=y;
	  targets[i].id=i;
	  targets[i].group=-1;
	  i++;
     }
}
