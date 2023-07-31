#include <wiringPi.h>
#include <stdio.h>

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


unsigned int LED[8] = {L1, L2, L3, L4, L5, L6, L7, L8}; // led 배열생성

unsigned int SW[4] = {S1, S2, S3, S4}; // 스위치 배열 생성

int Toggle1 = 0; // 1번 문제 상태변수 (0:정지 1:동작 )
int Toggle3 = 0; // 3번 문제 (0:정지 1:동작 )
int LED_number = 0; //led에서 표현하고 있는 10진수 (1~255) 

int delayTime = 1000; // 초기 딜레이 1초

void off_led();//모든 led를 끄는 함수
void setup();//입출력 모드 설정하는 함수
void Button_Control1();//1번 스위치의 상태를 컨트롤 하는 함수
void Button_Control3();//3번 스위치의 상태를 컨트롤 하는 함수
void displayBinaryRecursive(int number, int bitIndex);//led로 2진수를 나타내는 재귀함수, number: 나타내고 싶은 10진수 ,bitIndex: led 인덱스 개수  
void LED_UP_COUNT();//2진수 업카운트 함수(1번)
void LED_DOWN_COUNT();//2진수 다운카운트 함수(3번)
void LEDStop();//led정지 함수
void Button1();//1번 문제
void Button2();//2번 문제
void Button3();//3번 문제

int main(void) {
    if (wiringPiSetup() < 0) { // wiringPi 라이브러리 초기화
        return -1; // 초기화 과정에서 오류가 나오면 -1로 반환
    }

    setup();//핀모드 설정
    off_led();//컴파일 시킬때 마다 모든 led 끄기

    while (1) {//버튼함수 안에는 각 버튼의 상태를 입력받는 함수가 존재, main문에서 while문을 돌며 1,2,3번에 대한 버튼입력을 자유롭게 받을 수 있음
        Button1(); // 버튼 1번 상태 제어
        Button2(); // 버튼 2번 상태 제어
        Button3(); // 버튼 3번 상태 제어
    }

    return 0;
}

void off_led() {//모든 led 끄기
    for (int i = 0; i < 8; i++) {
        digitalWrite(LED[i], LOW);
    }
}

void setup() { // 입출력 모드 설정
    for (int i = 0; i < 8; i++) {
        pinMode(LED[i], OUTPUT); 
    }

    for (int i = 0; i < 4; i++) {
        pinMode(SW[i], INPUT); 
    }
}

void Button_Control1() {
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
    Button_Control1(); // 1번 버튼 상태를 컨트롤

    if (Toggle1 == 1) {
        delay(200);//디바운싱 방지 딜레이
        Toggle3=0;//업카운팅,다운카운팅의 충돌을 막기위함
        LED_UP_COUNT(); // 업카운팅
    } else {//토글이 0이라면 
        LEDStop(); // LED 정지
    }
}

void LED_UP_COUNT() {//2진수 업 카운트
    displayBinaryRecursive(LED_number, 7);//표현하고 싶은 수를 재귀함수에 넣어서 호출함
    LED_number = (LED_number + 1) % 256;//업카운팅 이므로 인덱스 증가
    delay(delayTime);
}

void displayBinaryRecursive(int number, int bitIndex) {
  if (bitIndex >= 0) {//재귀하기 위한 조건 
    int bit = (number >> bitIndex) & 1; //표현하고 싶은수를 오른쪽으로 비트 인덱스 만큼 시프트 연산후, 1과 and연산해서 bit에 저장 
    digitalWrite(LED[bitIndex], bit); // 비트 값을 LED에 출력
    displayBinaryRecursive(number, bitIndex - 1); // 다음 비트로 재귀 호출
  }
}

void LEDStop() {// LED 현재 위치에서 정지
    while(1){//동작을 중지하기 위한 무한루프
        Button_Control1();//1번 버튼의 상태를 입력받음
        Button_Control3();//3번 버튼의 상태를 입력받음
        if(Toggle1==1||Toggle3==1) break;//1번 or 3번 에서의 탈출조건, 무한루프 안에서 버튼이 한번더 눌리면 탈출
    }
}

void Button2(void) {
    static int SW_2 = HIGH; // 2번 버튼 초기 상태 설정
    static int pro_SW_2 = HIGH; // 이전 2번 버튼 초기 상태 설정

    SW_2 = digitalRead(SW[1]); // 2번 버튼 상태 읽기

    if (SW_2 == LOW && pro_SW_2 == HIGH) {//현재스위치 상태가 1이고 이전스위치 상태가 0이면
        delay(200);//디바운싱 방지 딜레이
        delayTime += 2000; // 딜레이 시간을 2초씩 증가시킴
        if (delayTime > 5000) {//딜레이가 5초 이상이 될경우
            delayTime = 1000; // 다시 1초로 초기화
        }
    }

    pro_SW_2 = SW_2; // 이전 2번 버튼 상태 업데이트
}


void Button_Control3() {
    static int SW_3 = HIGH; //3번 버튼 초기 상태 설정
    static int pro_SW_3 = HIGH; // 이전 3번 버튼 초기 상태 설정

    SW_3 = digitalRead(SW[2]); //3번 버튼 입력받기

    if (SW_3 == LOW && pro_SW_3 == HIGH) {//현재 스위치 상태가 1이고 이전 스위치 상태가 0이면
        delay(200);//디바운싱 방지 딜레이
        Toggle3 ^= 1; //  버튼을 눌렸을 때마다 상태반전
    }

    pro_SW_3 = SW_3; //현재 버튼 상태를 이전 버튼 상태에 저장
}

void Button3(void) {
    Button_Control3(); // 3번 버튼상태 컨트롤

    if (Toggle3 == 1) 
    {
      delay(200);//디바운싱 방지 딜레이
      Toggle1=0;//업카운팅,다운카운팅의 충돌을 막기위함
      LED_DOWN_COUNT(); // 다운카운팅
    } else {//토글이 0이라면 led정지
        LEDStop();// LED 정지
    }
}

void LED_DOWN_COUNT() {//2진수 다운 카운트
    displayBinaryRecursive(LED_number, 7);
    LED_number = (LED_number - 1) % 256;//다운 카운팅 이므로 인덱스 감소 
    delay(delayTime);
}





