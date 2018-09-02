#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>
#include <time.h>
#include <thread>
#include <iostream>
#include <pthread.h>
#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"
using namespace BlackLib;
#define vertical 20
#define horizontal 80
#define down 115
#define up 119
#define left 97
#define right 100
int snake_speed=100000;

int rPinBt = 67;//GPIO_67,input
int wPinBt = 68;//GPIO_68,input
int bPinBt = 44;//GPIO_44,input
int yPinBt = 26;//GPIO_26,input

BlackGPIO rBt((gpioName)rPinBt,input);
BlackGPIO wBt((gpioName)wPinBt,input);
BlackGPIO bBt((gpioName)bPinBt,input);
BlackGPIO yBt((gpioName)yPinBt,input);

typedef struct Snake
{
    char symbol;
    int size;
    char direction;
    char prev_direction;
    int tail_X;
    int tail_Y;
    int head_X;
    int head_Y;

}snake;


typedef struct snake_pos
{
    int Y[vertical*horizontal];
    int X[vertical*horizontal];

}snake_pos;



typedef struct food
{
    int X;
    int Y;
    char symbol;

}food;


int kbhit();
void snake_init(snake *snake1);
void pos_init(snake_pos *pos1);
void food_init(food *food1);
void gotoxy(int,int);
void snake_place(snake *snake1, snake_pos *pos1);
void snake_move(BlackGPIO* bt,snake *snake1, snake_pos *pos1, food *food1, int*);
void move_tail(snake *snake1, snake_pos *pos1);
void move_head(BlackGPIO* bt,snake *snake1, snake_pos *pos1);
void food_print(food *food1);
int game_over(snake *snake1, snake_pos *pos1);
void set_borders();
void print_score(int*);



int main(int argc ,char * argv[])
{
     char p;
     time_t tempo1, tempo2, deltaT;
     tempo1 = time( (time_t *) 0);
     int score=0;
     int num =0;


      snake snake1;
      snake_pos pos1;
      food food1;
      snake_init(&snake1);
      pos_init(&pos1);
      food_init(&food1);
string bt1 = bBt.getValue();
    string rt1 = rBt.getValue();
   string yt1 = yBt.getValue();
 string wt1 = wBt.getValue();

    thread red (move_head, &rBt,&snake1,&pos1);
    thread white (move_head, &wBt,&snake1,&pos1);
    thread blue (move_head, &bBt,&snake1,&pos1);
    thread yellow (move_head, &yBt,&snake1,&pos1);
	
      system("clear");
      system("stty -echo");
      int curs_set(int num);   
         
      snake_place(&snake1,&pos1);
      set_borders();
      food_print(&food1); 


  

      system ("/bin/stty raw");


  

      while(!(game_over(&snake1,&pos1)))
      {
	 while (!kbhit())
          {
                 usleep(snake_speed);
                 snake_move(&bt,&snake1,&pos1,&food1,&score);
                 if (game_over(&snake1,&pos1))
                 {
                     break;
                 }

	 	}
       

          snake1.prev_direction=snake1.direction;
	  	 for (int a=0;a<4;a++){
	  		if (bt1=="1"){
           		p='a';
	 	 	}
			else {
	 			if(rt1=="1"){
				p='s';
				}
				else{
					if (yt1=="1"){
           				p='w';
	 				}else{
						if(wt1=="1"){
							p='d';
						}
						else{
							p='s';
						}
					}	
				}
			}
	 	 }
	     snake1.direction=p;

     }
      tempo2 = time( (time_t *) 0);
      system ("/bin/stty cooked");
      system("stty echo");
      system("clear");
      printf("\n\n Resultado final: %d \n\n", score);

      deltaT = tempo2-tempo1;
      printf("\n\n TEMPO TOTAL: %d SEGUNDOS", deltaT);


      return 0;

}




void snake_init(snake *snake1)
{
    snake1->symbol='o';
    snake1->size=3;
    snake1->direction=right;
    snake1->prev_direction=down;
    snake1->tail_X=5;
    snake1->tail_Y=5;
    snake1->head_X=snake1->tail_X+snake1->size-1;
    snake1->head_Y=5;
}


