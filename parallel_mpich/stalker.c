#include "cluster.h"

void* stalker_proc(void *arg);
void stalker_thread(int s_id,int total_dist,int target ,int move_dist);

/*implemented in mpi_inbox.c*/
int inbox_wait(int cluster,int target);
void signal_inbox();
void inc_stalkers();
void dec_stalkers();

void spawn_stalkers(int num_stalkers,bool stalker_reminder){
     int i,base;

     base=mpi_rank*num_stalkers;
     for(i=0;i<num_stalkers;i++)
	  inc_stalkers();
     if(stalker_reminder)
	  inc_stalkers();
     for(i=0;i<num_stalkers;i++){
	  stalker_thread(base+i,0,i%targets.size,0);
     }
     if(stalker_reminder){
	  base=num_stalkers*(mpi_size-1);
	  stalker_thread(base+mpi_rank,0,i%targets.size,0);
     }
}
void stalker_thread(int s_id,int total_dist,int target ,int move_dist){
     struct trail_st *tmp=malloc(sizeof(struct trail_st) );
     pthread_t thread;

     start_list(&tmp->path);
     list_add(&tmp->path,targets.arr[target].global_index,move_dist);
     tmp->first_target=target;
     tmp->s_id=s_id;
     tmp->total_dist=total_dist+move_dist;
     pthread_create(&thread,NULL,&stalker_proc,(void*)tmp);
}

/*********************************/
pthread_mutex_t avrg_mux=PTHREAD_MUTEX_INITIALIZER;
double average_samples=0;
pthread_mutex_t send_mux=PTHREAD_MUTEX_INITIALIZER;
void competition_sync(int s_id);
void competition_out();
void choose_next_local(int *next_target,int *next_group,int *dist,int total_dist);
void choose_next_cross(int *next_target,int *next_group,int *dist,int total_dist);
void send_trail(struct trail_st *trail);
void hold_reduce();
/**-----------------------------**/
void* stalker_proc(void *arg){
     struct trail_st *trail=(struct trail_st*)arg;
     int move=0,dist=0;
     int next_target,next_group=mpi_rank;
     int buff[4];
     bool stalker_end=true,trail_end=false;

     list_tail(&trail->path,&next_target,&move);/*param2 ignored*/
     next_target=trail->first_target;
     targets.arr[next_target].samples++;
     pthread_mutex_lock(&avrg_mux);
	  average_samples+=1/(double)targets.size+0.01;/*global_*/
     pthread_mutex_unlock(&avrg_mux);
     while(!trail_end){
	  competition_sync(trail->s_id);
	  if(move>0 ) move--;
	  else{
	       if(next_group!=mpi_rank){
		    buff[0]=next_target;
		    buff[1]=trail->s_id;
		    buff[2]=trail->total_dist;
		    buff[3]=dist;
		    MPI_Send(buff,4,MPI_INT,next_group,PASS_STALKER,MPI_COMM_WORLD);
		    trail_end=true;
		    stalker_end=false;
	       }else{
		    
		    choose_next_local(&next_target,&next_group,&dist,trail->total_dist);
		    if( next_group==mpi_rank){
			 if(dist!=-1){
			      list_add(&trail->path,targets.arr[next_target].global_index,dist);
			      move=dist;
			      trail->total_dist+=dist;
			      targets.arr[ next_target ].samples++;
			      pthread_mutex_lock(&avrg_mux);
				   average_samples+=1/(double)targets.size+0.01;
				   /*should be globalsize*/
			      pthread_mutex_unlock(&avrg_mux);
			 }else{
			      stalker_end=true;
			      trail_end=true;
			      printf("%d stalker end\n",trail->s_id);
			 }
		    }else
			 move=-1;
	       }
	 }
     }
     competition_out();
     send_trail(trail);
     if(stalker_end)
	  MPI_Send(&trail->s_id,1,MPI_INT,monitor_rank,STALKER_END,MPI_COMM_WORLD);
     free(trail);
     return (void*)0;
}
/**-----------------------------**/
#define AVRG_FACTOR 1
void choose_next_cross(int *next_target,int *next_group,int *dist,int total_dist){
     int i=0,samples=-1;
     struct distance* priority;

     priority=targets.arr[ *next_target ].dist_priority;
     do{

	  if(i>=targets.global_size-1){
	       *dist=-1;
	       *next_group=mpi_rank;
	       break;
	  }
	  *next_target=priority[i].serial;
	  *next_group=priority[i].group;
	  *dist  =priority[i].value;
	  i++;
	  if(*next_group!=mpi_rank){
	       MPI_Send(next_target,1,MPI_INT,*next_group,GET_SAMPLES,MPI_COMM_WORLD);
	       samples=inbox_wait(*next_group,*next_target);
	  }else
	       samples=targets.arr[ *next_target  ].samples;

     }while(samples>average_samples*AVRG_FACTOR ||
	       *dist + total_dist > time_period );

}

