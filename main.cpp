#include <iostream>
#include "wasp_m.h"

int main()
{
	Wasp wasp( 
		"C:/Users/chipb/Desktop/wasp/settings.json", 
		"C:/Users/chipb/Desktop/wasp/build.json",
		"C:/Users/chipb/Desktop/wasp/config.json");
	wasp.Execute();

	return 0;
}
