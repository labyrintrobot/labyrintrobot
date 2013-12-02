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
	
	if (! queue_is_empty(&q)) {
		return 1;
	}
	if (! queue_is_empty(&q)) {
		return 2;
	}
	
	queue_add(&q, 13, 14);
	
	if (queue_is_empty(&q)) {
		return 3;
	}
	
	uint8_t h;
	uint8_t d;
	
	if (! queue_remove(&q, &h, &d)) {
		return 4;
	}
	if (h != 13 || d != 14) {
		return 5;
	}
	if (! queue_is_empty(&q)) {
		return 6;
	}
	
	if (queue_remove(&q, &h, &d)) {
		return 7;
	}
	if (! queue_is_empty(&q)) {
		return 8;
	}
	if (queue_remove(&q, &h, &d)) {
		return 9;
	}
	if (! queue_is_empty(&q)) {
		return 10;
	}
	
	int i;
	for (i = 0; i < 100; i++) {
		queue_add(&q, i, i);
	}
	if (queue_is_empty(&q)) {
		return 11;
	}
	
	for (i = 0; i < 100; i++) {
		queue_remove(&q, &h, &d);
		if (h != i || d != i) {
			return 12;
		}
	}
	if (!queue_is_empty(&q)) {
		return 13;
	}
	if (queue_remove(&q, &h, &d)) {
		return 14;
	}
	if (!queue_is_empty(&q)) {
		return 15;
	}
	
	return 0;
}