#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>//시리얼 통신을 위한 헤더파일
#include <errno.h>//에러 메세지 출력을 위한 헤더파일
#include <string.h>//문자열처리 라이브러리
#include <stdlib.h>//문자열 변환을 위한 라이브러리

//핀번호 설정
#define L1 29
#define L2 25
#define L3 28
#define L4 24
#define L5 27
#define L6 23
#define L7 22
#define L8 26

#define S1 5
#define S2 4
#define S3 3
#define S4 2

//디바운스 방지딜레이
#define Prevention_debounce 200



//LED,스위치 배열생성

unsigned int LED[8] = {L1, L2, L3, L4, L5, L6, L7, L8}; 
unsigned int SW[4] = {S1, S2, S3, S4}; 



int fd = 0;//serialOpen에 의해 반환되는 파일디스크립터 값을 저장하기 위한 변수
char print_arnold[3][25]={"hello!\t","my name is\t","arnold schwarzenegger\n\r"};//1번 문제를 위해 사용되는 문자열 배열
int count_push_sw1=0;//1번 문제에 사용되는 버튼 카운트 변수
int triangleNum[45]={0};//4번 문제에 사용될 삼각수를 저장하는 배열



void setup();//핀모드설정
void off_led();//모든 led끄기
void problome_1(int n);//1번 문제함수, 버튼 카운트 변수가 인수로 들어간다.
void problome_2();//2번 문제함수
void problome_3();//3번 문제함수
void problome_4();//4번 문제함수
void displayBinaryRecursive(int number, int bitIndex);//2번 문제에 사용될 led로 2진수를 나타내는 재귀함수
void print_starcraft(char *buffer);//3번 문제에 사용될 starcraft 문자열 출력하는 함수, 입력받은 문자열(buffer)이 인수로 들어간다.
void initTriangleNum();//4번 문제에 사용될 삼각수 만드는 함수
int isEurekaNumber(int num);//4번 문제에 사용될 유레카넘버 판단함수


int main(void)
{
   if(wiringPiSetup()< 0)//wiringPi 초기화여부 판단                                             
   {
      fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
      return -1;
   }

   if((fd = serialOpen("/dev/ttyAMA1", 9600)) < 0)//시리얼 연결상태 판단                              
   {
      fprintf(stderr, "unable to open serial device: %s\n", strerror(errno));
      return -1;
   }

   setup();//핀모드설정
   off_led();//모든 led끄기

   //putty와 라즈베리파이에 접속성공 출력

   printf("\n Connection to PC was successful. \n");
   serialPuts(fd,"Connection to Raspberry Pi was successful.\n\r");

   while(1)//while문을 돌며 스위치 값을 계속 입력받음
   {
      if(digitalRead(SW[0])==0) 
      {
         delay(Prevention_debounce);
         problome_1(count_push_sw1);//버튼눌린 횟수를 인수로 사용
      }
      else if(digitalRead(SW[1])==0)
      {
         delay(Prevention_debounce);
         problome_2();
      }
      else if(digitalRead(SW[2])==0)
      {
         delay(Prevention_debounce);
         problome_3();
      }

      else if(digitalRead(SW[3])==0){
         delay(Prevention_debounce);
         problome_4();
      }
   }
   return 0;
}

void off_led() {//모든 led 끄기
    for (int i = 0; i < 8; i++) {
        digitalWrite(LED[i], LOW);
    }
}

void setup() {//핀모드 설정
    for (int i = 0; i < 8; i++) {
        pinMode(LED[i], OUTPUT); 
    }

    for (int i = 0; i < 4; i++) {
        pinMode(SW[i], INPUT); 
    }
}

void problome_1(int n){
   count_push_sw1++;//누른횟수 증가
   if(count_push_sw1==4) count_push_sw1=1;//카운트가 3번이상 될경우 1로 초기화 시킨다
   serialPuts(fd,print_arnold[n-1]);//문자열 배열의 인덱스에 맞는 문자열을 출력한다.
}

void problome_2(){
   char data=0;//문자를 입력받을 변수
   char buffer[100]="";//버퍼 역할을 해줄 배열
   int process_data=0;//입력완료 판단 플래그비트 1:입력완료 0입력중
   int index=0;

   serialPuts(fd,"2번 입력대기 모드\t");
   serialPuts(fd,"숫자를 입력하세요:\t");

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
            int number=atoi(buffer);//입력했던 문자열 숫자를 정수형으로 바꿈
            displayBinaryRecursive(number,7);//재귀함수를 호출해서 number의 2진수 표현을 led로 나타낸다.
            delay(2000);
            off_led();
            serialPuts(fd,"2번 프로그램이 종료 되었습니다.\n\r");
            break;
         }
      }
   }
}

