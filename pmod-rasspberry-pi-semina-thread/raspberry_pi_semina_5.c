#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>//uint8_t 과 같은 자료형을 사용하기 위함 
#include <fcntl.h>//파일디스크립터 다루는 헤더파일 
#include <string.h>
//입출력 제어관련 라이브러리
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>//리눅스 계열에서 지원하는 spi통신을 위한 헤더파일

#include <wiringPi.h>
#include <pthread.h>//쓰레드 헤더파일
#include <softPwm.h>
#include <wiringSerial.h>

#define CS_MCP3208 10 //cs핀
#define SERVO 1 //cs핀

#define SPI_CHANNEL 0 //채널설정
#define SPI_SPEED 1000000  // 1MHz로 속도 지정


int ADC_Value = 0;//adc값
int fd;//spi 파일 디스크립터
int FD;//시리얼 파일 디스크립터
int adcChannel = 0;//adc 채널
int flag = 1;//시리얼 플래그 비트

static const uint8_t     spiBPW = 8;//단어당 비트, 8비트
static const uint16_t    spiDelay = 0;//지연시간 0초

static uint32_t    spiSpeeds[2];
static int         spiFds[2];

void Pwm(void) {//PWM초기화 함수
	pinMode(SERVO, PWM_OUTPUT);
	softPwmCreate(SERVO, 0, 200); // 서보모터 50HZ 
}

int ADC_RW(int channel, unsigned char* data, int len) {//채널,버퍼,읽을 버퍼의 길이를 입력받는다.
	struct spi_ioc_transfer spi;//spi통신을 위한 구조체 생성


	memset(&spi, 0, sizeof(spi));//구조체 초기화

	spi.tx_buf = (unsigned long)data;//수신 데이터용 버퍼
	spi.rx_buf = (unsigned long)data;//송신 데이터용 버퍼
	spi.len = len;// 수신 및 송신할 버퍼의 길이 
	spi.delay_usecs = spiDelay;//칩상태가 변경되고 다음 전송되기 까지의 지연시간, 0초로 설정
	spi.speed_hz = spiSpeeds[channel];//비트 전송률, 클럭속도
	spi.bits_per_word = spiBPW;//단어당 비트, 8비트

	return ioctl(spiFds[channel], SPI_IOC_MESSAGE(1), &spi);//구조체에 저장된 데이터를 파일디스크립터(spi장치)와 '1'번 송수신 하기, 그성공 여부를 리턴
} 

int read_mcp3208_adc(unsigned char adcChannel) {//송수신한 데이터의 값을 반환하는 함수 
	unsigned char buff[3];
	int adcValue = 0;

	buff[0] = 0x06 | ((adcChannel & 0x07) >> 2);
	buff[1] = ((adcChannel & 0x07) << 6);
	buff[2] = 0x00;

	digitalWrite(CS_MCP3208, 0);

	ADC_RW(SPI_CHANNEL, buff, 3);

	buff[1] = 0x0F & buff[1];
	adcValue = (buff[1] << 8) | buff[2];

	digitalWrite(CS_MCP3208, 1);

	return adcValue;
}

void* senser()//adc값 출력 쓰레드
{
	while (1)
	{
		ADC_Value = read_mcp3208_adc(adcChannel);
		printf(" ADC : %d \n", ADC_Value);
		delay(100);
	}
}

void* servo()//서보모터 제어 쓰레드
{
	while (1) {
		int servo_angle;
		int i = 0;


		if (0 <= ADC_Value && ADC_Value <= 250) servo_angle = 4;
		else if (251 <= ADC_Value && ADC_Value <= 1600) servo_angle = 6;
		else if (ADC_Value >= 1601) servo_angle = 8;


		if (flag == 1) {//초기상태,putty에서 g를 입력했을때 동작

			for (i = servo_angle; i <= 24; i += servo_angle) {//0~180도 까지 일정한 각도로 회전

				softPwmWrite(SERVO, i);
				delay(1000);

			}
		}

		else if (flag == 0) softPwmWrite(SERVO, 6);//puty에서 s를 입력했을 때 정지
	}
}


void* serial()//시리얼 통신 쓰레드
{

	while (1)
	{
		if (serialDataAvail(FD) > 0)//데이터를 입력 받기를 기다림
		{
			int data = 0;
			data = serialGetchar(FD);//문자를 입력받아서 저장
			serialPutchar(FD, data); //시리얼 창에 입력받은 문자 출력

			if (data == 's')
			{
				flag = 0;
			}
			else if (data == 'g')
			{
				flag = 1;
			}


		}
	}
}

int main(void)
{

	if (wiringPiSetup() == -1)//wiringPi 초기화
	{
		fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
		return 1;
	}

	if ((FD = serialOpen("/dev/ttyAMA1", 9600)) < 0) //시리얼 오픈함수
	{
		fprintf(stdout, "Unable to open serial device:%s\n", strerror(errno));
		return 1;
	}

	if ((fd = open("/dev/spidev0.0", O_RDWR)) < 0)//송,수신 가능모드로 spi장치 열기, spi초기화
		return wiringPiFailure(WPI_ALMOST, "Unable to open SPI device: %s\n", strerror(errno));


	pthread_t p_thread[3];	//스레드 ID저장할 변수 생성
	int thread_id;
	int status;

	thread_id = pthread_create(&p_thread[0], NULL, senser, NULL);//adc값 출력 쓰레드 초기화
	if (thread_id < 0)
	{
		perror("thread create error : "); 						//오류 출력을 위한 함수
		return 0; 												//프로그램 종료함수
	}

	thread_id = pthread_create(&p_thread[1], NULL, servo, NULL);//서보모터 제어 쓰레드 초기화
	if (thread_id < 0)
	{
		perror("thread create error : ");
		return 0;
	}

	thread_id = pthread_create(&p_thread[1], NULL, serial, NULL);//시리얼 통신 쓰레드 초기화
	if (thread_id < 0)
	{
		perror("thread create error : ");
		return 0;
	}

	spiSpeeds[adcChannel] = SPI_SPEED;
	spiFds[adcChannel] = fd;

	pinMode(CS_MCP3208, OUTPUT);//cs신호를 '보내야' 하므로 output 

	Pwm();

  //지정된 쓰레드가 종료할때 까지 기다림 
	pthread_join(p_thread[0], (void**)&status);
	pthread_join(p_thread[1], (void**)&status);
	pthread_join(p_thread[2], (void**)&status);


	return 0;
}