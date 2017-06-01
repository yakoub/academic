#include "cluster.h"
#include<stdlib.h>
#include<math.h>
#include<string.h>

struct trail_st *trail_arr;
int *dist_arr;
void get_trail(int s_id,int source);
void monitor(int num_stalkers){
     int i,msg,active_stalkers=num_stalkers;
     bool terminate=false;
     MPI_Status stat;

     trail_arr=malloc(sizeof(struct trail_st)*num_stalkers);
     for(i=0;i<num_stalkers;i++)
	  start_list(&(trail_arr[i].path) );
     dist_arr=malloc(sizeof(int)*num_stalkers);
     memset(dist_arr,0,sizeof(int)*num_stalkers);
     while(!terminate){
	  MPI_Recv(&msg,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG
	       ,MPI_COMM_WORLD,&stat);
	  switch(stat.MPI_TAG){
	       case PASS_TRAIL:
		    get_trail(msg,stat.MPI_SOURCE);
		    break;
	       case STALKER_END:
		    active_stalkers--;
		    if(active_stalkers==0)
			 terminate=true;
		    break;
	  }
     }
     for(i=0;i<mpi_size-1;i++)
	  MPI_Send(&msg,1,MPI_INT,i,TERM,MPI_COMM_WORLD);
}

void get_trail(int s_id,int source){
     struct trail_st *trail=trail_arr+s_id;
     int *dist=dist_arr+s_id;
     int *dist_buff,total_dist;
     int size,*targets_buff;
     
     MPI_Recv(&size,1,MPI_INT,source,PASS_TRAIL,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
     dist_buff=malloc(sizeof(int)*size);
     targets_buff=malloc(sizeof(int)*size);
     MPI_Recv(targets_buff,size,MPI_INT,source,PASS_TRAIL
	  ,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
     MPI_Recv(dist_buff,size,MPI_INT,source,PASS_TRAIL
	  ,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
     MPI_Recv(&total_dist,1,MPI_INT,source,PASS_TRAIL
	  ,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
     (*dist)=total_dist;
     concat_list(&trail->path,targets_buff,dist_buff,size);
}

/****************************************/

void output_solution(struct g_targets_arr* global_targets,int num_stalkers){
     int i,buff[2],*samples,sum;
     int *moves,j,size;
     int *distances;
     FILE *fout;
     
     samples=malloc(sizeof(int)*global_targets->size);
     for(i=0;i<global_targets->size;i++){
	  MPI_Recv(buff,2,MPI_INT,MPI_ANY_SOURCE,SEND_SAMPLES
		    ,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  samples[buff[0] ]=buff[1];
     }
     sum=0;
     fout=fopen("solution.txt","w");
     for(i=0;i<global_targets->size;i++){
	  printf("[%d %d]",i,samples[i] );
	  fprintf(fout,"%d",global_targets->arr[i].group );
	  if(i<global_targets->size-1)
	       fprintf(fout,"-");
	  sum+=samples[i];
     }
     fprintf(fout,"#\n");
     printf("\n total:%d\n",sum);
     free(samples);

     for(i=0;i<num_stalkers;i++){
	  size=extract_list(&trail_arr[i].path,&moves,&distances);
	  printf("s_id %d %d\n",i,dist_arr[i]);
	  for(j=0;j<size;j++){
	       fprintf(fout,"%d",moves[j]);
	       if(j<size-1)
		    fprintf(fout,",");
	  }
	  fprintf(fout,"#\n");
     }
     free(dist_arr);
     free(trail_arr);
}
