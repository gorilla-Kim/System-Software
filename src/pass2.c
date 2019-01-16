#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stddef.h>
#include "struct.h"

extern int LOCCTR;						//위치카운터
extern int Optab_counter;				//opcode 카운터
extern int Symtab_counter;				//심볼테이블 카운터
extern int flag;						//몇 번째 op테이블에 있는지 확인해줄 변수
extern int start;						//프로그램 시작주소를 저장하는 변수
extern int Literal_count;				// 리터럴 테이블 카운터
extern int counter;

struct OPTAB{							//OPTAB 테이블s
    char op_name[8];					// opcode 이름
    int op_code;						// 코드 값
} Optab[] = {{"START",0x00},{"JSUB",0x48},{"COMP",0x28},{"JEQ",0x30},{"LDL",0x08},{"RSUB",0x4c},{"LDA",0x00},{"STA",0x0c},{"STL",0x14},{"J",0x3c},{"RD",0xd8},{"LDX",0x04},{"TD",0xe0},{"STCH",0x54},{"JLT",0x38},{"STX",0x10}, {"TIX",0x2c},{"LDCH",0x50},{"WD",0xdc}};


int opcode_location(char operation[]){				// Operation의 코드번호와 일치하면 해당 Operation 코드 counter 반환
	int i,k,o;
	for(i = 0 ; i < Optab_counter; i++ ){
		k = strlen(Optab[i].op_name);
		o = strlen(operation);
		if(k==o){
		if(!strcmp(Optab[i].op_name,operation))		// 비교대상과 optab에 있는 내용이 같으면
			break;									//break
		}
	}
	return i;
}

int length_obcode(int l){		// 목적코드를 생성할 때 한줄에 1~10개의 명령어가 들어감 이때 총 명령어의 길이를 계산함
	int length = 0,k, i;
	char buf[8];
	int cnt = 0;
	int len;
	for(i = l;i <l+10; i++){
		k = strlen((DATA[i].Operation));
		
		if( !strncmp(DATA[i].Operation,"RESW",k)|| !strncmp(DATA[i].Operation,"RESB",k)){}	//Resw, resb의 경우 길이에 포함 되지 않음
			
		else if(!strncmp(DATA[i].Operation,"BYTE",k)){	
			if(strstr(DATA[i].Operand,"C'")!= NULL){
				len = strlen(DATA[i].Operand) - 3;										// ---의 길이를 구하고
				strncpy(buf, DATA[i].Operand + 2,len);	
				for(int i=0; buf[i] != '\0'; i++){
					cnt++;
				}
				length = length + cnt -1;
			
				
			}
			else if(strstr(DATA[i].Operand,"X'")!= NULL){
				len = strlen(DATA[i].Operand) - 3;										// ---의 길이를 구하고
				strncpy(buf, DATA[i].Operand + 2,len);	
				for(int i=0; buf[i] != '\0'; i++){
					cnt++;
				}
				length = length +cnt/2;
				
			}
		}
		else{
			if(!strcmp(DATA[i].Operation,"END")){}		// END 는 길이에 포함 되지 않음 
				
			else
				length = length + 3;					// 그 이외는 SIC 포멧이기 때문에 3바이트의 형식을 따른다
			
		}
	}
	
	return length;											// 길이 반환
 }

int operation_location(char operation[]){					// 오퍼레이션과 일치하는 주소의 index 반환
	int i,k;
	
	for(i = 0 ; i < Symtab_counter; i++ ){					// Symtab의 코드번호와 일치하면 해당 Symtab 코드 counter 반환
		k = strlen(Symtab[i].sym_name);
		if(!strncmp(Symtab[i].sym_name, operation,k)){		// 같은 값이 있으면 break 하고
			break;
		}
	}	
		return i;											// 해당 인덱스를 반환 
}

