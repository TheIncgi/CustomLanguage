#include "Env.h"



Env::Env() {
	startBlock();
	level = 0;
}


Env::~Env() {
}

Compiler::val Env::get(const std::string & key) {
		for (int i = level; i >= 0; i--) {
			std::map<std::string, Compiler::val>::iterator it = scopes[i].find(key);
			if (it != scopes[i].end())
				return it->second;
		}
		return Compiler::val();
	
}

void Env::startBlock() {
	std::map<std::string, Compiler::val> map;
	scopes.push_back(map);
	level++;
}

void Env::endBlock() {
	scopes.pop_back();
	level--;
}

void Env::set(std::string key, Compiler::val &val) {
	for (int i = level; i >= 0; i--) {
		std::map<std::string, Compiler::val>::iterator it = scopes[i].find(key);
		if (it != scopes[i].end()) {
			scopes[i][key] = val;
			return;
		}
	}
	scopes[0][key] = val;
}

void Env::setLocal(std::string key, Compiler::val & val) {
	scopes[level][key] = val;
}
