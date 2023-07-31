#include<stdio.h>//표준입출력 라이브러리
#include<ctype.h>//문자처리 라이브러리
#include<string.h>//문자열 처리 라이브러리

void problome_1(void);
void problome_2(void);
void problome_3(void);
void problome_4(void);
int isLower_Case_string(char* str);//소문자 판별 사용자 정의함수, 1번 문제에 사용
void rotatematrix(int m, int n, int matrix[][100]);//2차원 배열을 인수로 받아 행렬을 돌리는 사용자 정의함수, 2번 문제에 사용
int max(int* p, int i, int size);//최대값의 인덱스를 반환하는 사용자 정의함수, 4번 문제에 사용
int min(int* p, int i, int size);//최소값의 인덱스를 반환하는 사용자 정의함수, 4번 문제에 사용 
void swap(int* a, int* b);//요소 교환 사용자 정의함수, 4번 문제에 사용

int main(void)
{
    printf("1 분해한 문자열의 개수를 출력하는 프로그램\n");
    printf("2 R,C를 입력받아서 행렬을 오른쪽으로 돌리는 프로그램\n");
    printf("3.비트마스크 연산 프로그램\n");
    printf("4.행렬을 입력받아 최대 최소 형식으로 재정렬 하는 프로그램\n\n");
    printf("문항번호를 0으로 입력하면 문제풀이 프로그램이 종료됩니다\n");
    while (1) {// 종료조건을 만족하기 전까지 반복함

        int problome_num;
        
        printf("\n\n원하시는 문제 번호를 입력하십시오\n");
        scanf("%d", &problome_num);
        
        if (problome_num == 0) break;//종료조건

        else if (problome_num == 1) problome_1();
        else if (problome_num == 2) problome_2();
        else if (problome_num == 3) problome_3();
        else if (problome_num == 4) problome_4();      
    }
    return 0;
}

void problome_1(void) {
    printf("만약 end를 입력하면 프로그램이 종료됩니다\n");
    while (1) {//종료조건을 만족하기 전까지 반복함
        char* input_s_pointer;//문자형 포인터 선언
        char input_s[10000];//입력받은 문자열을 저장할 문자형 배열 선언
        printf("분리하고 싶은 문자열을 입력하세요\n");
        
        scanf("%s", input_s);
        input_s_pointer = input_s;//포인터를 배열처럼 사용함
        char first_s = input_s_pointer[0];//입력받은 문자열의 첫번째 요소를 시작단어로 설정

        if (strcmp(input_s, "end") == 0) break;//종료조건

        if ((strlen(input_s_pointer) < 1 || strlen(input_s_pointer) > 10000) || isLowerCaseString(input_s_pointer) == 0) {
            printf("조건에 맞게 다시 입력하세요\n\n");
        }//예외조건

        else { 
            int first_s_k=0, n_first_s_k = 0;// 첫번째문자와 같은횟수 카운트, 첫번째문자와 다른횟수 카운트
            int count = 0;//단어의 개수를 세기위한 변수
            for (int i = 0; i <= strlen(input_s); i++) {//배열의 첫번째 요소부터 마지막 요소까지

                if (input_s_pointer[i] == first_s) first_s_k++;//i번째 문자가 첫번째 문자와 같으면 같은횟수 증가 
                else  n_first_s_k++; //다른횟수 증가
                
                if (first_s_k== n_first_s_k) {//두 횟수가 같다면
                    count++;//단어의 개수를 증가시키고
                    first_s = input_s_pointer[i + 1];//첫번째 문자는 다음요소가 된다.  'b'anana -> ba 'n'ana
                }
            }

            if (count != 0) printf("단어의 개수는 %d 개 입니다\n\n\n", count);//단어가 분리된 경우 단어의 개수를 출력하고
            else printf("단어의 개수는 1개 입니다\n\n\n");//분리되지 않은경우 단어의 1개
            
        }
    }
    return 0;
}


