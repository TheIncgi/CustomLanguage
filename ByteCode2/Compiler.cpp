#include "Compiler.h"
#include <iostream>

Compiler::TYPE Compiler::typeOf(std::string chunk){
	if (chunk.size() == 0) {
		//std::cout << "EMPTY\n";
		return TYPE_INVALID;
	}
	if (isLetter(chunk[0]) || chunk[0] == '_') {
		for (int i = 1; i < chunk.size(); i++) {
			char c = chunk[i];
			if (!isLetter(c) && !isNumber(c)) {
				return c=='.'? TYPE_COMPOUND : TYPE_INVALID;
			}
		}
		return TYPE_VAR;
	}
	if(chunk.size() >= 2 && isQuote(chunk[0])){
		//std::cout << "DEBUG: " << chunk[0] << " == " << chunk[chunk.size() - 1] << std::endl;
		if (chunk[chunk.size() - 1] == chunk[0]) {
			//std::cout << "DEBUG: Full string" << std::endl;
			return TYPE_STRING;
		}else {
			return TYPE_INCOMPLETE_STRING;
		}
	}
	if (isNumber(chunk[0]) || chunk[0]=='.') {
		int numDec = 0;
		for (int i = 0; i < chunk.size(); i++) {
			if (chunk[i] == '.') {
				numDec++;
			}
			else if (!isNumber(chunk[i])) {
				return TYPE_INVALID;
			}
		}
		if (numDec == 0)
			return TYPE_INT;
		else if (numDec == 1)
			return TYPE_DOUBLE;
		else
			return TYPE_INVALID;
	}
	bool containsPar = false;
	for (int i = 0; i < chunk.size(); i++) {
		if (!isSymbol(chunk[i])) return TYPE_INVALID;
		containsPar |= getOpCode(chunk.substr(i,1)) == Op::O_PAR;
		containsPar |= getOpCode(chunk.substr(i,1)) == Op::C_PAR;
		containsPar |= getOpCode(chunk.substr(i,1)) == Op::O_BRK;
		containsPar |= getOpCode(chunk.substr(i,1)) == Op::C_BRK;
	}
	return containsPar? (chunk.size()==1? TYPE_OP : TYPE_INVALID) : TYPE_OP;
	
}

//Compiler::val Compiler::valueOf(std::string chunk)
//{
//	Compiler::val v;
//	v.type = Compiler::typeOf(chunk);
//	return v;
//}



int Compiler::getOpLvl(TYPE op)
{
	switch (op) {
		case Op::ADD_EQL:
		case Op::SUB_EQL:
		case Op::MUL_EQL:
		case Op::DIV_EQL:
		case Op::EQUALS:
			return -2;
		case Op::LESS_THAN:
		case Op::GREATER_THAN:
		case Op::LESS_THAN_EQU:
		case Op::GREATER_THAN_EQU:
			return -1;
	case Op::ADD:
	case Op::SUB:
		return 0;
	case Op::MUL:
	case Op::MOD:
	case Op::DIV:
		return 1;
	case Op::O_BRK:
	case Op::C_BRK:
	case Op::O_PAR:
	case Op::C_PAR:
		return -999999;
	default:
		return -1;
	}
}


