#pragma once
#include "Common.h"
#include "IntermediateCode.h"
#ifndef PARSER_H
#define PARSER_H

//����
class Symbol 
{
public:
	bool is_Vt;                //�Ƿ�Ϊ�ս��
	string content;            //����
	friend bool operator ==(const Symbol&one, const Symbol&other);
	friend bool operator < (const Symbol& one, const Symbol& other);
	friend bool operator > (const Symbol&one, const Symbol&other);
	Symbol(const Symbol& sym);
	Symbol(const bool &is_Vt, const string& content);
	Symbol();
};

//�������ͣ���������/����������
enum DecType 
{
	DEC_VAR,
	DEC_FUN
};

//��������
enum DType 
{ 
	D_VOID,
	D_INT, 
	D_CHAR, 
	D_FLOAT 
};


// ����
struct Var 
{
	string name;   // ���֣���ʶ����
	DType type;  
	int level;     // ������㼶
};

// ����
struct Func 
{
	string name;              // ���֣���������
	DType return_type;        // ��������
	list<DType> param_types;  // �����б�
	int enter_point;          // ������ڵ�λ��
};

// ��ʶ��
class Id :public Symbol 
{
public:
	string name;
	Id(const Symbol& sym, const string& name);
};

// ���α���
class NumInt :public Symbol 
{
public:
	string number;
	NumInt(const Symbol& sym,const string& number);
};

// �������
class NumFloat :public Symbol
{
public:
	string number;
	NumFloat(const Symbol& sym, const string& number);
};

// �ַ�����
class Char :public Symbol
{
public:
	string ascii;  // asciiֵ
	Char(const Symbol& sym, const string& acsii);
};

// ��������
class FunctionDeclare :public Symbol 
{
public:
	list<DType>plist;                   // �����б�
	FunctionDeclare(const Symbol& sym);
};

 
// ��������
class Parameter :public Symbol
{
public:
	list<DType>plist;
	Parameter(const Symbol& sym);
};

class Param : public Symbol 
{
public:
	DType type;  // ��������
	Param(const Symbol& sym, DType t);
};

// �����б�
class ParameterList :public Symbol 
{
public:
	list<DType>plist;
	ParameterList(const Symbol& sym);
};

// ����(��{}���������ĵط�,��������������������)
class SentenceBlock :public Symbol 
{
public:
	list<int>next_list;
	SentenceBlock(const Symbol& sym);
};

// ����б�
class SentenceList :public Symbol
{
public:
	list<int>next_list;
	SentenceList(const Symbol& sym);
};

// ��ͨ���
class Sentence :public Symbol
{
public:
	list<int>next_list;
	Sentence(const Symbol& sym);
};

class WhileSentence :public Symbol
{
public:
	list<int>next_list;
	WhileSentence(const Symbol& sym);
};

class IfSentence :public Symbol 
{
public:
	list<int>next_list;
	IfSentence(const Symbol& sym);
};

// ���ʽ��=��-��x��/��
class Expression :public Symbol 
{
public:
	string name;
	list<int>false_list;     // �������ʽ
	Expression(const Symbol& sym);
};

// ���嶯��
class M :public Symbol
{
public:
	int quad;
	M(const Symbol& sym);
};

// ���嶯��
class N :public Symbol 
{
public:
	list<int> next_list;
	N(const Symbol& sym);
};

class AddExpression :public Symbol 
{
public:
	string name;
	AddExpression(const Symbol& sym);
};

class Nomial :public Symbol
{
public:
	string name;
	Nomial(const Symbol& sym);
};

class Factor :public Symbol
{
public:
	string name;
	Factor(const Symbol& sym);
};

class ArgumentList :public Symbol 
{
public:
	list<string> alist;
	ArgumentList(const Symbol& sym);
};

class Initializer :public Symbol
{
public:
	string name;
	Initializer(const Symbol& sym);
};

const Symbol symbol[] = 
{ 
	{true,"int"},{true,"void"},{true,"if"},{true,"else"},{true,"while"},{true,"return"},
	{true,"+"},{true,"-"},{true,"*"},{true,"/"},{true,"="},
	{true,"=="},{true,">"},{true,"<"},{true,"!="},{true,">="},{true,"<="},
	{true,";"},{true,","},{true,"("},{true,")"},{true,"{"},{true,"}"},{true,"ID"},
	{false,"ID"}
};

//����ʽ
struct Production 
{
	int id;//����ʽ�ı�ʶid������Ƚ�
	Symbol left;
	vector<Symbol>right;
};

//��Ŀ
struct Item 
{
	int pro;            //����ʽid
	int point_pos;      //.��λ��
	friend bool operator ==(const Item&one, const Item& other);
	friend bool operator <(const Item&one, const Item& other);
};

//DFA״̬
struct DFAState 
{
	set<Item> items;
};

typedef pair<int, Symbol> GOTO;

struct DFA 
{
	list<DFAState> stas;
	map<GOTO, int> goTo;
};

// �������ͣ���Լ���ƽ������ܣ�����
enum Behave 
{ 
	reduce, 
	shift, 
	accept, 
	error
};

// ����
struct Behavior 
{
	Behave behavior;   
	int next_stat;     // �ƽ�����һ��״̬����Լ�ǲ���ʽ���
};

class NewTemper 
{
private:
	int now;
public:
	NewTemper();
	string new_temp();
};

class ParserAndSemanticAnalyser
{
public:
	ParserAndSemanticAnalyser(const char*fileName);
	void outputDFA();
	void outputDFA(const char* fileName);
	void outputIntermediateCode();
	void outputIntermediateCode(const char* fileName);
	void analyse(list<Token>&words,const char*fileName);
	void analyse(list<Token>&words);
	vector<pair<int, string> > getFuncEnter();
	IntermediateCode* getIntermediateCode();

private:
	int line_count;                   // �ڼ���
	int now_level;                    // ��ǰ���������ڵ����鼶��
	vector<Production>productions;    // ����ʽ����
	DFA dfa;                          // ���ʶ��������Ŀ�����죩
	map<GOTO, Behavior> SLR1_table;   // �ɲ���ʽ�������SLR1��
	map<Symbol, set<Symbol> >first;   // �ɲ���ʽ�������first��
	map<Symbol, set<Symbol> >follow;  // �ɲ���ʽ�������follow��
	stack<Symbol*> symbol_stack;      // ����ջ
	stack<int> state_stack;           // ״̬ջ
	vector<Var> var_table;            // ������
	vector<Func> func_table;          // ������
	IntermediateCode code;            // ���ɵ���Ԫʽ
	NewTemper nt;

	void readProductions(const char* fileName);
	DFAState derive(Item item);
	void createDFA();
	void outputDFA(ostream& out);
	void analyse(list<Token>& words, ostream& out);
	void outputSymbolStack(ostream& out);
	void outputStateStack(ostream& out);
	void getFirst();
	void getFollow();
	Func* lookUpFunc(string ID);
	Var* lookUpVar(string ID);
	bool march(list<string>& argument_list, list<DType>& parameter_list);
	Symbol* popSymbol();
	void pushSymbol(Symbol* sym);
};

#endif // !PARSER_H