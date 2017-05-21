#include <iostream>

using namespace std;
double refcube(double &ra)
{
	ra *= ra * ra;
	return ra;
}
int main()
{
	double m;
	cout << refcube(m + 1) << endl;
	return 0;
}

