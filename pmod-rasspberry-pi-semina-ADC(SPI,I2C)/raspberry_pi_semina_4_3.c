//가변저항

#include <stdio.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>//i2c용 함수 라이브러리
#include <string.h>

#define Device_Address 0x2a	//MPU6050의 주소


unsigned int fd;//통신 열기

void I2C_Seting(unsigned char Channel)//통신 세팅
{
	//wiringPiI2CWriteReg8(fd, device 주소, bit data)
	wiringPiI2CWriteReg8 (fd, 0x00, 0x01);	// RR(1)모든 레지스터 초기화
	wiringPiI2CWriteReg8 (fd, 0x00, 0x96);	// RR(0) 표준동작, PUD(1) 디지털회로 전원 ON ,PUA(1) 아날로그회로 전원 ON,CS(1) ADC 변환시작, AVDDS(1) 내부AVDD사용
	wiringPiI2CWriteReg8 (fd, 0x01, 0x18);	//VLDO 011(3.6V), 실제로 측정되는 값이 3.3V임
	wiringPiI2CWriteReg8(fd, 0x02, (Channel << 7));//채널설정 0 or 1 채널                                  
}

int ADC()
{
	int adc_data=0;
	unsigned int data_H=0, data_M=0, data_L=0;
	int st = 0;
	
	do
	{
		st = wiringPiI2CReadReg8(fd,0x00); 
	}
	while( st & 0x28 == 0x00 );  //adc 변환할 준비가 될때까지 대기, CR(1) ADC DATA ready, PUR(1) Power Up ready 이면 데이터 읽음
	data_H = wiringPiI2CReadReg8(fd, 0x12);     //23:16 상위 비트
	data_M = wiringPiI2CReadReg8(fd, 0x13);     //15:8 중간 비트
	data_L = wiringPiI2CReadReg8(fd, 0x14);     //7:0 하위 비트

	adc_data = (data_H<<16)|(data_M<<8)|(data_L);  // 상위, 중간, 하위비트를 합쳐서 24비트 데이터를 읽어온다
	return adc_data;
}
int main()
{
	unsigned int adcvalue=0;
	
	if(wiringPiSetup() < 0)//wiringPi 초기화
	{
		fprintf (stdout, "Unable to start wiringPi : %s\n",strerror(errno));
		return 1;
	}
	
	if ((fd = wiringPiI2CSetup(Device_Address))<0) //SPI 채널 열기, slave 주소 0x2a
	{
		fprintf (stdout, "Unable to initialize I2C : %s\n",strerror(errno));
		return 1;
	}
	
	I2C_Seting(0);//통신 세팅
	
	
	while(1)
	{
		adcvalue = ADC();
		printf("sensor = %d \n" ,adcvalue);
		delay(200);
	}
}