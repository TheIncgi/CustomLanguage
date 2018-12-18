#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <map>
class Env;
#include "Env.h"
namespace Compiler
{
	struct val;
	typedef int TYPE;
	const TYPE TYPE_NIL = -5;
	const TYPE TYPE_INCOMPLETE_STRING = -4;
	const TYPE TYPE_INVALID = -3; //incorrect formating
	const TYPE TYPE_COMPOUND = -2; //invalid
	const TYPE TYPE_UNKNOWN = -1;
	const TYPE TYPE_INT = 0;
	const TYPE TYPE_DOUBLE = 1;
	const TYPE TYPE_STRING = 2;
	const TYPE TYPE_BOOL = 3;
	const TYPE TYPE_OP = 4;
	const TYPE TYPE_VAR = 5;

	namespace Op {
		const TYPE INVALID = -3;
		const TYPE INCOMPLETE = -2;
		const TYPE UNKNOWN = -1;
		const TYPE ADD = 0;
		const TYPE SUB = 1;
		const TYPE MUL = 3;
		const TYPE DIV = 4;
		const TYPE MOD = 5;
		const TYPE O_PAR = 6;
		const TYPE C_PAR = 7;
		const TYPE O_BRK = 8; //squar brackets []
		const TYPE C_BRK = 9;
		const TYPE ASSIGNMENT = 10;
		const TYPE EQUALS = 11;
		const TYPE ADD_EQL = 12;
		const TYPE SUB_EQL = 13;
		const TYPE NOT_EQUALS = 14;
		const TYPE LESS_THAN = 15;
		const TYPE GREATER_THAN = 16;
		const TYPE LESS_THAN_EQU = 17;
		const TYPE GREATER_THAN_EQU = 18;
		const TYPE AND = 19;
		const TYPE OR = 20;
		//const TYPE XOR = 21;
		const TYPE NOT = 22;
		const TYPE MUL_EQL = 23;
		const TYPE DIV_EQL = 24;
		const TYPE POW = 25;
	}

	
	TYPE typeOf(std::string chunk);
	//val valueOf(std::string chunk);
	inline bool isLetter(char c) {
		return ('a' <= c && c <= 'z') ||
			('A' <= c && c <= 'Z');
	}
	inline bool isNumber(char c) {
		return '0' <= c && c <= '9';
	}
	inline bool isNumber(std::string s) {
		int dot = 0;
		for (int i = 0; i < s.size(); i++) {
			if (!isNumber(s[i])) return false;
			if (s[i] == '.') dot++;
			if (dot >= 2) return false;
		}
		return true;
	}
	inline bool isWhitespace(char c) {
		return c == ' ' || c == '\t' || c == '\n';
	}
	inline bool isWhitespace(std::string s) {
		for (int i = 0; i < s.size(); i++) {
			if (!isWhitespace(s[i])) return false;
		}
		return true;
	}
	inline bool isQuote(char c) {
		return c == '\'' || c == '"';
	}
	inline bool isSymbol(char c) {
		return ! (isWhitespace(c) || isLetter(c) || isNumber(c) || isQuote(c));
	}
	inline bool isNumberType(TYPE t) {
		return t == TYPE_INT || t == TYPE_DOUBLE;
	}
	std::string rep(std::string s, int i);
	
	int getOpLvl(TYPE op);
	TYPE getOpCode(std::string opText);
	
	//needs pass by ref to work
	void chunker(std::string expr, std::vector<std::string> &out);
	void compileExpr(std::string expr, std::vector<val> &instr);
	std::string getTypeName(TYPE t);
	Compiler::val parseNumber(std::string s, val &v);
	std::string valToString(val &v);
	val evaluate(std::string expr, Env &env);  //FIXME


	
	const int TOL = .00000001;
	struct val {
		TYPE _type = TYPE_NIL;
		union Data {
			int i;
			double d;
			std::string* s;
			bool b;
			TYPE op;
			Data() {}
			~Data() {}
		} data;
		void set(int i) {
			reset();
			_type = TYPE_INT;
			data.i = i;
		}
		void set(double d) {
			reset();
			_type = TYPE_DOUBLE;
			data.d = d;
		}
		void setStr(std::string s) {
			reset();
			_type = TYPE_STRING;
			data.s = new std::string(s);
		}
		void setVar(std::string s) {
			reset();
			_type = TYPE_VAR;
			data.s = new std::string(s);
		}
		void setBool(bool b) {
			reset();
			_type = TYPE_BOOL;
			data.b = b;
		}
		void setOp(TYPE op) {
			reset();
			_type = TYPE_OP;
			data.op = op;
		}
		void reset() {
			if ((_type == TYPE_STRING ||
				_type == TYPE_VAR)
				&& data.s != nullptr) {
				delete data.s;
			}
			_type = TYPE_INVALID;
		}
		