Compiler::TYPE Compiler::getOpCode(std::string opText)
{
	if (opText == "+") {
		return Compiler::Op::ADD;
	}
	else if (opText == "-") {
		return Compiler::Op::SUB;
	}
	else if (opText == "*") {
		return Compiler::Op::MUL;
	}
	else if (opText == "/") {
		return Compiler::Op::DIV;
	}
	else if (opText == "%") {
		return Compiler::Op::MOD;
	}
	else if (opText == "(") {
		return Compiler::Op::O_PAR;
	}
	else if (opText == ")") {
		return Compiler::Op::C_PAR;
	}
	else if (opText == "[") {
		return Compiler::Op::O_BRK;
	}
	else if (opText == "]") {
		return Compiler::Op::C_BRK;
	}
	else if (opText == "=") {
		return Compiler::Op::ASSIGNMENT;
	}
	else if (opText == "==") {
		return Compiler::Op::EQUALS;
	}
	else if (opText == "+=") {
		return Compiler::Op::ADD_EQL;
	}
	else if (opText == "-=") {
		return Compiler::Op::SUB_EQL;
	}
	else if (opText == "!=") {
		return Compiler::Op::NOT_EQUALS; //TODO add remaining operators and add to the evaluation instr
	}
	else if (opText == "<") {
		return Compiler::Op::LESS_THAN;
	}
	else if (opText == ">") {
		return Compiler::Op::GREATER_THAN;
	}
	else if (opText == "<=") {
		return Compiler::Op::LESS_THAN_EQU;
	}
	else if (opText == ">=") {
		return Compiler::Op::GREATER_THAN_EQU;
	}
	else if (opText == "&&") {
		return Compiler::Op::AND;
	}
	else if (opText == "||") {
		return Compiler::Op::OR;
	}
	else if (opText == "!") {
		return Compiler::Op::NOT;
	}
	else if (opText == "^") {
		return Compiler::Op::POW;
	}
	else {// }else if (opText == "") {
		return Compiler::Op::UNKNOWN;
	}
}



void Compiler::compileExpr(std::string expr, std::vector<val> &instr) {
	std::vector<val> stack;
	std::vector<std::string> chunks;
	Compiler::chunker(expr, chunks);
	//std::cout << "Compiler: " << "Created stack, chunks(" << chunks.size() << ")" << std::endl;
	for (int i = 0; i < chunks.size(); i++) {
		std::string c = chunks[i];
		TYPE type = typeOf(c);
		val v;
		/*v.type = type;*/
		if (type == TYPE_OP) {
			TYPE opType = getOpCode(c);
			v.data.op = opType;
			if (opType == Op::O_PAR) {
				//std::cout << "PUSH (\n";
				stack.push_back(v);
			}else if (opType == Op::O_BRK) {
				//std::cout << "PUSH [\n";
				stack.push_back(v);
			}else if (opType == Op::C_PAR) {     // ]
				while (!stack.empty() &&
					   stack.back().data.op != Op::O_PAR) {
					
					instr.push_back(stack.back());
					//std::cout << "Pop from stack: '" << (instr.back().data.op) << "'" << std::endl;
					stack.pop_back();
				}
				if (stack.empty() || (!stack.empty() &&
					stack.back().data.op != Op::O_PAR)) {
					//std::cout << "Invalid expr" << std::endl;
					instr.clear();
					return;
				} else {
					//std::cout << "Pop '(' from size " << stack.size() << std::endl;
					stack.pop_back(); //pops the (
				}
			} else if (opType == Op::C_BRK) {
				while (!stack.empty() &&
					   stack.back().data.op != Op::O_BRK) {

					instr.push_back(stack.back());
					//std::cout << "Pop from stack: '" << (instr.back().data.op) << "'" << std::endl;
					stack.pop_back();
				}
				if (stack.empty() || (!stack.empty() &&
					stack.back().data.op != Op::O_BRK)) {
					//std::cout << "Invalid expr" << std::endl;
					instr.clear();
					return;
				} else {
					//std::cout << "Pop '(' from size " << stack.size() << std::endl;
					stack.pop_back(); //pops the (
				}
			} else{ //any other op
				//std::cout << "other op\n";
				while (!stack.empty() &&
					   getOpLvl(getOpCode(c)) <= getOpLvl(stack.back().data.op)) {
					//std::cout << c << " vs " << valToString(stack.back()) << " : "
					//	<< std::to_string(getOpLvl(getOpCode(c))) << " <= " << std::to_string(getOpLvl(stack.back().data.op));
					//std::cout << "instr <- stack pop (" << valToString(stack.back()) <<")" << std::endl;
					instr.push_back(stack.back());
					stack.pop_back();
				}
				v.setOp(getOpCode(c));
				stack.push_back(v);
			}
		}else if(type == TYPE_VAR) {
			v.setVar(c);
			instr.push_back(v);
			//std::cout << "Instr push " << valToString(v) << std::endl;
		}else if (type == TYPE_INT ||
				  type == TYPE_DOUBLE) {
			parseNumber(c, v);
			//std::cout << "Just parsed a number of type " << getTypeName(type) << " and prints to \n this "
			// << valToString(v) << std::endl;
			instr.push_back(v);
			//std::cout << "Instr push " << valToString(v) << std::endl;
		} else if (type == TYPE_STRING) {
			v.setStr(c.substr(1, c.size()-2));
			instr.push_back(v);
		} //TODO add bool here
	}
	while (!stack.empty()) {
		instr.push_back(stack.back());
		stack.pop_back();
	}
	//std::cout << "Compiler exited\n";
}

