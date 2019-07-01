#include <stdlib.h>
#include <stdio.h>
#include "mazeutil.h"
#include "LinkQueue.h" 


int MazeBFS(int entryX, int entryY, int exitX, int exitY, Maze* maze)
{
	int direction[8][2] = { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 },
	{ 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 } };
	
	LinkQueue linkQueueX = NULL;
	LinkQueue linkQueueY = NULL;
	int posX, posY;
	int preposX, preposY;
	int **preposMarkX;
	int **preposMarkY;
	int **mark;
	int i, j, mov;
	
	preposMarkX = (int**)malloc(sizeof(int*)* maze->size);
	for (i = 0; i<maze->size; i++)
	{
		preposMarkX[i] = (int*)malloc(sizeof(int)* maze->size);
	}
	
	preposMarkY = (int**)malloc(sizeof(int*)* maze->size);
	for (i = 0; i<maze->size; i++)
	{
		preposMarkY[i] = (int*)malloc(sizeof(int)* maze->size);
	}
	
	mark = (int**)malloc(sizeof(int*)* maze->size);
	for (i = 0; i<maze->size; i++)
	{
		mark[i] = (int*)malloc(sizeof(int)* maze->size);
	}
	for (i = 0; i<maze->size; i++)
	{
		for (j = 0; j<maze->size; j++)
		{
			preposMarkX[i][j] = -1;
			preposMarkY[i][j] = -1;
			mark[i][j] = 0;
		}
	}
	linkQueueX = SetNullQueue_Link(); 
	linkQueueY = SetNullQueue_Link();
	EnQueue_link(linkQueueX, entryX);
	EnQueue_link(linkQueueY, entryY);
	mark[entryX][entryY] = 1;  
	
	while (!IsNullQueue_Link(linkQueueX))
	{
		preposX = FrontQueue_link(linkQueueX); 
		DeQueue_link(linkQueueX);  
		preposY = FrontQueue_link(linkQueueY); 
		DeQueue_link(linkQueueY); 
		
		for (mov = 0; mov<8; mov++)
		{
			posX = preposX + direction[mov][0];
			posY = preposY + direction[mov][1];
			if (posX == exitX && posY == exitY)
			{
				preposMarkX[posX][posY] = preposX;
				preposMarkY[posX][posY] = preposY;
				printf("\nthe path is£º\n%d %d\t", posX, posY);
			
				while (!(posX == entryX && posY == entryY))
				{
				
					preposX = preposMarkX[posX][posY];
					preposY = preposMarkY[posX][posY];
					posX = preposX;
					posY = preposY;
					printf("%d %d\t", posX, posY);
				}
				return 1;
			}//end 64
		
			if (maze->data[posX][posY] == 0 && mark[posX][posY] == 0)
			{
				EnQueue_link(linkQueueX, posX); 
				EnQueue_link(linkQueueY, posY);
				mark[posX][posY] = 1; 
				preposMarkX[posX][posY] = preposX; 
				preposMarkY[posX][posY] = preposY;
end 82
end 60
end 53
	return 0;
}