void problome_2(void) {
    printf("만약 0,0을 입력하면 프로그램이 종료됩니다\n\n\n");
    while (1) {//종료조건을 만족하기 전까지 반복함
        int R;//행
        int C;//열
        int i = 1;//행렬의 값
        printf("행(R)과 열(C)을 ,로 구분하여 입력하세요\n");
        scanf("%d,%d", &R, &C);
        if (R == 0 && C == 0)break;//종료조건
        int** matrix_pointer;//이중포인터 생성
        int matrix[100][100];//2차원 배열 생성
        matrix_pointer = matrix;//포인터를 배열처럼 사용
        
        if (R < 3 || C < 3) {
            printf("잘못된 범위를 입력하셨습니다. 다시 입력하십시오\n\n");
        }//예외조건
        
        else {//R,C를 바탕으로 돌리기전 행렬 생성하기
            printf("생성된 행렬입니다(돌리기 전 행렬)\n\n\n");
            for (int r = 0; r < R; r++) {//2중 for문을 통해 행렬 만들기
                for (int c = 0; c < C; c++) {
                    matrix[r][c] = i;
                    i++;
                }
            }
            for (int r = 0; r < R; r++) {//2중 포문을 통해 행렬 출력하기
                for (int c = 0; c < C; c++) {
                    printf("%d\t", matrix[r][c]);
                }
             printf("\n");
            }
            rotatematrix(R, C, matrix_pointer);//행렬 돌리는 사용자 정의함수 호출

            printf("\n\n행렬 돌리기의 결과입니다\n\n\n");//돌린 후 결과 출력
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < C; c++) {
                    printf("%d\t", matrix[r][c]);
                }
                printf("\n");
            }
        }
    }
    return 0;
}

void problome_3(void) {
    static int zero_set = 0;//공집합을 생성
    printf("\n 비트마스크 명령을 입력하세요\n");
    printf("end 5 를 작성하면 프로그램이 종료됩니다\n");
    while (1) {//종료 조건을 만족하기 전까지 반복

        char func_name[7] = { '\0' };// 명령의 종류를 입력받기 위한 문자배열 ex) add remove check
        int number = 0;//명령시킬 숫자를 입력받을 정수형 변수
        scanf("%s %d", func_name, &number);//스페이스바를 기준으로 문자열과 숫자를 입력받는다. ex) add 3 --> func_name=add, number=3
        char* func_name_pointer;//정수형 포인터 선언
        func_name_pointer = func_name;//포인터를 배열처럼 사용
        if (strcmp(func_name_pointer, "end") == 0) break;//종료조건

        if (number < 1 || number > 20) printf("조건에 맞게 다시 입력하십시오\n");//예외조건

        else {
            if (strcmp(func_name_pointer, "add") == 0) {//add를 입력했을경우     
                zero_set |= (1 << number);//요소추가                                  ex)1<<3      1->1000 1을 왼쪽으로 3칸 밀고 밀린부분은 0으로 채움
            }                                                                          // 공집합 0과 1000을 or연산하면 1000이 되서 집합에 3이 추가됨

            else if (strcmp(func_name_pointer, "remove") == 0) {
                zero_set &= ~(1 << number);//요소 삭제
            }

            else if (strcmp(func_name_pointer, "check") == 0) {
                if (zero_set & (1 << number)){//요소 체크
                    printf("1\n");
                }
                else {
                    printf("0\n");
                }

            }

            else if (strcmp(func_name_pointer, "toggle") == 0) {
                zero_set ^= (1 << number);//요소 토글
            }

            else if (strcmp(func_name_pointer, "all") == 0) {
                zero_set = (1 << 21) - 1;//꽉찬 집합 만들기
            }

            else if (strcmp(func_name_pointer, "empty") == 0) {
                zero_set = 0;//공집합 만들기
            }         
        }
    }
    return 0;
}

void problome_4(void) {
    printf("만약 배열의 크기를 0으로 입력하면 프로그램이 종료됩니다\n\n");
    while (1) {//종료조건을 만나기 전까지 반복
        int array[100];//입력받은 배열을 저장할 정수형 배열선언
        int size;//입력받을 배열의 크기를 저장할 정수형 변수선언
        printf("입력받을 배열의 크기를 입력하세요\n");
        scanf("%d", &size);

        if (size == 0) break;//종료조건
        if (size <= 4) {//예외조건
            printf("잘못된 입력범위 입니다. 다시입력 하십시오\n\n");
        }
        else {
            printf("배열을 입력하세요");
            for (int i = 0; i < size; i++) {
                scanf("%d", &array[i]);
            }

            printf("\n\n입력받은 배열을 출력합니다\n\n");

            for (int i = 0; i < size; i++) {
                printf("%d\t", array[i]);
            }

            //재정렬 하는 부분
            for (int i = 0; i < size - 1; i += 2) {

                int max_index = max(array, i, size);//최대값 인덱스를 max_index에 저장                            1 2 3 4 5 '6'
                swap(&array[i], &array[max_index]);//배열의 첫번째 요소와 최대값의 위치를 바꾼다                  6 2 3 4 5 1
                int min_index = min(array, i, size);//최소값 인덱스를 min_index에 저장                            6 2 3 4 5 '1' 
                swap(&array[i + 1], &array[min_index]);//배열의 두번째 요소와  최소값의 위치를 바꾼다.            6 1 3 4 5 2
            }

            printf("\n\n변경된 배열을 출력합니다\n\n");
            for (int v = 0; v < size; v++) {
                printf("%d\t", array[v]);
            }
            printf("\n\n");
        }
    }
    return 0;
}