std::string Compiler::getTypeName(const TYPE t)
{
	if (t == TYPE_INT)
		return "INT";
	else if(t == TYPE_DOUBLE)
		return "DOUBLE";
	else if(t == TYPE_STRING)
		return "STRING";
	else if(t == TYPE_INCOMPLETE_STRING)
		return "INCOMPLETE STRING";
	else if(t == TYPE_INVALID)
		return "INVALID";
	else if(t == TYPE_COMPOUND)
		return "COMPOUND";
	else if(t == TYPE_BOOL)
		return "BOOL";
	else if(t == TYPE_OP)
		return "OP";
	else if(t == TYPE_VAR)
		return "VAR";
	else
		return "UNLABELED ("+std::to_string(t)+")";
	
}

Compiler::val Compiler::parseNumber(std::string s, Compiler::val &v) {
	//std::cout << "Parsing: " << s << std::endl;
	
	v._type = TYPE_INT;
	//std::cout << "Mysterious debug " << valToString(v) << std::endl;
	
	std::stringstream ss(s);

	bool failed = ((ss >> v.data.i).fail() || !(ss >> std::ws).eof());
	if (!failed) {
		v._type = TYPE_INT;
		//std::cout << "Read type int\n";
		return v;
	}
	ss = std::stringstream(s);
	failed = ((ss >> v.data.d).fail() || !(ss >> std::ws).eof());
	if (!failed) {
		v._type = TYPE_DOUBLE;
		//std::cout << "Read type double\n";
		return v;
	}
	//std::cout << "Invalid format  D:\n";
	v._type = TYPE_INVALID;
	return v;

	
}

std::string Compiler::rep(std::string s, int i) {
	if (i > 2000) {
		std::string msg = "Attempt to make a string " + std::to_string(i) + " times longer";
		throw std::exception(msg.c_str());
	} else if (i < 0) {
		return rep(std::string(s.rbegin(), s.rend()), -i);
	}
	std::string out = "";
	for (int j = 0; j < i; j++)
		out += s;
	return out;
}

std::string Compiler::valToString(val& v) {
	std::string out = getTypeName(v._type) + " ";
	if (v._type == TYPE_INT) {
		out += std::to_string(v.data.i);
	} else if (v._type == TYPE_DOUBLE) {
		out += std::to_string(v.data.d);
	} else if (v._type == TYPE_STRING) {
		out += "\"";
		out += *v.data.s;
		out += "\"";
	} else if (v._type == TYPE_OP) {
		switch (v.data.op) {
			case Op::ADD: out += "+"; break;
			case Op::SUB: out += "-"; break;
			case Op::MUL: out += "*"; break;
			case Op::DIV: out += "/"; break;
			case Op::MOD: out += "%"; break;
			default: out += std::to_string(v._type);
		}
	} else if (v._type == TYPE_VAR) {
		out += *v.data.s;
	} else if (v._type == TYPE_NIL) {
		out += "NIL";
	}
	return out;
}



