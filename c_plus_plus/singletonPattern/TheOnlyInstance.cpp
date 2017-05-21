#include <iostream>

class TheOnlyInstance
{
	public:
		static TheOnlyInstance* GetTheOnlyInstance();
	protected:
		TheOnlyInstance(){};
	private:
	//private data
};

TheOnlyInstance *TheOnlyInstance::GetTheOnlyInstance()
{
	static TheOnlyInstance objTheOnlyInstance;
	return &objTheOnlyInstance;
}

int main(void)
{
	TheOnlyInstance *pTheOnlyInstance = TheOnlyInstance::GetTheOnlyInstance();
	return 0;
}
