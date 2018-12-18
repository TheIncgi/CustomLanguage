#include <iostream>
#include <stdio.h>
#include "Compiler.h"
#include "Env.h"
#include <vector>
#include "Menu.h"
using namespace std;

void test(string s) {
	cout << s  << " is type " << Compiler::getTypeName( Compiler::typeOf(s) ) << endl;
}

void testChunk(std::string s) {
	vector<std::string> out;
	Compiler::chunker(s, out);
	cout << "chunks: (" << out.size() << ")" << endl;
	for (int i = 0; i < out.size(); i++) {
		cout << "\"" << out[i] << "\"\n";
	}
	cout << endl << endl;
}

void testInfix(std::string s) {
	vector<Compiler::val> out;
	Compiler::compileExpr(s, out);
	cout << "chunks: (" << out.size() << ")" << endl;
	for (int i = 0; i < out.size(); i++) {
		cout << Compiler::valToString(out[i]) << endl;
	}
	cout << endl << endl;
}

void eval(std::string s) {
	Env e;
	Compiler::val v = Compiler::evaluate(s, e);
	cout << s + " = " + Compiler::valToString(v) << endl;
}

void eval(std::string s, Env &env) {
	Compiler::val v = Compiler::evaluate(s, env);
	cout << s + " = " + Compiler::valToString(v) << endl;
}




int app() {
	test("");
	test("45");
	test("6");
	test("a");
	test("_a");
	test("+");
	test("\"Hello ");
	test("\"Hello World\"");
	test("24.3");
	test("2.4.3");
	test("hhy.opiihk");
	test("655+");
	test("\"Hello World\"+");
	test("+5");
	cout << "=========================================\n";
	cout << "============= END OF TEST 1 ===============\n";
	cout << "=========================================\n";
	
	testChunk("1+45-15%6/250+\"Hello\"+\" World!\"+5");
	cout << "=========================================\n";
	testChunk("a//bagles*15%3+' bagles'");
	testChunk("'bagles = ' (4+3)");
	cout << "=========================================\n";
	testChunk("a + b / c * w");
	cout << "=========================================\n";
	testChunk("a + b / c * (m+b) / c");
	cout << "=========================================\n";
	testChunk("a+b/c*(m+b)/c");
	cout << "=========================================\n";
	testChunk("a+b*(c^d-e)%(f+g*h)-i");

	testChunk("45");

	cout << "=========================================\n";
	cout << "============= END OF TEST 2 ===============\n";
	cout << "=========================================\n";

	testInfix("a + b / c * w");
	
	
	cout << "=========================================\n";

	testInfix("a+b*(c%d-e)%(f+g*h)-i");

	cout << "=========================================\n";

	testInfix("3.14159*2");

	cout << "=========================================\n";

	testInfix("-3+4");

	cout << "=========================================\n";

	testInfix("-(3+4)");

	testInfix("2*-(4+3)");

	testInfix("'bagles = ' + (4+3)");

	//yeilds a b c d %  e - f g h * + % + i -

	cout << "=========================================\n";
	cout << "============= END OF TEST ===============\n";
	cout << "============= final tests ===============\n";
	cout << "=========================================\n";
	testInfix("2*5+5");
	eval("2*5+5");
	eval("12/2");
	eval("7/3");
	eval("1+1+(1+1+1)*3");
	eval("24%5");
	//eval("(0-24)%5");
	eval("-24%5");
	eval("3.14159*2");
	eval("2*-(4+3)");
	eval("45");
	eval("'bagles = ' + (4+3)");

	cout << "=========================================\n";
	Env env;
	Compiler::val pi = Compiler::val(3.14159654);
	Compiler::val x = Compiler::val(14);
	env.set("pi", pi);
	env.set("x", x);

	Compiler::val tmp = env.get("x");
	cout << "Old x: " << Compiler::valToString( tmp ) << endl;
	eval("pi*2",     env);
	eval("x = x*pi", env);
	

	tmp = env.get("x");
	cout << "New x: " << Compiler::valToString(tmp) << endl;
	eval("x", env);

	return 0;
}



void runTests() {
	try {
		cout << "Running tests..." << endl;
		app();
	} catch (const std::exception &e) {
		cout << "ERROR: " << e.what() << std::endl;
		getchar();
		return;
	} catch (...) {
		cout << "other error type" << std::endl;
		getchar();
		return;
	}
	cout << "enter to exit" << endl;
	getchar();
	return ;
}

void repl() {
	std::string lineIn;
	Env env;
	cout << "Type 'EXIT' to exit REPL mode" << endl;
	while (true) {
		cin >> lineIn;
		if (lineIn == "EXIT") {
			break;
		}
		Compiler::val out = Compiler::evaluate(lineIn, env);
		if (out._type != Compiler::TYPE_INVALID) {
			if (out._type == Compiler::TYPE_VAR) {
				std::string name = *out.data.s;
				out = env.get(name);
				cout << "  " <<  name << " = " << Compiler::valToString(out) << endl;
			} else {
				cout << " -> " << Compiler::valToString(out) << endl;
			}
		}
	}
}

bool quit = false;
void exitApp() { quit = true; }
int main() {
	Menu menu("Main Menu");
	menu.add("Run Tests", runTests);
	menu.add("REPL", repl);
	menu.add("Quit", exitApp);
	while (!quit) {
		menu.showInstant();
	}
}