		void assertTypes(TYPE t1, TYPE t2, TYPE op) {
			if (t1 == TYPE_INVALID || t2 == TYPE_INVALID) {
				throw std::exception("Attempt to math on nil");
			}
		}

		val operator+(const val v2) {
			if (this->_type == v2._type) {
				switch (_type) {
					case TYPE_INT:
						return val(data.i + v2.data.i);
					case TYPE_DOUBLE:
						return val(data.d + v2.data.d);
					case TYPE_STRING:
						return val((*(data.s)) + (*(v2.data.s)));
					case TYPE_NIL:
						throw std::exception("Attempt to add nil with nil");
				}
			} else {
				int l1 = getTypeLevel(_type);
				int l2 = getTypeLevel(v2._type);
				val v1 = *this;
				val v3 = v2;
				if (l1 < l2) {
					v1 = v1.convert(l2);
				} else {
					v3 = v3.convert(l1);
				}
				return v1 + v3;
			}
		}
		val operator-(const val v2) {
			if (_type == TYPE_NIL || v2._type == TYPE_NIL) {
				std::string msg = "Attempt to subtract " + getTypeName(_type) + " with " + getTypeName(v2._type);
				throw std::exception();
			}
			if (this->_type == v2._type) {
				switch (_type) {
					case TYPE_INT:
						return val(data.i - v2.data.i);
					case TYPE_DOUBLE:
						return val(data.d - v2.data.d);
					case TYPE_STRING:
						throw std::exception("Attempt to subract string with string");
				}
			} else {
				if (_type == TYPE_STRING || v2._type == TYPE_STRING) {
					std::string msg = "Attempt to subtract " + getTypeName(_type) + " with " + getTypeName(v2._type);
					throw std::exception();
				}
				int l1 = getTypeLevel(_type);
				int l2 = getTypeLevel(v2._type);
				val v1 = *this;
				val v3 = v2;
				if (l1 < l2) {
					v1 = v1.convert(l2);
				} else {
					v3 = v3.convert(l1);
				}
				return v1 - v3;
			}
		}
		val operator*(const val v2) {
			if (_type == TYPE_NIL || v2._type == TYPE_NIL) {
				std::string msg = "Attempt to multiply " + getTypeName(_type) + " with " + getTypeName(v2._type);
				throw std::exception();
			}
			if (this->_type == v2._type) {
				switch (_type) {
					case TYPE_INT:
						return val(data.i * v2.data.i);
					case TYPE_DOUBLE:
						return val(data.d * v2.data.d);
					case TYPE_STRING:
						throw std::exception("Attempt to multiply string with string");
				}
			} else {
				if (_type == TYPE_STRING || v2._type == TYPE_STRING) {
					if (_type == TYPE_INT) {
						return val(rep(*v2.data.s, data.i));
					} else if (v2._type == TYPE_INT) {
						return val(rep(*data.s, v2.data.i));
					}
					std::string msg = "Attempt to multiply " + getTypeName(_type) + " to " + getTypeName(v2._type);
					throw std::exception();
				}
				int l1 = getTypeLevel(_type);
				int l2 = getTypeLevel(v2._type);
				val v1 = *this;
				val v3 = v2;
				if (l1 < l2) {
					v1 = v1.convert(l2);
				} else {
					v3 = v3.convert(l1);
				}
				return v1 * v3;
			}
		}
		val operator/(const val v2) {
			if (_type == TYPE_NIL || v2._type == TYPE_NIL) {
				std::string msg = "Attempt to divide " + getTypeName(_type) + " with " + getTypeName(v2._type);
				throw std::exception();
			}
			if (this->_type == v2._type) {
				switch (_type) {
					case TYPE_INT:
						return val(data.i / v2.data.i);
					case TYPE_DOUBLE:
						return val(data.d / v2.data.d);
					case TYPE_STRING:
						throw std::exception("Attempt to divide string with string");
				}
			} else {
				if (_type == TYPE_STRING || v2._type == TYPE_STRING) {
					std::string msg = "Attempt to divide " + getTypeName(_type) + " with " + getTypeName(v2._type);
					throw std::exception();
				}
				int l1 = getTypeLevel(_type);
				int l2 = getTypeLevel(v2._type);
				val v1 = *this;
				val v3 = v2;
				if (l1 < l2) {
					v1 = v1.convert(l2);
				} else {
					v3 = v3.convert(l1);
				}
				return v1 / v3;
			}
		}
		val operator%(const val v2) {
			if (_type == TYPE_NIL || v2._type == TYPE_NIL) {
				std::string msg = "Attempt to preform modulo on " + getTypeName(_type) + " with " + getTypeName(v2._type);
				throw std::exception();
			}
			if (this->_type == v2._type) {
				switch (_type) {
					case TYPE_INT:
						return val(data.i % v2.data.i);
					case TYPE_DOUBLE:
					{
						double m = data.d / v2.data.d;
						int    n = (int)m;
						m = m - n;
						return val(m);
					}
					case TYPE_STRING:
						throw std::exception("Attempt to divide string with string");
				}
			} else {
				if (_type == TYPE_STRING || v2._type == TYPE_STRING) {
					std::string msg = "Attempt to preform modulo on " + getTypeName(_type) + " with " + getTypeName(v2._type);
					throw std::exception();
				}
				int l1 = getTypeLevel(_type);
				int l2 = getTypeLevel(v2._type);
				val v1 = *this;
				val v3 = v2;
				if (l1 < l2) {
					v1 = v1.convert(l2);
				} else {
					v3 = v3.convert(l1);
				}
				return v1 % v3;
			}
		}
		val& operator=(const val &v2) {
			if (v2._type == TYPE_VAR || v2._type == TYPE_STRING) {
				this->setStr(*v2.data.s);
			} else {
				data = v2.data;
			}
			_type = v2._type;
			return *this;
		}

		
		/*const TYPE BOOL_LEVEL = 0;
		const TYPE INT_LEVEL = 1;
		const TYPE DOUBLE_LEVEL = 2;
		const TYPE STRING_LEVEL = 3;*/

