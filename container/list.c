#include <stdlib.h>
#include <stdio.h>

typedef struct List {
	int data;
	struct List* next;
}Node;


Node* head=NULL;

int add(int e) {
	Node* n = (Node*)malloc(sizeof(Node));
	n->data = e;

	n->next = head;
	head = n;
}

int delete(int e)
{
	Node* p = head;
	Node* q = p;
	while ( p->next ) {
		p = p->next;
		if (p->data == e) {
		   q->next = p -> next; 
		   free(p);
		   return 1;
		}
		q = p;
	}
	return 0;
}

int find(int e)
{
	Node* p = head;
	while (p->next) {
		p = p->next;
		if (p->data == e) {
			return 1;
		}
	}
	return 0;
}

static void print()
{
	Node* p = head;
	while (p->next) {
		printf("%d ",p->data);
		p = p->next;
	}
	return;
}

int main()
{
	for (int i=0; i< 10; ++i) {
	      add( i);	
	}
	print();
	return 1;
}