void printVector(std::vector<Compiler::val> &v) {
	for (int i = 0; i < v.size(); i++) {
		std::cout << Compiler::valToString(v[i]);
		if (i < v.size() -1)
			std::cout << ", ";
	}
	std::cout << std::endl;
}
void printVector(std::vector<std::string> &v) {
	for (int i = 0; i < v.size(); i++) {
		std::cout << v[i];
		if (i < v.size() - 1)
			std::cout << ", ";
	}
	std::cout << std::endl;
}

Compiler::val Compiler::evaluate(std::string expr, Env &env) {
	std::vector<val> datStk;
	std::vector<val> instr;
	compileExpr(expr, instr);
	
	for (int i = 0; i < instr.size(); i++) {
		val v = instr[i];
		//std::cout << "eval: [" << i << "] " << valToString(v) << std::endl;
		//std::cout << "\tinstr: ";
		//printVector(instr);
		//std::cout << "\tdatsk: ";
		//printVector(datStk);
		if (v._type == TYPE_VAR) {
			/*val tmp = env.get(*v.data.s);
			datStk.push_back(tmp);*/
			datStk.push_back(v);
		} else if (v._type != TYPE_OP) {
			datStk.push_back(instr[i]); //TODO left off here
		} else {
			TYPE op = v.data.op;
			switch (op) {
				case Op::ADD: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if(v2._type==TYPE_VAR) v2 = env.get(*v2.data.s);
					if(v1._type==TYPE_VAR) v1 = env.get(*v1.data.s);
					val ans = v1 + v2;
					datStk.push_back(ans);
					break;
				}
				case Op::SUB: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);
					val ans = v1 - v2;
					datStk.push_back(ans);
					break;
				}
				case Op::MUL: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);

					val ans = v1 * v2;
					datStk.push_back(ans);
					break;
				}
				case Op::DIV: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);

					if (v1._type == TYPE_INT && v2._type == TYPE_INT && ((v1%v2).data.i != 0))
						v2.set((double)v2.data.i);

					val ans = v1 / v2;
					datStk.push_back(ans);
					break;
				}
				case Op::MOD: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);
					val ans = v1 % v2;
					datStk.push_back(ans);
					break;
				}
				case Op::ASSIGNMENT: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); //no pop, no push
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					
					if (v1._type != TYPE_VAR) throw std::exception("Can not assign a value to a non variable");
					//TODO continue here
					//env.set(...) and push
					env.set(*v1.data.s, v2);
					break;
				}
				case Op::EQUALS: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);
					datStk.push_back(val(v1 == v2));
					break;
				}
				case Op::ADD_EQL: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); //doesnt pop, no push needed
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type != TYPE_VAR) throw std::exception("Can not assign a value to a non variable");
					val tmp = env.get(*v1.data.s);
					tmp = tmp + v2;
					env.set(*v1.data.s, tmp);
					break;
				}
				case Op::SUB_EQL: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); //doesnt pop, no push needed
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type != TYPE_VAR) throw std::exception("Can not assign a value to a non variable");
					val tmp = env.get(*v1.data.s);
					tmp = tmp - v2;
					env.set(*v1.data.s, tmp);
					break;
				}
				case Op::NOT_EQUALS:
				{
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);
					datStk.push_back(val(!(v1 == v2)));
					break;
				}
				case Op::LESS_THAN: { 
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);
					datStk.push_back(v1 < v2);
					break;
				}
				case Op::GREATER_THAN: { 
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);
					datStk.push_back(v1 > v2);
					break;
				}
				case Op::LESS_THAN_EQU: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);
					datStk.push_back(v1 <= v2); 
					break;
				}
				case Op::GREATER_THAN_EQU: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);
					datStk.push_back(v1 >= v2);
					break;
				}
				case Op::AND: { 
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);
					bool a = v1.convert(0).data.b;
					bool b = v2.convert(0).data.b;
					datStk.push_back( a && b? v2 : val(false) );
					break;
				}
				case Op::OR: { 
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); datStk.pop_back();
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);
					bool a = v1.convert(0).data.b;
					bool b = v2.convert(0).data.b;
					datStk.push_back( a? v1 : b? v2 : val(false) );
					break;
				}
				case Op::NOT: {
					val v1 = datStk.back(); datStk.pop_back();
					if (v1._type == TYPE_VAR) v1 = env.get(*v1.data.s);
					datStk.push_back(val(v1.convert(0).data.b));
					break;
				}
				case Op::MUL_EQL: { 
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); //doesnt pop, no push needed
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type != TYPE_VAR) throw std::exception("Can not assign a value to a non variable");
					val tmp = env.get(*v1.data.s);
					tmp = tmp * v2;
					env.set(*v1.data.s, tmp);
					break;
				}
				case Op::DIV_EQL: {
					val v2 = datStk.back(); datStk.pop_back();
					val v1 = datStk.back(); //doesnt pop, no push needed
					if (v2._type == TYPE_VAR) v2 = env.get(*v2.data.s);
					if (v1._type != TYPE_VAR) throw std::exception("Can not assign a value to a non variable");
					val tmp = env.get(*v1.data.s);
					tmp = tmp / v2;
					env.set(*v1.data.s, tmp);
					break;
				}
				default:
					throw std::exception("Unsupported operation");
			}
		}
	}
	if (datStk.size() == 0)
		return val();
	return datStk.back();
}



