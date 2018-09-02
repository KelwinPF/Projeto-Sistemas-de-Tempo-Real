#include <iostream>
#include <thread>
#include <pthread.h>
#include "BlackGPIO/BlackGPIO.h"

using namespace BlackLib;
using namespace std;

int rPinLed = 46;
int rPinBt = 67;

BlackGPIO entrada((gpioName)rPinBt,input);
BlackGPIO saida ((gpioName)rPinLed,output);

int main (int arcgc,char * argv[]){
  
  
  cout << "Example 1 - GPIO (in/out)" << endl;

  while(true){
  string val = entrada.getValue();
   if(val=="1"){
      saida.setValue(high);
      cout << "high" << endl;
   }else{
      saida.setValue(low);
      cout << "low" << endl;
   }
  }
 return 0;
}