void displayBinaryRecursive(int number, int bitIndex) {
  if (bitIndex >= 0) {
    int bit = (number >> bitIndex) & 1; //number=00000101 number>>7=00000000  number>>7&1=0    
    digitalWrite(LED[7-bitIndex], bit); //첫번째 led는 켜지지않음
    displayBinaryRecursive(number, bitIndex - 1);//재귀시켜서 다음자리 led 컨트롤
  }
}

void problome_3(){
   char data=0;
   char buffer[100]="";
   int process_data=0;
   int index=0;
   
   serialPuts(fd,"3번 입력대기 모드\t");
   serialPuts(fd,"문자열을 입력하세요:\t");

   while(1){
      if(serialDataAvail(fd)>0){
         
         data=serialGetchar(fd);
         serialPutchar(fd,data);
      
         if(data=='\r'){
            buffer[index]='\0';
            process_data=1;
            serialPuts(fd,"\n\r");
         }
         else{
            buffer[index]=data;
            index++;
         }
      
         if(process_data==1){
            print_starcraft(buffer);//입력받은 문자열의 주소를 함수의 인수로 전달
            break;
         }
      }
   }
}

void print_starcraft(char*buffer){

   char starcraft[10]={0};//입력한 문자열(buffer)중에서 starcraft만 추출해서 저장하기 위한 문자배열
   
   for(int i=0; i<9; i++){
      starcraft[i]=buffer[i+7];//starcraft 부분만 추출해서 저장
   }
   printf("\nfound:%s\n",starcraft);//라즈베리 창에 starcraft 출력
   serialPuts(fd,starcraft);//putty에 starcraft 출력
   serialPuts(fd,"\n\r3번 프로그램이 종료 되었습니다.\n\r");
}

void problome_4(){

    serialPuts(fd,"4번 입력대기 모드\t");
    serialPuts(fd,"몇개의 수를 테스트할지 입력하세요:\t");

    char data=0;
    char buffer[100]={0};
    int process_data=0;
    int index=0;
    int flag=0;//테스트할 숫자의 개수를 입력했는지 판단하는 플래그 비트
    int amount_input=0;//테스트할 숫자의 개수를 저장하는 변수
    int count_input=0;//테스트할 숫자 개수 카운트
    
    
    
    initTriangleNum();//삼각수 만들어놓기

    while(1){

      if(serialDataAvail(fd)>0){

         data=serialGetchar(fd);
         serialPutchar(fd,data);
      
         if(data=='\r'){
            buffer[index]='\0';
            process_data=1;
            serialPuts(fd,"\n\r");
         }

         else{
            buffer[index]=data;
            index++;
         }
      
         if(process_data==1){
            
            if(flag==0){//처음 숫자를 입력했을때, 테스트할 숫자의 개수를 입력받는 부분
               flag=1;//테스트할 숫자의 개수를 입력한것으로 처리
               amount_input=atoi(buffer);//테스트할 숫자의 개수를 저장
            }
            else if(flag==1){//테스트할 숫자의 개수가 입력되었을때
               int num=atoi(buffer);
               if(isEurekaNumber(num)>0) serialPuts(fd,"1\n\r");//입력한 숫자가 삼각수이면 putty에 1출력
               else serialPuts(fd,"0\n\r");//그렇지 않다면 putty에 0출력
               count_input++;//입력한 숫자의 개수 카운트 
            }
            
            //초기화 해야할 변수들

            process_data=0;
            index=0;
            memset(buffer, 0, sizeof(buffer));//저장했던 모든 문자열을 삭제한다.

            if(count_input==amount_input){//입력한 수의 개수가 테스트할 숫자개수와 같아지면 종료
               serialPuts(fd,"4번 프로그램이 종료 되었습니다.\n\r");
               break;
            }

         }
         
      }
    
   }
}

void initTriangleNum() {//삼각수 만드는 함수
    for (int i = 1; i < 45; i++) {
        triangleNum[i - 1] = (i * i + i) / 2;
    }
}

int isEurekaNumber(int num) {//유레카 넘버 판단함수
    
    for (int a = 0; a < 44; a++) {//44*44*44가지의 경우를 모두 따짐
        for (int b = 0; b < 44; b++) {
            for (int c = 0; c < 44; c++) {
                if (triangleNum[a] + triangleNum[b] + triangleNum[c] == num) {//3개의 삼각수로 표현이 되면
                    return 1;//1반환
                }
            }
        }
    }
    return 0;//그렇지 않으면 0반환
   
}


