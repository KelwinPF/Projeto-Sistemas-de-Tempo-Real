
#include <vector>
#include <time.h>
#include <unistd.h>
#include <thread>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"
 
using namespace BlackLib;
using namespace std;
 
//VARIAVEIS GLOBAIS*********************************************
int seqSize = 10;
int nColors = 3;
int tLed;
bool collectInputs = false;
bool endGame = false;
bool startGameBt = false;
bool resetGame = false;
time_t shutDownTime = 5;
unsigned int lvl;
int tela=0;

int *sequence;
int *userInputs;
int indiceInput;
 
//PINOS*********************************************************
int rPinBt = 67;//GPIO_67,input
int wPinBt = 68;//GPIO_68,input
int bPinBt = 44;//GPIO_44,input
int rPinLed = 46;//GPIO_46,output
int wPinLed = 65;//GPIO_65,output
int bPinLed = 47;//GPIO_47,output
int resetPinBt = 60;//GPIO_60,input
 
BlackGPIO rBt((gpioName)rPinBt,input);
BlackGPIO wBt((gpioName)wPinBt,input);
BlackGPIO bBt((gpioName)bPinBt,input);
BlackGPIO resetBt((gpioName)resetPinBt,input);
 
BlackGPIO rLed((gpioName)rPinLed,output);
BlackGPIO wLed((gpioName)wPinLed,output);
BlackGPIO bLed((gpioName)bPinLed,output);
vector<BlackGPIO*> arrayLeds;
 
//FUNCOES*******************************************************
 
void readInputs(BlackGPIO* bt, int color);
void readReset(BlackGPIO* bt);
void createNewSequence(int *s); 
void show(int color);
void blink(int time);
void blinkWin();
void blinkDerrota();
bool verifyAnswer(int i);
void ligarluzes();
 
//MAIN**********************************************************
 
int main(int argc, char * argv[]){
    //inicialização
    sequence = new int[seqSize];
    userInputs = new int[seqSize];  
    arrayLeds.push_back(&rLed);
    arrayLeds.push_back(&wLed);
    arrayLeds.push_back(&bLed);
    thread red (readInputs, &rBt, 0);
    thread white (readInputs, &wBt, 1);
    thread blue (readInputs, &bBt, 2);
    thread reset (readReset, &resetBt);
     
    bool correctAnswer, timeOut;
    time_t initialTime, currentTime, limitTime, oCTime;
    int errors, lTime;  
     
    //inicio do jogo
    while(!endGame){
        resetGame = false;
        printf("aperte sterat");
        ligarluzes();
        while(!resetGame && !endGame && startGameBt){
            int i = 0;
            errors = 0;
            tLed = 1000000/lvl;//tempo de led ligado em microssegundos
            createNewSequence(sequence);
            usleep(1000000);
            while(i<seqSize && !endGame && !resetGame){
                /*mostra sequencia*/               
                for(int j=0; j<=i; j++) {show(sequence[j]);}      
          
                /*le resposta dos botoes*/
                timeOut = false;
                correctAnswer = false;      
                lTime = limitTime = 2*(i+1)*tLed/1000000;
                indiceInput=0;
                collectInputs = true;       
                time(&initialTime);
                oCTime = initialTime;
                while(indiceInput<=i && !timeOut && !endGame && !resetGame){
                    time(&currentTime);
                    if(oCTime != currentTime){lTime--;oCTime = currentTime;}
                    if(currentTime - initialTime > limitTime) timeOut = true;
                }
                collectInputs = false;
                 
                /*verifica resposta*/
                if(!resetGame && !endGame){
                    if(!timeOut) correctAnswer = verifyAnswer(i);
                    if(correctAnswer){
                        blinkWin();
                        i++;
                        errors = 0;
                        if(i>seqSize-1) lvl = 2;
                    }
                    else{
                        errors++;
                        blinkDerrota();
                    }
                    if(errors>1) {startGameBt = false; resetGame = true;}
                    usleep(1000000);//1s
                }
            }
        }
    }
     
    red.join();
    white.join();
    blue.join();
    reset.join();

    delete[] sequence;
    delete[] userInputs;

    return 0;
}
//FUNCOES********************************************************
 void ligarluzes(){
 for(int i=0;i<nColors;i++) arrayLeds[i]->setValue(low);
 arrayLeds[0]->setValue(high);
 sleep(1);
 arrayLeds[1]->setValue(high);
 sleep(1);
 arrayLeds[2]->setValue(high);
 sleep(1);
}
void readReset(BlackGPIO* bt){
    string i;
    time_t iTime, cTime;
    while(!endGame){
        i = bt->getValue();
        if(i=="1"){
            lvl = 1;
            resetGame = true;
            startGameBt = true;
            time(&iTime);
            while(i=="1" && !endGame){
                i = bt->getValue();
                time(&cTime);
                if(cTime - iTime >= shutDownTime) endGame = true;
            }
        }
    }
 
}

bool verifyAnswer(int i){
    for(int j=0; j<=i; j++){
        if(sequence[j]!=userInputs[j]){
            return false;
            break;
        }
    }
    return true;
}
void show(int color){
    arrayLeds[color]->setValue(high);
    usleep(tLed);
    arrayLeds[color]->setValue(low);
    usleep(tLed/4);
}
void createNewSequence(int *s){
    srand((unsigned)time(NULL));
    for(int i=0; i<seqSize; i++) s[i] = rand()%nColors;
}
void readInputs(BlackGPIO* bt, int color){  
    string i;
    while(!endGame){
        i = bt->getValue();
        if(i=="1" and collectInputs){
            arrayLeds[color]->setValue(high);
            while(i=="1") i = bt->getValue();
            arrayLeds[color]->setValue(low);
            userInputs[indiceInput] = color;            
            indiceInput++;
        }
    }
}
void blink(int time){
    for(int i=0;i<nColors;i++) arrayLeds[i]->setValue(low);
    for(int i=0;i<nColors;i++) arrayLeds[i]->setValue(high);
    usleep(time);
    for(int i=0;i<nColors;i++) arrayLeds[i]->setValue(low);
}
void blinkWin(){
    for(int i=0;i<nColors;i++) arrayLeds[i]->setValue(low);
    for(int i=0;i<nColors;i++) {arrayLeds[i]->setValue(high);}
    sleep(1);
    for(int i=0;i<nColors;i++) {arrayLeds[i]->setValue(low);}
    sleep(1);
}
void blinkDerrota(){
    for(int i=nColors-1;i>=0;i--) arrayLeds[i]->setValue(low);
    arrayLeds[1]->setValue(high);
    sleep(1);
    arrayLeds[1]->setValue(low);
    sleep(1);
}