int Literal_find(char data_operand[]){						// 리터럴 테이블에 있는 리터럴 이름을 검색하고 대응 값을 찾아 리턴
	int i,k;
	
	for(i=0; i < Literal_count; i++){
		k = strlen(Literal_tab[i].inst_operand);				
		if(!strncmp(Literal_tab[i].inst_operand,data_operand,k)){	// 찾고자 하는게 있으면 break
			break;
		}
	}
	return i;														// 해당 인덱스 반환
}
void pass_2(){

	char temp[100];							// 리터럴 테이블에 저장 할때 출력 오류를 막기위해 일련의 작업이 필요한데 쓰이는 배열공간
	char buffer[10];						// 임시저장공간
	char buf[10];							// 임시저장공간
	int len;								// 길이 구할 때 쓰는 변수 아래 참고
	
	
	for(int a = 1 ; a < counter && strcmp(DATA[a].Operation,"END") != 0 ; a ++){ 				// end 를 만날 때까지 루프 실행
			if(strlen(DATA[a].Lable) > 0){														// **** 리터럴 테이블에 주소를 저장
				if(strcmp(DATA[a].Operation, "BYTE") == 0){
					if(strstr(DATA[a].Operand,"C'") !=NULL){					//c'---'를 만나면
						len = strlen(DATA[a].Operand) - 3;										// ---의 길이를 구하고
						strncpy(buf, DATA[a].Operand + 2,len);									
						for(int i=0; i < strlen(DATA[a].Operand)- 4; i++){
							sprintf(buffer+i*2, "%X", buf[i]);									// buf의 값을 16진수 문자열로 변환
								
						}
						Literal_tab[Literal_find(DATA[a].Operand)].inst_value = strtol(buffer, NULL, 16);	// 16진수 정수값으로 변경
					}
					else if(strstr(DATA[a].Operand,"X'") != NULL){					//X' 형태를 만나면 (16진수)
						int c;
						len = strlen(DATA[a].Operand) - 3;							//X' --- '에서 ---에 대한 길이를 구하고
						strncpy(buf, DATA[a].Operand + 2,len);						// --- 의 내용만 buf에 복사
						c = (int)strtol(buf,NULL,16);								// --- 내용을 10진수로 변환하고
							
						Literal_tab[Literal_find(DATA[a].Operand)].inst_value = c;	// 리터럴 값에 대입 (나중 목적코드 출력시에는 다시 16진수로 자동 변환되어 출력 )
						
					}
					
				}
				else if(!strncmp(DATA[a].Operation, "WORD",strlen(DATA[a].Operation))){	// word 지시자를 만나면
					Literal_tab[Literal_find(DATA[a].Operand)].inst_value = atoi(DATA[a].Operand);		// 오퍼랜드의 값을 정수로 변환하여 대입
				}
			}
		
		else{
				
			if(!strncmp(DATA[a].Operation,"RSUB",strlen("RSUB"))){			// rsub은 0000의 값을 가지므로 대입
				DATA[a]	.flag = 5;	
			}
		}
	}
	


	// 어셈블리 코드 출력
	 printf("%4.X %-8s %-8s %-10s\n",DATA[0].Location, DATA[0].Lable, DATA[0].Operation, DATA[0].Operand);  	// COPY START 1000	출력
	
	for(int a = 1 ; a < counter; a ++){																			// 나머지 행 출력
			printf("%4.X %-8s %-8s %-10s\n",DATA[a].Location, DATA[a].Lable, DATA[a].Operation, DATA[a].Operand);  
	}
    printf("==========================================\n");
 
    // 심볼테이블 출력
    printf("======SYMTAB======\n");
    printf("%8s %4s\n","Symbol","Location");
	printf("======== =========\n");
	for(int z = 0; z <Symtab_counter; z++){
        printf("%8s %4.X\n",Symtab[z].sym_name,Symtab[z].sym_value);
    }
    printf("=================\n");
	
	// 리터럴 테이블 출력
	printf("======LIBTAB======\n");
	printf("%8s %4s\n","Literal","value");		// sic 버전에서는 리터럴이 정의된 곳이 없으므로 주소값은 리터럴 테이블에 제외시킴
	printf("======== =========\n");
	for(int i =0; i < Literal_count; i++){
		strcpy(temp,Literal_tab[i].inst_operand);
		for(int j = 0; j < strlen(temp); j++){	// 리터럴은 operand 값이므로 마지막에 개행문자가 들어가 있다. 출력할때 정렬이 예쁘게 안되므로 널문자를 넣어준다.
			if(temp[j]== '\n'){					// 개행문자를 만나면
				temp[j] = '\0';					// 널 문자로 바꿈
			}
		}
		printf("%8s %02X\n",temp,Literal_tab[i].inst_value);		// 리터럴 이름과 값을 출력(위에 언급했듯이 주소는 제외함)
		
	}
	
 	// 총 프로그램 길이 출력 
    printf("\n*******************\nProgram Len : %06X\n*******************\n", LOCCTR - start);
	// 목적코드 생성///////////////////
	
	
	printf("\nH^%s  ^%02X%0X^%06X\n\n",DATA[0].Lable,Optab[0].op_code,DATA[0].Location,LOCCTR-start );	// 첫줄
	
	
	int j;
	for(int i= 1; i < counter; i+=10){
	
		printf("T^%06X^%02X",DATA[i].Location,length_obcode(i));							// H 시작 주소부분
		
		for( j = i; j< i+10; j++){															// T 부분 출력(한줄에 최대 10개씩)
				if(!strcmp(DATA[j].Operation,"END"))										// END 전까지 출력 FLAG 값에 따라 출력 형태가 달라짐
				   break;
				if(DATA[j].flag == 1)														// FLAG = 1
					printf("^%02X", Literal_tab[Literal_find(DATA[j].Operand)].inst_value);
				else if(DATA[j].flag == 2)													// FLAG = 2
					printf("^%02X%04X", Optab[opcode_location(DATA[j].Operation)].op_code,Literal_tab[Literal_find(DATA[j].Operand)].inst_value);
				else if(DATA[j].flag == 3)													// FLAG = 3
					printf("^%06X", Literal_tab[Literal_find(DATA[j].Operand)].inst_value);
				else if(DATA[j].flag == 4)													// FLAG = 4 -> 목적코드가 없는 부분
					printf("      ");														//(3바이트가 최대이므로 빈칸 6칸으로 대체)
				else if(DATA[j].flag == 0){													// FLAG = 0
					int num = operation_location(DATA[j].Operand);
					if(strstr(DATA[j].Operand,",X") != NULL){
						printf("^%02X%X", Optab[opcode_location(DATA[j].Operation)].op_code, 0X9039);		
						//BUFFER,X 와같은 경우 뒤의 주소값이 9039 였다(왜 9039인지를 몰라 로직화 시키지 못함)
					}
					else
						printf("^%02X%X", Optab[opcode_location(DATA[j].Operation)].op_code, Symtab[num].sym_value);		// 일반적인 경우
				}							//////////////////////////////////////////////////////////////
			else if(DATA[j].flag == 5){
				printf("^%02X%04X", Optab[opcode_location(DATA[j].Operation)].op_code,0x00);
			}
		}	  
		printf("\n\n");
		
	}
	printf("E^%06X\n",Symtab[operation_location(DATA[counter-1].Operand)].sym_value );									// 종료 레코드 출력
	

		
   
	}
