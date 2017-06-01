#include<stdlib.h>

struct move_node{
     int target;
     int dist;
     struct move_node* next;
};

typedef struct move_list_st{
     struct move_node *head;
     struct move_node *tail;
     int size;
}move_list;

void start_list(move_list *list);
void list_tail(move_list* list,int *target,int *dist);
void list_add(move_list *list,int target,int dist);
int extract_list(move_list *list,int **targets_param,int **distances_param);
void concat_list(move_list *list,int *targets_arr,int *distances,int size);
