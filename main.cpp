#include <iostream>
#include "wasp.h"

int main()
{
	// Grab the test files directory
	std::string testFilesDir(TEST_FILES_DIR);

	// Create Wasp instance
	Wasp wasp( 
		testFilesDir + "/settings.json", 
		testFilesDir + "/build.json",
		testFilesDir + "/config.json");

	// Start Wasp
	wasp.Execute();

	return 0;
}
