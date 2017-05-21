#include <iostream>

template <class Any>
void Swap(Any &a, Any &b);

struct job
{
	char name[40];
	double salary;
	int floor;
};

template <>void Swap<job>(job &j1, job &j2);
void Show(job &j);

int main()
{
	return 0;
}

template <class Any>
void Swap(Any &a, Any &b)
{
	Any Temp;
	temp = a;
	a = b;
	b = temp;
}

template <>void Swap<job>(job &j1, job &j2)
{
	double t1;
	int t2;
	t1 = j1.salary;
	j1.salary = j2.salary;
	js.salary = t1;
	t2 = j1.floor;
	j1.floor = j2.floor;
	j2.floor = t2;
}
void Show(job &j)
{
	std::cout << j.name << ": $" << j.salary
		<< " on floor " << j.floor << endl;
}














