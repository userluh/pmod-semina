#include <stdio.h> 
#include <string.h> 
#include <errno.h> 
#include <wiringPi.h> 
#include <wiringPiSPI.h>//spi용 함수 라이브러리
 
#define CS_MCP3208 8 //cs신호
#define SPI_CHANNEL 0 //사용채널
#define SPI_SPEED 1000000 //샘플링속도 지정 속도 지정 (50kHz ~ 32MHz) ,1Mhz 
 
// spi communication with Rpi and get sensor data 
 
int read_mcp3208_adc(unsigned char adcChannel) //채널을 인수로 받음
{
    unsigned char buff[3];
    int adcValue = 0;
    
    buff[0] = 0x06 | ((adcChannel & 0x07) >> 2);
    buff[1] = ((adcChannel & 0x07) << 6);
    buff[2] = 0x00;
    
    digitalWrite(CS_MCP3208, 0);
    wiringPiSPIDataRW(SPI_CHANNEL, buff, 3);
    
    buff[1] = 0x0f & buff[1];
    adcValue = (buff[1] << 8 ) | buff[2];
    
    digitalWrite(CS_MCP3208, 1);
    
    return adcValue;
}
 
int main(void) {
 
    unsigned char adcChannel_light = 0;
 
    int adcValue_light = 0;
 
    float vout_light;
    float vout_oftemp;
    float percentrh = 0;
    float supsiondo = 0;
    
    printf("start");
 
    
    if(wiringPiSetup() == -1)//wiringPi 초기화 
    {
        fprintf(stdout, "Unable to start wiringPi :%s\n", strerror(errno));
        return 1;
    }
    
    if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) == -1)//SPI 채널 열기
    {
        fprintf(stdout, "wiringPiSPISetup Failed :%s\n", strerror(errno));
        return 1;
    }
    
    pinMode(CS_MCP3208, OUTPUT);//MASTER(라즈베리파이)에서 CS신호를 '보내야'하므로 OUTPUT
    
    while(1) //ADC된 값을 받아와서 출력함 
    {
        adcValue_light = read_mcp3208_adc(adcChannel_light);
        
        //printf("Humiity = %u temparature = %u\n", adcValue_humi, adcValue_temp);
        printf("light sensor = %u\n", adcValue_light);
        
        delay(100);
    }
    return 0;
}