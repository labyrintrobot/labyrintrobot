#include <inttypes.h>



typedef struct _queue_element{
	
	uint8_t header;
	uint8_t data;
	
	struct _queue_element *next;
	
} queue_element;



typedef struct _queue{
	
	queue_element *first;
	queue_element *last;
	
} queue;
