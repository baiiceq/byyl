#include "ParserAndSemanticAnalyser.h"

list<int>merge(list<int>&l1, list<int>&l2) 
{
	list<int>ret;
	ret.assign(l1.begin(), l1.end());
	ret.splice(ret.end(), l2);
	return ret;
}

bool operator ==(const Symbol&one, const Symbol&other) 
{
	return one.content == other.content;
}
bool operator <(const Symbol&one, const Symbol&other) 
{
	return one.content < other.content;
}
bool operator>(const Symbol& one, const Symbol& other)
{
	return one.content > other.content;
}
bool operator < (const Item&one, const Item& other) 
{
	return pair<int, int>(one.pro, one.point_pos) < pair<int, int>(other.pro, other.point_pos);
}
bool operator ==(const Item&one, const Item& other)
{
	return one.pro == other.pro&&one.point_pos == other.point_pos;
}

Symbol::Symbol(const Symbol& sym) 
{
	this->content = sym.content;
	this->is_Vt = sym.is_Vt;
}

Symbol::Symbol(const bool & is_Vt, const string& content)
{
	this->is_Vt = is_Vt;
	this->content = content;
}

NewTemper::NewTemper() 
{
	now = 0;
}

string NewTemper::new_temp() 
{
	return string("$T") + to_string(now++);
}

Symbol::Symbol() 
{
	is_Vt = false;
	content = "";
}

Id::Id(const Symbol& sym, const string& name) : Symbol(sym) 
{
	this->name = name;
}

NumInt::NumInt(const Symbol& sym, const string& number) : Symbol(sym) 
{
	this->number = number;
}

NumFloat::NumFloat(const Symbol& sym, const string& number) : Symbol(sym)
{
	this->number = number;
}

Char::Char(const Symbol& sym, const string& ascii) : Symbol(sym)
{
	this->ascii = ascii;
}

FunctionDeclare::FunctionDeclare(const Symbol& sym) : Symbol(sym) {}

Parameter::Parameter(const Symbol& sym) : Symbol(sym) {}

Param::Param(const Symbol& sym, DType t) : Symbol(sym), type(t) {}

ParameterList::ParameterList(const Symbol& sym) : Symbol(sym) {}

SentenceBlock::SentenceBlock(const Symbol& sym) : Symbol(sym) {}

SentenceList::SentenceList(const Symbol& sym) : Symbol(sym) {}

Sentence::Sentence(const Symbol& sym) : Symbol(sym) {}

WhileSentence::WhileSentence(const Symbol& sym) : Symbol(sym) {}

IfSentence::IfSentence(const Symbol& sym) : Symbol(sym) {}

Expression::Expression(const Symbol& sym) : Symbol(sym) {}

M::M(const Symbol& sym) : Symbol(sym) {}

N::N(const Symbol& sym) : Symbol(sym) {}

AddExpression::AddExpression(const Symbol& sym) : Symbol(sym) {}

Nomial::Nomial(const Symbol& sym) : Symbol(sym) {}

Factor::Factor(const Symbol& sym) : Symbol(sym) {}

ArgumentList::ArgumentList(const Symbol& sym) : Symbol(sym) {}

Initializer::Initializer(const Symbol& sym) : Symbol(sym) {}


bool isVT(string s)
{
	if (s == "int" || s == "void" || s == "if" || s == "while" || s == "else" || s == "return" || s == "float" || s == "char")
	{
		return true;
	}
	if (s == "+" || s == "-" || s == "*" || s == "/" || s == "=" || s == "==" || s == ">" || s == "<" || s == "!=" || s == ">=" || s == "<=")
	{
		return true;
	}
	if (s == ";" || s == "," || s == "(" || s == ")" || s == "{" || s == "}" || s == "ID" || s == "NUM_INT" || s == "NUM_FLOAT")
	{
		return true;
	}
	return false;
}

ParserAndSemanticAnalyser::ParserAndSemanticAnalyser(const char*fileName) 
{
	readProductions(fileName);
	getFirst();
	getFollow();
	createDFA();
	line_count = 1;
}

void ParserAndSemanticAnalyser::getFirst() 
{
	bool change_flag = true;
	while (change_flag) 
	{
		change_flag = false;//first集改变标志
		//遍历每一个产生式
		for (vector<Production>::iterator iter = productions.begin(); iter != productions.end(); iter++)
		{
			vector<Symbol>::iterator sym_iter;
			//依次遍历产生式右部的所有符号
			for (sym_iter = iter->right.begin(); sym_iter != iter->right.end(); sym_iter++)
			{
				//这个右部符号是终结符
				if (sym_iter->is_Vt) 
				{
					if (first.count(iter->left) == 0) 
					{
						first[iter->left] = set<Symbol>();
					}
					//左部符号的first集不包含该右部符号
					if (first[iter->left].insert(*sym_iter).second == true) 
					{
						change_flag = true;
					}
					break;
				}
				//当前右部符号是非终结符
				else 
				{
					bool continue_flag = false;//是否继续读取下一个右部符号的first集
					set<Symbol>::iterator first_iter;
					//遍历该右部符号的first集
					for (first_iter = first[*sym_iter].begin(); first_iter != first[*sym_iter].end(); first_iter++)
					{
						//右部符号的first集中的元素包含EMPTY
						if (first_iter->content == "EMPTY")
						{
							continue_flag = true;
						}
						//右部符号的first集中的元素不在左部符号first集中
						else if (first[iter->left].find(*first_iter) == first[iter->left].end())
						{
							if (first.count(iter->left) == 0)
							{
								first[iter->left] = set<Symbol>();
							}
							first[iter->left].insert(*first_iter);
							change_flag = true;
						}
					}
					if (!continue_flag) 
					{
						break;
					}
				}
			}
			//遍历右部符号到了末尾,则EMPTY在其first集中
			if (sym_iter == iter->right.end()) 
			{
				if (first.count(iter->left) == 0)
				{
					first[iter->left] = set<Symbol>();
				}
				if (first[iter->left].insert(Symbol{ true,"EMPTY" }).second == true)
				{
					change_flag = true;
				}
			}
		}

	}
}

