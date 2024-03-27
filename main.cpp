#include <iostream>
#include "wasp.h"

int main()
{
	Wasp wasp( 
		"C:/Users/cbrommer/Desktop/wasp/settings.json", 
		"C:/Users/cbrommer/Desktop/wasp/build.json",
		"C:/Users/cbrommer/Desktop/wasp/config.json");
	wasp.Execute();

	return 0;
}
