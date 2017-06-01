#include"cluster.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>


struct Edge{
     int t_id1,t_id2;
     int value;
};
struct edge_arr{
     struct Edge* arr;
     int size;
};
struct group_arr{
     int *arr;
     int num_groups;
};
/*******************************************************************/
void fget_targets( struct g_targets_arr *global_targets,int *num_stalkers);

void set_distances( struct edge_arr* edges,struct g_targets_arr global_targets );

void partition(struct edge_arr *edges,struct g_targets_arr *gtargets
	       ,struct group_arr groups);

void send_partition(struct g_targets_arr global_targets,struct edge_arr edges
		    ,struct group_arr groups,int num_stalkers);
/*******************************************************************/

void scatter(struct g_targets_arr *global_targets,int *num_stalkers){
	    
     struct edge_arr edges;
     struct group_arr groups;
     int i,group_size,reminder,num_targets;
     
     fget_targets(global_targets,num_stalkers);
     num_targets=global_targets->size;
     /*******************************/
     groups.num_groups=mpi_size-1;
     edges.size=(int)( num_targets*(num_targets-1) )/2;
     edges.arr=(struct Edge*)malloc(sizeof(struct Edge)*edges.size);
     groups.arr=(int*)malloc(sizeof(int)*groups.num_groups);
     /*******************************/
     group_size=num_targets/groups.num_groups;
     for(i=0;i<groups.num_groups;i++) groups.arr[i]=group_size;
     reminder=num_targets%groups.num_groups;
     for(i=0; i<reminder ;i++) 
	  groups.arr[i]++;
     for(i=0;i<groups.num_groups;i++)
	  MPI_Send(groups.arr+i,1,MPI_INT,i,GROUP_SIZE,MPI_COMM_WORLD);
     /*******************************/

     set_distances(&edges,*global_targets);
     partition(&edges,global_targets,groups);
     send_partition(*global_targets,edges,groups,*num_stalkers);

     free(groups.arr);
     free(edges.arr);
}
/*******************************************************************/
void fget_targets( struct g_targets_arr *global_targets,int *num_stalkers){
     FILE *fin=fopen("targets.txt","r");
     double x,y;
     int i=0;

     if(fin==NULL) exit(0);
     fscanf(fin,"%d %d %d",&global_targets->size,num_stalkers,&time_period);
     global_targets->arr=malloc(sizeof(struct g_target)*global_targets->size);
     while( fscanf(fin,"%lf %lf",&x,&y) !=EOF && i< global_targets->size){
	  global_targets->arr[i].x=x;
	  global_targets->arr[i].y=y;
	  global_targets->arr[i].group_index=-1;
	  global_targets->arr[i].group=-1;
	  i++;
     }
}
/*******************************************************************/
double dist(const struct g_target* t1,const struct g_target* t2){
     double dx=t1->x - t2->x;
     double dy=t1->y - t2->y;
     return sqrt( (dx*dx) + (dy*dy) );
}

void set_distances( struct edge_arr* edges,struct g_targets_arr global_targets ){
     int i,j,base=0,idx;
     double tmp;
     for(i=0;i<global_targets.size;i++){
	  for(j=i+1;j<global_targets.size;j++){
		    idx=base+j-i-1;
		    edges->arr[idx].t_id1=i;
		    edges->arr[idx].t_id2=j;
		    tmp=dist(global_targets.arr+i,global_targets.arr+j);
		    edges->arr[idx].value=tmp;
		    if(edges->arr[idx].value <tmp)
			 edges->arr[idx].value++;
	  }
	  base+=global_targets.size-i-1;
     }
}
/************************************************************************************/
int edge_cmp(const void* d1,const void* d2){
     double x=((struct Edge*)d1)->value;
     double y=((struct Edge*)d2)->value;

     return x > y;
}

