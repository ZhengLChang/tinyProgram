%include{
#include <stdio.h>
#include <stdlib.h>
#include "test.h"
#include <iostream>
	using namespace std;
	}


%token_type { int }
%destructor expr {printf("free expr %d", $$);}
%type expr { int }

program ::= items.
items ::= items item.
items ::= .
item ::= expr.

expr(A) ::= NUM(B). {
	A = B;
	cout << A << endl;
	}

	%code{
		int main()
		{
			void *pParser = ParseAlloc(malloc);
			Parse(pParser, NUM, 1);
			Parse(pParser, NUM, 2);
			Parse(pParser, 0, 0);
			ParseFree(pParser, free);
			return 0;
		}
}
