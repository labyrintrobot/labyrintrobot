/*
 * queue.h
 *
 * Created: 11/28/2013 2:31:23 PM
 * Version 0.1
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#ifndef QUEUE_H_
#define QUEUE_H_

#include <inttypes.h>
#include <stdbool.h>

typedef struct _queue_element{
	uint8_t header;
	uint8_t data;
	
	struct _queue_element *next;
	
} queue_element;



typedef struct _queue {
	queue_element *first;
	queue_element *last;
	
} queue;

void queue_init(queue* q);

void queue_add(queue* q, uint8_t header, uint8_t data);

bool queue_remove(queue* q, uint8_t* header, uint8_t* data);

bool queue_is_empty(queue* q);

#endif /* QUEUE_H_ */