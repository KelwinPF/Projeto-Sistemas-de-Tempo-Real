#include <iostream>
#include "BlackGPIO/BlackGPIO.h"

using namespace BlackLib;
using namespace std;

int main (int arcgc,char * argv[]){
  cout << "Example 1 - GPIO (in/out)" << endl;
  BlackGPIO saida(GPIO_26,output);
  BlackGPIO entrada(GPIO_67,input); 
  while(true){
  string val = entrada.getValue();
   if(val=="1"){
      saida.setValue(low);
      cout << "high" << endl;
   }else{
      saida.setValue(high);
      cout << "low" << endl;
   }
  }
 return 0;
}
