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
bool operator < (const Item&one, const Item& other) 
{
	return pair<int, int>(one.pro, one.pointPos) < pair<int, int>(other.pro, other.pointPos);
}
bool operator ==(const Item&one, const Item& other)
{
	return one.pro == other.pro&&one.pointPos == other.pointPos;
}

Symbol::Symbol(const Symbol& sym) 
{
	this->content = sym.content;
	this->isVt = sym.isVt;
}

Symbol::Symbol(const bool &isVt, const string& content)
{
	this->isVt = isVt;
	this->content = content;
}

NewTemper::NewTemper() 
{
	now = 0;
}

string NewTemper::newTemp() 
{
	return string("T") + to_string(now++);
}

Symbol::Symbol() 
{
	isVt = false;
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

Char::Char(const Symbol& sym, const string& character) : Symbol(sym)
{
	this->character = character;
}

FunctionDeclare::FunctionDeclare(const Symbol& sym) : Symbol(sym) {}

Parameter::Parameter(const Symbol& sym) : Symbol(sym) {}

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

bool isVT(string s)
{
	if (s == "int" || s == "void" || s == "if" || s == "while" || s == "else" || s == "return" || s == "float")
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
	lineCount = 1;
}

void ParserAndSemanticAnalyser::getFirst() 
{
	bool changeFlag = true;
	while (changeFlag) 
	{
		changeFlag = false;//first���ı��־
		//����ÿһ������ʽ
		for (vector<Production>::iterator iter = productions.begin(); iter != productions.end(); iter++)
		{
			vector<Symbol>::iterator symIter;
			//���α�������ʽ�Ҳ������з���
			for (symIter = iter->right.begin(); symIter != iter->right.end(); symIter++)
			{
				//����Ҳ��������ս��
				if (symIter->isVt) 
				{
					if (first.count(iter->left) == 0) 
					{
						first[iter->left] = set<Symbol>();
					}
					//�󲿷��ŵ�first�����������Ҳ�����
					if (first[iter->left].insert(*symIter).second == true) 
					{
						changeFlag = true;
					}
					break;
				}
				//��ǰ�Ҳ������Ƿ��ս��
				else 
				{
					bool continueFlag = false;//�Ƿ������ȡ��һ���Ҳ����ŵ�first��
					set<Symbol>::iterator firstIter;
					//�������Ҳ����ŵ�first��
					for (firstIter = first[*symIter].begin(); firstIter != first[*symIter].end(); firstIter++)
					{
						//�Ҳ����ŵ�first���е�Ԫ�ذ���EMPTY
						if (firstIter->content == "EMPTY")
						{
							continueFlag = true;
						}
						//�Ҳ����ŵ�first���е�Ԫ�ز����󲿷���first����
						else if (first[iter->left].find(*firstIter) == first[iter->left].end())
						{
							if (first.count(iter->left) == 0)
							{
								first[iter->left] = set<Symbol>();
							}
							first[iter->left].insert(*firstIter);
							changeFlag = true;
						}
					}
					if (!continueFlag) 
					{
						break;
					}
				}
			}
			//�����Ҳ����ŵ���ĩβ,��EMPTY����first����
			if (symIter == iter->right.end()) 
			{
				if (first.count(iter->left) == 0)
				{
					first[iter->left] = set<Symbol>();
				}
				if (first[iter->left].insert(Symbol{ true,"EMPTY" }).second == true)
				{
					changeFlag = true;
				}
			}
		}

	}
}

void ParserAndSemanticAnalyser::getFollow()
{
	//��#������ʼ���ŵ�FOLLOW����
	follow[productions[0].left] = set<Symbol>();
	follow[productions[0].left].insert(Symbol{ true,"#" });
	bool changeFlag = true;
	while (changeFlag) 
	{
		changeFlag = false;
		//����ÿһ������ʽ
		for (vector<Production>::iterator proIter = productions.begin(); proIter != productions.end(); proIter++) 
		{
			//��������ʽ�Ҳ���ÿ������
			for (vector<Symbol>::iterator symIter = proIter->right.begin(); symIter != proIter->right.end(); symIter++) 
			{
				//��������ʽ�Ҳ��÷���֮��ķ���
				vector<Symbol>::iterator nextSymIter;
				for (nextSymIter = symIter + 1; nextSymIter != proIter->right.end(); nextSymIter++) 
				{
					Symbol nextSym = *nextSymIter;
					bool nextFlag = false;
					//���֮��ķ������ս��
					if (nextSym.isVt)
					{
						if (follow.count(*symIter) == 0) 
						{
							follow[*symIter] = set<Symbol>();
						}
						//����ɹ�������ֵ
						if (follow[*symIter].insert(nextSym).second == true)
						{
							changeFlag = true;
						}
					}
					else 
					{
						//����֮����ŵ�first��
						for (set<Symbol>::iterator fIter = first[nextSym].begin(); fIter != first[nextSym].end(); fIter++) {
							//�����ǰ����first������ �մ�
							if (fIter->content == "EMPTY") 
							{
								nextFlag = true;
							}
							else 
							{
								if (follow.count(*symIter) == 0) 
								{
									follow[*symIter] = set<Symbol>();
								}
								//����ɹ�������ֵ
								if (follow[*symIter].insert(*fIter).second == true) 
								{
									changeFlag = true;
								}
							}
						}
					}
					//�����ǰ����first����û�� �մ�
					if (!nextFlag) 
					{
						break;
					}

				}
				//����������˽�β,���󲿷��ŵ�FOLLOW��������FOLLOW����
				if (nextSymIter == proIter->right.end()) 
				{
					//�����󲿷��ŵ�FOLLOW��
					for (set<Symbol>::iterator followIter = follow[proIter->left].begin(); followIter != follow[proIter->left].end(); followIter++)
					{
						if (follow.count(*symIter) == 0)
						{
							follow[*symIter] = set<Symbol>();
						}
						//�����FOLLOW������ֵ
						if (follow[*symIter].insert(*followIter).second == true)
						{
							changeFlag = true;
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
	for (list<I>::iterator iter = dfa.stas.begin(); iter != dfa.stas.end(); iter++, nowI++) {
		out << "I" << nowI << "= [";
		for (set<Item>::iterator itIter = iter->items.begin(); itIter != iter->items.end(); itIter++) {
			out << "��";
			Production p = productions[itIter->pro];
			out << p.left.content << " -> ";
			for (vector<Symbol>::iterator symIter = p.right.begin(); symIter != p.right.end(); symIter++) {
				if (symIter - p.right.begin() == itIter->pointPos) {
					out << ". ";
				}
				out << symIter->content << " ";
			}
			if (p.right.size() == itIter->pointPos) {
				out << ". ";
			}
			out << "��";
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
	if (!fout.is_open()) {
		outputError("�ļ�" + string(fileName) + "��ʧ��");
	}
	outputDFA(fout);

	fout.close();
}

void ParserAndSemanticAnalyser::readProductions(const char*fileName) 
{
	ifstream fin;

	//�ļ��򿪴���
	fin.open(fileName, ios::in);
	if (!fin.is_open())
	{
		outputError("�ļ�" + string(fileName) + "��ʧ��");
	}

	int index = 0;//����ʽ���
	char buf[1024];
	while (fin >> buf) 
	{
		Production p;
		//����ʽ��Ÿ�ֵ
		p.id = index++;

		//����ʽ�󲿸�ֵ
		p.left = Symbol{ false,string(buf) };

		//�м�ӦΪ::=
		fin >> buf;
		assert(strcmp(buf, "::=") == 0);

		//����ʽ�Ҳ���ֵ
		fin.getline(buf, 1024);
		stringstream sstream(buf);
		string temp;
		while (sstream >> temp) {
			p.right.push_back(Symbol{ isVT(temp),string(temp) });
		}

		//�������ʽ
		productions.push_back(p);
	}
}

I ParserAndSemanticAnalyser::derive(Item item) 
{
	I i;
	// .����Ŀ����ʽ�����ұߣ�����һ����Լ��Ŀ
	if (productions[item.pro].right.size() == item.pointPos)
	{
		i.items.insert(item);
	}
	// .���ұ����ս��
	else if (productions[item.pro].right[item.pointPos].isVt) 
	{
		i.items.insert(item);
	}
	// .���ұ��Ƿ��ս��
	else
	{
		i.items.insert(item);
		vector<Production>::iterator iter;
		for (iter = productions.begin(); iter < productions.end(); iter++) 
		{
			//����ʽ���� == .�ұߵķ��ս��
			if (iter->left == productions[item.pro].right[item.pointPos]) 
			{
				//������ʽ����������I��
				I temp = derive(Item{ (int)(iter - productions.begin()),0 });

				set<Item>::iterator siter;
				for (siter = temp.items.begin(); siter != temp.items.end(); siter++) {
					i.items.insert(*siter);
				}
			}
		}
	}

	return i;
}

void ParserAndSemanticAnalyser::createDFA() 
{
	bool newFlag = true;//���µ�״̬������־
	int nowI = 0;//��ǰ״̬�ı��
	dfa.stas.push_back(derive(Item{ 0,0 }));
	//����ÿһ��״̬
	for (list<I>::iterator iter = dfa.stas.begin(); iter != dfa.stas.end(); iter++, nowI++) 
	{
		//����״̬��ÿһ����Ŀ
		for (set<Item>::iterator itIter = iter->items.begin(); itIter != iter->items.end(); itIter++)
		{
			// .����Ŀ����ʽ�����ұߣ�����һ����Լ��Ŀ
			if (productions[itIter->pro].right.size() == itIter->pointPos)
			{
				set<Symbol>FOLLOW = follow[productions[itIter->pro].left];
				for (set<Symbol>::iterator followIter = FOLLOW.begin(); followIter != FOLLOW.end(); followIter++) 
				{
					if (SLR1_Table.count(GOTO(nowI, *followIter)) == 1) 
					{
						string err = "�ķ�����SLR1�ķ����ƽ���Լ��ͻ";
						//err += string("GOTO(") + to_string(nowI) + "," + followIter->content + ")=" + to_string(SLR1_Table[GOTO(nowI, *followIter)].nextStat);
						outputError(err);
					}
					if (itIter->pro == 0) // OK
					{
						SLR1_Table[GOTO(nowI, *followIter)] = Behavior{ accept,itIter->pro };
					}
					else
					{
						SLR1_Table[GOTO(nowI, *followIter)] = Behavior{ reduct,itIter->pro };
					}

				}
				continue;
			}
			Symbol nextSymbol = productions[itIter->pro].right[itIter->pointPos];//.֮��ķ���

			//DFA��GOTO(nowI,nextSymbol)�Ѿ�����
			if (dfa.goTo.count(GOTO(nowI, nextSymbol)) == 1) 
			{
				continue;
			}

			I newI = derive(Item{ itIter->pro,itIter->pointPos + 1 });//�²�����״̬

			//���ҵ�ǰ״̬������GOTO[nowI,nextSymbol]
			//shiftIterָ��ǰ״̬��Ŀ����һ����Ŀ
			set<Item>::iterator shiftIter = itIter;
			shiftIter++;
			for (; shiftIter != iter->items.end(); shiftIter++)
			{
				//����ǹ�Լ��Ŀ
				if (productions[shiftIter->pro].right.size() == shiftIter->pointPos)
				{
					continue;
				}
				//������ƽ���Ŀ�����ƽ�nextSymbol
				else if (productions[shiftIter->pro].right[shiftIter->pointPos] == nextSymbol)
				{
					I tempI = derive(Item{ shiftIter->pro,shiftIter->pointPos + 1 });
					newI.items.insert(tempI.items.begin(), tempI.items.end());
				}
			}
			//��������״̬���Ƿ��Ѿ�������״̬
			bool searchFlag = false;
			int index = 0;//��ǰ״̬�ı��
			for (list<I>::iterator iterHave = dfa.stas.begin(); iterHave != dfa.stas.end(); iterHave++, index++) 
			{
				if (iterHave->items == newI.items)
				{
					dfa.goTo[GOTO(nowI, nextSymbol)] = index;
					if (SLR1_Table.count(GOTO(nowI, nextSymbol)) == 1)
					{
						outputError("confict");
					}
					SLR1_Table[GOTO(nowI, nextSymbol)] = Behavior{ shift,index };
					searchFlag = true;
					break;
				}
			}

			//û��������״̬���ҵ���״̬
			if (!searchFlag)
			{
				dfa.stas.push_back(newI);
				dfa.goTo[GOTO(nowI, nextSymbol)] = dfa.stas.size() - 1;
				if (SLR1_Table.count(GOTO(nowI, nextSymbol)) == 1)
				{
					outputError("confict");
				}
				SLR1_Table[GOTO(nowI, nextSymbol)] = Behavior{ shift,int(dfa.stas.size() - 1) };
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
	for (vector<Func>::iterator iter = funcTable.begin(); iter != funcTable.end(); iter++) {
		if (iter->name == ID) {
			return &(*iter);
		}
	}
	return NULL;
}

Var* ParserAndSemanticAnalyser::lookUpVar(string ID) 
{
	for (vector<Var>::reverse_iterator iter = varTable.rbegin(); iter != varTable.rend(); iter++) {
		if (iter->name == ID) {
			return &(*iter);
		}
	}
	return NULL;
}

bool ParserAndSemanticAnalyser::march(list<string>&argument_list, list<DType>&parameter_list) {
	if (argument_list.size() != parameter_list.size()) {
		return false;
	}
	else {
		return true;
	}
}

void ParserAndSemanticAnalyser::outputSymbolStack(ostream& out) {
	stack<Symbol*>temp = symStack;
	stack<Symbol*>other;
	while (!temp.empty()) {
		other.push(temp.top());
		temp.pop();
	}
	while (!other.empty()) {
		out << other.top()->content << " ";
		other.pop();
	}
	out << endl;
}

void ParserAndSemanticAnalyser::outputStateStack(ostream& out) {
	stack<int>temp = staStack;
	stack<int>other;
	while (!temp.empty()) {
		other.push(temp.top());
		temp.pop();
	}
	while (!other.empty()) {
		out << other.top() << " ";
		other.pop();
	}
	out << endl;

}

void ParserAndSemanticAnalyser::outputIntermediateCode() {
	code.output();
}

void ParserAndSemanticAnalyser::outputIntermediateCode(const char* fileName) {
	code.output(fileName);
}

Symbol* ParserAndSemanticAnalyser::popSymbol() {
	Symbol* ret = symStack.top();
	symStack.pop();
	staStack.pop();
	return ret;
}

void ParserAndSemanticAnalyser::pushSymbol(Symbol* sym) {
	symStack.push(sym);
	if (SLR1_Table.count(GOTO(staStack.top(), *sym)) == 0) {
		outputError(string("�﷨���󣺵�") + to_string(lineCount) + "�У����ڴ��ķ���" + sym->content);
	}
	Behavior bh = SLR1_Table[GOTO(staStack.top(), *sym)];
	staStack.push(bh.nextStat);
}

void ParserAndSemanticAnalyser::analyse(list<Token>&words, ostream& out) 
{
	bool acc = false;
	symStack.push(new Symbol(true, "#"));
	staStack.push(0);
	for (list<Token>::iterator iter = words.begin(); iter != words.end(); ) 
	{
		outputSymbolStack(out);
		outputStateStack(out);
		LexicalType LT = iter->first;
		string word = iter->second;

		//������ע�ͺͶ�ע��
		if (LT == LCOMMENT || LT == PCOMMENT) 
		{
			continue;
		}
		if (LT == NEXTLINE) 
		{
			lineCount++;
			continue;
		}

		Symbol* nextSymbol;
		if (LT == ID)
		{
			nextSymbol = new Id(Symbol{ true,"ID" }, word);
		}
		else if (LT == NUM_INT)
		{
			nextSymbol = new NumInt(Symbol{ true,"NUM_INT" }, word);
		}
		else if (LT == NUM_FlOAT)
		{
			nextSymbol = new NumInt(Symbol{ true,"NUM_INT" }, word);
		}
		else 
		{
			nextSymbol = new Symbol(true, word);
		}
		if (SLR1_Table.count(GOTO(staStack.top(), *nextSymbol)) == 0) 
		{
			outputError(string("�﷨���󣺵�")+to_string(lineCount)+"�У����ڴ��ķ���"+nextSymbol->content);
		}

		Behavior bh = SLR1_Table[GOTO(staStack.top(), *nextSymbol)];
		if (bh.behavior == shift) {
			symStack.push(nextSymbol);
			staStack.push(bh.nextStat);
			iter++;
		}
		else if (bh.behavior == reduct) {
			Production reductPro = productions[bh.nextStat];
			int popSymNum = reductPro.right.size();
			switch (bh.nextStat) {
				case 3://declare ::= int ID M A function_declare
				{
					FunctionDeclare *function_declare = (FunctionDeclare*)popSymbol();
					Symbol* A = popSymbol();
					M* m = (M*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _int = popSymbol();
					funcTable.push_back(Func{ ID->name,D_INT,function_declare->plist,m->quad });
					pushSymbol(new Symbol(reductPro.left));
					break;
				}
				case 4://declare ::= int ID var_declare
				{
					Symbol* var_declare = popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _int = popSymbol();
					varTable.push_back(Var{ ID->name,D_INT,nowLevel });
					pushSymbol(new Symbol(reductPro.left));
					break;
				}
				case 5://declare ::= void ID M A function_declare
				{
					FunctionDeclare* function_declare = (FunctionDeclare*)popSymbol();
					Symbol* A = popSymbol();
					M* m = (M*)popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* _void = popSymbol();
					funcTable.push_back(Func{ ID->name, D_VOID, function_declare->plist,m->quad });
					pushSymbol(new Symbol(reductPro.left));
					break;
				}
				case 6://A ::=
				{
					nowLevel++;
					pushSymbol(new Symbol(reductPro.left));
					break;
				}
				case 8://function_declare ::= ( parameter ) sentence_block
				{
					SentenceBlock* sentence_block = (SentenceBlock*)popSymbol();
					Symbol* rparen = popSymbol();
					Parameter* paramter = (Parameter*)popSymbol();
					Symbol* lparen = popSymbol();
					FunctionDeclare* function_declare = new FunctionDeclare(reductPro.left);
					function_declare->plist.assign(paramter->plist.begin(), paramter->plist.end());
					pushSymbol(function_declare);
					break;
				}
				case 9://parameter :: = parameter_list
				{
					ParameterList* parameter_list = (ParameterList*)popSymbol();
					Parameter *parameter = new Parameter(reductPro.left);
					parameter->plist.assign(parameter_list->plist.begin(), parameter_list->plist.end());
					pushSymbol(parameter);
					break;
				}
				case 10://parameter ::= void
				{
					Symbol* _void = popSymbol();
					Parameter* parameter = new Parameter(reductPro.left);
					pushSymbol(parameter);
					break;
				}
				case 11://parameter_list ::= param
				{
					Symbol* param = popSymbol();
					ParameterList* parameter_list = new ParameterList(reductPro.left);
					parameter_list->plist.push_back(D_INT);
					pushSymbol(parameter_list);
					break;
				}
				case 12://parameter_list1 ::= param , parameter_list2
				{
					ParameterList* parameter_list2 = (ParameterList*)popSymbol();
					Symbol* comma = popSymbol();
					Symbol* param = popSymbol();
					ParameterList *parameter_list1 = new ParameterList(reductPro.left);
					parameter_list2->plist.push_front(D_INT);
					parameter_list1->plist.assign(parameter_list2->plist.begin(), parameter_list2->plist.end());
					pushSymbol(parameter_list1);
					break;
				}
				case 13://param ::= int ID
				{
					Id* ID = (Id*)popSymbol();
					Symbol* _int = popSymbol();
					varTable.push_back(Var{ ID->name,D_INT,nowLevel });
					code.emit("get", "_", "_", ID->name);
					pushSymbol(new Symbol(reductPro.left));
					break;
				}
				case 14://sentence_block ::= { inner_declare sentence_list }
				{
					Symbol* rbrace = popSymbol();
					SentenceList* sentence_list = (SentenceList*)popSymbol();
					Symbol* inner_declare = popSymbol();
					Symbol* lbrace = popSymbol();
					SentenceBlock* sentence_block = new SentenceBlock(reductPro.left);
					sentence_block->nextList = sentence_list->nextList;
					nowLevel--;
					int popNum = 0;
					for (vector<Var>::reverse_iterator riter = varTable.rbegin(); riter != varTable.rend(); riter++) {
						if (riter->level > nowLevel)
							popNum++;
						else
							break;
					}
					for (int i = 0; i < popNum; i++) {
						varTable.pop_back();
					}
					pushSymbol(sentence_block);
					break;
				}
				case 17://inner_var_declare ::= int ID
				{
					Id* ID = (Id*)popSymbol();
					Symbol* _int = popSymbol();
					pushSymbol(new Symbol(reductPro.left));
					varTable.push_back(Var{ ID->name,D_INT,nowLevel });
					break;
				}
				case 18://sentence_list ::= sentence M sentence_list
				{
					SentenceList* sentence_list2 = (SentenceList*)popSymbol();
					M* m = (M*)popSymbol();
					Sentence* sentence = (Sentence*)popSymbol();
					SentenceList* sentence_list1 = new SentenceList(reductPro.left);
					sentence_list1->nextList = sentence_list2->nextList;
					code.back_patch(sentence->nextList, m->quad);
					pushSymbol(sentence_list1);
					break;
				}
				case 19://sentence_list ::= sentence
				{
					Sentence* sentence = (Sentence*)popSymbol();
					SentenceList* sentence_list = new SentenceList(reductPro.left);
					sentence_list->nextList = sentence->nextList;
					pushSymbol(sentence_list);
					break;
				}
				case 20://sentence ::= if_sentence
				{
					IfSentence* if_sentence = (IfSentence*)popSymbol();
					Sentence* sentence = new Sentence(reductPro.left);
					sentence->nextList = if_sentence->nextList;
					pushSymbol(sentence);
					break;
				}
				case 21://sentence ::= while_sentence
				{
					WhileSentence* while_sentence = (WhileSentence*)popSymbol();
					Sentence* sentence = new Sentence(reductPro.left);
					sentence->nextList = while_sentence->nextList;
					pushSymbol(sentence);
					break;
				}
				case 22://sentence ::= return_sentence
				{
					Symbol* return_sentence = popSymbol();
					Sentence* sentence = new Sentence(reductPro.left);
					pushSymbol(sentence);
					break;
				}
				case 23://sentence ::= assign_sentence
				{
					Symbol* assign_sentence = popSymbol();
					Sentence* sentence = new Sentence(reductPro.left);
					pushSymbol(sentence);
					break;
				}
				case 24://assign_sentence ::= ID = expression ;
				{
					Symbol* comma = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					Symbol* assign = popSymbol();
					Id* ID = (Id*)popSymbol();
					Symbol* assign_sentence = new Symbol(reductPro.left);
					code.emit("=", expression->name, "_", ID->name);
					pushSymbol(assign_sentence);
					break;
				}
				case 25://return_sentence ::= return ;
				{
					Symbol* comma = popSymbol();
					Symbol* _return = popSymbol();
					code.emit("return", "_", "_", "_");
					pushSymbol(new Symbol(reductPro.left));
					break;
				}
				case 26://return_sentence ::= return expression ;
				{
					Symbol* comma = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					Symbol* _return = popSymbol();
					code.emit("return", expression->name, "_", "_");
					pushSymbol(new Symbol(reductPro.left));
					break;
				}
				case 27://while_sentence ::= while M ( expression ) A sentence_block
				{
					SentenceBlock* sentence_block = (SentenceBlock*)popSymbol();
					Symbol* A = popSymbol();
					Symbol* rparen = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					Symbol* lparen = popSymbol();
					M* m = (M*)popSymbol();
					Symbol* _while = popSymbol();
					WhileSentence* while_sentence = new WhileSentence(reductPro.left);
					code.back_patch(sentence_block->nextList, m->quad);
					while_sentence->nextList = expression->falseList;
					code.emit("j", "_", "_", to_string(m->quad));
					pushSymbol(while_sentence);
					break;
				}
				case 28://if_sentence ::= if ( expression ) A sentence_block
				{
					SentenceBlock* sentence_block = (SentenceBlock*)popSymbol();
					Symbol* A = popSymbol();
					Symbol* rparen = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					Symbol* lparen = popSymbol();
					Symbol* _if = popSymbol();
					IfSentence* if_sentence = new IfSentence(reductPro.left);
					expression->falseList.splice(expression->falseList.begin(), sentence_block->nextList);
					if_sentence->nextList = expression->falseList;
					pushSymbol(if_sentence);
					break;
				}
				case 29://if_sentence ::= if ( expression ) A1 sentence_block1 N else M A2 sentence_block2
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
					IfSentence* if_sentence = new IfSentence(reductPro.left);
					code.back_patch(expression->falseList, m->quad);
					if_sentence->nextList = merge(sentence_block1->nextList, sentence_block2->nextList);
					if_sentence->nextList = merge(if_sentence->nextList, n->nextList);
					pushSymbol(if_sentence);
					break;
				}
				case 30://N ::= 
				{
					N* n = new N(reductPro.left);
					n->nextList.push_back(code.nextQuad());
					code.emit("j", "_", "_", "-1");
					pushSymbol(n);
					break;
				}
				case 31://M ::=
				{
					M* m = new M(reductPro.left);
					m->quad = code.nextQuad();
					pushSymbol(m);
					break;
				}
				case 32://expression ::= add_expression
				{
					AddExpression* add_expression = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reductPro.left);
					expression->name = add_expression->name;
					pushSymbol(expression);
					break;
				}
				case 33://expression ::= add_expression1 > add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* gt = popSymbol();
					AddExpression* add_expression1 = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reductPro.left);
					expression->falseList.push_back(code.nextQuad());
					code.emit("j<=", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 34://expression ::= add_expression1 < add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* lt = popSymbol();
					AddExpression* add_expression1 = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reductPro.left);
					expression->falseList.push_back(code.nextQuad());
					code.emit("j>=", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 35://expression ::= add_expression1 == add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol *eq = popSymbol();
					AddExpression *add_expression1 = (AddExpression*)popSymbol();
					Expression *expression = new Expression(reductPro.left);
					expression->falseList.push_back(code.nextQuad());
					code.emit("j!=", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 36://expression ::= add_expression1 >= add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* get = popSymbol();
					AddExpression* add_expression1 = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reductPro.left);
					expression->falseList.push_back(code.nextQuad());
					code.emit("j<", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 37://expression ::= add_expression1 <= add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* let = popSymbol();
					AddExpression* add_expression1 = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reductPro.left);
					expression->falseList.push_back(code.nextQuad());
					code.emit("j>", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 38://expression ::= add_expression1 != add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* neq = popSymbol();
					AddExpression* add_expression1 = (AddExpression*)popSymbol();
					Expression* expression = new Expression(reductPro.left);
					expression->falseList.push_back(code.nextQuad());
					code.emit("j==", add_expression1->name, add_expression2->name, "-1");
					pushSymbol(expression);
					break;
				}
				case 39://add_expression ::= item
				{
					Nomial* item = (Nomial*)popSymbol();
					AddExpression* add_expression = new AddExpression(reductPro.left);
					add_expression->name = item->name;
					pushSymbol(add_expression);
					break;
				}
				case 40://add_expression1 ::= item + add_expression2
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* add = popSymbol();
					Nomial* item = (Nomial*)popSymbol();
					AddExpression* add_expression1 = new AddExpression(reductPro.left);
					add_expression1->name = nt.newTemp();
					code.emit("+", item->name, add_expression2->name, add_expression1->name);
					pushSymbol(add_expression1);
					break;
				}
				case 41://add_expression ::= item - add_expression
				{
					AddExpression* add_expression2 = (AddExpression*)popSymbol();
					Symbol* sub = popSymbol();
					Nomial* item = (Nomial*)popSymbol();
					AddExpression* add_expression1 = new AddExpression(reductPro.left);
					add_expression1->name = nt.newTemp();
					code.emit("-", item->name, add_expression2->name, add_expression1->name);
					pushSymbol(add_expression1);
					break;
				}
				case 42://item ::= factor
				{
					Factor* factor = (Factor*)popSymbol();
					Nomial* item = new Nomial(reductPro.left);
					item->name = factor->name;
					pushSymbol(item);
					break;
				}
				case 43://item1 ::= factor * item2
				{
					Nomial* item2 = (Nomial*)popSymbol();
					Symbol* mul = popSymbol();
					Factor* factor = (Factor*)popSymbol();
					Nomial* item1 = new Nomial(reductPro.left);
					item1->name = nt.newTemp();
					code.emit("*", factor->name, item2->name, item1->name);
					pushSymbol(item1);
					break;
				}
				case 44://item1 ::= factor / item2
				{
					Nomial* item2 = (Nomial*)popSymbol();
					Symbol* div = popSymbol();
					Factor* factor = (Factor*)popSymbol();
					Nomial* item1 = new Nomial(reductPro.left);
					item1->name = nt.newTemp();
					code.emit("/", factor->name, item2->name, item1->name);
					pushSymbol(item1);
					break;
				}
				case 45://factor ::= NUM
				{
					Num* num = (Num*)popSymbol();
					Factor* factor = new Factor(reductPro.left);
					factor->name = num->number;
					pushSymbol(factor);
					break;
				}
				case 46://factor ::= ( expression )
				{
					Symbol* rparen = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					Symbol* lparen = popSymbol();
					Factor* factor = new Factor(reductPro.left);
					factor->name = expression->name;
					pushSymbol(factor);
					break;
				}
				case 47://factor ::= ID ( argument_list )
				{
					Symbol* rparen = popSymbol();
					ArgumentList* argument_list = (ArgumentList*)popSymbol();
					Symbol* lparen = popSymbol();
					Id* ID = (Id*)popSymbol();
					Factor* factor = new Factor(reductPro.left);
					Func* f = lookUpFunc(ID->name);
					if (!f) {
						outputError(string("�﷨���󣺵�") + to_string(lineCount) + "�У�δ�����ĺ���" + ID->name);
					}
					else if (!march(argument_list->alist, f->paramTypes)) {
						outputError(string("�﷨���󣺵�") + to_string(lineCount) + "�У�����" + ID->name + "������ƥ��");
					}
					else {
						for (list<string>::iterator iter = argument_list->alist.begin(); iter != argument_list->alist.end(); iter++) {
							code.emit("par", *iter, "_", "_");
						}
						factor->name = nt.newTemp();
						code.emit("call", ID->name,"_", "_");
						code.emit("=", "@RETURN_PLACE", "_", factor->name);
						
						pushSymbol(factor);
					}
					break;
				}
				case 48://factor ::= ID
				{
					Id* ID = (Id*)popSymbol();
					if (lookUpVar(ID->name) == NULL) {
						outputError(string("�﷨���󣺵�") + to_string(lineCount) + "�У�����" + ID->name + "δ����");
					}
					Factor* factor = new Factor(reductPro.left);
					factor->name = ID->name;
					pushSymbol(factor);
					break;
				}
				case 49://argument_list ::= 
				{
					ArgumentList* argument_list = new ArgumentList(reductPro.left);
					pushSymbol(argument_list);
					break;
				}
				case 50://argument_list ::= expression
				{
					Expression* expression = (Expression*)popSymbol();
					ArgumentList* argument_list = new ArgumentList(reductPro.left);
					argument_list->alist.push_back(expression->name);
					pushSymbol(argument_list);
					break;
				}
				case 51://argument_list1 ::= expression , argument_list2
				{
					ArgumentList* argument_list2 = (ArgumentList*)popSymbol();
					Symbol* comma = popSymbol();
					Expression* expression = (Expression*)popSymbol();
					ArgumentList* argument_list1 = new ArgumentList(reductPro.left);
					argument_list2->alist.push_front(expression->name);
					argument_list1->alist.assign(argument_list2->alist.begin(),argument_list2->alist.end());
					pushSymbol(argument_list1);
					break;
				}
				default:
					for (int i = 0; i < popSymNum; i++) {
						popSymbol();
					}
					pushSymbol(new Symbol(reductPro.left));
					break;
			}
		}
		else if (bh.behavior == accept) {//P ::= N declare_list
			acc = true;
			Func*f = lookUpFunc("main");
			popSymbol();
			N* n = (N*)popSymbol();
			code.back_patch(n->nextList, f->enterPoint);
			break;
		}
	}
	if (!acc) {
		outputError("�﷨����δ֪�Ľ�β");
	}
}

void ParserAndSemanticAnalyser::analyse(list<Token>&words, const char* fileName) {
	ofstream fout;
	fout.open(fileName);
	if (!fout.is_open()) {
		outputError("�ļ�" + string(fileName) + "��ʧ��");
	}
	analyse(words, fout);

	fout.close();
}

void ParserAndSemanticAnalyser::analyse(list<Token>&words) {
	analyse(words, cout);
}

vector<pair<int, string> >ParserAndSemanticAnalyser::getFuncEnter() {
	vector<pair<int, string> >ret;
	for (vector<Func>::iterator iter = funcTable.begin(); iter != funcTable.end(); iter++) {
		ret.push_back(pair<int, string>(iter->enterPoint, iter->name));
	}
	sort(ret.begin(), ret.end());
	return ret;
}

IntermediateCode* ParserAndSemanticAnalyser::getIntermediateCode() {
	return &code;
}