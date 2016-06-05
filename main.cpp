#include "Ricsi/ricsi.h"
#include "Aron/aron.h"

#include <map>
#include <conio.h>

void main()
{
	std::map<std::string, std::function<void()>> functionMap;
	
	// Add aron programs
	functionMap["aron_hello"] = aron_hello;
	//functionMap["aron_valami"] = aron_valami;

	// Add ricsi programs
	functionMap["ricsi_hello"] = ricsi_hello;
	//functionMap["ricsi_valami"] = ricsi_valami;

	std::puts("Enter program name to run: ");
	for(auto it = functionMap.begin(); it != functionMap.end(); ++it)
	{
		std::cout<<"\t\t\t\t" << it->first << ", \n";
	}

	std::string program;
	bool bProgramFound;
	do 
	{
		std::cin >> program;
		auto it = functionMap.find(program);
		bProgramFound = it != functionMap.end();

		if( bProgramFound )
		{
			it->second();
			break;
		}

		std::puts("Wrong name, try it again!\n");
	} while (!bProgramFound);

	_getch();
}