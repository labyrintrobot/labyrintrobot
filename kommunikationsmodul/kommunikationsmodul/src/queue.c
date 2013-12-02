/*
 * queue.c
 *
 *  Author: Emil Berg, Kristoffer Borg
 */ 

#include <inttypes.h>
#include <stdlib.h>
#include "queue.h"

void queue_init(queue* q) {
	q->first = 0;
	q->last = 0;
}

void queue_add(queue* q, uint8_t header, uint8_t data) {
	queue_element* e = malloc(sizeof(queue_element));
	e->data = data;
	e->header = header;
	e->next = 0;
	
	if (q->first == 0) {
		q->first = e;
		q->last = e;
	} else {
		q->last->next = e;
	}
	
	q->last = e;
}

int queue_remove(queue* q, uint8_t* header, uint8_t* data) {
	
	queue_element* e = q->first;
	
	if (e == 0) {
		return -1;
	} else {
		q->first = q->first->next;
		
		*header = e->header;
		*data = e->data;
		
		free(e);
	}
	
	return 0;
}

bool queue_is_empty(queue* q) {
	return (q->first == 0);
}