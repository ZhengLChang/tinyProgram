#include <iostream>
#include "namesp.h"

void other(void);
void another(void);

int main(void)
{
	return 0;
}



void other(void)
{
	using std::cout;
	using std::endl;
	using namespace debts;

	Person dg = {"Doodles", "Glister"};
	showPerson(dg);
	cout << endl;
	Debt zippy[3];
	int i;
	for(i = 0; i < 3; i++)
		getDebt();
}
void another(void)
{
}
