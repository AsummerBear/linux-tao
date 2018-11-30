#include <stdio.h>
#include <stdlib.h>
#include "linkstack.h"


PLinkStack createEmptyStack_link( )
{
	PLinkStack plstack;
	plstack = (PLinkStack)malloc(sizeof(struct LinkStack));
	if(plstack!=NULL)
		plstack->top = NULL;
	else
		printf("out of space\n");
	return plstack;
}


int isEmptyStack_link(PLinkStack plstack)
{
	return(plstack->top==NULL);
}


void push_link(PLinkStack plstack,DataType x){
	PNode p;
	p = (PNode)malloc(sizeof(struct Node));
	if(p == NULL)
		printf("out of spacce\n");
	else{
		p->info = x;
		p->link = plstack->top;
		plstack->top = p;
		}
}


void pop_link(PLinkStack plstack)
{
	PNode p;
	if(isEmptyStack_link(plstack))
		printf("Empty stack pop\n");
	else{
		p = plstack->top;
		plstack->top = plstack->top->link;
		free(p);
	}
}


DataType top_link(PLinkStack plstack)
{
	if(plstack->top == NULL)
	{
		printf("Stack is empty!\n");
		return 0;
	}
	else
		return(plstack->top->info);
}