void Compiler::chunker(std::string expr, std::vector<std::string> &chunks)
{
	//std::cout << "_1\n";
	

	int e = 0, s = 0;
	for (; e <= expr.size();) {
		//std::cout << "_2 ("<< s << ", " << e << ") [" << expr.substr(s,e-s) <<"]\n";
		Compiler::TYPE type = Compiler::typeOf(expr.substr(s, e-s));
		//std::cout << "TYPE:  " << getTypeName(type) << std::endl;
		/*std::cout << "  DEBUG:\n    type<0? " << std::to_string(type < 0)
			<< "\n    type != TYPE_INCOMPLETE_STRING? " << std::to_string(type != TYPE_INCOMPLETE_STRING)
			<< std::endl;*/
		if ((type < 0 || type == TYPE_STRING) &&
			(type != TYPE_INCOMPLETE_STRING
				// && Compiler::typeOf(expr.substr(s, e - s - 1)) == TYPE_STRING
			)
			) { //no longer valid and isn't incomplete string
			if (type == TYPE_STRING) { e++; }
			if (e <= s) { e++;  continue; } //error
			Compiler::TYPE typeb = Compiler::typeOf(expr.substr(s, e-s - 1));
			std::string tmp = expr.substr(s, e-s - 1);
			//std::cout << "Possible entry: " << tmp << std::endl;
			if (!Compiler::isWhitespace(tmp)) { //skip empty chunks
				chunks.push_back(tmp);
				//std::cout << "Insert (" << std::to_string(chunks.size()+1) << ") > " << tmp <<std::endl;
			}else if(tmp.size()>0) {
				//std::cout << "skiped whitespace";  //gets stuck here, check if statment checking for 
				//type string complete previously if && not incomplete
			}else{
				e++;
				continue;
			}
			//std::cout << getTypeName(typeb) << std::endl;
			s = e = s + tmp.size();
		}else {
			if (type == Compiler::TYPE_INCOMPLETE_STRING) {
				//std::cout << "INCOMP STR " << expr.substr(s, e-s) << std::endl;
			}
			e++;
		}
	}
	if (e != s) {
		if (e - s >= 2 && Compiler::typeOf(expr.substr(s, e-s-1)) >= 0) {
			std::string s1 = expr.substr(s, e - s - 1);
			std::string s2 = expr.substr(e - 1, expr.size() - e + 1);
			if(!s1.empty())
				chunks.push_back(s1);
			if(!s2.empty())
				chunks.push_back(s2);
		}else{ 
			std::string s1 = expr.substr(s, expr.size() - s);
			if(!s1.empty())
				chunks.push_back(s1);
		}
	}

	//printVector(chunks);

	for (int i = 0; i < chunks.size(); i++) {
		std::string s = chunks[i];
		if (s.size() >= 2 && s[s.size() - 1] == '-' && s != "--") {
			chunks[i] = s.substr(0, s.size() - 1);
			chunks.insert(chunks.begin() + i + 1, "-");
			i++;
		}
	}

	for (int i = 0; i < chunks.size()-1; i++) {
		if (chunks[i] == "-" && (i == 0 ||
			(i>0 && (!isNumber(chunks[i-1])) && (chunks[i-1]!=")")))) {
			if (chunks[i + 1] == "(") {
				int level = 0;
				int j;
				for (j = i + 1; j < chunks.size(); j++) {
					if (chunks[j] == ")") level--;
					if (chunks[j] == "(") level++;
					if (level == 0) break;
				}
				chunks.insert(chunks.begin() + j, ")");
				chunks.insert(chunks.begin() + i, "(");
				chunks.insert(chunks.begin() + i + 1, "0");
				i += 3;
			} else if (isNumber(chunks[i + 1])) {
				chunks.insert(chunks.begin() + i + 2, ")");
				chunks.insert(chunks.begin() + i, "(");
				chunks.insert(chunks.begin() + i + 1, "0");
				i += 3;
			}
		}

	}



	//printVector(chunks);

	//std::cout << "exit chunker\n";
	return;
}

