#include <stdio.h>
#include <stdlib.h>
#include "linkstack.h"
int main()
{

	int i,num,res,a,b;
  PLinkStack plastack;
	PLinkStack stack1,stack2;
	res=1;
	plastack=createEmptyStack_link();
	//BracketMatch(plastack);
	printf("please input number :\n");
	scanf("%d",&num);
	while(num)
	{
		push_link(plastack,num);
		num--;
	}
	while(!isEmptyStack_link(plastack))
	{
		res=res*top_link(plastack);
		pop_link(plastack);
	}
	printf("%d\n",res);
	return 0;	
}
