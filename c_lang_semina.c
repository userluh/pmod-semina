#include<stdio.h>//ǥ������� ���̺귯��
#include<ctype.h>//����ó�� ���̺귯��
#include<string.h>//���ڿ� ó�� ���̺귯��

void problome_1(void);
void problome_2(void);
void problome_3(void);
void problome_4(void);
int isLower_Case_string(char* str);//�ҹ��� �Ǻ� ����� �����Լ�, 1�� ������ ���
void rotatematrix(int m, int n, int matrix[][100]);//2���� �迭�� �μ��� �޾� ����� ������ ����� �����Լ�, 2�� ������ ���
int max(int* p, int i, int size);//�ִ밪�� �ε����� ��ȯ�ϴ� ����� �����Լ�, 4�� ������ ���
int min(int* p, int i, int size);//�ּҰ��� �ε����� ��ȯ�ϴ� ����� �����Լ�, 4�� ������ ��� 
void swap(int* a, int* b);//��� ��ȯ ����� �����Լ�, 4�� ������ ���

int main(void)
{
    printf("1 ������ ���ڿ��� ������ ����ϴ� ���α׷�\n");
    printf("2 R,C�� �Է¹޾Ƽ� ����� ���������� ������ ���α׷�\n");
    printf("3.��Ʈ����ũ ���� ���α׷�\n");
    printf("4.����� �Է¹޾� �ִ� �ּ� �������� ������ �ϴ� ���α׷�\n\n");
    printf("���׹�ȣ�� 0���� �Է��ϸ� ����Ǯ�� ���α׷��� ����˴ϴ�\n");
    while (1) {// ���������� �����ϱ� ������ �ݺ���

        int problome_num;
        
        printf("\n\n���Ͻô� ���� ��ȣ�� �Է��Ͻʽÿ�\n");
        scanf("%d", &problome_num);
        
        if (problome_num == 0) break;//��������

        else if (problome_num == 1) problome_1();
        else if (problome_num == 2) problome_2();
        else if (problome_num == 3) problome_3();
        else if (problome_num == 4) problome_4();      
    }
    return 0;
}

void problome_1(void) {
    printf("���� end�� �Է��ϸ� ���α׷��� ����˴ϴ�\n");
    while (1) {//���������� �����ϱ� ������ �ݺ���
        char* input_s_pointer;//������ ������ ����
        char input_s[10000];//�Է¹��� ���ڿ��� ������ ������ �迭 ����
        printf("�и��ϰ� ���� ���ڿ��� �Է��ϼ���\n");
        
        scanf("%s", input_s);
        input_s_pointer = input_s;//�����͸� �迭ó�� �����
        char first_s = input_s_pointer[0];//�Է¹��� ���ڿ��� ù��° ��Ҹ� ���۴ܾ�� ����

        if (strcmp(input_s, "end") == 0) break;//��������

        if ((strlen(input_s_pointer) < 1 || strlen(input_s_pointer) > 10000) || isLowerCaseString(input_s_pointer) == 0) {
            printf("���ǿ� �°� �ٽ� �Է��ϼ���\n\n");
        }//��������

        else { 
            int first_s_k=0, n_first_s_k = 0;// ù��°���ڿ� ����Ƚ�� ī��Ʈ, ù��°���ڿ� �ٸ�Ƚ�� ī��Ʈ
            int count = 0;//�ܾ��� ������ �������� ����
            for (int i = 0; i <= strlen(input_s); i++) {//�迭�� ù��° ��Һ��� ������ ��ұ���

                if (input_s_pointer[i] == first_s) first_s_k++;//i��° ���ڰ� ù��° ���ڿ� ������ ����Ƚ�� ���� 
                else  n_first_s_k++; //�ٸ�Ƚ�� ����
                
                if (first_s_k== n_first_s_k) {//�� Ƚ���� ���ٸ�
                    count++;//�ܾ��� ������ ������Ű��
                    first_s = input_s_pointer[i + 1];//ù��° ���ڴ� ������Ұ� �ȴ�.  'b'anana -> ba 'n'ana
                }
            }

            if (count != 0) printf("�ܾ��� ������ %d �� �Դϴ�\n\n\n", count);//�ܾ �и��� ��� �ܾ��� ������ ����ϰ�
            else printf("�ܾ��� ������ 1�� �Դϴ�\n\n\n");//�и����� ������� �ܾ��� 1��
            
        }
    }
    return 0;
}


