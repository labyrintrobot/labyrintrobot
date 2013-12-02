/*
 * queue_test.c
 *
 * Created: 12/2/2013 11:34:06 AM
 *  Author: Emil Berg
 */ 

#include "queue.h"
#include "queue_test.h"

int check_empty(queue* q, bool empty);

int queue_test() {
	queue q;
	queue_init(&q);
	
	queue_add(&q, 13, 14);
	
	uint8_t h;
	uint8_t d;
	
	queue_remove(&q, &h, &d);
	if (h != 13 || d != 14) {
		return -1;
	}
	
	queue_remove(&q, &h, &d);
	if (h != 0 || d != 0) {
		return -1;
	}
	
	queue_remove(&q, &h, &d);
	if (h != 0 || d != 0) {
		return -1;
	}
	
	int i;
	for (i = 0; i < 100; i++) {
		queue_add(&q, i, i);
	}
	
	for (i = 0; i < 100; i++) {
		queue_add(&q, i, i);
	}
	
	for (i = 0; i < 100; i++) {
		queue_remove(&q, &h, &d);
		if (h != i || d != i) {
			return -1;
		}
	}
	
	return 0;
}

int check_empty(queue* q, bool empty) {
	if (empty != queue_is_empty(q)) {
		return -1;
	}
	return 0;
}