bool operator==(Compiler::val & v1, const Compiler::val & v2) {
	if (v1._type == Compiler::TYPE_STRING && v2._type == Compiler::TYPE_STRING)
		return (*v1.data.s) == (*v2.data.s);
	if (Compiler::isNumberType(v1._type) && Compiler::isNumberType(v2._type)) {
		Compiler::val m = v1 - v2;
		if (m._type == Compiler::TYPE_DOUBLE)
			return abs(m.data.d) < Compiler::TOL;
		else if (m._type == Compiler::TYPE_INT)
			return m.data.i == 0;
	}
	return false;
}

bool operator<(Compiler::val & v1, const Compiler::val & v2) {
	if (v1._type != v2._type) {
		std::string msg = "Attempt to compare " + Compiler::getTypeName(v1._type) + " to " + Compiler::getTypeName(v2._type);
		throw std::exception(msg.c_str());
	}
	if (v1._type == Compiler::TYPE_STRING)
		return (*v1.data.s) < (*v2.data.s);
	else if (Compiler::isNumberType(v1._type)) {
		Compiler::val m = v1 - v2;
		if (m._type == Compiler::TYPE_DOUBLE)
			return m.data.d < 0;
		else if (m._type == Compiler::TYPE_INT)
			return m.data.i < 0;
	} else if (v1._type == Compiler::TYPE_BOOL)
		return v2.data.b && !v1.data.b;
}

bool operator>(Compiler::val & v1, const Compiler::val & v2) {
	if (v1._type != v2._type) {
		std::string msg = "Attempt to compare " + Compiler::getTypeName(v1._type) + " to " + Compiler::getTypeName(v2._type);
		throw std::exception(msg.c_str());
	}
	if (v1._type == Compiler::TYPE_STRING)
		return (*v1.data.s) > (*v2.data.s);
	else if (Compiler::isNumberType(v1._type)) {
		Compiler::val m = v1 - v2;
		if (m._type == Compiler::TYPE_DOUBLE)
			return m.data.d > 0;
		else if (m._type == Compiler::TYPE_INT)
			return m.data.i > 0;
	} else if (v1._type == Compiler::TYPE_BOOL)
		return !v2.data.b && v1.data.b;
}

bool operator<=(Compiler::val & v1, const Compiler::val & v2) {
	return (v1 < v2 || v1 == v2);
}

bool operator>=(Compiler::val & v1, const Compiler::val & v2) {
	return (v1 > v2 || v1 == v2);
}