int isLowerCaseString(char* str) {//소문자 판별함수
    int i = 0;//인덱스

    while (str[i] != '\0') {//null문자 만나기 전까지 수행
        if (!islower(str[i])) {
            return 0; // 소문자가 아닌 문자가 존재하면 0 반환
        }
        i++;
    }
    return 1; // 모든 문자가 소문자면 1 반환
}


void rotatematrix(int m, int n, int matrix[][100])// 행개수,열개수, 2차원 배열을 인수로 받아 행렬을 오른쪽으로 돌리는 함수
{
    int row = 0, col = 0;//초기 행,열
    int prev, curr;//바꾸어야 하는값, 요소교환을 위한 변수

    while (row < m && col < n) {

        if (row + 1 == m || col + 1 == n)//탈출조건
            break;

        prev = matrix[row + 1][col];
                                                                                                   //R,C=4일때 
                                                                                                   // 1  2  3  4
                                                                                                   //'5' 6  7  8     // 5가 prev
                                                                                                   // 9 10  11 12
                                                                                                   //13 14  15 16



                                                                                                  
        for (int i = col; i < n; i++) {//윗변 돌리기                                               // 5  1  2  3
            curr = matrix[row][i];//첫번째 행 고정,열을 1~4로 변화시키며 요소교환                  //'4' 6  7  8 
            matrix[row][i] = prev;                                                                 // 9 10  11 12
            prev = curr;                                                                           //13 14  15 16
        }
        row++;


        for (int i = row; i < m; i++) {//오른쪽 변 돌리기                                          // 5  1  2  3              
            curr = matrix[i][n - 1];//마지막 열 고정, 행을 2~4로 변화시키며 요소교환              //'16' 6  7  4 
            matrix[i][n - 1] = prev;                                                               // 9 10  11 8
            prev = curr;                                                                           //13 14  15 12
        }
        n--;



        if (row < m) {//내측 행렬을 돌릴때 조건                                                   // 5  1  2  3 
            for (int i = n - 1; i >= col; i--) {//아랫변 돌리기                                  //'13' 6  7  4 
                curr = matrix[m - 1][i];//마지막 행 고정, 열을 3~1로 변화시키며 요소교환          // 9 10  11 8
                matrix[m - 1][i] = prev;                                                          //14 15  16 12
                prev = curr;
            }
        }
        m--;
 
        if (col < n) {//내측 행렬을 돌릴때 조건                                                  // 5  1  2  3 
            for (int i = m - 1; i >= row; i--) {//왼쪽변 돌리기                                  // 9  6  7  4
                curr = matrix[i][col];//첫번째 열을 고정, 행을 3~1로 변화시키며 요소교환         //13 10  11 8
                matrix[i][col] = prev;                                                           //14 15  16 12
                prev = curr;
            }
        }
        col++;
    }
}

int max(int* p, int i, int size) {
    int bigist = i;//최대값 인덱스는 i, 메인함수에서 for루프를 한번 돌고난후 i를 2만큼 증가
    
    for (int j = i + 1; j < size; j++) {//그에따라 3번째 인덱스부터 탐색                   3 4 5 2중에서 탐색
        if (p[j] > p[bigist]) bigist = j;//j번째 요소가 더크면 최대값 인덱스를 j로 설정
    }
    return bigist;//인덱스 반환
}

int min(int* p, int i, int size) {
    int minist = i;
    
    for (int j = i + 1; j < size; j++) {
        if (p[j] < p[minist]) minist = j;
    }
    return minist;
}

void swap(int* a, int* b) {//두개의 요소 바꾸기,배열을 인수로 받았기 때문에 값에 의한 호출이 실현
    int tmp = *a;
    *a = *b;
    *b = tmp;
}