void partition(struct edge_arr *edges,struct g_targets_arr *gtargets
	       ,struct group_arr groups){
     int i,t1,t2,next_group=0;
     
     qsort(edges->arr,edges->size,sizeof(struct Edge),&edge_cmp);
     for(i=0;i<edges->size;i++){
	  t1=edges->arr[i].t_id1; 
	  t2=edges->arr[i].t_id2; 
	  if(gtargets->arr[t1].group!=gtargets->arr[t2].group){
	       if(gtargets->arr[t2].group==-1 &&
			      groups.arr[gtargets->arr[t1].group ]>0 ){
			 gtargets->arr[t2].group=gtargets->arr[t1].group;
			 groups.arr[gtargets->arr[t2].group]--;
			 gtargets->arr[t2].group_index=groups.arr[gtargets->arr[t2].group];
	       }else {
		    if(gtargets->arr[t1].group==-1 &&
			      groups.arr[gtargets->arr[t2].group ]>0 ){
			 gtargets->arr[t1].group=gtargets->arr[t2].group;
			 groups.arr[gtargets->arr[t1].group]--;
			 gtargets->arr[t1].group_index=groups.arr[gtargets->arr[t1].group];
		    }
	       }
	  }else{ 
	       if(gtargets->arr[t1].group==-1 && next_group<groups.num_groups){
		    gtargets->arr[t1].group=gtargets->arr[t2].group=next_group;
		    gtargets->arr[t1].group_index=groups.arr[next_group]-1;
		    gtargets->arr[t2].group_index=groups.arr[next_group]-2;
		    groups.arr[next_group]-=2;
		    next_group++;
	       }
	  }
     }
     for(i=0;i<gtargets->size;i++)
	  if(gtargets->arr[i].group==-1)
	       gtargets->arr[i].group=next_group;
			 
}

/************************************************************************************/

void send_partition(struct g_targets_arr global_targets
	            ,struct edge_arr edges,struct group_arr groups
	            ,int num_stalkers){
     int i,dest,buff[6],t1,t2;
     
     buff[0]=global_targets.size;
     buff[1]=num_stalkers/groups.num_groups;
     if(num_stalkers>=groups.num_groups) buff[2]=num_stalkers%groups.num_groups;
     else buff[2]=num_stalkers;
     buff[3]=time_period;
     MPI_Bcast(buff,4,MPI_INT,monitor_rank,MPI_COMM_WORLD);
     
     for(i=0;i<global_targets.size;i++){
	  buff[0]=i;/*global index*/
	  buff[1]=global_targets.arr[i].group_index;
	  dest=global_targets.arr[i].group;
	  MPI_Send(buff,2,MPI_INT,dest,TARG,MPI_COMM_WORLD);
     }
     /*****************************************************************/
     for(i=0;i<edges.size;i++){
	  t1=edges.arr[i].t_id1;
	  t2=edges.arr[i].t_id2;
	  buff[4]=edges.arr[i].value;/*distance*/
	  
	  buff[0]=global_targets.arr[ t1 ].group_index;/* 1 */
	  buff[1]=global_targets.arr[ t2 ].group_index;/* 2 */
	  buff[2]=global_targets.arr[ t2 ].group;/* 2 group */
	  buff[3]=t2;/*receiver uses global index for initial assignment*/
	  /*send [1,2] to group of 1 */
	  dest=global_targets.arr[ t1 ].group;
	  MPI_Send(buff,5,MPI_INT,dest,DIST,MPI_COMM_WORLD);

	  buff[0]=global_targets.arr[ t2 ].group_index;/* 1 */
	  buff[1]=global_targets.arr[ t1 ].group_index;/* 2 */
	  buff[2]=global_targets.arr[ t1 ].group;/* 2 group */
	  buff[3]=t1;
	  /*send [2,1] to group of 2 */
	  dest=global_targets.arr[ t2 ].group;;
	  MPI_Send(buff,5,MPI_INT,dest,DIST,MPI_COMM_WORLD);
     }
     buff[0]=-1;/*end of transmission*/
     for(i=0;i<mpi_size-1;i++)
	  MPI_Send(buff,1,MPI_INT,i,DIST,MPI_COMM_WORLD);
}


