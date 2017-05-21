#include <iostream>
#include "stock1.h"

Stock::Stock()
{
	std::cout << "Default constructor called\n";
	std::strcpy(company, "no name");
	shares = 0;
	share_val = 0.0;
	total_val = 0.0;
}

Stock::Stock(const char *co, int n, double pr)
{
	std::cout << "Constructor using " << co << " called";
	std::strncpy(company, co, 29);
	company[29] = '\0';
	if(n < 0)
	{
		std::cerr << "Number of shares can't be negative; "
			<< company << " shares set to 0.\n";
		shares = 0;
	}
	else
		shares = n;
	share_val = pr;
	set_tot();
}

Stock::~Stock()
{
	std::cout << "Bye, " << company << "!\n";
}

void Stock::buy(int num, double price)
{
	if(num < 0)
	{
		std::cerr << "Number of shareds"
	}
}















