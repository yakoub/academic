#include "cluster.h"

/*monitor.c********/
void monitor(int num_stalkers);
void output_solution(struct g_targets_arr *global_targets,int num_stalkers);
/*scatter.c********/
void scatter(struct g_targets_arr *global_targets 
	    ,int* num_stalkers );
void receive(struct l_targets_arr *local_targets 
	    ,int* num_stalkers, bool *stalker_reminder);
void sort_distances();
/*stalker.c********/
void spawn_stalkers(int num_stalkers,bool stalker_reminder);
/*mpi_inbox.c******/
void open_inbox();
void mpi_inbox();
/*here*************/
void send_samples();

int main(){
     int num_stalkers;
     bool stalker_reminder;/*used for local id allocation*/
     struct g_targets_arr global_targets;
     MPI_Group world_group,cluster_group;

     MPI_Init(0,0);
     MPI_Comm_rank(MPI_COMM_WORLD,&mpi_rank);
     MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
     monitor_rank=mpi_size-1;
     MPI_Comm_group(MPI_COMM_WORLD,&world_group);
     MPI_Group_excl(world_group,1,&monitor_rank,&cluster_group);
     MPI_Comm_create(MPI_COMM_WORLD,cluster_group,&cluster_comm);

     if(mpi_rank== monitor_rank){
	  scatter(&global_targets,&num_stalkers);
	  monitor(num_stalkers);
	  output_solution(&global_targets,num_stalkers);
	  free(global_targets.arr);
	  MPI_Finalize();
	  return 0;
     }
     receive(&targets,&num_stalkers,&stalker_reminder);
     sort_distances();
     open_inbox();
     spawn_stalkers(num_stalkers,stalker_reminder);
     mpi_inbox();
     send_samples();
     free(targets.arr);
     MPI_Finalize();
     return 0;
}

/***************************************************************/
void send_samples(){
     int i,buff[2];

     for(i=0;i<targets.size;i++){
	  buff[0]=targets.arr[i].global_index;
	  buff[1]=targets.arr[i].samples;
	  MPI_Send(buff,2,MPI_INT,monitor_rank,SEND_SAMPLES,MPI_COMM_WORLD);
	  free(targets.arr[i].dist_priority);
     }
}
