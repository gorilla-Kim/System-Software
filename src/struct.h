//심볼테이블
struct SYMTAB{					
    char sym_name[9];			// 레이블 이름
    int sym_value;				// 레이블 주소(1 패스에서 부여함)
}Symtab[100];


//각 라인에 있는 LABLE OPERATION OPERAND  를 저장 할 배열
struct DATA{					
    int Location;				// 명령어 주소
    char Lable[8];				// 레이블
    char Operation[8];			// 오퍼레이션
    char Operand[9];			// 오퍼랜드
	int flag;					// 오퍼레이션의 종류에 따라 FLAG값이 정해지고 그 값에 따라 출력할 때 참조 테이블과 참조 값이 정해짐
}DATA[100];
 

// 리터럴 테이블
struct Literal_tab{				
	char inst_operand[8];		//리터럴 이름
	int inst_value;				// 리터럴 값
}Literal_tab[100];