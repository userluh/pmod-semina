#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>//소프트웨어 pwm을 사용하기 위한 헤더파일
#include <stdlib.h>//atoi()함수 사용을 위한 헤더파일
#include <wiringSerial.h>//시리얼 통신을 위한 헤더파일
#include <string.h>//문자열처리 라이브러리
#include <queue>//큐를 사용하기 위한 헤더파일
using namespace std;//큐를 사용하기 위한 namespace선언

#define LED_pwm 1//led밝기제어를 위한 hardware pwm
#define DC_pwm 25//모터의 속도제어를 위한 software pwm
#define DC_IN1 28//모터 방향제어핀
#define DC_IN2 27//모터 방향제어핀 
//스위치 핀설정
#define S1 5
#define S2 4
#define S3 3
#define S4 2
#define Prevention_debounce 200//디바운스 방지를 위한 딜레이
#define DC_RPM 300//DC모터의 최대RPM

unsigned int SW[4] = {S1, S2, S3, S4};//스위치 배열 
int Toggle1 = 0; // 1번 문제 상태변수 (0:정지 1:동작 )
int DC_speed=0;//DC모터의 속도
//모터의 방향제어핀, 01이면 CCW, 10이면 CW
int DC_direction_1=0;
int DC_direction_2=1;
int fd=0;//파일 디스크립터
char  print_DC_speed[100]="";//모터의 rpm을 putty에 출력하기 위한 문자열

void set_up();//셋업
void Button1();//1번문제
void Button_Control1();//1번 스위치의 상태를 컨트롤 하는 함수
void LED_pwm_control();//led밝기 제어
void Button2();//2번 문제
void DC_CCW();//모터 ccw회전, 3번 버튼으로 방향전환 가능
void Button3();//3번문제, 방향전환
void Button4();//4번문제
void problome4(int N);//카드문제

int main(void) {
    if (wiringPiSetup() < 0) { // wiringPi 라이브러리 초기화
        return -1; 
    }

    if((fd = serialOpen("/dev/ttyAMA1", 9600)) < 0)//시리얼 연결상태 판단                              
    {
      return -1;
    }
    set_up();//셋업

   //연결성공 출력
    printf("\n Connection to PC was successful. \n");
    serialPuts(fd,"Connection to Raspberry Pi was successful.\n\r");

    while (1) {
        Button1(); 
        Button2();
        Button3();
        Button4();
      }
    return 0;
}

void set_up(){//입출력 핀설정,pwm 범위설정
    for (int i = 0; i < 4; i++) {
        pinMode(SW[i], INPUT); 
    }

   pinMode(DC_pwm,PWM_OUTPUT); 
   pinMode(DC_IN1,OUTPUT);
   pinMode(DC_IN2,OUTPUT);
   pinMode(LED_pwm,PWM_OUTPUT);
   pwmSetRange(500);
   softPwmCreate(DC_pwm,1,DC_RPM);
   }

void Button_Control1() {//1번 버튼 컨트롤 함수
    static int SW_1 = HIGH; // 1번 버튼 초기 상태 설정
    static int pro_SW_1 = HIGH; // 이전 1번 버튼 초기 상태 설정

    SW_1 = digitalRead(SW[0]); //1번 버튼 입력받기

    if (SW_1 == LOW && pro_SW_1 == HIGH) {//현재 스위치 상태가 1이고 이전 스위치 상태가 0이면
        delay(200); // 디바운싱 방지 딜레이 
        Toggle1 ^= 1; // 버튼을 눌렸을 때마다 상태반전
    }

    pro_SW_1 = SW_1; // 현재 버튼 상태를 이전 버튼 상태에 저장
}

void Button1(void) {
    Button_Control1(); 

    if (Toggle1 == 1){//토글이 1이면
        LED_pwm_control(); 
    } else {//토글이 0이면
        pwmWrite(LED_pwm,0);//모터정지
    }
}

void LED_pwm_control(void){
   
   for(int i=0; i<501; i++){//1.5초 주기로 밝아짐
      pwmWrite(LED_pwm,i);
      delay(3);
      if(digitalRead(SW[1])==0||digitalRead(SW[2])==0||digitalRead(SW[3])==0) Toggle1=0;//다른 버튼입력이 들어오면 led를 끔
    } 
   
    for(int i=500; i>=0; i--){//1.5초 주기로 어두워짐
      pwmWrite(LED_pwm,i);
      delay(3);
      if(digitalRead(SW[1])==0||digitalRead(SW[2])==0||digitalRead(SW[3])==0) Toggle1=0;//다른 버튼입력이 들어오면 led를 끔
    }
}

