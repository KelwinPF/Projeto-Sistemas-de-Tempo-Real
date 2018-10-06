#define _GNU_SOURCE
 
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>
#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"
 
using namespace BlackLib;
using namespace std;
 
void carga(int k);
int setCPU(int i);
 
int main(){
 
    int i=0;
    int tCarga = 300;
 
    unsigned int pidFilho1, pidFilho2;
 
    BlackGPIO f1Led(GPIO_46,output);
    BlackGPIO f2Led(GPIO_65,output);
    ADC pot1(AIN0);
    ADC pot2(AIN2);
    float potValue;
    int alta = -10;
    int media = 0;
    int baixa = 5;
 
    pidFilho1 = fork();//cria filho 1
    switch(pidFilho1){
        case -1:
            exit(1);
        case 0://executa filho 1
            setCPU(0);
            while(1){
                carga(tCarga);
                if(i%2==0) f1Led.setValue(high);
                else f1Led.setValue(low);
                i++;
            }
            exit(0);
        default://executa pai       
            pidFilho2 = fork();//cria filho 2
            switch(pidFilho2){
                case -1:
                    exit(1);
                case 0://executa filho 2
                    setCPU(0);
                    while(1){
                        carga(tCarga);
                        if(i%2==0) f2Led.setValue(high);
                        else f2Led.setValue(low);
                        i++;
                    }
                    exit(0);
                default://codigo do pai
                    setCPU(0);
                    setpriority(PRIO_PROCESS, 0, alta);
                    while(1){
                        if(pot1.getFloatValue() > 1.0) setpriority(PRIO_PROCESS, pidFilho1, media);
                        else  setpriority(PRIO_PROCESS, pidFilho1, baixa);
                        if(pot2.getFloatValue() > 1.0) setpriority(PRIO_PROCESS, pidFilho2, media);
                        else  setpriority(PRIO_PROCESS, pidFilho2, baixa);
                    };
                    exit(0);
            }
    }
    exit(0);
}
 
