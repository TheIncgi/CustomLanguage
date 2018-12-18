#include "Menu.h"
#include <sstream>
#include <conio.h> //for getch

#include <chrono> //for sleep debounce
#include <thread> //for sleep debounce

Menu::Menu(std::string title) {
	this->title = title;
	maxLen = title.size();
}

Menu::~Menu() {}

void Menu::wait() {
	std::cout << "\nPress any key to continue...\n";
	_getch(); _getch();
}

void Menu::add(std::string opt, void(*f)()) {
	option sOpt;
	sOpt.txt = opt;
	sOpt.func = f;
	options.push_back(sOpt);
	maxLen = maxLen > opt.size() ? maxLen : opt.size();

}

//for menus with single char options
void Menu::showInstant() {
	drawInstant();
	int tragicFailures = 0;
	std::string in;

	while(true) {
		char c = _getch() | ' '; //bitwise op makes it lowercase always
		int choice = -1;
		if('1' <= c && c <= '9')
			choice = c - '0';
		else if('a' <= c && c <= 'z')
			choice = 10 + c - 'a';
		
		if(choice == 0 || choice == -1);
		else if(choice <= 0 || options.size() < choice) {
			std::cout << "INVALID INPUT ( " << choice << " )\n";
			tragicFailures++;
			if(tragicFailures % 5 == 0 && tragicFailures != 0)
				drawInstant();
		} else {
			std::cout << c << "\n" << std::endl;
			options[choice - 1].func();
			return;
		}
	}
}

void Menu::show() {
	draw();
	int tragicFailures = 0;
	std::string in;

	while(true) {
		std::getline(std::cin, in);
		std::stringstream tmpInS(in);
		int choice;

		tmpInS >> choice;

		if(tmpInS.fail() || choice <= 0 || options.size() < choice) {
				std::cout << "INVALID INPUT\n";
				tragicFailures++;
				if(tragicFailures % 5 == 0 && tragicFailures != 0)
					draw();
		} else {
			options[choice - 1].func();
			return;
		}
	}
}

void Menu::draw() {
	std::string titleFormat = "| %-"+std::to_string(maxLen + 4)+"s |\n";
	std::string optFormat   = "| %2d) %-" + std::to_string(maxLen) + "s |\n";
	std::string horz = "+" + rep("=", 6+maxLen) +"+\n";

	std::cout << horz;
	printf(titleFormat.c_str(), title.c_str());
	std::cout << horz;
	for(int i = 0; i < options.size(); i++) {
		printf(optFormat.c_str(), i+1, options[i].txt.c_str());
	}
	std::cout << horz;
	std::cout << "Choose option: ";
}

void Menu::drawInstant() {
	std::string titleFormat = "| %-" + std::to_string(maxLen + 3) + "s |\n";
	std::string optFormat = "| %1s) %-" + std::to_string(maxLen) + "s |\n";
	std::string horz = "+" + rep("=", 5 + maxLen) + "+\n";

	std::string optNames = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	std::cout << horz;
	printf(titleFormat.c_str(), title.c_str());
	std::cout << horz;
	
	for(int i = 0; i < options.size(); i++) {
		printf(optFormat.c_str(), optNames.substr(i, 1).c_str(), options[i].txt.c_str());
	}
	std::cout << horz;
	std::cout << "Choose option: ";
}

std::string Menu::rep(std::string str, int times) {
	std::string out;
	for(int i = 0; i < times; i++)
		out += str;
	return out;
}