/************************************************************************************/
void set_size(int buff[3],int *global_size,int *num_stalkers,bool *stalker_reminder );
void add_target(struct l_targets_arr *local_targets,int buff[2]);
void add_distance(struct l_targets_arr *local_targets,double value
		    ,int serial1,int serail2,int group,int index);

void receive(struct l_targets_arr *local_targets
	       ,int *num_stalkers,bool *stalker_reminder){
     int buff[5],i,count;
     MPI_Recv(buff,1,MPI_INT,monitor_rank,GROUP_SIZE,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
     local_targets->size=buff[0];
     local_targets->arr=malloc(sizeof(struct l_target)*local_targets->size);
     MPI_Bcast(buff,4,MPI_INT,monitor_rank,MPI_COMM_WORLD);
     set_size(buff,&(local_targets->global_size),num_stalkers,stalker_reminder);

     for(i=0;i<local_targets->size;i++){
	  local_targets->arr[i].global_index=-1;
	  local_targets->arr[i].samples=0;
	  local_targets->arr[i].dist_priority=
	  (struct distance*)malloc(sizeof(struct distance)*local_targets->global_size);
     }
     for(i=0;i<local_targets->size;i++){
	  MPI_Recv(buff,2,MPI_INT,monitor_rank,TARG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  add_target(local_targets,buff);
     }
     buff[0]=1;count=0;
     while(buff[0]!=-1){
	  MPI_Recv(buff,5,MPI_INT,monitor_rank,DIST,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  if(buff[0]!=-1){
	       add_distance(local_targets,buff[4],buff[0],buff[1],buff[2],buff[3]);
	       count++;
	  }
     }

     if(count< local_targets->size * (local_targets->global_size-1) ){
	  printf("%d receive error %d\n",mpi_rank,count);
	  MPI_Abort(MPI_COMM_WORLD,-1);
     }
}

void set_size(int buff[3],int *global_size,int *num_stalkers,bool *stalker_reminder){
     *global_size=buff[0];
     *num_stalkers=buff[1];
     if(mpi_rank<buff[2] ){
	  *stalker_reminder=true;
     }else 
	  *stalker_reminder=false;
     time_period=buff[3];
}

void add_target(struct l_targets_arr *local_targets,int buff[2]){
     if(buff[1]>local_targets->size){
	  printf("add_target rank %d buff[1] %d\n",mpi_rank,buff[1]);
	  MPI_Abort(MPI_COMM_WORLD,-1);
     }
     if( local_targets->arr[ buff[1] ].global_index!=-1 ){
	  printf("add_target rank %d serial %d\n",mpi_rank,buff[1] );
	  MPI_Abort(MPI_COMM_WORLD,-1);
     }
     local_targets->arr[ buff[1] ].global_index=buff[0];
     local_targets->arr[ buff[1] ].dist_priority[buff[0] ].value=MAX_DIST;
}
void add_distance(struct l_targets_arr *local_targets,double value
		    ,int serial1,int serial2,int group,int index){
     
     if(serial1>local_targets->size || index>local_targets->global_size){
	  printf("add_distance serail1 %d index %d\n",serial1,index);
	  MPI_Abort(MPI_COMM_WORLD,-1);
     }
     local_targets->arr[ serial1 ].dist_priority[ index ].serial=serial2; 
     local_targets->arr[ serial1 ].dist_priority[ index ].group=group; 
     local_targets->arr[ serial1 ].dist_priority[ index ].value=value;
}

/************************************************************/
int dist_cmp(const void* d1,const void* d2){
     double x=((struct distance*)d1)->value;
     double y=((struct distance*)d2)->value;

     return x > y;
}
void sort_distances(){
     int i;
     for(i=0;i<targets.size;i++)
	  qsort(targets.arr[i].dist_priority ,targets.global_size
	       ,sizeof(struct distance) ,&dist_cmp );
}
