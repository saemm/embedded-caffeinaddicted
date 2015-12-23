 #include<wiringPi.h>
 #include<stdio.h>
 #include<stdlib.h>
 #include<stdint.h>
 #include<time.h>
 #include<wiringPiSPI.h>
 #include<softTone.h>
  
 #define MAXTIMINGS  85
 #define DHTPIN      4  //GPIO 23
 #define LED1        1  //GPIO 18
 #define LED2        27 //GPIO 16
 #define LED3        15 //GPIO 14
 #define LED4        8  //GPIO 2
 #define MOTOR1      28 //GPIO 20
 #define MOTOR2      24 //GPIO 19
 #define SOUND       25 //GPIO 26
 #define CS_MCP3208  10 //GPIO 8
 #define SPI_SPEED   1000000   // 1MHz
 #define SPI_CHANNEL 0
 
 #define MOISTURE    100  //수분센서 지정값 -> 100이하면 물 뿌리기
 #define WATER       200  //수위센서 지정값 -> 200이하면 물 없음
 #define TEMPERATURE  22 //온도 지정값	
 
 int dht11_dat[5]={0,};
 
 int read_mcp3208_adc(unsigned char adcChannel)
 {
 	unsigned char buff[3];
        int adcValue = 0;
 
      buff[0] = 0x06 | ((adcChannel & 0x07) >> 2);
      buff[1] = ((adcChannel & 0x07) << 6);
      buff[2] = 0x00;
 
      digitalWrite(CS_MCP3208, 0);  // Low : CS Active
      delay(3000);
      wiringPiSPIDataRW(SPI_CHANNEL, buff, 3);
 
      buff[1] = 0x0F & buff[1];
      adcValue = ( buff[1] << 8) | buff[2];
      digitalWrite(CS_MCP3208, 1);  // High : CS Inactive
      delay(3000);
 
      return adcValue;
  }
 
  void read_dht11_dat()   //온도와 습도 측정해서 출력해주는 함수
  {
      uint8_t laststate=HIGH;
      uint8_t counter=0;
      uint8_t j=0,i;
      uint8_t flag=HIGH;
      uint8_t state =0;
      float f;
      dht11_dat[0]=dht11_dat[1]=dht11_dat[2]=dht11_dat[3]=dht11_dat[4]=0;
 
      pinMode(DHTPIN,OUTPUT);
      digitalWrite(DHTPIN,LOW);
      delay(18);
      digitalWrite(DHTPIN,HIGH);
      delayMicroseconds(40);
      pinMode(DHTPIN,INPUT);
 
      for(i=0;i<MAXTIMINGS;i++){
          counter=0;
          while ( digitalRead(DHTPIN) == laststate) {
              counter++ ;
              delayMicroseconds(1) ;
              if (counter == 255)
                  break ;
          }
 
          laststate = digitalRead(DHTPIN) ;
          if (counter == 255)
              break ; // if while breaked by timer, break for
 
          if ((i >= 4) && (i % 2 == 0)) {
              dht11_dat[j / 8] <<= 1 ;
              if (counter > 16) dht11_dat[j / 8] |= 1 ;
                  j++ ;
         }
     }

if((j>=40)&& (dht11_dat[4]==((dht11_dat[0]+dht11_dat[1]+dht11_dat[2]+dht11_dat[3])&0xff))){
         printf("humidity = %d.%d %% Temperature = %d.%d *C \n", dht11_dat[0],dht11_dat[1], dht11_dat[2], dht11_dat[3]) ;
     }

     else
         ;//printf("Data get failed\n") ;
 }
 
 int speaker(){  // 수위센서 지정값 이하일 때 알람 울리기
     int i;
     softToneCreate(SOUND);
     softToneWrite(SOUND, 659.25);
     delay(300);
     softToneWrite(SOUND, 523.25);
     delay(300);
     return 0;
 }

 int moisture(){  //수분센서 값 측정 함수
     int adcChannel=0;
     int adcValue[8]={0};

     pinMode(CS_MCP3208, OUTPUT);

       digitalWrite(CS_MCP3208, HIGH);
       delay(1000);
       adcValue[2] = read_mcp3208_adc(2); // Moisture Sensor
       printf("moisture= %5d\n", adcValue[2]);
       digitalWrite(CS_MCP3208, LOW);
       delay(1000);
 
       return adcValue[2];
 }

int water(){ //수위센서 값 측정 함수
     int adcChannel=0;
     int adcValue[8]={0};

     pinMode(CS_MCP3208, OUTPUT);


         digitalWrite(CS_MCP3208, HIGH);
         delay(1000);
         adcValue[3] = read_mcp3208_adc(3); // Moisture Sensor
         printf("water = %5d\n", adcValue[3]);
         digitalWrite(CS_MCP3208, LOW);
         delay(1000);

     return adcValue[3];
 }
 
 int main(void){
     int i;
     int tmp=0;
     int waterSensor;
    // int Check_LED = atoi(argv[1]);
    // int check_WATER = atoi(argv[2]);
	
     if (wiringPiSetup() == -1)
         return 1;
     if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) == -1)
         return 1;

     softToneCreate(SOUND);

     pinMode(LED1, OUTPUT);
     pinMode(LED2, OUTPUT);
     pinMode(LED3, OUTPUT);
     pinMode(LED4, OUTPUT);
     pinMode(MOTOR1, OUTPUT);
     pinMode(MOTOR2, OUTPUT);
     pinMode(CS_MCP3208,OUTPUT);

     digitalWrite(LED1, LOW);
     digitalWrite(LED2, LOW);
     digitalWrite(LED3, LOW);
     digitalWrite(LED4, LOW);
     delay(1000);
	 
     while(1){
         read_dht11_dat();  //온습도 값 측정, 출력

         if(dht11_dat[2]!=0 && dht11_dat[2]<30){
             while(1){
                 if(dht11_dat[2]!=0 && dht11_dat[2]<30) //현재 온도가 지정한 온도보다 낮으면
                 {
                     digitalWrite(LED1, HIGH) ; // On
                     digitalWrite(LED2, HIGH);
                     digitalWrite(LED3, HIGH);
                     digitalWrite(LED4, HIGH);
                     delay(2000);
                 }
				 else{
					 digitalWrite(LED1, LOW) ; // OFF
                     digitalWrite(LED2, LOW);
                     digitalWrite(LED3, LOW);
                     digitalWrite(LED4, LOW);
				 }
                 if(moisture() < 300){ //화분 속 수분이 지정값보다 작으면
                     digitalWrite(MOTOR1, HIGH); //모터 작동해서 물 주기
                     delay(2000);
                     digitalWrite(MOTOR1, LOW);
				     delay(1000);
                     digitalWrite(MOTOR2, HIGH);
                     delay(2000);
                     digitalWrite(MOTOR2, LOW);
					 delay(1000);
                 }
                 if(water() < 100){ //수조속에 물이 지정값보다 적으면
                    delay(3000); 
					for(i=0; i<3; i++)
                         speaker();   //알람 울리기
                     softToneWrite(SOUND, 0);
                  }
                  read_dht11_dat();
			 }
		 }
	 }
     return 0;
 }

dht.c
dht.c 표시 중입니다.