void ParserAndSemanticAnalyser::getFollow()
{
	//将#放入起始符号的FOLLOW集中
	follow[productions[0].left] = set<Symbol>();
	follow[productions[0].left].insert(Symbol{ true,"#" });
	bool change_flag = true;
	while (change_flag) 
	{
		change_flag = false;
		//遍历每一个产生式
		for (vector<Production>::iterator pro_iter = productions.begin(); pro_iter != productions.end(); pro_iter++) 
		{
			//遍历产生式右部的每个符号
			for (vector<Symbol>::iterator sym_iter = pro_iter->right.begin(); sym_iter != pro_iter->right.end(); sym_iter++) 
			{
				//遍历产生式右部该符号之后的符号
				vector<Symbol>::iterator next_symbol_iter;
				for (next_symbol_iter = sym_iter + 1; next_symbol_iter != pro_iter->right.end(); next_symbol_iter++)
				{
					Symbol nextSym = *next_symbol_iter;
					bool nextFlag = false;
					//如果之后的符号是终结符
					if (nextSym.is_Vt)
					{
						if (follow.count(*sym_iter) == 0) 
						{
							follow[*sym_iter] = set<Symbol>();
						}
						//如果成功插入新值
						if (follow[*sym_iter].insert(nextSym).second == true)
						{
							change_flag = true;
						}
					}
					else 
					{
						//遍历之后符号的first集
						for (set<Symbol>::iterator f_iter = first[nextSym].begin(); f_iter != first[nextSym].end(); f_iter++) {
							//如果当前符号first集中有 空串
							if (f_iter->content == "EMPTY") 
							{
								nextFlag = true;
							}
							else 
							{
								if (follow.count(*sym_iter) == 0) 
								{
									follow[*sym_iter] = set<Symbol>();
								}
								//如果成功插入新值
								if (follow[*sym_iter].insert(*f_iter).second == true) 
								{
									change_flag = true;
								}
							}
						}
					}
					//如果当前符号first集中没有 空串
					if (!nextFlag) 
					{
						break;
					}

				}
				//如果遍历到了结尾,将左部符号的FOLLOW集加入其FOLLOW集中
				if (next_symbol_iter == pro_iter->right.end())
				{
					//遍历左部符号的FOLLOW集
					for (set<Symbol>::iterator followIter = follow[pro_iter->left].begin(); followIter != follow[pro_iter->left].end(); followIter++)
					{
						if (follow.count(*sym_iter) == 0)
						{
							follow[*sym_iter] = set<Symbol>();
						}
						//如果该FOLLOW集是新值
						if (follow[*sym_iter].insert(*followIter).second == true)
						{
							change_flag = true;
						}
					}
				}
			}
		}
	}
}

void ParserAndSemanticAnalyser::outputDFA(ostream& out)
{
	int nowI = 0;
	for (list<DFAState>::iterator iter = dfa.stas.begin(); iter != dfa.stas.end(); iter++, nowI++)
	{
		out << "I" << nowI << "= [";
		for (set<Item>::iterator itIter = iter->items.begin(); itIter != iter->items.end(); itIter++) 
		{
			out << "【";
			Production p = productions[itIter->pro];
			out << p.left.content << " -> ";
			for (vector<Symbol>::iterator symIter = p.right.begin(); symIter != p.right.end(); symIter++) 
			{
				if (symIter - p.right.begin() == itIter->point_pos)
				{
					out << ". ";
				}
				out << symIter->content << " ";
			}
			if (p.right.size() == itIter->point_pos)
			{
				out << ". ";
			}
			out << "】";
		}
		out << "]" << endl << endl;
	}
}

void ParserAndSemanticAnalyser::outputDFA()
{
	outputDFA(cout);
}

void ParserAndSemanticAnalyser::outputDFA(const char* fileName) 
{
	ofstream fout;
	fout.open(fileName);
	if (!fout.is_open())
	{
		outputError("文件" + string(fileName) + "打开失败");
	}
	outputDFA(fout);

	fout.close();
}

void ParserAndSemanticAnalyser::readProductions(const char*fileName) 
{
	ifstream fin;

	//文件打开处理
	fin.open(fileName, ios::in);
	if (!fin.is_open())
	{
		outputError("文件" + string(fileName) + "打开失败");
	}

	int index = 0;//产生式序号
	char buf[1024];
	while (fin >> buf) 
	{
		Production p;
		//产生式序号赋值
		p.id = index++;

		//产生式左部赋值
		p.left = Symbol{ false,string(buf) };

		//中间应为::=
		fin >> buf;
		assert(strcmp(buf, "::=") == 0);

		//产生式右部赋值
		fin.getline(buf, 1024);
		stringstream sstream(buf);
		string temp;
		while (sstream >> temp) 
		{
			p.right.push_back(Symbol{ isVT(temp),string(temp) });
		}

		//插入产生式
		productions.push_back(p);
	}
}

