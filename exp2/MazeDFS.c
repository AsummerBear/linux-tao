#include <stdlib.h>
#include <stdio.h>
#include "mazeutil.h"
#include "linkstack.h" //包含链栈头文件

//迷宫深度遍历算法
int MazeDFS(int entryX, int entryY, int exitX, int exitY, Maze* maze)
{
	int direction[8][2] = { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 },
	{ 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 } };
	//用于两个栈，分别保存路径中点（x，y）值
	LinkStack linkStackX = NULL;
	LinkStack linkStackY = NULL;
	int posX, posY;//临时变量，存放点坐标（x，y）
	int preposX, preposY;
	int **mark; //标记二维数组，标记哪些点走过，不再重复走
	int i, j;//循环变量
	int mov;//移动的方向
	// 给做标记的二维数组分配空间，并赋初值 
	mark = (int**)malloc(sizeof(int*)* maze->size);
	for (i = 0; i<maze->size; i++)
		mark[i] = (int*)malloc(sizeof(int)* maze->size);
	//给所有元素设置初值 
	for (i = 0; i<maze->size; i++)
	{
		for (j = 0; j<maze->size; j++)
			mark[i][j] = 0;
	}
	linkStackX = SetNullStack_Link(); //初始化栈
	linkStackY = SetNullStack_Link(); //初始化栈
	mark[entryX][entryY] = 1; //入口点设置标志位
	Push_link(linkStackX, entryX); //入口点入栈
	Push_link(linkStackY, entryY); //入口点入栈
	//如果栈不为空且还没有找到迷宫出口点
	while (!IsNullStack_link(linkStackX))
	{
		preposX = Top_link(linkStackX);
		preposY = Top_link(linkStackY);
		Pop_link(linkStackX);
		Pop_link(linkStackY);
		mov = 0;
		while (mov < 8)
		{
			posX = preposX + direction[mov][0];
			posY = preposY + direction[mov][1];
			if (posX == exitX && posY == exitY)//到达终点
			{
				Push_link(linkStackX, preposX); //出口点入栈
				Push_link(linkStackY, preposY); //出口点入栈
				printf("\n深度搜索迷宫路径如下：\n");
				printf("%d %d\t", exitX, exitY); //打印入口点
				while (!IsNullStack_link(linkStackX))//将路径逆序输出
				{
					posX = Top_link(linkStackX); //取栈顶元素
					posY = Top_link(linkStackY);//取栈顶元素
					Pop_link(linkStackX); //出栈
					Pop_link(linkStackY); //出栈
					printf("%d %d\t", posX, posY);	//输出栈顶元素
				}//end 52
				return 1;
			}//end 46
			//还有路可以走通
			if (maze->data[posX][posY] == 0 && mark[posX][posY] == 0)
			{
				mark[posX][posY] = 1;
				Push_link(linkStackX, preposX);//入栈
				Push_link(linkStackY, preposY);//入栈
				preposX = posX;
				preposY = posY;
				mov = 0;//已经往前走了，因此方向重新从0号方向开始搜索
			}//end 63
			else
				mov++;  //换个方向试试
		}//end 42
	} //end 35
	return 0;
}