void problome_2(void) {
    printf("���� 0,0�� �Է��ϸ� ���α׷��� ����˴ϴ�\n\n\n");
    while (1) {//���������� �����ϱ� ������ �ݺ���
        int R;//��
        int C;//��
        int i = 1;//����� ��
        printf("��(R)�� ��(C)�� ,�� �����Ͽ� �Է��ϼ���\n");
        scanf("%d,%d", &R, &C);
        if (R == 0 && C == 0)break;//��������
        int** matrix_pointer;//���������� ����
        int matrix[100][100];//2���� �迭 ����
        matrix_pointer = matrix;//�����͸� �迭ó�� ���
        
        if (R < 3 || C < 3) {
            printf("�߸��� ������ �Է��ϼ̽��ϴ�. �ٽ� �Է��Ͻʽÿ�\n\n");
        }//��������
        
        else {//R,C�� �������� �������� ��� �����ϱ�
            printf("������ ����Դϴ�(������ �� ���)\n\n\n");
            for (int r = 0; r < R; r++) {//2�� for���� ���� ��� �����
                for (int c = 0; c < C; c++) {
                    matrix[r][c] = i;
                    i++;
                }
            }
            for (int r = 0; r < R; r++) {//2�� ������ ���� ��� ����ϱ�
                for (int c = 0; c < C; c++) {
                    printf("%d\t", matrix[r][c]);
                }
             printf("\n");
            }
            rotatematrix(R, C, matrix_pointer);//��� ������ ����� �����Լ� ȣ��

            printf("\n\n��� �������� ����Դϴ�\n\n\n");//���� �� ��� ���
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
    static int zero_set = 0;//�������� ����
    printf("\n ��Ʈ����ũ ����� �Է��ϼ���\n");
    printf("end 5 �� �ۼ��ϸ� ���α׷��� ����˴ϴ�\n");
    while (1) {//���� ������ �����ϱ� ������ �ݺ�

        char func_name[7] = { '\0' };// ����� ������ �Է¹ޱ� ���� ���ڹ迭 ex) add remove check
        int number = 0;//��ɽ�ų ���ڸ� �Է¹��� ������ ����
        scanf("%s %d", func_name, &number);//�����̽��ٸ� �������� ���ڿ��� ���ڸ� �Է¹޴´�. ex) add 3 --> func_name=add, number=3
        char* func_name_pointer;//������ ������ ����
        func_name_pointer = func_name;//�����͸� �迭ó�� ���
        if (strcmp(func_name_pointer, "end") == 0) break;//��������

        if (number < 1 || number > 20) printf("���ǿ� �°� �ٽ� �Է��Ͻʽÿ�\n");//��������

        else {
            if (strcmp(func_name_pointer, "add") == 0) {//add�� �Է��������     
                zero_set |= (1 << number);//����߰�                                  ex)1<<3      1->1000 1�� �������� 3ĭ �а� �и��κ��� 0���� ä��
            }                                                                          // ������ 0�� 1000�� or�����ϸ� 1000�� �Ǽ� ���տ� 3�� �߰���

            else if (strcmp(func_name_pointer, "remove") == 0) {
                zero_set &= ~(1 << number);//��� ����
            }

            else if (strcmp(func_name_pointer, "check") == 0) {
                if (zero_set & (1 << number)){//��� üũ
                    printf("1\n");
                }
                else {
                    printf("0\n");
                }

            }

            else if (strcmp(func_name_pointer, "toggle") == 0) {
                zero_set ^= (1 << number);//��� ���
            }

            else if (strcmp(func_name_pointer, "all") == 0) {
                zero_set = (1 << 21) - 1;//���� ���� �����
            }

            else if (strcmp(func_name_pointer, "empty") == 0) {
                zero_set = 0;//������ �����
            }         
        }
    }
    return 0;
}

void problome_4(void) {
    printf("���� �迭�� ũ�⸦ 0���� �Է��ϸ� ���α׷��� ����˴ϴ�\n\n");
    while (1) {//���������� ������ ������ �ݺ�
        int array[100];//�Է¹��� �迭�� ������ ������ �迭����
        int size;//�Է¹��� �迭�� ũ�⸦ ������ ������ ��������
        printf("�Է¹��� �迭�� ũ�⸦ �Է��ϼ���\n");
        scanf("%d", &size);

        if (size == 0) break;//��������
        if (size <= 4) {//��������
            printf("�߸��� �Է¹��� �Դϴ�. �ٽ��Է� �Ͻʽÿ�\n\n");
        }
        else {
            printf("�迭�� �Է��ϼ���");
            for (int i = 0; i < size; i++) {
                scanf("%d", &array[i]);
            }

            printf("\n\n�Է¹��� �迭�� ����մϴ�\n\n");

            for (int i = 0; i < size; i++) {
                printf("%d\t", array[i]);
            }

            //������ �ϴ� �κ�
            for (int i = 0; i < size - 1; i += 2) {

                int max_index = max(array, i, size);//�ִ밪 �ε����� max_index�� ����                            1 2 3 4 5 '6'
                swap(&array[i], &array[max_index]);//�迭�� ù��° ��ҿ� �ִ밪�� ��ġ�� �ٲ۴�                  6 2 3 4 5 1
                int min_index = min(array, i, size);//�ּҰ� �ε����� min_index�� ����                            6 2 3 4 5 '1' 
                swap(&array[i + 1], &array[min_index]);//�迭�� �ι�° ��ҿ�  �ּҰ��� ��ġ�� �ٲ۴�.            6 1 3 4 5 2
            }

            printf("\n\n����� �迭�� ����մϴ�\n\n");
            for (int v = 0; v < size; v++) {
                printf("%d\t", array[v]);
            }
            printf("\n\n");
        }
    }
    return 0;
}

int isLowerCaseString(char* str) {//�ҹ��� �Ǻ��Լ�
    int i = 0;//�ε���

    while (str[i] != '\0') {//null���� ������ ������ ����
        if (!islower(str[i])) {
            return 0; // �ҹ��ڰ� �ƴ� ���ڰ� �����ϸ� 0 ��ȯ
        }
        i++;
    }
    return 1; // ��� ���ڰ� �ҹ��ڸ� 1 ��ȯ
}


void rotatematrix(int m, int n, int matrix[][100])// �ళ��,������, 2���� �迭�� �μ��� �޾� ����� ���������� ������ �Լ�
{
    int row = 0, col = 0;//�ʱ� ��,��
    int prev, curr;//�ٲپ�� �ϴ°�, ��ұ�ȯ�� ���� ����

    while (row < m && col < n) {

        if (row + 1 == m || col + 1 == n)//Ż������
            break;

        prev = matrix[row + 1][col];
                                                                                                   //R,C=4�϶� 
                                                                                                   // 1  2  3  4
                                                                                                   //'5' 6  7  8     // 5�� prev
                                                                                                   // 9 10  11 12
                                                                                                   //13 14  15 16



                                                                                                  
        for (int i = col; i < n; i++) {//���� ������                                               // 5  1  2  3
            curr = matrix[row][i];//ù��° �� ����,���� 1~4�� ��ȭ��Ű�� ��ұ�ȯ                  //'4' 6  7  8 
            matrix[row][i] = prev;                                                                 // 9 10  11 12
            prev = curr;                                                                           //13 14  15 16
        }
        row++;


        for (int i = row; i < m; i++) {//������ �� ������                                          // 5  1  2  3              
            curr = matrix[i][n - 1];//������ �� ����, ���� 2~4�� ��ȭ��Ű�� ��ұ�ȯ              //'16' 6  7  4 
            matrix[i][n - 1] = prev;                                                               // 9 10  11 8
            prev = curr;                                                                           //13 14  15 12
        }
        n--;



        if (row < m) {//���� ����� ������ ����                                                   // 5  1  2  3 
            for (int i = n - 1; i >= col; i--) {//�Ʒ��� ������                                  //'13' 6  7  4 
                curr = matrix[m - 1][i];//������ �� ����, ���� 3~1�� ��ȭ��Ű�� ��ұ�ȯ          // 9 10  11 8
                matrix[m - 1][i] = prev;                                                          //14 15  16 12
                prev = curr;
            }
        }
        m--;
 
        if (col < n) {//���� ����� ������ ����                                                  // 5  1  2  3 
            for (int i = m - 1; i >= row; i--) {//���ʺ� ������                                  // 9  6  7  4
                curr = matrix[i][col];//ù��° ���� ����, ���� 3~1�� ��ȭ��Ű�� ��ұ�ȯ         //13 10  11 8
                matrix[i][col] = prev;                                                           //14 15  16 12
                prev = curr;
            }
        }
        col++;
    }
}

int max(int* p, int i, int size) {
    int bigist = i;//�ִ밪 �ε����� i, �����Լ����� for������ �ѹ� ������ i�� 2��ŭ ����
    
    for (int j = i + 1; j < size; j++) {//�׿����� 3��° �ε������� Ž��                   3 4 5 2�߿��� Ž��
        if (p[j] > p[bigist]) bigist = j;//j��° ��Ұ� ��ũ�� �ִ밪 �ε����� j�� ����
    }
    return bigist;//�ε��� ��ȯ
}

int min(int* p, int i, int size) {
    int minist = i;
    
    for (int j = i + 1; j < size; j++) {
        if (p[j] < p[minist]) minist = j;
    }
    return minist;
}

void swap(int* a, int* b) {//�ΰ��� ��� �ٲٱ�,�迭�� �μ��� �޾ұ� ������ ���� ���� ȣ���� ����
    int tmp = *a;
    *a = *b;
    *b = tmp;
}