		TYPE getTypeLevel(TYPE type) {
			switch (type) {
				case TYPE_BOOL:   return 0;
				case TYPE_INT:    return 1;
				case TYPE_DOUBLE: return 2;
				case TYPE_STRING: return 3;
			}
			return -1;
		}
		val convert(const TYPE level) {
			switch (level) {
				case 0:{//bool
					val v;
					v.setBool((bool)(_type == TYPE_STRING) ||
						(_type == TYPE_INT && data.i) ||
							  (_type == TYPE_DOUBLE && (data.d < 0 ? (-data.d) < TOL : (data.d < TOL))));
					return v;
				}
				case 1: //int
					switch (_type) {
						case TYPE_BOOL:
							return val(data.b);
						case TYPE_INT:
							return val(data.i);
						case TYPE_DOUBLE:
							return val((int)data.d);
						case TYPE_STRING:
						{
							val tmp;
							parseNumber(*data.s, tmp);
							if (tmp._type == TYPE_DOUBLE)
								tmp.set((int)tmp.data.d);
							if (tmp._type == TYPE_INT)
								return tmp;
							throw std::exception("Could not convert string to type int");
						}
						default:
							throw std::exception("Unsupported type conversion");
					}
				case 2: //double
					switch (_type) {
						case TYPE_BOOL:
							return val((double)data.b);
						case TYPE_INT:
							return val((double)data.i);
						case TYPE_DOUBLE:
							return val(data.d);
						case TYPE_STRING:
						{
							val tmp;
							parseNumber(*data.s, tmp);
							if (tmp._type == TYPE_INT)
								tmp.set((double)tmp.data.i);
							if (tmp._type == TYPE_DOUBLE)
								return tmp;
							throw std::exception("Could not convert string to type double");
						}
						default:
							throw std::exception("Unsupported type conversion");
					}
				case 3: //TYPE_STRING
					switch (_type) {
						case TYPE_BOOL:
							return val(data.b ? "true" : "false");
						case TYPE_INT: {
						   std::string tmp = std::to_string(data.i);
						   return val(tmp);
						}
						case TYPE_DOUBLE: {
							std::string tmp = std::to_string(data.d);
							return val(tmp);
						}
						case TYPE_STRING:
							return val(*data.s);
						default:
							throw std::exception("Unsupported type conversion");
					}
				default:
					std::string append = (
						level == 0 ? "bool" : (
						level == 1 ? "int" : (
						level == 2 ? "double" :
						"?")));
					std::string msg = "Unable to convert type " + getTypeName(_type) + " to " + append;
					throw std::exception(msg.c_str());
			}
		}

		

		val() : data(), _type(TYPE_INVALID) {
			data.i = 0;
		}
		val(bool b) : val() { setBool(b); }
		val(int i) : val() { set(i); }
		val(double d) : val() { set(d); }
		val(std::string s) : val() { setStr(s); }
		val(const val &v) {
			this->_type = v._type;
			if (v._type == TYPE_STRING ||
				v._type == TYPE_VAR) {
				this->data.s = new std::string(*v.data.s);
			} else {
				this->data = v.data;
			}
		}
		~val() { reset(); }
	};
};


bool operator ==(Compiler::val &v1, const Compiler::val &v2);
bool operator <(Compiler::val &v1, const Compiler::val &v2);
bool operator >(Compiler::val &v1, const Compiler::val &v2);
bool operator <=(Compiler::val &v1, const Compiler::val &v2);
bool operator >=(Compiler::val &v1, const Compiler::val &v2);


