#include <iostream>
#include "BlackGPIO/BlackGPIO.h"

using namespace BlackLib;

int main (int arcgc,char * argv[]){
  std::cout << "Example 1 - GPIO (in/out)" << endl;
  BlackGPIO saida(GPIO_14,output);
  BlackGPIO entrada(GPIO_67,output); 
  while(true){
  std:string val = entrada.getvalue();
   if(val=="1"){
      saida.getvalue(high);
      std::cout << "high" << endl;
   }else{
      saida.getvalue(low);
      std::cout << "low" << endl;
   }
  }
 return 0;
}
