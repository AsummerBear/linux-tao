#ifndef LINKSTACK_H
#define LINKSTACK_H

typedef int DataType;
struct Node{
	DataType	  info;
	struct Node*  link;
};
typedef struct Node  *LinkList,*PNode;

struct LinkStack{
	PNode top;
};
typedef struct LinkStack *PLinkStack;

PLinkStack createEmptyStack_link(void);
int isEmptyStack_link(PLinkStack plstack);
void push_link(PLinkStack plstack, DataType x);
void pop_link(PLinkStack plstack);
DataType top_link(PLinkStack plstack);

#endif