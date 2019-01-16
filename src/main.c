#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stddef.h>
#include "main.h"

int LOCCTR;						//위치카운터
int Optab_counter;				//opcode 카운터
int Symtab_counter;				//심볼테이블 카운터
int flag;						//몇 번째 op테이블에 있는지 확인해줄 변수
int start;						//프로그램 시작주소를 저장하는 변수
int Literal_count;				// 리터럴 테이블 카운터
int counter;					//총 프로그램 행의 개수 

void init(){					//전역변수 초기화
    LOCCTR = 0x00;				// 0000번지로 초기화
    Optab_counter = 19;			// operation의 개수만큼
    Symtab_counter = 0;			// 패스1과정에서 심볼테이블에 심볼과 할당된 주소값을 저장할때 심볼이 중복저장을 막기위해 쓰이는 카운터 변수 
    start = 0x00;				// 시작주소
    Literal_count =0;			//리터럴 탭의 리터럴 개수
	counter=0;
	
}
void main(){					// 메인 함수

	pass_1();
	pass_2();
	
	
}