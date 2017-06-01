#include <mpi.h>
#include <pthread.h>
#include "move_list.h"
#include <stdio.h>

#ifndef BOOL_TYPE
#define BOOL_TYPE
typedef enum{ false=0,true=1 } bool;
#endif
typedef enum{ GROUP_SIZE            	/*0*/
	       ,TARG			/*1*/
	       ,DIST			/*2*/
	       ,GET_SAMPLES		/*3*/
	       ,GIVE_SAMPLES		/*4*/
	       ,PASS_STALKER 		/*5*/
	       ,PASS_TRAIL		/*6*/
	       ,STALKER_END		/*7*/
	       ,TERM			/*8*/
	       ,SEND_SAMPLES		/*9*/
} Tag;
#define MAX_DIST 999999

/*inter cluster target description used for scatter*/
struct g_target{
     int group,group_index;
     double x,y;
};
struct g_targets_arr{
     struct g_target *arr;
     int size;
};

/*local cluster target */
struct distance{
     int group,serial;
     int value;
};
struct l_target{
     int global_index;
     int samples;
     struct distance *dist_priority;
};

struct l_targets_arr{
     struct l_target *arr;
     int size;
     int global_size;
}; 
/****************************************************/
struct trail_st{
     move_list path;/*has global_index+cross distance */
     int s_id;
     int first_target;/*list has global_index not target serial*/
     int total_dist;
};

/*cluster global objects*/
struct l_targets_arr targets;
int mpi_rank,mpi_size;
int monitor_rank;
int time_period;
MPI_Comm cluster_comm;
