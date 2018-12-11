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
#include <thread>
#include <semaphore.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace BlackLib;
using namespace std;
 
void carga(int k);
int setCPU(int i);
BlackGPIO f1Led(GPIO_46,output);
BlackGPIO f2Led(GPIO_65,output);
BlackGPIO f3Led(GPIO_26,output);
BlackGPIO f4Led(GPIO_66,output);

ADC pot1(AIN0);
ADC pot2(AIN2);
ADC pot3(AIN4);
ADC pot4(AIN6);

float pa, pb, pc, pd;
unsigned short portaRecebe = 1408;
unsigned short portaEnvio = 1408;   
bool trocaPorta;

#define MULTICAST_ADDR "225.0.0.37"

void enviaPrioridades(){
    int sockfd;
    int len;
    socklen_t len_recv;
    struct sockaddr_in address;
    float prioridades[2];
     
    sockfd  = socket(AF_INET, SOCK_DGRAM,0);  // criacao do socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
    address.sin_port = htons(portaEnvio);    
    len = sizeof(address);
 
    while (1){
        prioridades[0]= pot1.getPercentValue();
        prioridades[1]= pot2.getPercentValue();
        prioridades[2]= pot3.getPercentValue();
        prioridades[3]= pot4.getPercentValue();
        sendto(sockfd, &prioridades,sizeof(prioridades),0,(struct sockaddr *) &address, len);
        usleep(1000000);
    }
    exit(0);
}
void recebePrioridades(){
    int server_sockfd, client_sockfd;
    size_t server_len;
    socklen_t client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    struct ip_mreq mreq;  
     
    if ((server_sockfd = socket(AF_INET, SOCK_DGRAM, 0) )  < 0  ){
        printf(" Houve erro na ebertura do socket ");
        exit(1);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(portaRecebe);
    server_len = sizeof(server_address);
    if(bind(server_sockfd, (struct sockaddr *) &server_address, server_len) < 0 ){
        perror("Houve error no Bind");
        exit(1);
    }

    mreq.imr_multiaddr.s_addr=inet_addr(MULTICAST_ADDR);
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);
    if (setsockopt(server_sockfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
        perror("setsockopt");
        exit(1);
    }
    printf(" IPPROTO_IP = %d\n", IPPROTO_IP);
    printf(" SOL_SOCKET = %d\n", SOL_SOCKET);
    printf(" IP_ADD_MEMBERSHIP = %d \n", IP_ADD_MEMBERSHIP);
    float prioridades[4];    
    while (!trocaPorta){
        client_len = sizeof(client_address);
        if(recvfrom(server_sockfd, &prioridades, sizeof(prioridades),0,(struct sockaddr *) &client_address, &client_len) < 0 ){
            perror(" erro no RECVFROM( )");
            exit(1);
        }
        pa = prioridades[0];
        pb = prioridades[1];
        pc = prioridades[2];
        pd = prioridades[3];
         
    }
 
    close(server_sockfd);
    exit(0);
}

int main(){
 
    int i=0;
    int tCarga = 50;
 	
    unsigned int pidFilho1, pidFilho2,pidFilho3,pidFilho4;

    int alta = -10;
    int media = 0;
    int baixa = 5;

    pidFilho1 = fork();//cria filho 1
    thread envia(enviaPrioridades);
    usleep(1000000);
    trocaPorta = false;
    cout<<"digite a porta para envio";

    cin>> portaEnvio;

    thread recebe(recebePrioridades);

    usleep(1000000);

 switch(pidFilho1){
        case -1:
            exit(1);
        case 0://executa filho 1
            setCPU(0);
            while(1){
                carga(tCarga);
                if(i%2==0) {
   
                 f1Led.setValue(high);
                }
                else{ 

                 f1Led.setValue(low);
                }
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
                if(i%2==0) {
   
                 f2Led.setValue(high);
                }
                else{ 

                 f2Led.setValue(low);
                }
                i++;
            }
            exit(0);
        default://executa pai       
            pidFilho3 = fork();//cria filho 3
    switch(pidFilho3){
        case -1:
            exit(1);
        case 0://executa filho 3
            setCPU(0);
            while(1){
                carga(tCarga);
                if(i%2==0) {
   
                 f3Led.setValue(high);
                }
                else{ 

                 f3Led.setValue(low);
                }
                i++;
            }
            exit(0);
        default://executa pai       
            pidFilho4 = fork();//cria filho 4
            switch(pidFilho4){
                case -1:
                    exit(1);
                case 0://executa filho 4
                    setCPU(0);
                    while(1){
                        carga(tCarga);
                        if(i%2==0) f4Led.setValue(high);
                        else f4Led.setValue(low);
                        i++;
                    }
                    exit(0);
                default://codigo do pai
                    setCPU(0);
                    setpriority(PRIO_PROCESS, 0, alta);
                    while(1){
                        if(pa> 1.0) setpriority(PRIO_PROCESS, pidFilho1, media);
                        else  setpriority(PRIO_PROCESS, pidFilho1, baixa);
                        if(pb > 1.0) setpriority(PRIO_PROCESS, pidFilho2, media);
                        else  setpriority(PRIO_PROCESS, pidFilho2, baixa);
                        if(pc> 1.0) setpriority(PRIO_PROCESS, pidFilho3, media);
                        else  setpriority(PRIO_PROCESS, pidFilho3, baixa);
                        if(pd > 1.0) setpriority(PRIO_PROCESS, pidFilho4, media);
                        else  setpriority(PRIO_PROCESS, pidFilho4, baixa);
                    		};
                    		exit(0);
            		}
	      	  }
    	  }
    }
    exit(0);
}

void carga(int k){
    float f = 0.999999;
    for(int i=0; i<k; i++){
        f = f*f*f*f*f;
        f = 1.56;
        for(int j=0; j<k; j++) f = sin(f)*sin(f)*f*f*f;
    }
}
 
int setCPU(int i){
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(i, &mask);
    return sched_setaffinity(0, sizeof(mask), &mask);
}
