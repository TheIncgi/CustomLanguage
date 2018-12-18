#pragma once
#include <vector>
#include <map>
#include <string>

#include "Compiler.h"
namespace Compiler {
	struct val;
}
class Env {
	public:
	Env();
	~Env();

	
	Compiler::val get(const std::string &key);
	void startBlock();
	void endBlock();
	//sets top existing level of var or global level if none
	void set(std::string key, Compiler::val &val);

	//sets var at current level
	void setLocal(std::string key, Compiler::val &val);


	private:
	std::vector<std::map<std::string, Compiler::val>> scopes;
	int level = 0;
};

