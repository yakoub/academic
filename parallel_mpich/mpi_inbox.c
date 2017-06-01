#include "cluster.h"

/****************************/
pthread_mutex_t reply_mux=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t incoming=PTHREAD_COND_INITIALIZER;
pthread_cond_t reply_checked=PTHREAD_COND_INITIALIZER;
struct box_1{
     int samples,cluster,target;
     int reply_count;
}reply_box;
/*****************************/
pthread_mutex_t inbox_count_mux=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t inbox_on=PTHREAD_COND_INITIALIZER;
int inbox_count=0;

/*****************************/
pthread_mutex_t stalker_count_mux=PTHREAD_MUTEX_INITIALIZER;
int stalker_count=0;
int get_stalker_count(){
     int tmp;
     pthread_mutex_lock(&stalker_count_mux);
     tmp=stalker_count;
     pthread_mutex_unlock(&stalker_count_mux);
     return tmp;
}
void inc_stalkers(){
     pthread_mutex_lock(&stalker_count_mux);
     if(stalker_count==0){
	  pthread_mutex_lock(&inbox_count_mux);
	  inbox_count=0;
	  pthread_mutex_unlock(&inbox_count_mux);
     }
     stalker_count++;
     pthread_mutex_unlock(&stalker_count_mux);
}
void dec_stalkers(){
     pthread_mutex_lock(&stalker_count_mux);
     stalker_count--;
     if(stalker_count==0){
	  pthread_mutex_lock(&inbox_count_mux);
	  inbox_count++;/*serve other clusters*/
	  pthread_mutex_unlock(&inbox_count_mux);
	  pthread_cond_signal(&inbox_on);
     }
     pthread_mutex_unlock(&stalker_count_mux);
}

int inbox_wait(int cluster,int target){
     int samples;
     pthread_mutex_lock(&inbox_count_mux);
     inbox_count++;/*thread waits for inbox*/
     pthread_cond_signal(&inbox_on);
     pthread_mutex_unlock(&inbox_count_mux);
     
     pthread_mutex_lock(&reply_mux);
     reply_box.reply_count++;/* reading contents*/
     while( reply_box.cluster!=cluster && reply_box.target!=target ){
	  reply_box.reply_count--;/*contents read*/
	  pthread_cond_signal(&reply_checked);
	  pthread_cond_wait(&incoming,&reply_mux);
	  reply_box.reply_count++;/* reading contents*/
     }
     pthread_mutex_lock(&inbox_count_mux);
     inbox_count--;/*thread stops waiting for inbox*/
     pthread_mutex_unlock(&inbox_count_mux);

     samples=reply_box.samples;
     reply_box.reply_count--;/*contents read*/
     pthread_mutex_unlock(&reply_mux);
     pthread_cond_signal(&reply_checked);
     
     return samples;
}

void signal_inbox(){
     pthread_mutex_lock(&inbox_count_mux);
	  pthread_cond_signal(&inbox_on);
     pthread_mutex_unlock(&inbox_count_mux);
}

void open_inbox(){
     pthread_mutex_lock(&inbox_count_mux);
	  inbox_count=0;
	  reply_box.reply_count=0;
	  reply_box.cluster=-1;
	  reply_box.target=-1;
	  reply_box.samples=-1;
     pthread_mutex_unlock(&inbox_count_mux);
}
/**************************************************/
bool tag_switch(MPI_Status* status,int buff[4]);
void get_samples(int *buff,MPI_Status *status);
void give_samples(int target,MPI_Status* status);
void receive_stalker(int *buff,MPI_Status *status);
/*---------------------------------------------*/
void mpi_inbox(){
     bool terminate=false;
     int buff[4],probe;
     MPI_Status status;

     while(!terminate){
	  pthread_mutex_lock(&inbox_count_mux);
	  if(inbox_count==0 && get_stalker_count()>0 )
	       pthread_cond_wait(&inbox_on,&inbox_count_mux);
	  while(inbox_count>0){
	       pthread_mutex_unlock(&inbox_count_mux);
	       MPI_Recv(buff,4,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG
			 ,MPI_COMM_WORLD,&status );
	       terminate=tag_switch(&status,buff);
	       if(terminate) inbox_count=0;
	       pthread_mutex_lock(&inbox_count_mux);
	  }
	  pthread_mutex_unlock(&inbox_count_mux);
	  probe=1;
	  if(!terminate){
	       while(probe){
		    MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD
			      ,&probe,&status);
		    if(probe){
			 MPI_Recv(buff,4,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG
				   ,MPI_COMM_WORLD,&status );
			 terminate=tag_switch(&status,buff);
			 if(terminate) probe=false;
		    }
	       }
	  }
if(terminate) printf("%d term recved\n",mpi_rank);
     }
}

/*---------------------------------------------*/
bool tag_switch(MPI_Status* status,int buff[4]){
     switch(status->MPI_TAG){
	  case GIVE_SAMPLES:
	       get_samples(buff,status);
	  case GET_SAMPLES:
	       give_samples(buff[0],status);
	       break;
	  case PASS_STALKER:
	       receive_stalker(buff,status);
	       break;
	  case TERM:
	       inbox_count=0;
	       if(stalker_count>0)
		    printf("error rank %d count %d\n"
			 ,mpi_rank,stalker_count);
	       return true;
     }
     return false;
}
void give_samples(int target,MPI_Status* status){
     int buff[2];
     buff[0]=target;
     buff[1]=targets.arr[target].samples;
     MPI_Send(buff,2,MPI_INT,status->MPI_SOURCE
	       ,GIVE_SAMPLES,MPI_COMM_WORLD);
}
void get_samples(int *buff,MPI_Status* status){
     pthread_mutex_lock(&reply_mux);
     reply_box.samples=buff[1];
     reply_box.cluster=status->MPI_SOURCE;
     reply_box.target=buff[0];
     pthread_cond_broadcast(&incoming);
     while(reply_box.reply_count>0 && get_stalker_count() >0)
	  pthread_cond_wait(&reply_checked,&reply_mux);
     pthread_mutex_unlock(&reply_mux);
}
/*stalker.c*/
void stalker_thread(int s_id,int total_dist,int target ,int move_dist);
void receive_stalker(int *buff,MPI_Status *status){
     inc_stalkers();
     stalker_thread(buff[1],buff[2],buff[0],buff[3]); 
}

