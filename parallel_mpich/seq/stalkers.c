#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define NUM_TARGETS 100
#define NUM_STALKERS 25
#define MAX_CORD 1000

struct distance{
     int t_id;
     double d;
};

struct Target{
     int id;
     double x,y;
     int sampled;
};

struct Stalker{
     int travel[NUM_TARGETS],last;
     int total;
     int move;
};

int time_period;
double average_samples;
struct distance sorted_distances[NUM_TARGETS][NUM_TARGETS];
int num_targets,num_stalkers;

void fget_targets(struct Target* , int* ,int* );
void sort_distances(struct Target* );
void scatter(struct Target*,struct Stalker *);
void travel(struct Target*,struct Stalker *);
void fout_solution(struct Target*,struct Stalker *);

int main(){
     struct Target targets[NUM_TARGETS];
     struct Stalker stalkers[NUM_STALKERS];

     average_samples=0;
     time_period=MAX_CORD/5;

     fget_targets(targets,&num_targets,&num_stalkers);
     sort_distances(targets);
     scatter(targets,stalkers);
     travel(targets,stalkers);
     fout_solution(targets,stalkers);
     return 0;
}

double dist(const struct Target* t1,const struct Target* t2){
     double dx=t1->x - t2->x;
     double dy=t1->y - t2->y;
     return sqrt( (dx*dx) + (dy*dy) );
}

int dist_cmp(const void* d1,const void* d2){
     double x=((struct distance*)d1)->d;
     double y=((struct distance*)d2)->d;

     return x > y;
}
void sort_distances(struct Target* targets){
     int i,j;
     for(i=0;i<num_targets;i++){
	  for(j=0;j<num_targets;j++){
	       if(i!=j){
		    sorted_distances[i][j].t_id=j;
		    sorted_distances[i][j].d=dist( targets+i,targets+j );
	       }else{
		    sorted_distances[i][i].t_id=i;
		    sorted_distances[i][i].d=MAX_CORD;
	       }
	  }
     }
     for(i=0;i<num_targets;i++)
	  qsort(sorted_distances[i],num_targets,
		    sizeof(struct distance),&dist_cmp );
}
int y_cmp(const void* t1,const void* t2){
     double y1=((struct Target*)t1)->y;
     double y2=((struct Target*)t2)->y;

     return y1 < y2;
}
int x_cmp(const void* t1,const void* t2){
     double x1=((struct Target*)t1)->x;
     double x2=((struct Target*)t2)->x;

     return x1 < x2;
}
void scatter(struct Target* targets,struct Stalker * stalkers){
     struct Target xsorted[NUM_TARGETS];
     /*struct Target ysorted[NUM_TARGETS];*/
     int i;
     double t;
     
     memcpy(xsorted,targets,sizeof(struct Target)*num_targets);
    /* memcpy(ysorted,targets,sizeof(Target)*num_targets);*/

     qsort(xsorted,num_targets,sizeof(struct Target),&x_cmp);
     /*qsort(ysorted,num_targets,sizeof(struct Target),&y_cmp);*/

     t=num_targets/num_stalkers;
     for(i=0;i<num_stalkers;i++){
	  stalkers[i].last=0;
	  stalkers[i].travel[0]=xsorted[(int)(i*t)].id;
	  targets[ stalkers[i].travel[0] ].sampled=1;
	  average_samples+=1/(double)num_targets;
	  stalkers[i].move=0;
     }
}

void travel(struct Target* targets,struct Stalker * stalkers){
     int current,next,j,i,hasmoved=1;
     struct Stalker* S;
     
     while(hasmoved>0){
	  hasmoved=0;
	  for(i=0;i<num_stalkers;i++){
	       S=&stalkers[i];
	       if(S->move>0 ){
		    S->move--;
		    hasmoved++;
	       }else {
		    j=-1;
		    current=S->travel[S->last];
		    do{
			 j++; 
			 if(j>=num_targets){
			      S->move=-1;
			      break;
			 }else if(j!=i){
			      next=sorted_distances[current][j].t_id;
			      S->move=(int)(sorted_distances[current][j].d +1);
			 }
		    }while(targets[next].sampled>average_samples ||
				   S->move + S->total > time_period );
		    if(S->move!=-1){
			 S->total+=S->move;
			 targets[next].sampled++;
			 S->travel[++(S->last)]=next;
			 average_samples+=1/(double)num_targets+0.01;
			 hasmoved++;
		    }
	       }
	  }
     }
}

void fout_solution(struct Target *targets,struct Stalker *stalkers){
     FILE* fout=fopen("solution.txt","w");
     int i,j,t1,t2,total=0;
     if(fout==NULL) exit(0);

     for(i=0;i<num_stalkers;i++){
	  for(j=0;j<stalkers[i].last;j++){
	       t1=stalkers[i].travel[j];
	       t2=stalkers[i].travel[j+1];
	       fprintf(fout,"%d", t1 );
	       if(j<stalkers[i].last-1) 
		    fprintf(fout,",");
	  }
	  fprintf(fout,"#\n");
     }
     printf("\n sampled targets:\n");
     for(i=0;i<num_targets;i++){
	  total+=targets[i].sampled;
	  printf(" %d(%d) ",i,targets[i].sampled);
     }
     printf("\ntotal sampling :%d",total);
}
void fget_targets(struct Target* targets,int *num_targets,int *num_stalkers){
     FILE *fin=fopen("targets.txt","r");
     double x,y;
     int i=0;

     if(fin==NULL) exit(0);
     fscanf(fin,"%d %d",num_targets,num_stalkers);
     if(*num_targets>NUM_TARGETS || *num_stalkers> NUM_STALKERS){
	  printf("input range error\n");
	  exit(0);
     }
     while( fscanf(fin,"%lf %lf",&x,&y) !=EOF && i< *num_targets){
	  targets[i].x=x;
	  targets[i].y=y;
	  targets[i].id=i;
	  targets[i].sampled=0;
	  i++;
     }
}
