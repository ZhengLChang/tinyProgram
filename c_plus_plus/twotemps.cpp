#include <iostream>

template <class Any>
void Swap(Any &a, Any &b);

template <class Any>
void Swap(Any *a, Any *b, int n);

void Show(int a[]);
const int Lim = 8;

int main()
{
	int i = 10, j = 20;
	std::cout << "i, j = " << i << ", " << j << ".\n";
	std::cout << "using compiler-generated int swapper: \n";
	Swap(i, j);
	std::cout << "Now i, j = " << i << ", " << j << ".\n";
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

template <class Any>
void Swap(Any a[], Any b[], int n)
{
	Any temp;
	for(int i = 0; i < n; i++)
	{
		temp = a[i];
		a[i] = b[i];
		b[i] = temp;
	}
}

void Show(int a[])
{
	using namespace std;
	cout << a[0] << a[1] << "/";
	cout << a[2] << a[3] << "/";
	for(int i = 4; i < Lim; i++)
		cout << a[i];
	cout << endl;
}