void Button2(){
   DC_CCW();//DC모터 CCW회전,초기 속도값은 0으로, 정지해있음
   if(digitalRead(SW[1])==0){//입력 대기모드 
   //delay(Prevention_debounce);//디바운스 방지
   softPwmWrite(DC_pwm,0);//입력대기 모드일때는 모터정지
   char data=0;//문자를 입력받을 변수
   char buffer[100]="";//버퍼 역할을 해줄 배열
   int process_data=0;//입력완료 판단 플래그비트 1:입력완료 0입력중
   int index=0;

   serialPuts(fd,"2번 입력대기 모드\t");
   serialPuts(fd,"RPM을 입력하세요:\t");

   while(1){//무한 루프를 돌며
      if(serialDataAvail(fd)>0){//putty에서 문자를 입력하기를 기다리고, 계속 문자를 받음
         
         data=serialGetchar(fd);//putty에서 하나의 문자를 받아서 문자형 변수 data에 저장
         serialPutchar(fd,data);//입력한 문자를 putty에 출력
      
         if(data=='\r'){//putty에서 엔터를 쳤을시
            buffer[index]='\0';//현재 입력을 문자열의 끝을 알리는 NULL문자로 설정
            process_data=1;//플래그비트를 1로 만듬
            serialPuts(fd,"\n\r");
         }
         else{//입력받은 문자가 엔터가 아니라면 버퍼에 계속 저장
            buffer[index]=data;
            index++;
         }
      
         if(process_data==1){//입력이 완료됬으면
            //예외처리
            if(atoi(buffer)<0||atoi(buffer)>300){
                serialPuts(fd,"버튼을 누르고 다시 입력하세요\n\r");
                DC_speed=0;
                break;
            }
            strcpy(print_DC_speed,buffer);//1초당 현재 RPM을 출력하기 위해 print_dc_speed문자열에 입력한 PRM값을 복사
            DC_speed=atoi(buffer);//입력했던 RPM을 숫자로 바꿔서 속도에 저장
            break;
         }
      }
   }
}
}

void DC_CCW(){
   //방향제어
   digitalWrite(DC_IN1,DC_direction_1);
   digitalWrite(DC_IN2,DC_direction_2);

   softPwmWrite(DC_pwm,DC_speed);//속도제어
   if(DC_speed>0){//1초마다 현재 RPM값 출력
   delay(1000);
   serialPuts(fd,print_DC_speed);
   serialPuts(fd,"\n\r");
   }
}

void Button3(){
    if(digitalRead(SW[2])==0){//스위치 눌릴때 마다
       // delay(Prevention_debounce);
        //방향전환
        DC_direction_1=!DC_direction_1;
        DC_direction_2=!DC_direction_2;
    }
}

void Button4(){
   if(digitalRead(SW[3])==0){
   //delay(Prevention_debounce);
   //모터정지
   softPwmWrite(DC_pwm,0);
   DC_speed=0;
   char data=0;//문자를 입력받을 변수
   char buffer[100]="";//버퍼 역할을 해줄 배열
   int process_data=0;//입력완료 판단 플래그비트 1:입력완료 0입력중
   int index=0;

   serialPuts(fd,"4번 입력대기 모드\t");
   serialPuts(fd,"카드의 개수를 입력하세요:\t");

   while(1){//무한 루프를 돌며
      if(serialDataAvail(fd)>0){//putty에서 문자를 입력하기를 기다리고, 계속 문자를 받음
         
         data=serialGetchar(fd);//putty에서 하나의 문자를 받아서 문자형 변수 data에 저장
         serialPutchar(fd,data);//입력한 문자를 putty에 출력
      
         if(data=='\r'){//putty에서 엔터를 쳤을시
            buffer[index]='\0';//현재 입력을 문자열의 끝을 알리는 NULL문자로 설정
            process_data=1;//플래그비트를 1로 만듬
            serialPuts(fd,"\n\r");
         }
         else{//입력받은 문자가 엔터가 아니라면 버퍼에 계속 저장
            buffer[index]=data;
            index++;
         }
      
         if(process_data==1){//입력이 완료됬으면
            //예외처리
            if(atoi(buffer)<1||atoi(buffer)>500000){
                serialPuts(fd,"버튼을 누르고 다시 입력하세요\n\r");
                break;
            }
            int N=atoi(buffer);//입력한 숫자를 정수형으로 변환해서
            problome4(N);//카드 함수에 대입
            break;
         }
      }
   }
}
}


void problome4(int N){
   
   char remaining_cards[10]="";//PUTTY에 정답을 출력하기 위한 문자열 배열
	queue <int> q;//큐 정수형 객체 선언

	for (int i = 0; i < N; i++) {
		q.push(i + 1);//숫자를 차례대로 집어넣음
	}


	while (q.size() != 1) {// 큐에 마지막 원소 하나 남을때까지 아래 작업 반복

		q.pop(); // 1. 맨 위 원소를 삭제              
		q.push(q.front());// 2. 다음 위에 있는 원소를 가장 뒤에 대입
		q.pop();// 3. 앞에 있는 원소 삭제(2, 3을 통해 원소가 뒤로 이동한 꼴)            
	}
   //과정을 마치고나면 남는카드는 q.front()
   
   sprintf(remaining_cards,"%d",q.front());//sprintf 함수를 통해 정답을 문자열로 변환
	serialPuts(fd,remaining_cards);//PUTTY에 정답출력
   serialPuts(fd,"\n\r");
}

