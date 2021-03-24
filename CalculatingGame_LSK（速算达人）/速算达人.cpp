#include<stdio.h>
#include<graphics.h>
#include<conio.h>
#include<windows.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<Mmsystem.h>
#pragma comment(lib,"winmm.lib")
typedef struct rank//构造分数的结构体，为排行榜功能做准备。
{
	char name[10];
	int score;
}RANK;
typedef struct node//链表的每个结点有三个数字和一个符号构成，其中num3又另外三个经过计算求得。
{
	int num1;
	int num2;
	char symbol;//运算符，包括"+","-","*","/"。
	int num3;
	int random_place;//num3即正确答案的随机位置。
	int condition;//答题状况，1代表正确，0代表错误。
	struct node *next;
}NODE;
int min_num(int m, int n);
int extends_confirmation();//选择读取存档则返回1，选择不读取存档则返回2。
NODE* add(NODE *h, int Num_1, int Num_2, char Symbol);//链表：增加题目
NODE* del(NODE *h);//链表：减少题目。
int getQuestionNum();//得到题目数量：成功则返回题目数量，不成功则返回-1。
NODE* loadQuestion(NODE *h, int question_num);//将txt文件转化为链表：成功则返回1，不成功则返回0。
int menu(int *sound);//进入菜单界面，进行开始游戏、帮助、退出的选择。
int prepare(char name[10], int sound, int save_extends);//开始游戏前准备阶段，包括增减题目，立刻开始。
int add_question(NODE *h);//增加题目，成功则返回1，不成功则返回0。
int row_count();//统计文件行数，为删除题目做准备；成功则返回行数，不成功则返回-1。
int del_question(NODE *h);//删除题目，成功则返回1，不成功则返回0。
void getReady(int sound);//进行开始游戏前的3秒倒计时动画。
void play(NODE *h, int question_number, int* score, double *interval, int* time_left, char name[10], int sound, int time_ordered, int* combo, int* wrong_answer);//进入游戏界面。
void score_confirm(NODE* h, int score, char name[10], int sound, int time_left, int wrong_answer);//进入分数确认界面。
void review(NODE* h, int sound, int wrong_answer);//回顾错误题目。
void ranking(int sound);//加载排行榜。
void help(int sound);//进入帮助界面。
void information(int sound);//进入信息界面
void exit();//进入退出界面。
int main()
{
	srand(time(0));
	int score;
	int time_ordered = 30;
	int question_current = 1;//当前题目。
	double interval;
	int time_left;//数值为1表明还有剩余时间,数值为0表明没有剩余时间。
	int sound = 1;//数值为1代表声音开启，数值为0代表声音关闭。
	int combo = 0;//用来表示连胜次数。
	char name[10];
	char name_load[10];//用来储存存档中的用户名。
	int score_load;//用来储存存档中的分数。
	int time_load;//用来储存存档中的剩余时间。
	int questionNum_load;//用来储存存档中的题目序号。
	int combo_load;//用来储存存档中的连胜次数。
	int error_load = 0;//用来存储存档中的错题数目。
	int save_extends = 0;//数值为1代表继承存档，数值为0代表不继承存档。
	int wrong_answer = 0;//用来表示错题数目。
	NODE *h = NULL;
	NODE *p = NULL;
	time_t program_begin = time(NULL), game_begin;
	h = loadQuestion(h, getQuestionNum());//将txt文件中的数据构造为链表,并返回头结点。
	while (1)
	{
		int choice = menu(&sound);
		score = 0;
		time_left = 1;
		save_extends = 0;
		time_ordered = 30;
		combo = 0;
		question_current = 1;
		wrong_answer = 0;
		for (p = h; p; p = p->next)
		p->condition = 1;
		switch (choice)
		{
		case 1: InputBox(name, 10, "请输入你的姓名", "速算达人", "Player");
			FILE* fp_save;
			if ((fp_save = fopen(".\\速算达人相关文档\\save_message.txt", "r")) == NULL)
			{
				settextcolor(BLACK);
				settextstyle(32, 0, "宋体");
				outtextxy(160, 144, "当前无法加载相关文件");
			}
			else
			{
				fscanf(fp_save, "%s%d%d%d%d%d", name_load, &score_load, &time_load, &questionNum_load, &combo_load, &error_load);
			}
			if (strcmp(name, name_load) == 0)
			{
				if (extends_confirmation() == 1)
				{
					score = score_load;
					time_ordered = time_load;
					question_current = questionNum_load;
					save_extends = 1;
					combo = combo_load;
					wrong_answer = error_load;
					for (int i = 1; i <= error_load; i++)
					{
						int error_num;
						int count = 0;
						fscanf(fp_save, "%d", &error_num);
						for (p = h; p; p = p->next)
						{
							if (count == error_num)
							{
								p->condition = 0;
								break;
							}
							count++;
						}
					}
				}
			}
			fclose(fp_save);
		again:
			switch (prepare(name, sound, save_extends))
			{
			case 1: add_question(h); goto again;
			case 2: getReady(sound);
				if (sound == 1)
				{
					PlaySound(".\\速算达人相关音频\\bk_music3.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
				}
				game_begin = time(NULL);
				interval = game_begin - program_begin;
				for (int i = question_current; i <= getQuestionNum(); i++)
				{
					if (time_left)
					{
						play(h, i, &score, &interval, &time_left, name, sound, time_ordered, &combo, &wrong_answer);
					}
					else
					{
						break;
					}
				}
				score_confirm(h, score, name, sound, time_left, wrong_answer);
				break;
			case 3: del_question(h); goto again;
			}
			break;
		case 2: help(sound);  break;
		case 3: exit(); return 0;
		case 4: ranking(sound); break;
		case 5: information(sound); break;
		}
	}
}
int min_num(int m, int n)
{
	return(m < n ? m : n);
}
int extends_confirmation()
{
	IMAGE extends_confirm, title, titleWhenClicked;
	loadimage(&extends_confirm, ".\\速算达人相关图片\\extends_confirm.jpg", 400, 250);
	loadimage(&title, ".\\速算达人相关图片\\title.jpg", 128, 36);
	loadimage(&titleWhenClicked, ".\\速算达人相关图片\\titleWhenClicked.jpg", 128, 36);
	putimage(120, 55, &extends_confirm);
	putimage(140, 240, &title);
	putimage(372, 240, &title);
	settextcolor(BLACK);
	settextstyle(30, 0, "宋体");
	outtextxy(185, 130, "发现存档，是否读取?");
	outtextxy(189, 245, "是");
	outtextxy(421, 245, "否");
	while (1)
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 140 && m.x < 268 && m.y>240 && m.y < 276)
		{
			settextcolor(BLACK);
			putimage(140, 240, &titleWhenClicked);
			outtextxy(189, 245, "是");
			Sleep(100);
			putimage(140, 240, &title);
			outtextxy(189, 245, "是");
			Sleep(100);
			return 1;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 372 && m.x < 500 && m.y>240 && m.y < 276)
		{
			settextcolor(BLACK);
			putimage(372, 240, &titleWhenClicked);
			outtextxy(421, 245, "否");
			Sleep(100);
			putimage(372, 240, &title);
			outtextxy(421, 245, "否");
			Sleep(100);
			return 2;
		}
		else if (m.x > 140 && m.x < 268 && m.y>240 && m.y < 276)
		{
			settextcolor(RED);
			outtextxy(189, 245, "是");
		}
		else if (m.x > 372 && m.x < 500 && m.y>240 && m.y < 276)
		{
			settextcolor(RED);
			outtextxy(421, 245, "否");
		}
		else
		{
			settextcolor(BLACK);
			outtextxy(189, 245, "是");
			outtextxy(421, 245, "否");
		}
	}
}
NODE* add(NODE *h, int Num_1, int Num_2, char Symbol)
{
	NODE *p, *s, *q;
	if (h == NULL)
	{
		h = (NODE *)malloc(sizeof(NODE));
		p = h;
		p->num1 = Num_1;
		p->num2 = Num_2;
		p->symbol = Symbol;
		p->random_place = rand() % 4 + 1;
		switch (Symbol)
		{
		case '+': p->num3 = Num_1 + Num_2; break;
		case '-': p->num3 = Num_1 - Num_2; break;
		case '*': p->num3 = Num_1 * Num_2; break;
		case '/': p->num3 = Num_1 / Num_2; break;
		}
		p->next = NULL;
	}
	else
	{
		q = (NODE *)malloc(sizeof(NODE));
		q->num1 = Num_1;
		q->num2 = Num_2;
		q->symbol = Symbol;
		switch (Symbol)
		{
		case '+': q->num3 = Num_1 + Num_2; break;
		case '-': q->num3 = Num_1 - Num_2; break;
		case '*': q->num3 = Num_1 * Num_2; break;
		case '/': q->num3 = Num_1 / Num_2; break;
		}
		q->random_place = rand() % 4 + 1;
		for (p = h;; p = p->next)
		{
			if (p->next == NULL)
			{
				s = p;
				break;
			}
		}
		s->next = q;
		q->next = NULL;
	}
	return h;
}
NODE* del(NODE *h)
{
	NODE *p;
	for (p = h; ; p = p->next)
	{
		if (p->next->next == NULL)
		{
			p->next = NULL;
			break;
		}
	}
	return h;
}
int getQuestionNum()
{
	FILE* fp;
	int question_num;
	if ((fp = fopen(".\\速算达人相关文档\\question_num.txt", "r")) == NULL)
	{
		settextcolor(BLACK);
		settextstyle(32, 0, "宋体");
		outtextxy(160, 144, "当前无法加载相关文件");
		Sleep(1000);
		return -1;
	}
	else
	{
		fscanf(fp, "%d", &question_num);
		fclose(fp);
		return question_num;
	}
}
NODE* loadQuestion(NODE *h, int question_num)
{
	int Num_1, Num_2;
	char Symbol;
	FILE* fp;
	if ((fp = fopen(".\\速算达人相关文档\\game_message.txt", "r")) == NULL)
	{
		settextcolor(BLACK);
		settextstyle(32, 0, "宋体");
		outtextxy(160, 144, "当前无法加载相关文件");
		Sleep(1000);
		return 0;
	}
	else
	{
		for (int i = 1; i <= question_num; i++)
		{
			fscanf(fp, "%d%d%c", &Num_1, &Num_2, &Symbol);
			h = add(h, Num_1, Num_2, Symbol);
		}
		fclose(fp);
		return h;
	}
}
int menu(int *sound)
{
	initgraph(640, 360);
	setbkmode(TRANSPARENT);
	IMAGE background, title_1, title_2, titleWhenClicked, sound_open, sound_close, trophy, information;
	loadimage(&background, ".\\速算达人相关图片\\background.jpg", 640, 360);
	loadimage(&title_1, ".\\速算达人相关图片\\title.jpg", 270, 56);
	loadimage(&title_2, ".\\速算达人相关图片\\title.jpg", 128, 36);
	loadimage(&titleWhenClicked, ".\\速算达人相关图片\\titleWhenClicked.jpg", 128, 36);
	loadimage(&sound_open, ".\\速算达人相关图片\\sound_open.jpg", 40, 40);
	loadimage(&sound_close, ".\\速算达人相关图片\\sound_close.jpg", 40, 40);
	loadimage(&trophy, ".\\速算达人相关图片\\trophy.jpg", 40, 40);
	loadimage(&information, ".\\速算达人相关图片\\information.jpg", 40, 40);
	putimage(0, 0, &background);
	settextcolor(BLACK);
	settextstyle(48, 0, "宋体");
	for (int i = 304; i >= 40; i--)
	{
		IMAGE temp;
		getimage(&temp, 175, i, 455, i + 56);
		BeginBatchDraw();
		putimage(180, i, &title_1);
		outtextxy(186, i + 5, "速 算 达 人");
		FlushBatchDraw();
		Sleep(1);
		if (i == 40) break;
		putimage(175, i, &temp);
	}
	EndBatchDraw();
	if (*sound)
	{
		PlaySound(".\\速算达人相关音频\\bk_music1.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	}
	settextstyle(32, 0, "宋体");
	for (int i = 0; i <= 256; i++)
	{
		IMAGE temp1, temp2, temp3;
		setcolor(BLACK);
		getimage(&temp1, i, 120, i + 128, 156);
		getimage(&temp2, 512 - i, 160, 640 - i, 196);
		getimage(&temp3, i, 200, i + 128, 236);
		BeginBatchDraw();
		putimage(i, 120, &title_2);
		putimage(512 - i, 160, &title_2);
		putimage(i, 200, &title_2);
		FlushBatchDraw();
		Sleep(1);
		if (i == 256) break;
		putimage(i, 120, &temp1);
		putimage(512 - i, 160, &temp2);
		putimage(i, 200, &temp3);
	}
	EndBatchDraw();
	FlushMouseMsgBuffer();
	outtextxy(256, 120, "开始游戏");
	outtextxy(256, 160, "  帮助  ");
	outtextxy(256, 200, "  退出  ");
	if (*sound == 1)
	{
		putimage(590, 310, &sound_open);
	}
	else
	{
		putimage(590, 310, &sound_close);
	}
	putimage(10, 310, &trophy);
	putimage(590, 10, &information);
	while (1)
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 256 && m.x < 384 && m.y>120 && m.y < 152)//选择开始游戏功能。
		{
			if (*sound)
			{
				PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(256, 120, &titleWhenClicked);
			outtextxy(256, 120, "开始游戏");
			Sleep(100);
			putimage(256, 120, &title_2);
			outtextxy(256, 120, "开始游戏");
			Sleep(100);
			return 1;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 256 && m.x < 384 && m.y>160 && m.y < 192)//选择帮助功能。
		{
			if (*sound)
			{
				PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(256, 160, &titleWhenClicked);
			outtextxy(256, 160, "  帮助  ");
			Sleep(100);
			putimage(256, 160, &title_2);
			outtextxy(256, 160, "  帮助  ");
			Sleep(100);
			return 2;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 256 && m.x < 384 && m.y>200 && m.y < 232)//选择退出功能。
		{
			if (*sound)
			{
				PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(256, 200, &titleWhenClicked);
			outtextxy(256, 200, "  退出  ");
			Sleep(100);
			putimage(256, 200, &title_2);
			outtextxy(256, 200, "  退出  ");
			Sleep(100);
			return 3;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 590 && m.x < 630 && m.y>310 && m.y < 350)//选择开关音乐功能。
		{
			if (*sound == 1)
			{
				*sound = 0;
				putimage(590, 310, &sound_close);
				PlaySound(NULL, NULL, SND_FILENAME);
			}
			else
			{
				*sound = 1;
				putimage(590, 310, &sound_open);
				PlaySound(".\\速算达人相关音频\\bk_music1.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
			}
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 10 && m.x < 50 && m.y>310 && m.y < 350)//选择排行榜功能。
		{
			return 4;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 590 && m.x < 630 && m.y>10 && m.y < 50)//选择信息功能。
		{
			return 5;
		}
		else if (m.x > 256 && m.x < 384 && m.y>120 && m.y < 152)
		{
			settextcolor(RED);
			settextstyle(32, 0, "宋体");
			outtextxy(256, 120, "开始游戏");
		}
		else if (m.x > 256 && m.x < 384 && m.y>160 && m.y < 192)
		{
			settextcolor(RED);
			settextstyle(32, 0, "宋体");
			outtextxy(256, 160, "  帮助  ");
		}
		else if (m.x > 256 && m.x < 384 && m.y>200 && m.y < 232)
		{
			settextcolor(RED);
			settextstyle(32, 0, "宋体");
			outtextxy(256, 200, "  退出  ");
		}
		else
		{
			settextcolor(BLACK);
			settextstyle(32, 0, "宋体");
			outtextxy(256, 120, "开始游戏");
			outtextxy(256, 160, "  帮助  ");
			outtextxy(256, 200, "  退出  ");
			settextstyle(20, 0, "宋体");
		}
	}
}
int prepare(char name[10], int sound, int save_extends)
{
	settextstyle(32, 0, "宋体");
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	if (sound)
	{
		PlaySound(".\\速算达人相关音频\\bk_music2.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	}
	IMAGE background, title, titleWhenClicked, blackboard;
	loadimage(&background, ".\\速算达人相关图片\\background.jpg", 640, 360);
	loadimage(&title, ".\\速算达人相关图片\\title.jpg", 180, 56);
	loadimage(&titleWhenClicked, ".\\速算达人相关图片\\titleWhenClicked.jpg", 180, 56);
	loadimage(&blackboard, ".\\速算达人相关图片\\blackboard.jpg", 500, 250);
	putimage(0, 0, &background);
	putimage(70, 30, &blackboard);
	putimage(25, 290, &title);
	putimage(230, 290, &title);
	putimage(435, 290, &title);
	outtextxy(51, 302, "增加题目");
	outtextxy(256, 302, "立刻开始");
	outtextxy(461, 302, "删除题目");
	outtextxy((512 - (strlen(name) + 1) * 16) / 2, 120, "Welcome,");
	outtextxy((768 - (strlen(name) + 1) * 16) / 2, 120, name);
	outtextxy(376 + 8 * strlen(name), 120, "!");
	if (save_extends)
		outtextxy(240, 175, "已读取存档");
	while (1)
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 25 && m.x < 205 && m.y>290 && m.y < 346)
		{
			if (sound)
			{
				PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(25, 290, &titleWhenClicked);
			outtextxy(51, 302, "增加题目");
			Sleep(100);
			putimage(25, 290, &title);
			outtextxy(51, 302, "增加题目");
			Sleep(100);
			return 1;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 230 && m.x < 410 && m.y>290 && m.y < 346)
		{
			if (sound)
			{
				PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(230, 290, &titleWhenClicked);
			outtextxy(256, 302, "立刻开始");
			Sleep(100);
			putimage(230, 290, &title);
			outtextxy(256, 302, "立刻开始");
			Sleep(100);
			return 2;
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x > 435 && m.x < 615 && m.y>290 && m.y < 346)
		{
			if (sound)
			{
				PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(435, 290, &titleWhenClicked);
			outtextxy(461, 302, "删除题目");
			Sleep(100);
			putimage(435, 290, &title);
			outtextxy(461, 302, "删除题目");
			Sleep(1000);
			return 3;
		}
		if (m.x > 25 && m.x < 205 && m.y>290 && m.y < 346)
		{
			settextcolor(RED);
			settextstyle(32, 0, "宋体");
			outtextxy(51, 302, "增加题目");
		}
		else if (m.x > 230 && m.x < 410 && m.y>290 && m.y < 346)
		{
			settextcolor(RED);
			settextstyle(32, 0, "宋体");
			outtextxy(256, 302, "立刻开始");
		}
		else if (m.x > 435 && m.x < 615 && m.y>290 && m.y < 346)
		{
			settextcolor(RED);
			settextstyle(32, 0, "宋体");
			outtextxy(461, 302, "删除题目");
		}
		else
		{
			settextcolor(BLACK);
			settextstyle(32, 0, "宋体");
			outtextxy(51, 302, "增加题目");
			outtextxy(256, 302, "立刻开始");
			outtextxy(461, 302, "删除题目");
		}
	}
}
int add_question(NODE *h)
{
	FILE* fp;
	char Num_1[3], Num_2[3], Symbol[2];
	int num1, num2, question_num = getQuestionNum();
	InputBox(Num_1, 3, "请输入第一个数:");
	sscanf(Num_1, "%d", &num1);
	InputBox(Num_2, 3, "请输入第二个数:");
	sscanf(Num_2, "%d", &num2);
	InputBox(Symbol, 2, "请输入运算符:");
	switch (Symbol[0])
	{
	case '+':add(h, num1, num2, '+'); break;
	case '-':add(h, num1, num2, '-'); break;
	case '*':add(h, num1, num2, '*'); break;
	case '/':add(h, num1, num2, '/'); break;
	}
	if ((fp = fopen(".\\速算达人相关文档\\game_message.txt", "a")) == NULL)
	{
		settextcolor(BLACK);
		settextstyle(32, 0, "宋体");
		outtextxy(160, 144, "当前无法加载相关文件");
		Sleep(1000);
		return 0;
	}
	else
	{
		fprintf(fp, "\n%d %d%c", num1, num2, Symbol[0]);
		fclose(fp);
	}
	if ((fp = fopen(".\\速算达人相关文档\\question_num.txt", "w")) == NULL)
	{
		settextcolor(BLACK);
		settextstyle(32, 0, "宋体");
		outtextxy(160, 144, "当前无法加载相关文件");
		Sleep(1000);
		return 0;
	}
	else
	{
		fprintf(fp, "%d", question_num + 1);
		fclose(fp);
	}
	return 1;
}
int row_count()
{
	int lines = 1;
	FILE *fp;
	if ((fp = fopen(".\\速算达人相关文档\\game_message.txt", "r")) == NULL)
	{
		settextcolor(BLACK);
		settextstyle(32, 0, "宋体");
		outtextxy(160, 144, "当前无法加载相关文件");
		Sleep(1000);
		return -1;
	}
	while (!feof(fp))
	{
		char ch = fgetc(fp);
		if (ch == '\n')
		{
			lines++;
		}
	}
	fclose(fp);
	return lines;
}
int del_question(NODE *h)
{
	del(h);
	FILE* fp_1;
	FILE* fp_2;
	char ch;
	int row = row_count(), count = 1;
	int question_num = getQuestionNum();
	if ((fp_1 = fopen(".\\速算达人相关文档\\game_message.txt", "r")) == NULL)
	{
		settextcolor(BLACK);
		settextstyle(32, 0, "宋体");
		outtextxy(160, 144, "当前无法加载相关文件");
		Sleep(1000);
		return 0;
	}
	fp_2 = fopen(".\\速算达人相关文档\\temp.txt", "w+");
	while ((ch = fgetc(fp_1)) != EOF)
	{
		if (ch == '\n') count++;
		if (count == row) break;
		fputc(ch, fp_2);
	}
	fclose(fp_1);
	fclose(fp_2);
	remove(".\\速算达人相关文档\\game_message.txt");
	rename(".\\速算达人相关文档\\temp.txt", "速算达人相关文档\\game_message.txt");
	if ((fp_1 = fopen(".\\速算达人相关文档\\question_num.txt", "w")) == NULL)
	{
		settextcolor(BLACK);
		settextstyle(32, 0, "宋体");
		outtextxy(160, 144, "当前无法加载相关文件");
		Sleep(1000);
		return 0;
	}
	else
	{
		fprintf(fp_1, "%d", question_num - 1);
		fclose(fp_1);
	}
}
void getReady(int sound)
{
	IMAGE background;
	IMAGE blackboard;
	IMAGE temp;
	loadimage(&background, ".\\速算达人相关图片\\background.jpg", 640, 360);
	loadimage(&blackboard, ".\\速算达人相关图片\\blackboard.jpg", 500, 250);
	putimage(0, 0, &background);
	putimage(70, 30, &blackboard);
	getimage(&temp, 300, 100, 340, 150);
	settextstyle(64, 0, "宋体");
	settextcolor(BLACK);
	for (int i = 3; i >= 1; i--)
	{
		TCHAR num[3];
		_stprintf(num, _T("%d"), i);
		outtextxy(304, 108, num);
		if (sound)
		{
			PlaySound(".\\速算达人相关音频\\count_1.wav", NULL, SND_ASYNC | SND_FILENAME);
		}
		Sleep(1000);
		putimage(300, 100, &temp);
	}
	outtextxy(272, 108, "go!");
	if (sound)
	{
		PlaySound(".\\速算达人相关音频\\count_2.wav", NULL, SND_ASYNC | SND_FILENAME);
	}
	Sleep(1000);
}
void play(NODE *h, int question_number, int* score, double* interval, int* time_left, char name[10], int sound, int time_ordered, int* combo,int* wrong_answer)
{
	NODE *p;
	int count = 0;
	int errorAnswer[3];//构造错误答案。
	int is_available[41];//判断混淆项是否用过。
	time_t pause_begin, pause_end;//计算暂停时间。
	memset(is_available, 0, sizeof(is_available));
	settextstyle(32, 0, "宋体");
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	IMAGE background, title, titleWhenClicked, blackboard, pause, prompt, save, saveWhenClicked;
	loadimage(&background, ".\\速算达人相关图片\\background.jpg", 640, 360);
	loadimage(&title, ".\\速算达人相关图片\\title.jpg", 180, 56);
	loadimage(&titleWhenClicked, ".\\速算达人相关图片\\titleWhenClicked.jpg", 180, 56);
	loadimage(&blackboard, ".\\速算达人相关图片\\blackboard.jpg", 500, 250);
	loadimage(&pause, ".\\速算达人相关图片\\pause.jpg", 40, 40);
	loadimage(&prompt, ".\\速算达人相关图片\\prompt.jpg", 40, 40);
	loadimage(&save, ".\\速算达人相关图片\\save.jpg", 30, 30);
	loadimage(&saveWhenClicked, ".\\速算达人相关图片\\saveWhenClicked.jpg", 30, 30);
	putimage(0, 0, &background);
	putimage(70, 30, &blackboard);
	putimage(0, 0, &pause);
	putimage(600, 0, &prompt);
	putimage(305, 0, &save);
	ellipse(55, 290, 165, 350);
	ellipse(195, 290, 305, 350);
	ellipse(335, 290, 445, 350);
	ellipse(475, 290, 585, 350);
	setfillstyle(BS_SOLID);
	setfillcolor(LIGHTGRAY);
	floodfill(110, 320, BLACK);
	floodfill(250, 320, BLACK);
	floodfill(390, 320, BLACK);
	floodfill(530, 320, BLACK);
	TCHAR Score[4];
	_stprintf(Score, _T("%d"), *score);
	settextcolor(WHITE);
	outtextxy(100, 50, "Score:");
	outtextxy(196, 50, Score);
	outtextxy(320 - strlen(name) * 8, 50, name);
	for (p = h;; p = p->next)
	{
		count++;
		if (count == question_number)
		{
			break;
		}
	}
	TCHAR Num_1[3], Num_2[3], Num_3[4];//将数字转化为字符串形式
	int lengthOfNum1 = strlen(Num_1);
	_stprintf(Num_1, _T("%d"), p->num1);
	_stprintf(Num_2, _T("%d"), p->num2);
	_stprintf(Num_3, _T("%d"), p->num3);
	settextcolor(BLACK);
	settextstyle(64, 0, "宋体");
	outtextxy(188, 128, Num_1);
	outtextxy(304, 128, p->symbol);
	outtextxy(420, 128, Num_2);
	for (int i = 0; i <= 2; i++)
	{
		do
		{
			errorAnswer[i] = rand() % 41 - 20;//混淆项设置为-20到20之间。
		} while (errorAnswer[i] == p->num3 || is_available[errorAnswer[i] + 20] == 1);
		is_available[errorAnswer[i] + 20] = 1;
	}
	TCHAR error_1[4], error_2[4], error_3[4];//将三个混淆项数字转化为字符串模式。
	_stprintf(error_1, _T("%d"), errorAnswer[0]);
	_stprintf(error_2, _T("%d"), errorAnswer[1]);
	_stprintf(error_3, _T("%d"), errorAnswer[2]);
	settextstyle(48, 0, "宋体");
	RECT place_1 = { 55, 290, 165,350 };
	RECT place_2 = { 195, 290, 305,350 };
	RECT place_3 = { 335, 290, 445,350 };
	RECT place_4 = { 475, 290, 585,350 };
	switch (p->random_place)
	{
	case 1: drawtext(Num_3, &place_1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(error_1, &place_2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(error_2, &place_3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(error_3, &place_4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		break;
	case 2: drawtext(error_1, &place_1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(Num_3, &place_2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(error_2, &place_3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(error_3, &place_4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		break;
	case 3: drawtext(error_1, &place_1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(error_2, &place_2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(Num_3, &place_3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(error_3, &place_4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		break;
	case 4: drawtext(error_1, &place_1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(error_2, &place_2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(error_3, &place_3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(Num_3, &place_4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		break;
	}
	if (*combo > 0)
	{
		settextcolor(YELLOW);
		settextstyle(20, 0, "宋体");
		TCHAR Combo[3];
		_stprintf(Combo, _T("%d"), *combo);
		outtextxy(100, 90, "连胜* ");
		if (*combo >= 10)
			outtextxy(160, 90, "Max");
		else
			outtextxy(160, 90, Combo);
	}
	while (1)
	{
		MOUSEMSG m;
		IMAGE temp;
		getimage(&temp, 400, 50, 600, 100);
		TCHAR Time[6];
		if (time_ordered + *interval - (double)clock() / CLOCKS_PER_SEC >= time_ordered)
		{
			settextcolor(WHITE);
			settextstyle(32, 0, "宋体");
			outtextxy(450, 50, "30.00");
		}
		else if ((time_ordered + *interval - (double)clock() / CLOCKS_PER_SEC > 0) && (time_ordered + *interval - (double)clock() / CLOCKS_PER_SEC < time_ordered))
		{
			_stprintf(Time, _T("%.2f"), time_ordered + *interval - (double)clock() / CLOCKS_PER_SEC);
			settextcolor(WHITE);
			settextstyle(32, 0, "宋体");
			outtextxy(450, 50, Time);
		}
		else
		{
			*time_left = 0;
			break;
		}
		while (MouseHit())
		{
			settextstyle(48, 0, "宋体");
			m = GetMouseMsg();
			if (m.uMsg == WM_LBUTTONDOWN && m.x > 55 && m.x < 165 && m.y>290 && m.y < 350)
			{
				if (p->random_place == 1)
				{
					settextcolor(GREEN);
					drawtext(Num_3, &place_1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					*score += 10 + min_num(*combo, 10);
					*combo += 1;
				}
				else
				{
					settextcolor(RED);
					drawtext(error_1, &place_1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					*combo = 0;
					*wrong_answer += 1;
					p->condition = 0;
				}
				Sleep(200);
				return;
			}
			else if (m.uMsg == WM_LBUTTONDOWN && m.x > 195 && m.x < 305 && m.y>290 && m.y < 350)
			{
				if (p->random_place == 2)
				{
					settextcolor(GREEN);
					drawtext(Num_3, &place_2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					*score += 10 + min_num(*combo, 10);
					*combo += 1;
				}
				else if (p->random_place == 1)
				{
					settextcolor(RED);
					drawtext(error_1, &place_2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					*combo = 0;
					*wrong_answer += 1;
					p->condition = 0;
				}
				else
				{
					settextcolor(RED);
					drawtext(error_2, &place_2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					*combo = 0;
					*wrong_answer += 1;
					p->condition = 0;
				}
				Sleep(200);
				return;
			}
			else if (m.uMsg == WM_LBUTTONDOWN && m.x > 335 && m.x < 445 && m.y>290 && m.y < 350)
			{
				if (p->random_place == 3)
				{
					settextcolor(GREEN);
					drawtext(Num_3, &place_3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					*score += 10 + min_num(*combo, 10);
					*combo += 1;
				}
				else if (p->random_place == 4)
				{
					settextcolor(RED);
					drawtext(error_3, &place_3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					*combo = 0;
					*wrong_answer += 1;
					p->condition = 0;
				}
				else
				{
					settextcolor(RED);
					drawtext(error_2, &place_3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					*combo = 0;
					*wrong_answer += 1;
					p->condition = 0;
				}
				Sleep(200);
				return;
			}
			else if (m.uMsg == WM_LBUTTONDOWN && m.x > 475 && m.x < 585 && m.y>290 && m.y < 350)
			{
				if (p->random_place == 4)
				{
					settextcolor(GREEN);
					drawtext(Num_3, &place_4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					*score += 10 + min_num(*combo, 10);
					*combo += 1;
				}
				else
				{
					settextcolor(RED);
					drawtext(error_3, &place_4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					*combo = 0;
					*wrong_answer += 1;
					p->condition = 0;
				}
				Sleep(200);
				return;
			}
			else if (m.uMsg == WM_LBUTTONDOWN && m.x > 600 && m.x < 640 && m.y>0 && m.y < 40)
			{
				switch (p->random_place)
				{
				case 1: settextcolor(YELLOW);
					drawtext(Num_3, &place_1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					break;
				case 2: settextcolor(YELLOW);
					drawtext(Num_3, &place_2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					break;
				case 3: settextcolor(YELLOW);
					drawtext(Num_3, &place_3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					break;
				case 4: settextcolor(YELLOW);
					drawtext(Num_3, &place_4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					break;
				}
			}
			else if (m.uMsg == WM_LBUTTONDOWN && m.x > 0 && m.x < 40 && m.y>0 && m.y < 40)
			{
				pause_begin = time(NULL);
				settextstyle(24, 0, "宋体");
				settextcolor(WHITE);
				outtextxy(450, 82, "(已暂停)");
				while (1)
				{
					MOUSEMSG m3;
					m3 = GetMouseMsg();
					if (m3.uMsg == WM_LBUTTONDOWN && m3.x > 0 && m3.x < 40 && m3.y>0 && m3.y < 40)
					{
						pause_end = time(NULL);
						*interval += (pause_end - pause_begin);
						break;
					}
				}
				FlushMouseMsgBuffer();
			}
			else if (m.uMsg == WM_LBUTTONDOWN && m.x > 305 && m.x < 335 && m.y>0 && m.y < 30)
			{
				putimage(305, 0, &saveWhenClicked);
				FILE* fp_save;
				if ((fp_save = fopen(".\\速算达人相关文档\\save_message.txt", "w")) == NULL)
				{
					settextcolor(BLACK);
					settextstyle(32, 0, "宋体");
					outtextxy(160, 144, "当前无法加载相关文件");
					Sleep(1000);
				}
				else
				{
					NODE* q;
					int num_count=0;
					fprintf(fp_save, "%s\n", name);
					fprintf(fp_save, "%d\n", *score);
					fprintf(fp_save, "%d\n", (int)(time_ordered + *interval - (double)clock() / CLOCKS_PER_SEC));
					fprintf(fp_save, "%d\n", question_number);
					fprintf(fp_save, "%d\n", *combo);
					fprintf(fp_save, "%d\n", *wrong_answer);
					for (q = h; q; q = q->next)
					{
						if(q->condition==0)
						fprintf(fp_save, "%d\n", num_count);
						num_count++;
					}
					fclose(fp_save);
				}
			}
		}
		Sleep(10);
		putimage(400, 50, &temp);
	}
}
void score_confirm(NODE* h, int score, char name[10], int sound, int time_left, int wrong_answer)
{
	TCHAR Score[4];
	TCHAR Wrong_answer[4];
	_stprintf(Score, _T("%d"), score);
	_stprintf(Wrong_answer, _T("%d"), wrong_answer);
	IMAGE background, title, titleWhenClicked, blackboard;
	loadimage(&background, ".\\速算达人相关图片\\background.jpg", 640, 360);
	loadimage(&title, ".\\速算达人相关图片\\title.jpg", 180, 56);
	loadimage(&titleWhenClicked, ".\\速算达人相关图片\\titleWhenClicked.jpg", 180, 56);
	loadimage(&blackboard, ".\\速算达人相关图片\\blackboard.jpg", 500, 250);
	putimage(70, 30, &blackboard);
	settextcolor(WHITE);
	settextstyle(32, 0, "宋体");
	outtextxy(100, 50, "Score:");
	outtextxy(196, 50, Score);
	outtextxy(450, 50, "0.00");
	settextcolor(BLACK);
	settextstyle(48, 0, "宋体");
	if (time_left)
	{
		outtextxy(212, 136, "题目完成！");
	}
	else
	{
		outtextxy(236, 136, "时间到！");
	}
	Sleep(2000);
	FlushMouseMsgBuffer();
	putimage(0, 0, &background);
	putimage(70, 30, &blackboard);
	outtextxy(140, 136, "你的分数为：");
	outtextxy(428, 136, Score);
	putimage(80, 290, &title);
	putimage(380, 290, &title);
	settextstyle(32, 0, "宋体");
	outtextxy(138, 302, "回顾");
	outtextxy(438, 302, "返回");
	settextcolor(WHITE);
	outtextxy(100, 50, "错题:");
	outtextxy(180, 50, Wrong_answer);
	settextcolor(BLACK);
	FILE* fp;
	if ((fp = fopen(".\\速算达人相关文档\\ranking_list.txt", "a")) == NULL)
	{
		settextcolor(BLACK);
		settextstyle(32, 0, "宋体");
		outtextxy(160, 144, "当前无法加载相关文件");
		Sleep(1000);
	}
	else
	{
		fprintf(fp, "%s %d\n", name, score);
		fclose(fp);
	}
	while (1)
	{
		MOUSEMSG m2;
		m2 = GetMouseMsg();
		if (m2.uMsg == WM_LBUTTONDOWN && m2.x > 380 && m2.x < 560 && m2.y>290 && m2.y < 346)
		{
			if (sound)
			{
				PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(380, 290, &titleWhenClicked);
			outtextxy(438, 302, "返回");
			Sleep(100);
			putimage(380, 290, &title);
			outtextxy(438, 302, "返回");
			Sleep(100);
			return;
		}
		else if (m2.uMsg == WM_LBUTTONDOWN && m2.x > 80 && m2.x < 260 && m2.y>290 && m2.y < 346)
		{
			if (sound)
			{
				PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(80, 290, &titleWhenClicked);
			outtextxy(138, 302, "回顾");
			Sleep(100);
			putimage(80, 290, &title);
			outtextxy(138, 302, "回顾");
			Sleep(100);
			review(h, sound, wrong_answer);
			return;
		}
		else if (m2.x > 380 && m2.x < 560 && m2.y>290 && m2.y < 346)
		{
			settextcolor(RED);
			outtextxy(438, 302, "返回");
		}
		else if (m2.x > 80 && m2.x < 260 && m2.y>290 && m2.y < 346)
		{
			settextcolor(RED);
			outtextxy(138, 302, "回顾");
		}
		else
		{
			settextcolor(BLACK);
			outtextxy(438, 302, "返回");
			outtextxy(138, 302, "回顾");
		}
	}
}
void review(NODE *h, int sound, int wrong_answer)
{
	NODE* p;
	int count = 0;
	setbkmode(TRANSPARENT);
	IMAGE background, title, titleWhenClicked, blackboard;
	loadimage(&background, ".\\速算达人相关图片\\background.jpg", 640, 360);
	loadimage(&title, ".\\速算达人相关图片\\title.jpg", 128, 36);
	loadimage(&titleWhenClicked, ".\\速算达人相关图片\\titleWhenClicked.jpg", 128, 36);
	loadimage(&blackboard, ".\\速算达人相关图片\\blackboard.jpg", 500, 250);
	if (wrong_answer == 0)
	{
		putimage(0, 0, &background);
		putimage(70, 30, &blackboard);
		settextstyle(48, 0, "宋体");
		settextcolor(WHITE);
		outtextxy(104, 120, "全部正确，无需回顾");
		Sleep(2000);
		return;
	}
	for (p = h; p; p = p->next)
	{
		if (p->condition == 0)
		{
			putimage(0, 0, &background);
			putimage(70, 30, &blackboard);
			putimage(472, 300, &title);
			TCHAR Num_1[3], Num_2[3], Num_3[4];//将数字转化为字符串形式
			int lengthOfNum1 = strlen(Num_1);
			_stprintf(Num_1, _T("%d"), p->num1);
			_stprintf(Num_2, _T("%d"), p->num2);
			_stprintf(Num_3, _T("%d"), p->num3);
			settextcolor(BLACK);
			settextstyle(64, 0, "宋体");
			outtextxy(188, 128, Num_1);
			outtextxy(304, 128, p->symbol);
			outtextxy(420, 128, Num_2);
			settextstyle(32, 0, "宋体");
			settextcolor(WHITE);
			outtextxy(100, 50, "正确答案:");
			outtextxy(244, 50, Num_3);
			settextcolor(BLACK);
			count++;
			while (1)
			{
				MOUSEMSG m;
				m = GetMouseMsg();
				settextstyle(32, 0, "宋体");
				if (m.uMsg == WM_LBUTTONDOWN && m.x > 504 && m.x < 568 && m.y>300 && m.y < 332)
				{
					if (sound)
					{
						PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
					}
					putimage(472, 300, &titleWhenClicked);
					if (count == wrong_answer)
					outtextxy(504, 300, "返回");
					else
					outtextxy(504, 300, "向后");
					Sleep(100);
					putimage(472, 300, &title);
					if (count == wrong_answer)
					outtextxy(504, 300, "返回");
					else
					outtextxy(504, 300, "向后");
					Sleep(100);
					break;
				}
				else if (m.x > 504 && m.x < 568 && m.y>300 && m.y < 332)
				{
					settextcolor(RED);
					if (count == wrong_answer)
					outtextxy(504, 300, "返回");
					else
					outtextxy(504, 300, "向后");
				}
				else
				{
					settextcolor(BLACK);
					if (count == wrong_answer)
					outtextxy(504, 300, "返回");
					else
					outtextxy(504, 300, "向后");
				}
			}
		}
	}
}
void ranking(int sound)//加载排行榜，成功则返回1，不成功则返回0。
{
	PlaySound(NULL, NULL, SND_FILENAME);
	RANK player[50];
	RANK temp;
	int i = 0, j, sum;
	FILE *fp;
	if ((fp = fopen(".\\速算达人相关文档\\ranking_list.txt", "r")) == NULL)
	{
		settextcolor(BLACK);
		settextstyle(32, 0, "宋体");
		outtextxy(160, 144, "当前无法加载相关文件");
		Sleep(1000);
		return;
	}
	while (!feof(fp))
	{
		fscanf(fp, "%s%d", player[i].name, &player[i].score);
		i++;
	}
	sum = i;
	for (i = 0; i <= sum - 1; i++)
	{
		for (j = i + 1; j <= sum - 1; j++)
		{
			if (player[i].score < player[j].score)
			{
				temp = player[i];
				player[i] = player[j];
				player[j] = temp;
			}
		}
	}
	setbkmode(TRANSPARENT);
	cleardevice();
	IMAGE background, title_1, title_2, titleWhenClicked;
	loadimage(&background, ".\\速算达人相关图片\\background.jpg", 640, 360);
	loadimage(&title_1, ".\\速算达人相关图片\\title.jpg", 500, 36);
	loadimage(&title_2, ".\\速算达人相关图片\\title.jpg", 128, 36);
	loadimage(&titleWhenClicked, ".\\速算达人相关图片\\titleWhenClicked.jpg", 128, 36);
	putimage(0, 0, &background);
	settextcolor(BLACK);
	settextstyle(32, 0, "宋体");
	outtextxy(272, 0, "排行榜");
	putimage(70, 35, &title_1);
	putimage(70, 80, &title_1);
	putimage(70, 125, &title_1);
	putimage(70, 170, &title_1);
	putimage(70, 215, &title_1);
	putimage(70, 260, &title_1);
	putimage(472, 300, &title_2);
	outtextxy(504, 300, "返回");
	char Score[5], place[2];
	for (i = 0; i < min_num(sum, 7) - 1; i++)
	{
		setcolor(YELLOW);
		sprintf(place, "%d", i + 1);
		outtextxy(120, 37 + i * 45, place);
		setcolor(RED);
		sprintf(Score, "%d", player[i].score);
		outtextxy(450, 37 + i * 45, Score);
		setcolor(BLACK);
		outtextxy(270, 37 + i * 45, player[i].name);
	}
	while (1)
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		settextstyle(32, 0, "宋体");
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 504 && m.x < 568 && m.y>300 && m.y < 332)
		{
			if (sound)
			{
				PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(472, 300, &titleWhenClicked);
			outtextxy(504, 300, "返回");
			Sleep(100);
			putimage(472, 300, &title_2);
			outtextxy(504, 300, "返回");
			Sleep(100);
			return;
		}
		else if (m.x > 504 && m.x < 568 && m.y>300 && m.y < 332)
		{
			settextcolor(RED);
			outtextxy(504, 300, "返回");
		}
		else
		{
			settextcolor(BLACK);
			outtextxy(504, 300, "返回");
		}
	}
}
void help(int sound)
{
	setbkmode(TRANSPARENT);
	cleardevice();
	IMAGE background, title, textbox, titleWhenClicked;
	loadimage(&background, ".\\速算达人相关图片\\background.jpg", 640, 360);
	loadimage(&title, ".\\速算达人相关图片\\title.jpg", 128, 36);
	loadimage(&textbox, ".\\速算达人相关图片\\textbox.jpg", 400, 250);
	loadimage(&titleWhenClicked, ".\\速算达人相关图片\\titleWhenClicked.jpg", 128, 36);
	putimage(0, 0, &background);
	putimage(472, 300, &title);
	putimage(120, 30, &textbox);
	settextcolor(BLACK);
	settextstyle(32, 0, "宋体");
	outtextxy(288, 35, "帮助");
	outtextxy(504, 300, "返回");
	settextstyle(20, 0, "宋体");
	Sleep(200);
	outtextxy(120, 80, "    开始游戏后，会出现算术题，下方显示有");
	Sleep(500);
	outtextxy(120, 110, "4个不同答案。点击正确答案获得一分，点击");
	Sleep(500);
	outtextxy(120, 140, "错误答案不得分。30秒内回答正确个数为最终");
	Sleep(500);
	outtextxy(120, 170, "得分。游戏开始前，玩家也可以选择增加或减");
	Sleep(500);
	outtextxy(120, 200, "少题目，但题目数量不得少于20个。");
	FlushMouseMsgBuffer();
	while (1)
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		settextstyle(32, 0, "宋体");
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 504 && m.x < 568 && m.y>300 && m.y < 332)
		{
			if (sound)
			{
				PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(472, 300, &titleWhenClicked);
			outtextxy(504, 300, "返回");
			Sleep(100);
			putimage(472, 300, &title);
			outtextxy(504, 300, "返回");
			Sleep(100);
			return;
		}
		else if (m.x > 504 && m.x < 568 && m.y>300 && m.y < 332)
		{
			settextcolor(RED);
			outtextxy(504, 300, "返回");
		}
		else
		{
			settextcolor(BLACK);
			outtextxy(504, 300, "返回");
		}
	}
}
void information(int sound)
{
	PlaySound(NULL, NULL, SND_FILENAME);
	setbkmode(TRANSPARENT);
	cleardevice();
	IMAGE background, title, textbox, titleWhenClicked;
	loadimage(&background, ".\\速算达人相关图片\\background.jpg", 640, 360);
	loadimage(&title, ".\\速算达人相关图片\\title.jpg", 128, 36);
	loadimage(&textbox, ".\\速算达人相关图片\\textbox.jpg", 400, 250);
	loadimage(&titleWhenClicked, ".\\速算达人相关图片\\titleWhenClicked.jpg", 128, 36);
	putimage(0, 0, &background);
	putimage(472, 300, &title);
	putimage(120, 30, &textbox);
	settextcolor(BLACK);
	settextstyle(32, 0, "宋体");
	outtextxy(288, 35, "关于");
	outtextxy(504, 300, "返回");
	settextstyle(25, 0, "宋体");
	outtextxy(140, 80, "速算达人");
	outtextxy(140, 130, "制作人：罗世琨");
	outtextxy(140, 180, "2019年2月至4月");
	while (1)
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		settextstyle(32, 0, "宋体");
		if (m.uMsg == WM_LBUTTONDOWN && m.x > 504 && m.x < 568 && m.y>300 && m.y < 332)
		{
			if (sound)
			{
				PlaySound(".\\速算达人相关音频\\click.wav", NULL, SND_ASYNC | SND_FILENAME);
			}
			putimage(472, 300, &titleWhenClicked);
			outtextxy(504, 300, "返回");
			Sleep(100);
			putimage(472, 300, &title);
			outtextxy(504, 300, "返回");
			Sleep(100);
			return;
		}
		else if (m.x > 504 && m.x < 568 && m.y>300 && m.y < 332)
		{
			settextcolor(RED);
			outtextxy(504, 300, "返回");
		}
		else
		{
			settextcolor(BLACK);
			outtextxy(504, 300, "返回");
		}
	}
}
void exit()
{
	setbkmode(TRANSPARENT);
	cleardevice();
	IMAGE background;
	loadimage(&background, _T(".\\速算达人相关图片\\background.jpg"), 640, 360);
	putimage(0, 0, &background);
	settextcolor(BLACK);
	settextstyle(48, 0, "宋体");
	outtextxy(224, 120, "游戏结束");
	Sleep(500);
}