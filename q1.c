//
//  main.c
//  CircularBufferC
//
//  Created by Zaki Shaheen on 3/1/14.
//  Copyright (c) 2014 Zaki Shaheen. All rights reserved.
//


/*
This is a solution to question #1 on http://evernote.com/careers/challenge.php
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


/*
 State machine to process input
 */

#define START_STATE 0
#define SIZE_RECIEVED 1
#define APPENDING 2

/*
 Basic node of a linked list.
 */
struct node{
    struct node *next;
    void *data;
};
typedef struct node node;


/*
 Internal representation of a circular buffer.
 */
typedef struct{
    int limit;
    int count;
    node *head;
    node *tail;
    
}circular_buffer;

/*
 In pure OO, buffer and buffer_manager would be the same but
 I have to distinguish here.
 */


/*
 interface of a buffer_manager.
 */
typedef circular_buffer* (*t_init)(int n);                   //allocates and returns a buffer
typedef void (*t_append)(circular_buffer* buffer, void *o);  //append an item
typedef void (*t_remove)(circular_buffer* buffer);           //remove an item
typedef void (*t_list)(circular_buffer* buffer);             //list an item


/*
 decleration of buffer manager
 */
typedef struct{
    t_init init_buffer;
    t_append push;
    t_remove pop;
    t_list list;
}buffer_manager;




/*
 concrete implementation of buffer manager
 */
circular_buffer* bm_init_buffer(int n);
void bm_append(circular_buffer *buffer, void* o);
void bm_remove(circular_buffer *buffer);
void bm_list(circular_buffer *buffer);


circular_buffer* bm_init_buffer(int n){

    assert(n>0);
    
    circular_buffer *result = NULL;
    result = malloc(sizeof(circular_buffer));
    result->limit = n;
    result->count = 0;
    result->head = NULL;
    result->tail = NULL;
    
    return result;
}

void bm_append(circular_buffer *buffer, void* o){
    
    assert(buffer!=NULL);
    assert(o!=NULL);

    
    if (buffer->count < buffer->limit) {
        //we still have capacity
        
        node *new_node = malloc(sizeof(node));
        new_node->data = o;
        

        
        if (buffer->count==0) {
            //inserting first node
            buffer->head = new_node;
            buffer->tail = new_node;
            new_node->next = new_node;
            
        }else{
            buffer->tail->next = new_node;
            new_node->next = buffer->head;
            buffer->tail = new_node;
        }
        
        buffer->count++;
    }else{
        //we're out of capacity
        
        buffer->head->data = o;
        buffer->tail = buffer->tail->next;
        buffer->head = buffer->head->next;
        
    }
}


void bm_remove(circular_buffer *buffer){
    //cant pop a non-existent buffer
    assert(buffer!=NULL);
    
    //cant pop on an empty buffer
    if (buffer->count==0) {
        return;
    }

    //deleting last element
    if (buffer->count==1) {
        //TODO: Not memory efficent
        
        
        buffer->head = NULL;
        buffer->tail = NULL;

    }else{
        
        //TODO: Not memory efficent
        
        buffer->head = buffer->head->next; //shift head ahead.
        buffer->tail->next = buffer->head; //maintain circular
    }
    
    buffer->count--;
    
}

void bm_list(circular_buffer *buffer){
    assert(buffer!=NULL);
    if (buffer->count==0) {
        printf(""); return;
    }
    
    node *n = buffer->head;
    
    do {
        
        printf("%s\n", n->data);
        n = n->next;
    } while (n!=buffer->head);
    

}



/*
 Sort of factory to create the buffer manager and hookup the OO.
 */
buffer_manager* create_manager(){
    buffer_manager *manager = malloc(sizeof(buffer_manager));
    manager->init_buffer = &bm_init_buffer;
    manager->push = &bm_append;
    manager->pop = &bm_remove;
    manager->list = &bm_list;
    
    
    return manager;
}


int main(int argc, const char * argv[])
{
    circular_buffer *buffer = NULL;
    buffer_manager *manager = create_manager();
    
    
    char line[1024];
    int append_count = 0;
    
    int state = START_STATE;
    
//    FILE *f = fopen("input01.txt", "r");
    
    while (fgets(line, 1024, stdin)!=NULL) {
        
        strtok(line, "\n");
        
        switch (state) {
                
                
            case START_STATE:{
                int size = atoi(line);
                buffer = manager->init_buffer(size);
                state = SIZE_RECIEVED;
                break;
            }
                
                
            case SIZE_RECIEVED:{
                char *first = strtok(line, " ");
                char *last = strtok(NULL, " ");
                
                if (0==strcmp(first, "A")) {
                    state = APPENDING; append_count = atoi(last);
                }
                
                else if (0==strcmp(first, "R")) {
                    state = SIZE_RECIEVED;
                    
                    for (int count = atoi(last); count>0; count--) {
                        manager->pop(buffer);
                    }
                }
                
                else if (0==strcmp(first, "L")) {
                    manager->list(buffer);
                }
                break;
            }
                
                
                
            case APPENDING:{
                append_count--;
                if (append_count==0) {
                    state= SIZE_RECIEVED;
                }
                
                unsigned long length = strlen(line);
                char *newstring = malloc(sizeof(char) * length+1);
                strcpy(newstring, line);
                
                manager->push(buffer, newstring);
                break;
            }
                
                
            default:
                break;
        }
        
        
    }

    return 0;
}
