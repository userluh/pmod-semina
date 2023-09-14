//자이로 센서


#include <wiringPiI2C.h>//i2c용 함수 라이브러리
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <errno.h>	
#include <string.h>

#define Device_Address 0x68	//MPU6050의 주소

//데이터 레지스터 주소
#define PWR_MGMT_1   0x6B
#define SMPLRT_DIV   0x19
#define CONFIG       0x1A
#define GYRO_CONFIG  0x1B
#define INT_ENABLE   0x38
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define GYRO_XOUT_H  0x43
#define GYRO_YOUT_H  0x45
#define GYRO_ZOUT_H  0x47

int fd=0;// 통신 프로토콜 열기

void MPU6050_Init(){//MPU6050 초기화
	
	wiringPiI2CWriteReg8 (fd, PWR_MGMT_1, 0x01); //내부 8MHz 오실레이터 사용
	wiringPiI2CWriteReg8 (fd, CONFIG, 0);		//자이로스코프 출력속도 8khz
	wiringPiI2CWriteReg8 (fd, SMPLRT_DIV, 0x07); //샘플링 속도를 1khz로 설정  *샘플링 속도 = 자이로스코프 출력 속도 / (1 + SMPLRT_DIV(7))*
	wiringPiI2CWriteReg8 (fd, GYRO_CONFIG, 24);	//자이로스코프의 출력범위를 2000 °/s로 설정한다.
	wiringPiI2CWriteReg8 (fd, INT_ENABLE, 0x01);//모든 센서 레지스터에 대한 쓰기 작업이 완료될 때마다 발생하는 데이터 준비 인터럽트를 활성화

	} 
short read_raw_data(int addr){//레지스터 주소를 인수로 받음
	short high_byte,low_byte,value;//상위비트, 하위비트, adc값 저장할 변수
	high_byte = wiringPiI2CReadReg8(fd, addr);//상위비트 데이터 읽어오기
	low_byte = wiringPiI2CReadReg8(fd, addr+1);//하위비트 데이터 읽어오기
	value = (high_byte << 8) | low_byte;//상위비트+하위비트 해서 adc값 읽어오기
	return value;
}

int main(){

        if(wiringPiSetup()== -1)//wiringPi 초기화, wiringPi 관련 함수를 사용하려면 필수
	{
		fprintf(stdout, "Unable to start wiringPi:%s\n",strerror(errno));
		return -1;
	}
	
	float Acc_x,Acc_y,Acc_z;
	float Gyro_x,Gyro_y,Gyro_z;
	float Ax=0, Ay=0, Az=0;
	float Gx=0, Gy=0, Gz=0;
	fd = wiringPiI2CSetup(Device_Address); //통신열기
	MPU6050_Init(); //MPU6050 초기화		              
	
	while(1)
	{
		//해당 레지스터 주소에서 센서값 읽어오기
		Acc_x = read_raw_data(ACCEL_XOUT_H);
		Acc_y = read_raw_data(ACCEL_YOUT_H);
		Acc_z = read_raw_data(ACCEL_ZOUT_H);
		
		Gyro_x = read_raw_data(GYRO_XOUT_H);
		Gyro_y = read_raw_data(GYRO_YOUT_H);
		Gyro_z = read_raw_data(GYRO_ZOUT_H);
		
		//센서의 출력을 물리적 단위로 변환하여 측정값을 의미있도록 함
		Ax = Acc_x/16384.0;
		Ay = Acc_y/16384.0;
		Az = Acc_z/16384.0;
		
		Gx = Gyro_x/131;
		Gy = Gyro_y/131;
		Gz = Gyro_z/131;
		
		printf("\n Gx=%.3f °/s\tGy=%.3f °/s\tGz=%.3f °/s\tAx=%.3f g\tAy=%.3f g\tAz=%.3f g\n",Gx,Gy,Gz,Ax,Ay,Az);
		delay(500);
		
	}
	return 0;
}