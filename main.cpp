
#include "mbed.h"
//include <cstring>
#include <math.h> 
#include <stdio.h> 

#define MAXIMUM_BUFFER_SIZE
using namespace std;

Thread thread1;
Thread thread2;
Thread thread3;
Thread thread4;

    
    char ns, ew, tf, status;
    int fq, nst, fix, date;                                     // fix quality, Number of satellites being tracked, 3D fix
    float timefix, latitude, longitude, speed, altitude;
 



float Lastlocation_lat= 54.4312f;
float Lastlocation_lon= 55.5612f;


UnbufferedSerial pc(USBTX, USBRX);
UnbufferedSerial  dev(PC_6, PC_7); //  GPS  UART6 PC_6 - TX, PC_7 - RX
UnbufferedSerial  rak811(D8, D2); //UART D8- TX, D2 - RX

int dev_RxLen=0;
int pc_RxLen=0;

int rak811_RxLen=0;


int nomer_stroki=0;
int konec_stroki=0;
int m=0;
int j;

  
 char dev_RxBuf[4096] = {0};
 char rak811_RxBuf[4096] = {0};

char pc_RxBuf[4096] = {0};
  char buf4[4096] = {0};

 char buf5[4096] = {0};

   char parse_bufer[4096] = {0};

 // char lat_char[]={"5443.176270"};
  char lat_char[20]={};
// char lon_char[]={"5555.872559"};
  char lon_char[20]={};
 


  
// обработчики прерываний по приему байта с устройства и с компа- просто заполняют буферы свои- при достижении 64 байта в буфере -начинают заполнять снова с нуля

void dev_recv()
{

   if (dev_RxLen<4096)
   {
    dev.read(&dev_RxBuf[dev_RxLen], sizeof(dev_RxBuf[dev_RxLen]));    //  Got 1 char    
    dev_RxLen++;
    }
    else
     { dev_RxLen=0;
    }
}

void rak811_recv()
{
   if (rak811_RxLen<4096)
        {
    rak811.read(&rak811_RxBuf[dev_RxLen], sizeof(rak811_RxBuf[rak811_RxLen]));    //  Got 1 char    
    rak811_RxLen++;
        }
else    {
        rak811_RxLen=0;
        }
}

 
void pc_recv()
{
       if (pc_RxLen<4096)
   {
    pc.read(&pc_RxBuf[pc_RxLen], sizeof(pc_RxBuf[pc_RxLen]));  //  Got 1 char
    pc_RxLen++;
   }
else {
 pc_RxLen=0;
    }
}




