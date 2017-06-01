#include "move_list.h"
void start_list(move_list *list){
     list->head=list->tail=NULL; 
     list->size=0;
}

void list_add(move_list *list,int target,int dist){
     if(list->head==NULL){
	  list->head=malloc(sizeof(struct move_node));
	  list->tail=list->head;
     }else{
	  list->tail->next=malloc(sizeof(struct move_node));
	  list->tail=list->tail->next;
     }
     list->tail->target=target;
     list->tail->dist=dist;
     list->tail->next=0;
     list->size++;
}

void list_tail(move_list* list,int *target,int *dist){
     *target=list->tail->target;
     *dist=list->tail->dist;
}
int extract_list(move_list *list,int **targets_param,int **distances_param){
     int i,*targets_arr;
     int *distances;
     struct move_node* current;

     if(list->size==0) return -1;
     
     *targets_param=malloc(sizeof(int)*list->size );
     *distances_param=malloc(sizeof(int)*list->size );
     targets_arr=*targets_param;
     distances=*distances_param;

     current=list->head;
     for(i=0;i<list->size;i++){
	  targets_arr[i]=current->target;
	  distances[i]=current->dist;
	  current=current->next;
	  free(list->head);
	  list->head=current;
     }
     return list->size;
}

void concat_list(move_list *list,int *targets_arr,int *distances,int size){
     int i;
     for(i=0;i<size;i++)
	  list_add(list,targets_arr[i],distances[i]);
}
