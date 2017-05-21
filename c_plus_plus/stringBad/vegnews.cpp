#include <iostream>
#include "strngbad.h"

using std::cout;

void callme1(StringBad &);
void callme2(StringBad);

int main(void)
{
	using std::endl;
	StringBad headline1("Celery Stalks at Midnight");
	StringBad headline2("Letture Prey");
	StringBad sports("Spinach Leaves Bowl for Dollars");

	callme1(headline1);
	cout << "headline1: " << headline1 << endl;
	callme2(headline2);
	return 0;
}

void callme1(StringBad &rsb)
{
	cout << "String passed by reference: \n";	
	cout << " \"" << rsb << "\"\n";
}

void callme2(StringBad sb)
{
	cout << "String passed by value: \n";
	cout << " \"" << sb << "\"\n";
}
