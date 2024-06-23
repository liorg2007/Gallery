#include <iostream>
#include <string>
#include <ctime>
#include "Windows.h"
#include "MemoryAccess.h"
#include "AlbumManager.h"
#include <csignal>
#include "DatabaseAccess.h"
#include <mutex>

#define DEVELOPER_NAME "Lior Gleizer"

AlbumManager* _manager_for_interrupt = nullptr;

void interruptHandler(int signum)
{
	_manager_for_interrupt->interrupt();
	std::signal(SIGINT, interruptHandler);
}

int getCommandNumberFromUser()
{
	std::string message("\nPlease enter any command(use number): ");
	std::string numericStr("0123456789");
	
	std::cout << message << std::endl;
	std::string input;
	std::getline(std::cin, input);
	
	while (std::cin.fail() || std::cin.eof() || input.find_first_not_of(numericStr) != std::string::npos) {

		std::cout << "Please enter a number only!" << std::endl;

		if (input.find_first_not_of(numericStr) == std::string::npos) {
			std::cin.clear();
		}

		std::cout << std::endl << message << std::endl;
		std::getline(std::cin, input);
	}
	
	return std::atoi(input.c_str());
}


void printSysInfo()
{
	//get the time
	time_t now = time(0);
	// convert now to string form
	char* date_time = ctime(&now);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //get handle to the command line
	SetConsoleTextAttribute(hConsole, 5);	//change color
	std::cout << "Startup time: " << date_time << std::endl;
	SetConsoleTextAttribute(hConsole, 3);	//change color
	std::cout << "Developed by: " << DEVELOPER_NAME << std::endl << std::endl;
	SetConsoleTextAttribute(hConsole, 7); //change back to normal
}


void inputLoop()
{
	// initialization data access
	DatabaseAccess dataAccess;

	// initialize album manager
	AlbumManager albumManager(dataAccess);
	
	//Initialize the signal iterrupt replacement
	_manager_for_interrupt = &albumManager;

	std::string albumName;
	std::cout << "Welcome to Gallery!" << std::endl;
	std::cout << "===================" << std::endl;
	std::cout << "Type " << HELP << " to a list of all supported commands" << std::endl;

	do {
		int commandNumber = getCommandNumberFromUser();

		try {
			albumManager.executeCommand(static_cast<CommandType>(commandNumber));
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	} while (true);
}

int main(void)
{
	std::signal(SIGINT, interruptHandler);
	//print sys information
	printSysInfo();
	//main program loop
	inputLoop();

	return 0;
}