DFAState ParserAndSemanticAnalyser::derive(Item item) 
{
	DFAState new_state;
	// .在项目产生式的最右边，即是一个规约项目
	if (productions[item.pro].right.size() == item.point_pos)
	{
		new_state.items.insert(item);
	}
	// .的右边是终结符
	else if (productions[item.pro].right[item.point_pos].is_Vt) 
	{
		new_state.items.insert(item);
	}
	// .的右边是非终结符
	else
	{
		new_state.items.insert(item);
		vector<Production>::iterator iter;
		for (iter = productions.begin(); iter < productions.end(); iter++) 
		{
			//产生式的左部 == .右边的非终结符
			if (iter->left == productions[item.pro].right[item.point_pos]) 
			{
				//将产生式的派生加入I中
				DFAState temp = derive(Item{ (int)(iter - productions.begin()),0 });

				set<Item>::iterator siter;
				for (siter = temp.items.begin(); siter != temp.items.end(); siter++) {
					new_state.items.insert(*siter);
				}
			}
		}
	}

	return new_state;
}

void ParserAndSemanticAnalyser::createDFA() 
{
	bool new_flag = true;//有新的状态产生标志
	int now_state = 0;//当前状态的编号
	dfa.stas.push_back(derive(Item{ 0,0 }));
	//遍历每一个状态
	for (list<DFAState>::iterator iter = dfa.stas.begin(); iter != dfa.stas.end(); iter++, now_state++) 
	{
		//遍历状态的每一个项目
		for (set<Item>::iterator itIter = iter->items.begin(); itIter != iter->items.end(); itIter++)
		{
			// .在项目产生式的最右边，即是一个规约项目
			if (productions[itIter->pro].right.size() == itIter->point_pos)
			{
				set<Symbol>FOLLOW = follow[productions[itIter->pro].left];
				for (set<Symbol>::iterator followIter = FOLLOW.begin(); followIter != FOLLOW.end(); followIter++) 
				{
					if (SLR1_table.count(GOTO(now_state, *followIter)) == 1) 
					{
						string err = "文法不是SLR1文法，移进规约冲突";
						//err += string("GOTO(") + to_string(nowI) + "," + followIter->content + ")=" + to_string(SLR1_Table[GOTO(nowI, *followIter)].nextStat);
						outputError(err);
					}
					if (itIter->pro == 0) // OK
					{
						SLR1_table[GOTO(now_state, *followIter)] = Behavior{ accept,itIter->pro };
					}
					else
					{
						SLR1_table[GOTO(now_state, *followIter)] = Behavior{ reduce,itIter->pro };
					}

				}
				continue;
			}
			Symbol next_symbol = productions[itIter->pro].right[itIter->point_pos];//.之后的符号

			//DFA中GOTO(now_state,next_symbol)已经存在
			if (dfa.goTo.count(GOTO(now_state, next_symbol)) == 1) 
			{
				continue;
			}

			DFAState new_state = derive(Item{ itIter->pro,itIter->point_pos + 1 });//新产生的状态

			//查找当前状态中其他GOTO[nowI,nextSymbol]
			//shiftIter指向当前状态项目的下一个项目
			set<Item>::iterator shiftIter = itIter;
			shiftIter++;
			for (; shiftIter != iter->items.end(); shiftIter++)
			{
				//如果是规约项目
				if (productions[shiftIter->pro].right.size() == shiftIter->point_pos)
				{
					continue;
				}
				//如果是移进项目，且移进nextSymbol
				else if (productions[shiftIter->pro].right[shiftIter->point_pos] == next_symbol)
				{
					DFAState temp_state = derive(Item{ shiftIter->pro,shiftIter->point_pos + 1 });
					new_state.items.insert(temp_state.items.begin(), temp_state.items.end());
				}
			}
			//查找已有状态中是否已经包含该状态
			bool searchFlag = false;
			int index = 0;//当前状态的编号
			for (list<DFAState>::iterator iterHave = dfa.stas.begin(); iterHave != dfa.stas.end(); iterHave++, index++)
			{
				if (iterHave->items == new_state.items)
				{
					dfa.goTo[GOTO(now_state, next_symbol)] = index;
					if (SLR1_table.count(GOTO(now_state, next_symbol)) == 1)
					{
						outputError("confict");
					}
					SLR1_table[GOTO(now_state, next_symbol)] = Behavior{ shift,index };
					searchFlag = true;
					break;
				}
			}

			//没有在已有状态中找到该状态
			if (!searchFlag)
			{
				dfa.stas.push_back(new_state);
				dfa.goTo[GOTO(now_state, next_symbol)] = dfa.stas.size() - 1;
				if (SLR1_table.count(GOTO(now_state, next_symbol)) == 1)
				{
					outputError("confict");
				}
				SLR1_table[GOTO(now_state, next_symbol)] = Behavior{ shift,int(dfa.stas.size() - 1) };
			}
			else 
			{
				continue;
			}

		}
	}
}

Func* ParserAndSemanticAnalyser::lookUpFunc(string ID)
{
	for (vector<Func>::iterator iter = func_table.begin(); iter != func_table.end(); iter++) 
	{
		if (iter->name == ID) 
		{
			return &(*iter);
		}
	}
	return NULL;
}

