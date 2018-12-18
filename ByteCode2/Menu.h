#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

class Menu {
	private:
	unsigned int maxLen = 0;
	struct option {
		std::string txt;
		void(*func)();
	};
	std::vector<option> options;
	std::string title;

	public:
	Menu(std::string title);
	~Menu();

	static void wait();

	void add(std::string opt, void(*f)());

	void show();
	void showInstant();

	private:
	void draw();
	void drawInstant();
	std::string rep(std::string str, int times);
};

