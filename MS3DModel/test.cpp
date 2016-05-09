
#include "MS3DModel.h"
#include <iostream>

using namespace std;

int main()
{
	MS3DModel myModel;
	if(!myModel.LoadMS3DModel("models/zombie02.ms3d"))
		cout << "ERROR";
	return 0;
}