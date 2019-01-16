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

void init();
 

int pass_1(){
 	
	init();	
	char buf[30];			// txt 파일로 부터 읽어와서 저장할 임시저장 공간
	char *token;			// 토크나이저 띄어쓰기 단위로 lable operation operand를 구분
	int i = 0;				// 토크나이저에서 토큰의 개수를 기억
	FILE * f =NULL;
	
	char lable[8], opcode[8], operand[8];	// 
	
	f = fopen("team.txt","r");		// 파일 오픈
    if(f == NULL){
        fprintf(stderr,"file open error!\n");
        return -1 ;
    }
	
	
    while(fgets(buf,30,f) != NULL){			// 한 줄 읽어서
        token = strtok(buf," ");			// 띄어쓰기로 구분
        while(token != NULL){				//토큰으로 끊어 읽어 토큰이 3개인지, 2개인지 1개인지 알아낸다
            switch(i){       				//배열에 삽입 lable, operation, operand 
                case 0:
                    strcpy(lable,token);	//토큰이 0개 일때 label에 저장
                    break;
                case 1:
                    strcpy(opcode,token);	// 토큰이 1개 일때 operation 저장
                    break;
                case 2:
                    strcpy(operand,token);	// 토큰이 2개일때 operand에 저장
        }i++;								// 토큰 개수 증가	(위에 전역변수로 선언 됨 초기값 0)		
            token = strtok(NULL," ");		
        }
        
	/*경우의 수 3가지 1.operation,operand  2.lable,operation  3.operation 만 있을때*/
		if( i == 2){								// operation 와 operand 만 있을 때
			strcpy(DATA[counter].Operation,lable);
			strcpy(DATA[counter].Operand, opcode);
		}
		else if( i == 3){							// 다 있을때
            strcpy(DATA[counter].Lable,lable);
            strcpy(DATA[counter].Operation,opcode);
            strcpy(DATA[counter].Operand,operand);
        }
		else{										// operation 만 있을때
          
			strcpy(DATA[counter].Operation,lable);
		}
 
        counter++; 						// 1줄 증가, 
		i = 0;							// i 초기화 하고 다음 줄 읽어 토크나이징 함
    }
    
	fclose(f);
	////////// 1 패스 과정//////////
	if(strcmp(DATA[0].Operation,"START") == 0){				//첫 번째 라인이 START면 Operand에 있는 값을
        LOCCTR = strtol(DATA[0].Operand,NULL,16);			//LOCCTR에 삽입
    }else{													//아니라면 0으로 초기화하게 함
        LOCCTR = 0x00;
    }
   

	start = LOCCTR;											//첫 시작 위치를 start 에 저장 -> 나중에 프로그램의 총 길이를 구할 때 쓰임.
    DATA[0].Location = LOCCTR;								// 프로그램 (명령어) 첫 줄의 시작주소를 지정
    	
  
	for(int a = 1 ; a < counter && strcmp(DATA[a].Operation,"END") != 0 ; a ++){ 				// end 를 만날 때까지 루프 실행
			if(strlen(DATA[a].Lable) > 0){														// **** symtab 에 레이블과 주소를 저장(1패스 과정)
				if(strcmp(DATA[a].Operation, "BYTE") == 0){
					if(strstr(DATA[a].Operand,"C'") != NULL){
						strcpy(Literal_tab[Literal_count].inst_operand, DATA[a].Operand);		// 리터럴 탭에 저장
					
						DATA[a].flag=1;
						Literal_count++;
					}
					else if(strstr(DATA[a].Operand,"X'") != NULL){
						strcpy(Literal_tab[Literal_count].inst_operand, DATA[a].Operand);		// 리터럴 탭에 저장
						
						DATA[a].flag=1;
						Literal_count++;
					}
					
					
				}
				else if(!strncmp(DATA[a].Operation, "WORD",strlen(DATA[a].Operation))){
					strcpy(Literal_tab[Literal_count].inst_operand, DATA[a].Operand);		// 리터럴 탭에 저장
					
					DATA[a].flag=3;
					Literal_count++;
				}
				else if(!strncmp(DATA[a].Operation, "RESW",strlen(DATA[a].Operation)) || !strncmp(DATA[a].Operation, "RESB",strlen(DATA[a].Operation))){
					DATA[a].flag = 4;
			}	
				else{
					DATA[a].flag=0;
			}
				strcpy(Symtab[Symtab_counter].sym_name,DATA[a].Lable);		//  심볼테이블에 레이블 저장
				Symtab[Symtab_counter].sym_value = LOCCTR;					// 	심볼테이블에 주소값 저장
				Symtab_counter++;
		}
				
		else{
			int c;
			if(!strncmp(DATA[a].Operation,"RSUB",strlen("RSUB"))){	
				DATA[a].flag=5;
				
			}
			else{
				DATA[a].flag = 0;
			}
			
		}
		
			DATA[a].Location = LOCCTR;													// **** 명령문에 주소 할당 (1패스 과정)																									
        	
        
    		if(strcmp(DATA[a].Operation,"COPY") == 0);
		
        	else if(strcmp(DATA[a].Operation,"WORD") == 0){							// word 지시자의 경우 3바이트 증가
           	 	LOCCTR += 3;
        	}
			else if(strcmp(DATA[a].Operation,"RESW") == 0){							// 워드 예약 3 * n 바이트 예약
            	LOCCTR += (3 * atoi(DATA[a].Operand));
        	}
			else if(strcmp(DATA[a].Operation,"RESB") == 0){							// n 바이트 예약가능
				LOCCTR += atoi(DATA[a].Operand);
       		}
				
			else if(strcmp(DATA[a].Operation,"BYTE") == 0){							// 1 바이트 예약
				int count=0;
				if(strstr(DATA[a].Operand,"C'")!=NULL){
					for(int i=4; i < strlen(DATA[a].Operand); i++){
						count++;
					}
					LOCCTR = LOCCTR + count;
				}
				else if(strstr(DATA[a].Operand,"X'")!=NULL){
					for(int i=4; i < strlen(DATA[a].Operand); i++){
						count++;
					}
					LOCCTR = LOCCTR + count/2;
				}
				
      	  	}

			else{
       	    	 LOCCTR += 3;															// 그 이외 3바이트 씩 증가
        	}   
   	
	}

}