void parse(char *cmd, int n)
{
      //if(strncmp(cmd,"$GPGGA", 6) == 0)
    if ((cmd[0]=='$')&&(cmd[1]=='G')&&(cmd[2]=='P')&&(cmd[3]=='G')&&(cmd[4]=='G')&&(cmd[5]=='A')) 
    {

//    НИЖЕ - ЭТО ЗАПИСЬ КООРДИНАТ В ВИДЕ СТРОКИ СИМВОЛОВ. БЕЗ ПАРСИНГА.       
        for (int i=0; i<10; i++)
        {
        lat_char[i]=cmd[17+i]; 
      //  lon_char[i]=cmd[30+i];  
        }

 for (int i=0; i<11; i++)
        {
     //   lat_char[i]=cmd[17+i]; 
        lon_char[i]=cmd[30+i];  
        }
//

/*
////////////////// ЭТОТ КУСОК ОСТАВЛЕН ДЛЯ ОТЛАДКИ . ОТКЛЮЧИЛ.  РАСПЕЧАТЫВАЕТ В КОНСОЛЬ СЧИТАННЫЕ КООРДИНАТЫ (БЕЗ ПАРСИНГА)
 for (int i = 0; i < 10; i++) {  
//распечатать на консоль 
    pc.write(&lat_char[i], sizeof(lat_char[i]));
 } 
printf ("latttttttt");
for (int i = 0; i < 10; i++) {  
//распечатать на консоль
    pc.write(&lon_char[i], sizeof(lon_char[i]));
 } 
 printf ("lonnnnnn");
///////////////////
*/

// ПОЛУЧАЕМ КООРДИНАТЫ  В ПЕРЕМЕННЫЕ 
        sscanf(cmd, "$GPGGA,%f,%f,%c,%f,%c,%d,%d,%*f,%f", &timefix, &latitude, &ns, &longitude, &ew, &fq, &nst, &altitude);

    // ОСТАВИЛ ДЛЯ ПРОВЕРКИ РАБОТОСПОСОБНОСТИ ПАРСИНГА - РАСПЕЧАТАТЬ В КОНСОЛЬ ПОЛУЧЕННЫЕ ПАРАМЕТРЫ
    //    printf("GPGGA Fix taken at: %f, Latitude: %f %c, Longitude: %f %c, Fix quality: %d, Number of sat: %d, Altitude: %f M\n", timefix, latitude, ns, longitude, ew, fq, nst, altitude);




/* 
    // ОСТАВИЛ ДЛЯ ПРОВЕРКИ РАБОТОСПОСОБНОСТИ ПРЕОБРАЗОВАНИЯ КООРДИНАТ ОБРАТНО В СТРОКУ СИМВОЛОВ - ЭТО НЕ РАБОТАЕТ!! 
     sprintf(lat_char,"%4.5f", &latitude);
    sprintf(lon_char,"%5.5f", &longitude);

 for (int i = 0; i < 10; i++) {  
   // dev.write(&buf4[i], sizeof(buf4[i]));
//распечатать на консоль то что послано на  модуль
    pc.write(&lat_char[i], sizeof(lat_char[i]));
        }  
printf ("jhfgjfjg");
*/
     
                
    }
    

    /////////////// ЭТО ПАРСИНГ ОСТАЛЬНЫХ ПАРМЕТРОВ - ВСЕ РАБОТАЕТ. ОТКЛЮЧИЛ ЗА НЕНАДОБНОСТЬЮ
/*
    // Satellite status
 //   if(strncmp(cmd,"$GPGSA", 6) == 0) 
    if ((cmd[0]=='$')&&(cmd[1]=='G')&&(cmd[2]=='P')&&(cmd[3]=='G')&&(cmd[4]=='S')&&(cmd[5]=='A'))
    {
        sscanf(cmd, "$GPGSA,%c,%d,%d", &tf, &fix, &nst);
  //      printf("GPGSA Type fix: %c, 3D fix: %d, number of sat: %d\r\n", tf, fix, nst);
    }
    
    // Geographic position, Latitude and Longitude
  //  if(strncmp(cmd,"$GPGLL", 6) == 0) 
    if ((cmd[0]=='$')&&(cmd[1]=='G')&&(cmd[2]=='P')&&(cmd[3]=='G')&&(cmd[4]=='L')&&(cmd[5]=='L'))
    {
        sscanf(cmd, "$GPGLL,%f,%c,%f,%c,%f", &latitude, &ns, &longitude, &ew, &timefix);
//       printf("GPGLL Latitude: %f %c, Longitude: %f %c, Fix taken at: %f\n", latitude, ns, longitude, ew, timefix);
    }
    
    // Geographic position, Latitude and Longitude
    //if(strncmp(cmd,"$GPRMC", 6) == 0) 
    if ((cmd[0]=='$')&&(cmd[1]=='G')&&(cmd[2]=='P')&&(cmd[3]=='R')&&(cmd[4]=='M')&&(cmd[5]=='C'))
    {
        sscanf(cmd, "$GPRMC,%f,%c,%f,%c,%f,%c,%f,,%d", &timefix, &status, &latitude, &ns, &longitude, &ew, &speed, &date);
  //      printf("GPRMC Fix taken at: %f, Status: %c, Latitude: %f %c, Longitude: %f %c, Speed: %f, Date: %d\n", timefix, status, latitude, ns, longitude, ew, speed, date);
    }

*/
}




////////// ЭТА ФУНКЦИЯ НЕ РАБОТАЕТ!!! ОСТАВИЛ  ВДРУГ ПРИГОДИТСЯ
float GpsToDecimalDegrees(char* nmeaPos, char quadrant)
{
  float v= 0;
  //if(strlen(nmeaPos)>5)
  if(sizeof(nmeaPos)>6)
  {
    char integerPart[3+1];
    int digitCount= (nmeaPos[4]=='.' ? 2 : 3);
    memcpy(integerPart, nmeaPos, digitCount);
    integerPart[digitCount]= 0;
    nmeaPos+= digitCount;
    v= atoi(integerPart) + atof(nmeaPos)/60.;
    if(quadrant=='W' || quadrant=='S')
      v= -v;
  }
  return v;
}
//////////////////////////


