%name configparser
%include {
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "calculate_2.h"
using namespace std;
struct Token {
	int value;
	unsigned n;
};
}
%token_type {Token}
%default_type {Token}
%left PLUS MINUS.
%left DIVIDE TIMES.

%syntax_error {
	cout << "Syntax error!!!" << endl;
}

program ::= expr(A). {
	cout << "Result.value = " << A.value << endl;
	cout << "Result.n = " << A.n << endl;
}

expr(A) ::= expr(B) MINUS expr(C). {
	A.value = B.value - C.value;
	A.n = B.n + 1 + C.n + 1;
}

expr(A) ::= expr(B) PLUS expr(C). {
	A.value = B.value + C.value;
	A.n = B.n + 1 + C.n + 1;
}

expr(A) ::= expr(B) TIMES expr(C). {
	A.value = B.value * C.value;
	A.n = B.n + 1 + C.n + 1;
}

expr(A) ::= expr(B) DIVIDE expr(C). {
	if(C.value != 0)
	{
		A.value = B.value / C.value;
		A.n = B.n + 1 + C.n + 1;
	}
	else
	{
		cout << "Divide by zero" << endl;
	}
}

expr(A) ::= NUM(B).{
	A.value = B.value;
	A.n = B.n + 1;
}

%code {
	int main()
	{
		void *pParser = configparserAlloc(malloc);
		struct Token t0, t1;
		t0.value = 4;
		t0.n = 0;
		t1.value = 13;
		t1.n = 0;

		cout << "\t t0.value(4) PLUS t1.value(13) " << endl;
		configparser(pParser, NUM, t0);
		configparser(pParser, PLUS, t0);
		configparser(pParser, NUM, t1);
		configparser(pParser, 0, t0);

		configparserFree(pParser, free);
		return 0;
	}
}