void snake_place(snake *snake1, snake_pos *pos1)
{
    int i;
    for (i=0; i<snake1->size; ++i)
    {
        gotoxy(snake1->tail_X,snake1->tail_Y);
        printf("%c",snake1->symbol);
        pos1->X[i]=snake1->tail_X;
        pos1->Y[i]=snake1->tail_Y;
        snake1->tail_X+=1;
    }

}


void set_borders()
{
    int i;
    for (i=0; i<vertical; ++i)
    {
        gotoxy(0,i);
        printf("#");
        gotoxy(horizontal,i);
        printf("#");
    }

    for (i=0; i<horizontal; ++i)
        {
            gotoxy(i,0);
            printf("#");
            gotoxy(i,vertical);
            printf("#");
        }
printf("\n");
printf("                                   JOGO DA COBRINHA");
printf("\n");
}



void snake_move(BlackGPIO* bt,snake *snake1, snake_pos *pos1, food *food1, int *score)
{
    move_head(bt,snake1,pos1);

    if (!((snake1->head_X==food1->X) && (snake1->head_Y==food1->Y)))
    {
        move_tail(snake1,pos1);
    }
    else
    {
        snake1->size++;
	snake_speed=snake_speed-20000;
        *score=*score+1;
        food1->X=rand()%(horizontal-8)+5;
        food1->Y=rand()%(vertical-8)+5;
        food_print(food1);
    }
}



void move_tail(snake *snake1, snake_pos *pos1)
{
    int i;

    // remove last cell of tail
    gotoxy(pos1->X[0],pos1->Y[0]);
    printf(" ");


    // update new tail position
    for (i=0; i<snake1->size; ++i)
    {
        pos1->X[i]=pos1->X[i+1];
        pos1->Y[i]=pos1->Y[i+1];
    }
}



void move_head(BlackGPIO* bt,snake *snake1, snake_pos *pos1)
{
    switch (snake1->direction)
        {
            case 'd':
                if (snake1->prev_direction==left)
                {
                    snake1->head_X--;
                    break;
                }
                    snake1->head_X++;
                    break;
		    snake1->prev_direction=right;
		    

            case 's':
                if (snake1->prev_direction==right)
                {
                    snake1->head_X++;
                    break;
                }
                    snake1->head_X--;
                    break;
		    snake1->prev_direction=down;


            case 'w':
                if (snake1->prev_direction==down)
                {
                    snake1->head_Y++;
                    break;
                }
                    snake1->head_Y--;
                    break;
		    snake1->prev_direction=up;


            case 'a':
                if (snake1->prev_direction==up)
                {
                    snake1->head_Y--;
                    break;
                }
                    snake1->head_Y++;
                    break;
		    snake1->prev_direction=down;
		    
            default:
                 break;
        }

        // update tail position
        pos1->X[snake1->size]=snake1->head_X;
        pos1->Y[snake1->size]=snake1->head_Y;

        gotoxy(pos1->X[snake1->size],pos1->Y[snake1->size]);
        printf("%c",snake1->symbol);
}



void food_init(food *food1)
{
    srand((unsigned)time(NULL));
    food1->X=((rand()%(horizontal-5))+1);
    food1->Y=((rand()%(vertical-5))+1);
    food1->symbol='@';
}


void food_print(food *food1)
{
    gotoxy(food1->X,food1->Y);
    printf("%c",food1->symbol);

}


void gotoxy(int x,int y)
{
    printf("%c[%d;%df",0x1B,y,x);
}



void pos_init(snake_pos *pos1)
{
    memset(pos1, 0, sizeof(*pos1));
}


int game_over(snake *snake1, snake_pos *pos1)
{
    int i;

    for (i=0; i<snake1->size-1; ++i)
    {
        if ((pos1->X[i]==snake1->head_X) && (pos1->Y[i]==snake1->head_Y))
        {
            return 1;
        }
    }


    if ((snake1->head_X==horizontal) || (snake1->head_X==1) || (snake1->head_Y==vertical) || (snake1->head_Y==1))
        {
            return 1;
        }


    return 0;
}
int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}
