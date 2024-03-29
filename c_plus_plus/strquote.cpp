#include <iostream>
#include <string>

using namespace std;

string version1(const string &s1, const string &s2);
const string &version2(string &s1, const string &s2);
const string &version3(string &s1, const string &s2);

int main()
{
	string input;
	string copy;
	string result;

	cout << "Enter a string: ";
	getline(cin, input);
	copy = input;
	cout << "Your string is entered: " << input << endl;
	result = version1(input, "***");
	cout << "Your string enhanced: " << result << endl;
	cout << "Your original string: " << input << endl;
	
	result  = version2(input, "###");
	cout << "Your string enhanced: " << result << endl;
	cout << "Your original string: " << input << endl;
	cout << "Resetting original string.\n";

	input = copy;
	string result1 = version3(input, "@@@");
	cout << "&result1 = " << &result1 << endl;
	cout << "Your string enhanced: " << result << endl;
	cout << "Your original string: " << input << endl;
	return 0;
}

string version1(const string &s1, const string &s2)
{
	string temp;
	temp = s2 + s1 + s2;
	return temp;
}

const string &version2(string &s1, const string &s2)
{
	s1 = s2 + s1 + s2;
	return s1;
}

const string &version3(string &s1, const string &s2)
{
	string temp;
	cout << "&temp1 = " << &temp << endl;
	temp = s2 + s1 + s2;
	return temp;
}