float convertToDecimalDegrees(char *latLon, char *direction)
{
  char deg[4] = {0};
  char *dot, *min;
  int len;
  float dec = 0;

  if ((dot = strchr(latLon, '.')))
  {                                         // decimal point was found
    min = dot - 2;                          // mark the start of minutes 2 chars back
    len = min - latLon;                     // find the length of degrees
    strncpy(deg, latLon, len);              // copy the degree string to allow conversion to float
    dec = atof(deg) + atof(min) / 60;       // convert to float
    if (strcmp(direction, "S") == 0 || strcmp(direction, "W") == 0)
      dec *= -1;
  }
  return dec;
}
//////////////////////



void copy_to_my_bufer()
{
int ii; 
    while(1)
    {
if (dev_RxLen>0)
{

for (int j = 0; j < dev_RxLen; j++) 
{

if(dev_RxBuf[j] == '$')
{
    ii=0;
parse_bufer[ii]='$';
nomer_stroki++;
ii++;
}
else
{
parse_bufer[ii]=dev_RxBuf[j];
ii++;

if(dev_RxBuf[j] == '\r')
{
konec_stroki=nomer_stroki;
}
}
}
dev_RxLen=0;
}
    ThisThread::yield();
    }
}




void parse_from_my_bufer()
{
    while(1)
    {
    if (konec_stroki==nomer_stroki)
    {
      if (nomer_stroki=!0)
       {
    parse(parse_bufer, m);
    m = sizeof(parse_bufer); 
    }   
    }  
ThisThread::yield();
}
}
   

void read_datchik_and_send_to_server_lora()
{
       while (true) 
{
 
//float latitude_= GpsToDecimalDegrees(lat_char,ns);
float latitude_= convertToDecimalDegrees(lat_char,&ns);
// latitude_ = 54.4312;

//float longitude_= GpsToDecimalDegrees(lon_char,ew);
float longitude_= convertToDecimalDegrees(lon_char,&ew);
//longitude_= 55.5612;

        sprintf(buf5,"at+send=lora:1:%4X%4X\r\n",*( unsigned  int *)&latitude_, *( unsigned  int *)&longitude_);          

         for (uint8_t i = 0; i < 33 ; i++)            
           {  
    rak811.write(&buf5[i], sizeof(buf5[i]));
    //продублировать это в консоль
   pc.write(&buf5[i], sizeof(buf5[i]));
   }          
      ThisThread::sleep_for(20s);
     //  ThisThread::yield();
}
}


void recieve_otvety()
{                   
while(1)
{    
        if (rak811_RxLen>0)
        {    
    //нужно только для отладки -можно убрать
    //распечатать на консоль то что пришло  с рак811
    for (uint8_t i = 0; i < rak811_RxLen; i++) {  
   pc.write(&rak811_RxBuf[i], sizeof(rak811_RxBuf[i]));}
    
        rak811_RxLen=0;
       // ThisThread::sleep_for(1s);
        
        }
        ThisThread::yield();

   }
   
}



int main()
{
    pc.attach(&pc_recv, UnbufferedSerial::RxIrq);
    dev.attach(&dev_recv, UnbufferedSerial::RxIrq);
    rak811.attach(&rak811_recv, UnbufferedSerial::RxIrq);    
    pc.baud(115200);
    dev.baud(9600);
    rak811.baud(115200);

    thread4.start(recieve_otvety);
    thread1.start(copy_to_my_bufer);
  thread2.start(parse_from_my_bufer);

   sprintf(buf4,"at+join\r\n"); 
  for (uint8_t i = 0; i < 9; i++) {  
    rak811.write(&buf4[i], sizeof(buf4[i]));
//распечатать на консоль то что послано на  рак811
    pc.write(&buf4[i], sizeof(buf4[i]));
        }  
               ThisThread::sleep_for(1s); 


  thread3.start(read_datchik_and_send_to_server_lora);
}