void choose_next_local(int *next_target,int *next_group,int *dist,int total_dist){
     int i=0,samples=-1;
     struct distance* priority;

     priority=targets.arr[ *next_target ].dist_priority;
     do{

	  if(i>=targets.global_size-1){
	       *dist=-1;
	       *next_group=mpi_rank;
	       break;
	  }
	  *next_target=priority[i].serial;
	  *next_group=priority[i].group;
	  *dist  =priority[i].value;
	  i++;
	  if(*next_group==mpi_rank)
	       samples=targets.arr[ *next_target  ].samples;
	  else
	       *dist=time_period+1;
     }while(samples>average_samples*AVRG_FACTOR ||
	       *dist + total_dist > time_period );
}

void send_trail(struct trail_st *trail){
     int *trail_targets,size;
     int *trail_distances;
     size=extract_list(&trail->path,&trail_targets,&trail_distances);
     if(size<=0){
	  printf("stalker %d has %d size trail\n",trail->s_id,size);
	  MPI_Abort(MPI_COMM_WORLD,-1);
     }
     pthread_mutex_lock(&send_mux);
	  MPI_Send(&trail->s_id,1,MPI_INT,monitor_rank,PASS_TRAIL,MPI_COMM_WORLD);
	  MPI_Send(&size,1,MPI_INT,monitor_rank,PASS_TRAIL,MPI_COMM_WORLD);
	  MPI_Send(trail_targets,size,MPI_INT,monitor_rank,PASS_TRAIL,MPI_COMM_WORLD);
	  MPI_Send(trail_distances,size,MPI_INT,monitor_rank,PASS_TRAIL,MPI_COMM_WORLD);
	  MPI_Send(&trail->total_dist,1,MPI_INT,monitor_rank,PASS_TRAIL,MPI_COMM_WORLD);
     pthread_mutex_unlock(&send_mux);
	  free(trail_targets);
	  free(trail_distances);
     
}

pthread_mutex_t step=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tick=PTHREAD_COND_INITIALIZER;
int count=0;
int get_stalker_count();
void reduce_average();
/*---------------*/
void competition_sync(int s_id){
     pthread_mutex_lock(&step);
	  if(count>=get_stalker_count()-1 ){
	       count=0;
	       signal_inbox();
/*	       reduce_average();
*/	       pthread_cond_broadcast(&tick);
	  }else{
	       count++;
	       pthread_cond_wait(&tick,&step);
	  }
     pthread_mutex_unlock(&step);
}
void competition_out(){
     pthread_mutex_lock(&step);
     if(count==get_stalker_count()-1 ){
	  pthread_cond_broadcast(&tick);
	  count=0;
	  signal_inbox();
     }
     dec_stalkers();
     if(get_stalker_count()==0) 
	  signal_inbox();
     pthread_mutex_unlock(&step);
}
void reduce_average(){
     double sum;
     MPI_Allreduce(&average_samples,&sum,1,MPI_DOUBLE,MPI_SUM,cluster_comm);
     average_samples=sum/(mpi_size-1);
}
void hold_reduce(){
     while(get_stalker_count() <= 0){
	  signal_inbox();
	  reduce_average();
     }
}