Var* ParserAndSemanticAnalyser::lookUpVar(string ID) 
{
	for (vector<Var>::reverse_iterator iter = var_table.rbegin(); iter != var_table.rend(); iter++)
	{
		if (iter->name == ID) 
		{
			return &(*iter);
		}
	}
	return NULL;
}

bool ParserAndSemanticAnalyser::march(list<string>&argument_list, list<DType>&parameter_list) 
{
	if (argument_list.size() != parameter_list.size())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void ParserAndSemanticAnalyser::outputSymbolStack(ostream& out) 
{
	stack<Symbol*>temp = symbol_stack;
	stack<Symbol*>other;
	while (!temp.empty()) 
	{
		other.push(temp.top());
		temp.pop();
	}
	while (!other.empty()) 
	{
		out << other.top()->content << " ";
		other.pop();
	}
	out << endl;
}

void ParserAndSemanticAnalyser::outputStateStack(ostream& out)
{
	stack<int>temp = state_stack;
	stack<int>other;
	while (!temp.empty()) 
	{
		other.push(temp.top());
		temp.pop();
	}
	while (!other.empty()) 
	{
		out << other.top() << " ";
		other.pop();
	}
	out << endl;

}

void ParserAndSemanticAnalyser::outputIntermediateCode()
{
	code.output();
}

void ParserAndSemanticAnalyser::outputIntermediateCode(const char* fileName) 
{
	code.output(fileName);
}

Symbol* ParserAndSemanticAnalyser::popSymbol() 
{
	Symbol* ret = symbol_stack.top();
	symbol_stack.pop();
	state_stack.pop();
	return ret;
}

void ParserAndSemanticAnalyser::pushSymbol(Symbol* sym) 
{
	symbol_stack.push(sym);
	if (SLR1_table.count(GOTO(state_stack.top(), *sym)) == 0) 
	{
		outputError(string("语法错误：第") + to_string(line_count) + "行，不期待的符号" + sym->content);
	}
	Behavior bh = SLR1_table[GOTO(state_stack.top(), *sym)];
	state_stack.push(bh.next_stat);
}

void ParserAndSemanticAnalyser::analyse(list<Token>&words, ostream& out) 
{
	bool acc = false;
	symbol_stack.push(new Symbol(true, "#"));
	state_stack.push(0);
	for (list<Token>::iterator iter = words.begin(); iter != words.end(); ) 
	{
		LexicalType LT = iter->first;
		string word = iter->second;

		//忽略行注释和段注释
		if (LT == LCOMMENT || LT == PCOMMENT)
		{
			iter++;
			continue;
		}
		if (LT == NEXTLINE)
		{
			line_count++;
			iter++;
			continue;
		}

		outputSymbolStack(out);
		outputStateStack(out);

		Symbol* next_symbol;
		if (LT == ID)
		{
			next_symbol = new Id(Symbol{ true,"ID" }, word);
		}
		else if (LT == NUM_INT)
		{
			next_symbol = new NumInt(Symbol{ true,"NUM_INT" }, word);
		}
		else if (LT == NUM_FLOAT)
		{
			next_symbol = new NumFloat(Symbol{ true,"NUM_FLOAT" }, word);
		}
		else if (LT == CHAR_VAL)
		{
			next_symbol = new Char(Symbol{ true,"CHAR_VAL" }, word);
		}
		else 
		{
			next_symbol = new Symbol(true, word);
		}

		if (SLR1_table.count(GOTO(state_stack.top(), *next_symbol)) == 0) 
		{
			outputError(string("语法错误：第")+to_string(line_count)+"行，不期待的符号"+next_symbol->content);
		}

		Behavior bh = SLR1_table[GOTO(state_stack.top(), *next_symbol)];

		// 移进
		if (bh.behavior == shift) 
		{
			symbol_stack.push(next_symbol);
			state_stack.push(bh.next_stat);
			iter++;
		}
		// 规约
		else if (bh.behavior == reduce)
		{
			Production reduce_pro = productions[bh.next_stat];
			int pop_symbol_cnt = reduce_pro.right.size();
			switch (bh.next_stat) 
			{
				case 3://declare ::= int ID M A function_declare
				{
					FunctionDeclare *function_declare = (FunctionDeclare*)popSymbol();
					Symbol* A = popSymbol();
					M* m = (M*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _int = popSymbol();
					func_table.push_back(Func{ ID->name,D_INT,function_declare->plist,m->quad });
					pushSymbol(new Symbol(reduce_pro.left));
					break;
				}
				case 4://declare ::= int ID initializer var_declare
				{
					Symbol* var_declare = popSymbol();
					Initializer* initializer = (Initializer*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _int = popSymbol();
					var_table.push_back(Var{ ID->name,D_INT,now_level });
					pushSymbol(new Symbol(reduce_pro.left));
					if (!initializer->name.empty())
					{
						code.emit("=", initializer->name, "_", ID->name);
					}
					break;
				}
				case 5://declare ::= float ID M A function_declare
				{
					FunctionDeclare* function_declare = (FunctionDeclare*)popSymbol();
					Symbol* A = popSymbol();
					M* m = (M*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _float = popSymbol();
					func_table.push_back(Func{ ID->name,D_FLOAT,function_declare->plist,m->quad });
					pushSymbol(new Symbol(reduce_pro.left));
					break;
				}
				case 6://declare ::= float ID initializer var_declare 
				{
					Symbol* var_declare = popSymbol();
					Initializer* initializer = (Initializer*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _float = popSymbol();
					var_table.push_back(Var{ ID->name,D_FLOAT,now_level });
					pushSymbol(new Symbol(reduce_pro.left));
					if (!initializer->name.empty())
					{
						code.emit("=", initializer->name, "_", ID->name);
					}
					break;
				}
				case 7://declare ::= char ID M A function_declare
				{
					FunctionDeclare* function_declare = (FunctionDeclare*)popSymbol();
					Symbol* A = popSymbol();
					M* m = (M*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _char = popSymbol();
					func_table.push_back(Func{ ID->name,D_CHAR,function_declare->plist,m->quad });
					pushSymbol(new Symbol(reduce_pro.left));
					break;
				}
				case 8://declare ::= char ID initializer var_declare
				{
					Symbol* var_declare = popSymbol();
					Initializer* initializer = (Initializer*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _char = popSymbol();
					var_table.push_back(Var{ ID->name,D_CHAR,now_level });
					pushSymbol(new Symbol(reduce_pro.left));
					if (!initializer->name.empty())
					{
						code.emit("=", initializer->name, "_", ID->name);
					}
					break;
				}
				case 9://declare ::= void ID M A function_declare
				{
					FunctionDeclare* function_declare = (FunctionDeclare*)popSymbol();
					Symbol* A = popSymbol();
					M* m = (M*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _void = popSymbol();
					func_table.push_back(Func{ ID->name, D_VOID, function_declare->plist,m->quad });
					pushSymbol(new Symbol(reduce_pro.left));
					break;
				}
				case 10://A ::=
				{
					now_level++;
					pushSymbol(new Symbol(reduce_pro.left));
					break;
				}
				case 12://function_declare ::= ( parameter ) sentence_block
				{
					SentenceBlock* sentence_block = (SentenceBlock*)popSymbol();
					Symbol* rparen = popSymbol();
					Parameter* paramter = (Parameter*)popSymbol();
					Symbol* lparen = popSymbol();
					FunctionDeclare* function_declare = new FunctionDeclare(reduce_pro.left);
					function_declare->plist.assign(paramter->plist.begin(), paramter->plist.end());
					pushSymbol(function_declare);
					break;
				}
				case 13://parameter :: = parameter_list
				{
					ParameterList* parameter_list = (ParameterList*)popSymbol();
					Parameter *parameter = new Parameter(reduce_pro.left);
					parameter->plist.assign(parameter_list->plist.begin(), parameter_list->plist.end());
					pushSymbol(parameter);
					break;
				}
				case 14://parameter ::= void
				{
					Symbol* _void = popSymbol();
					Parameter* parameter = new Parameter(reduce_pro.left);
					pushSymbol(parameter);
					break;
				}
				case 15://parameter_list ::= param
				{
					Param* param = (Param*)popSymbol();
					ParameterList* parameter_list = new ParameterList(reduce_pro.left);
					parameter_list->plist.push_back(param->type);
					pushSymbol(parameter_list);
					break;
				}
				case 16://parameter_list1 ::= param , parameter_list
				{
					ParameterList* parameter_list2 = (ParameterList*)popSymbol();
					Symbol* comma = popSymbol();
					Param* param = (Param*)popSymbol();
					ParameterList *parameter_list1 = new ParameterList(reduce_pro.left);
					parameter_list2->plist.push_front(param->type);
					parameter_list1->plist.assign(parameter_list2->plist.begin(), parameter_list2->plist.end());
					pushSymbol(parameter_list1);
					break;
				}
				case 17://param ::= int ID
				{
					Id* ID = (Id*)popSymbol();
					Symbol* _int = popSymbol();
					var_table.push_back(Var{ ID->name,D_INT,now_level });
					code.emit("get", "_", "_", ID->name);
					pushSymbol(new Param(reduce_pro.left, D_INT));
					break;
				}
				case 18://param ::= char ID
				{
					Id* ID = (Id*)popSymbol();
					Symbol* _char = popSymbol();
					var_table.push_back(Var{ ID->name,D_CHAR,now_level });
					code.emit("get", "_", "_", ID->name);
					pushSymbol(new Param(reduce_pro.left, D_CHAR));
					break;
				}
				case 19://param ::= float ID
				{
					Id* ID = (Id*)popSymbol();
					Symbol* _float = popSymbol();
					var_table.push_back(Var{ ID->name,D_FLOAT,now_level });
					code.emit("get", "_", "_", ID->name);
					pushSymbol(new Param(reduce_pro.left, D_FLOAT));
					break;
				}
				case 20://sentence_block ::= { inner_declare sentence_list }
				{
					Symbol* rbrace = popSymbol();
					SentenceList* sentence_list = (SentenceList*)popSymbol();
					Symbol* inner_declare = popSymbol();
					Symbol* lbrace = popSymbol();
					SentenceBlock* sentence_block = new SentenceBlock(reduce_pro.left);
					sentence_block->next_list = sentence_list->next_list;
					now_level--;
					int pop_num = 0;
					for (vector<Var>::reverse_iterator riter = var_table.rbegin(); riter != var_table.rend(); riter++)
					{
						if (riter->level > now_level)
							pop_num++;
						else
							break;
					}
					for (int i = 0; i < pop_num; i++)
					{
						var_table.pop_back();
					}
					pushSymbol(sentence_block);
					break;
				}
				case 23://inner_var_declare ::= int ID initializer
				{
					Initializer* initializer = (Initializer*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _int = popSymbol();
					pushSymbol(new Symbol(reduce_pro.left));
					var_table.push_back(Var{ ID->name,D_INT,now_level });
					if (!initializer->name.empty())
					{
						code.emit("=", initializer->name, "_", ID->name);
					}
					break;
				}
				case 24://inner_var_declare ::= float ID initializer
				{
					Initializer* initializer = (Initializer*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _float = popSymbol();
					pushSymbol(new Symbol(reduce_pro.left));
					var_table.push_back(Var{ ID->name,D_FLOAT,now_level });
					if (!initializer->name.empty())
					{
						code.emit("=", initializer->name, "_", ID->name);
					}
					break;
				}
				case 25://inner_var_declare ::= char ID initializer
				{
					Initializer* initializer = (Initializer*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _char = popSymbol();
					pushSymbol(new Symbol(reduce_pro.left));
					var_table.push_back(Var{ ID->name,D_CHAR,now_level });
					if (!initializer->name.empty())
					{
						code.emit("=", initializer->name, "_", ID->name);
					}
					break;
				}
				case 26://sentence_list ::= sentence M sentence_list
				{
					SentenceList* sentence_list2 = (SentenceList*)popSymbol();
					M* m = (M*)popSymbol();
					Sentence* sentence = (Sentence*)popSymbol();
					SentenceList* sentence_list1 = new SentenceList(reduce_pro.left);
					sentence_list1->next_list = sentence_list2->next_list;
					code.back_patch(sentence->next_list, m->quad);
					pushSymbol(sentence_list1);
					break;
				}
				case 27://sentence_list ::= sentence
				{
					Sentence* sentence = (Sentence*)popSymbol();
					SentenceList* sentence_list = new SentenceList(reduce_pro.left);
					sentence_list->next_list = sentence->next_list;
					pushSymbol(sentence_list);
					break;
				}
				case 28://sentence ::= if_sentence
				{
					IfSentence* if_sentence = (IfSentence*)popSymbol();
					Sentence* sentence = new Sentence(reduce_pro.left);
					sentence->next_list = if_sentence->next_list;
					pushSymbol(sentence);
					break;
				}
				case 29://sentence ::= while_sentence
				{
					WhileSentence* while_sentence = (WhileSentence*)popSymbol();
					Sentence* sentence = new Sentence(reduce_pro.left);
					sentence->next_list = while_sentence->next_list;
					pushSymbol(sentence);
					break;
				}
				case 30://sentence ::= return_sentence
				{
					Symbol* return_sentence = popSymbol();
					Sentence* sentence = new Sentence(reduce_pro.left);
					pushSymbol(sentence);
					break;
				}
				case 31://sentence ::= assign_sentence
				{
					Symbol* assign_sentence = popSymbol();
					Sentence* sentence = new Sentence(reduce_pro.left);
					pushSymbol(sentence);
					break;
				}
				case 32://assign_sentence ::= ID = expression ;
				{
					Symbol* comma = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					Symbol* assign = popSymbol();
					Id* ID = (Id*)popSymbol();
					if (lookUpVar(ID->name) == nullptr)
					{
						outputError(string("语法错误：第") + to_string(line_count) + "行，变量" + ID->name + "未声明--葛神");
					}
					Symbol* assign_sentence = new Symbol(reduce_pro.left);
					code.emit("=", expression->name, "_", ID->name);
					pushSymbol(assign_sentence);
					break;
				}
				case 33://return_sentence ::= return ;
				{
					Symbol* comma = popSymbol();
					Symbol* _return = popSymbol();
					code.emit("return", "_", "_", "_");
					pushSymbol(new Symbol(reduce_pro.left));
					break;
				}
				case 34://return_sentence ::= return expression ;
				{
					Symbol* comma = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					Symbol* _return = popSymbol();
					code.emit("return", expression->name, "_", "_");
					pushSymbol(new Symbol(reduce_pro.left));
					break;
				}
				case 35://while_sentence ::= while M ( expression ) A sentence_block
				{
					SentenceBlock* sentence_block = (SentenceBlock*)popSymbol();
					Symbol* A = popSymbol();
					Symbol* rparen = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					Symbol* lparen = popSymbol();
					M* m = (M*)popSymbol();
					Symbol* _while = popSymbol();
					WhileSentence* while_sentence = new WhileSentence(reduce_pro.left);
					code.back_patch(sentence_block->next_list, m->quad);
					while_sentence->next_list = expression->false_list;
					code.emit("j", "_", "_", to_string(m->quad));
					pushSymbol(while_sentence);
					break;
				}
				case 36://if_sentence ::= if ( expression ) A sentence_block
				{
					SentenceBlock* sentence_block = (SentenceBlock*)popSymbol();
					Symbol* A = popSymbol();
					Symbol* rparen = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					Symbol* lparen = popSymbol();
					Symbol* _if = popSymbol();
					IfSentence* if_sentence = new IfSentence(reduce_pro.left);
					expression->false_list.splice(expression->false_list.begin(), sentence_block->next_list);
					if_sentence->next_list = expression->false_list;
					pushSymbol(if_sentence);
					break;
				}
				case 37://if_sentence ::= if ( expression ) A1 sentence_block1 N else M A2 sentence_block2
				{
					SentenceBlock* sentence_block2 = (SentenceBlock*)popSymbol();
					Symbol* A2 = popSymbol();
					M* m = (M*)popSymbol();
					Symbol* _else = popSymbol();
					N* n = (N*)popSymbol();
					SentenceBlock* sentence_block1 = (SentenceBlock*)popSymbol();
					Symbol* A1 = popSymbol();
					Symbol* rparen = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					Symbol* lparen = popSymbol();
					Symbol* _if = popSymbol();
					IfSentence* if_sentence = new IfSentence(reduce_pro.left);
					code.back_patch(expression->false_list, m->quad);
					if_sentence->next_list = merge(sentence_block1->next_list, sentence_block2->next_list);
					if_sentence->next_list = merge(if_sentence->next_list, n->next_list);
					pushSymbol(if_sentence);
					break;
				}
				case 38://N ::= 
				{
					N* n = new N(reduce_pro.left);
					n->next_list.push_back(code.nextQuad());
					code.emit("j", "_", "_", "-1");
					pushSymbol(n);
					break;
				}
				case 39://M ::=
				{
					M* m = new M(reduce_pro.left);
					m->quad = code.nextQuad();
					pushSymbol(m);
					break;
				}
				case 40://expression ::= add_expression
				{
					AddExpression* add_expression = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reduce_pro.left);
					expression->name = add_expression->name;
					pushSymbol(expression);
					break;
				}
				case 41://expression ::= add_expression1 > add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* gt = popSymbol();
					AddExpression* add_expression1 = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reduce_pro.left);
					expression->false_list.push_back(code.nextQuad());
					code.emit("j<=", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 42://expression ::= add_expression1 < add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* lt = popSymbol();
					AddExpression* add_expression1 = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reduce_pro.left);
					expression->false_list.push_back(code.nextQuad());
					code.emit("j>=", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 43://expression ::= add_expression1 == add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol *eq = popSymbol();
					AddExpression *add_expression1 = (AddExpression*)popSymbol();
					Expression *expression = new Expression(reduce_pro.left);
					expression->false_list.push_back(code.nextQuad());
					code.emit("j!=", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 44://expression ::= add_expression1 >= add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* get = popSymbol();
					AddExpression* add_expression1 = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reduce_pro.left);
					expression->false_list.push_back(code.nextQuad());
					code.emit("j<", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 45://expression ::= add_expression1 <= add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* let = popSymbol();
					AddExpression* add_expression1 = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reduce_pro.left);
					expression->false_list.push_back(code.nextQuad());
					code.emit("j>", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 46://expression ::= add_expression1 != add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* neq = popSymbol();
					AddExpression* add_expression1 = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reduce_pro.left);
					expression->false_list.push_back(code.nextQuad());
					code.emit("j==", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 47://add_expression ::= item
				{
					Nomial* item = (Nomial*)popSymbol();
					AddExpression* add_expression = new AddExpression(reduce_pro.left);
					add_expression->name = item->name;
					pushSymbol(add_expression);
					break;
				}
				case 48://add_expression1 ::= item + add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* add = popSymbol();
					Nomial* item = (Nomial*)popSymbol();
					AddExpression* add_expression1 = new AddExpression(reduce_pro.left);
					add_expression1->name = nt.new_temp();
					code.emit("+", item->name, add_expression2->name, add_expression1->name);
					pushSymbol(add_expression1);
					break;
				}
				case 49://add_expression ::= item - add_expression
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* sub = popSymbol();
					Nomial* item = (Nomial*)popSymbol();
					AddExpression* add_expression1 = new AddExpression(reduce_pro.left);
					add_expression1->name = nt.new_temp();
					code.emit("-", item->name, add_expression2->name, add_expression1->name);
					pushSymbol(add_expression1);
					break;
				}
				case 50://item ::= factor
				{
					Factor* factor = (Factor*)popSymbol();
					Nomial* item = new Nomial(reduce_pro.left);
					item->name = factor->name;
					pushSymbol(item);
					break;
				}
				case 51://item1 ::= factor * item2
				{
					Nomial* item2 = (Nomial*)popSymbol();
					Symbol* mul = popSymbol();
					Factor* factor = (Factor*)popSymbol();
					Nomial* item1 = new Nomial(reduce_pro.left);
					item1->name = nt.new_temp();
					code.emit("*", factor->name, item2->name, item1->name);
					pushSymbol(item1);
					break;
				}
				case 52://item1 ::= factor / item2
				{
					Nomial* item2 = (Nomial*)popSymbol();
					Symbol* div = popSymbol();
					Factor* factor = (Factor*)popSymbol();
					Nomial* item1 = new Nomial(reduce_pro.left);
					item1->name = nt.new_temp();
					code.emit("/", factor->name, item2->name, item1->name);
					pushSymbol(item1);
					break;
				}
				case 53://factor ::= NUM_INT
				{
					NumInt* num_int = (NumInt*)popSymbol();
					Factor* factor = new Factor(reduce_pro.left);
					factor->name = num_int->number;
					pushSymbol(factor);
					break;
				}
				case 54://factor ::= NUM_FLOAT
				{
					NumInt* num_float = (NumInt*)popSymbol();
					Factor* factor = new Factor(reduce_pro.left);
					factor->name = num_float->number;
					pushSymbol(factor);
					break;
				}
				case 55://factor ::= CHAR_VAL
				{
					Char* _char = (Char*)popSymbol();
					Factor* factor = new Factor(reduce_pro.left);
					factor->name = _char->ascii;
					pushSymbol(factor);
					break;
				}
				case 56://factor ::= ( expression )
				{
					Symbol* rparen = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					Symbol* lparen = popSymbol();
					Factor* factor = new Factor(reduce_pro.left);
					factor->name = expression->name;
					pushSymbol(factor);
					break;
				}
				case 57://factor ::= ID ( argument_list )
				{
					Symbol* rparen = popSymbol();
					ArgumentList* argument_list = (ArgumentList*)popSymbol();
					Symbol* lparen = popSymbol();
					Id* ID = (Id*)popSymbol();
					Factor* factor = new Factor(reduce_pro.left);
					Func* f = lookUpFunc(ID->name);
					if (!f) 
					{
						outputError(string("语法错误：第") + to_string(line_count) + "行，未声明的函数" + ID->name);
					}
					else if (!march(argument_list->alist, f->param_types)) 
					{
						outputError(string("语法错误：第") + to_string(line_count) + "行，函数" + ID->name + "参数不匹配");
					}
					else {
						for (list<string>::iterator iter = argument_list->alist.begin(); iter != argument_list->alist.end(); iter++) {
							code.emit("par", *iter, "_", "_");
						}
						factor->name = nt.new_temp();
						code.emit("call", ID->name,"_", "_");
						code.emit("=", "@RETURN_PLACE", "_", factor->name);
						
						pushSymbol(factor);
					}
					break;
				}
				case 58://factor ::= ID
				{
					Id* ID = (Id*)popSymbol();
					if (lookUpVar(ID->name) == nullptr)
					{
						outputError(string("语法错误：第") + to_string(line_count) + "行，变量" + ID->name + "未声明");
					}
					Factor* factor = new Factor(reduce_pro.left);
					factor->name = ID->name;
					pushSymbol(factor);
					break;
				}
				case 59://factor ::= - factor
				{
					Factor* factor = (Factor*)popSymbol();
					Symbol* sub = popSymbol();
					Factor* new_factor = new Factor(reduce_pro.left);
					new_factor->name = nt.new_temp();
					code.emit("-", "0", factor->name, new_factor->name);
					pushSymbol(new_factor);
					break;
				}
				case 60://factor ::= + factor
				{
					Factor* factor = (Factor*)popSymbol();
					Symbol* sub = popSymbol();
					Factor* new_factor = new Factor(reduce_pro.left);
					new_factor->name = nt.new_temp();
					code.emit("+", "0", factor->name, new_factor->name);
					pushSymbol(new_factor);
					break;
				}
				case 61://argument_list ::= 
				{
					ArgumentList* argument_list = new ArgumentList(reduce_pro.left);
					pushSymbol(argument_list);
					break;
				}
				case 62://argument_list ::= expression
				{
					Expression* expression = (Expression*)popSymbol();
					ArgumentList* argument_list = new ArgumentList(reduce_pro.left);
					argument_list->alist.push_back(expression->name);
					pushSymbol(argument_list);
					break;
				}
				case 63://argument_list1 ::= expression , argument_list2
				{
					ArgumentList* argument_list2 = (ArgumentList*)popSymbol();
					Symbol* comma = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					ArgumentList* argument_list1 = new ArgumentList(reduce_pro.left);
					argument_list2->alist.push_front(expression->name);
					argument_list1->alist.assign(argument_list2->alist.begin(),argument_list2->alist.end());
					pushSymbol(argument_list1);
					break;
				}
				case 64://initializer ::=
				{
					Initializer* initializer = new Initializer(reduce_pro.left);
					initializer->name = "";
					pushSymbol(initializer);
					break;
				}
				case 65://initializer ::= = expression
				{
					Expression* expression = (Expression*)popSymbol();
					Symbol* assign = popSymbol();
					Initializer* initializer = new Initializer(reduce_pro.left);
					initializer->name = expression->name;
					pushSymbol(initializer);
					break;
				}
				default:
					for (int i = 0; i < pop_symbol_cnt; i++)
					{
						popSymbol();
					}
					pushSymbol(new Symbol(reduce_pro.left));
					break;
			}
		}
		else if (bh.behavior == accept)
		{
			//P ::= N declare_list
			acc = true;
			Func*f = lookUpFunc("main");
			popSymbol();
			N* n = (N*)popSymbol();
			code.back_patch(n->next_list, f->enter_point);
			outputSymbolStack(out);
			outputStateStack(out);
			break;
		}
	}
	if (!acc) 
	{
		outputError("语法错误：未知的结尾");
	}
}

void ParserAndSemanticAnalyser::analyse(list<Token>&words, const char* fileName) 
{
	ofstream fout;
	fout.open(fileName);
	if (!fout.is_open())
	{
		outputError("文件" + string(fileName) + "打开失败");
	}
	analyse(words, fout);

	fout.close();
}

void ParserAndSemanticAnalyser::analyse(list<Token>&words) 
{
	analyse(words, cout);
}

vector<pair<int, string> >ParserAndSemanticAnalyser::getFuncEnter()
{
	vector<pair<int, string> >ret;
	for (vector<Func>::iterator iter = func_table.begin(); iter != func_table.end(); iter++)
	{
		ret.push_back(pair<int, string>(iter->enter_point, iter->name));
	}
	sort(ret.begin(), ret.end());
	return ret;
}

IntermediateCode* ParserAndSemanticAnalyser::getIntermediateCode()
{
	return &code;
	return &code;
}

