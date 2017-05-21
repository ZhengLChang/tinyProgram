#include <iostream>

template <class Any>
void Swap(Any &a, Any &b);

int main(void)
{
	using namespace std;
	//using namespace std;
	int i = 10;
	int j = 20;
	cout << "i, j = " << i << ", " << j << ".\n";
	cout << "Using compiler-generated int swapper: \n";
	Swap(i, j);
	cout << "Now i, j = " << i << ", " << j << ".\n";

	double x = 24.5;
	double y = 81.7;
	cout << "x, y = " << x << ", " << y << ".\n";
	cout << "Enter a string: ";
	cout << "Using compiler-generated double swapper; \n";
	Swap(x, y);
	cout << "Now x, y = " << x << ", " << y << ".\n";
	return 0;
}

template <class Any>
void Swap(Any &a, Any &b)
{
	Any temp;
	temp = a;
	a = b;
	b = temp;
}
