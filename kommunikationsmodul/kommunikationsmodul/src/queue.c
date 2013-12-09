/*
 * queue.c
 * A simple queue implementation. Currently unused.
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
	} else {
		q->last->next = e;
	}
	
	q->last = e;
}

bool queue_remove(queue* q, uint8_t* header, uint8_t* data) {
	
	queue_element* e = q->first;
	
	if (e == 0) {
		*header = 0xFF;
		*data = 0xFF;
		return false;
	} else {
		q->first = q->first->next;
		
		*header = e->header;
		*data = e->data;
		
		free(e);
	}
	
	return true;
}

bool queue_is_empty(queue* q) {
	return (q->first == 0);
}