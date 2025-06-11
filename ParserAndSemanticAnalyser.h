#pragma once
#include "Common.h"
#include "IntermediateCode.h"
#ifndef PARSER_H
#define PARSER_H

//符号
class Symbol 
{
public:
	bool is_Vt;                //是否为终结符
	string content;            //内容
	friend bool operator ==(const Symbol&one, const Symbol&other);
	friend bool operator < (const Symbol& one, const Symbol& other);
	friend bool operator > (const Symbol&one, const Symbol&other);
	Symbol(const Symbol& sym);
	Symbol(const bool &is_Vt, const string& content);
	Symbol();
};

//声明类型（变量声明/函数声明）
enum DecType 
{
	DEC_VAR,
	DEC_FUN
};

//数据类型
enum DType 
{ 
	D_VOID,
	D_INT, 
	D_CHAR, 
	D_FLOAT 
};


// 变量
struct Var 
{
	string name;   // 名字（标识符）
	DType type;  
	int level;     // 作用域层级
};

// 函数
struct Func 
{
	string name;              // 名字（函数名）
	DType return_type;        // 返回类型
	list<DType> param_types;  // 参数列表
	int enter_point;          // 函数入口点位置
};

// 标识符
class Id :public Symbol 
{
public:
	string name;
	Id(const Symbol& sym, const string& name);
};

// 整形变量
class NumInt :public Symbol 
{
public:
	string number;
	NumInt(const Symbol& sym,const string& number);
};

// 浮点变量
class NumFloat :public Symbol
{
public:
	string number;
	NumFloat(const Symbol& sym, const string& number);
};

// 字符变量
class Char :public Symbol
{
public:
	string ascii;  // ascii值
	Char(const Symbol& sym, const string& acsii);
};

// 函数声明
class FunctionDeclare :public Symbol 
{
public:
	list<DType>plist;                   // 参数列表
	FunctionDeclare(const Symbol& sym);
};

 
// 函数参数
class Parameter :public Symbol
{
public:
	list<DType>plist;
	Parameter(const Symbol& sym);
};

class Param : public Symbol 
{
public:
	DType type;  // 参数类型
	Param(const Symbol& sym, DType t);
};

// 参数列表
class ParameterList :public Symbol 
{
public:
	list<DType>plist;
	ParameterList(const Symbol& sym);
};

// 语句块(用{}包括起来的地方,包括声明语句和其他句子)
class SentenceBlock :public Symbol 
{
public:
	list<int>next_list;
	SentenceBlock(const Symbol& sym);
};

// 语句列表
class SentenceList :public Symbol
{
public:
	list<int>next_list;
	SentenceList(const Symbol& sym);
};

// 普通语句
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

// 表达式（=，-，x，/）
class Expression :public Symbol 
{
public:
	string name;
	list<int>false_list;     // 布尔表达式
	Expression(const Symbol& sym);
};

// 语义动作
class M :public Symbol
{
public:
	int quad;
	M(const Symbol& sym);
};

// 语义动作
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

//产生式
struct Production 
{
	int id;//产生式的标识id，方便比较
	Symbol left;
	vector<Symbol>right;
};

//项目
struct Item 
{
	int pro;            //产生式id
	int point_pos;      //.的位置
	friend bool operator ==(const Item&one, const Item& other);
	friend bool operator <(const Item&one, const Item& other);
};

//DFA状态
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

// 动作类型（规约，移进，接受，错误）
enum Behave 
{ 
	reduce, 
	shift, 
	accept, 
	error
};

// 动作
struct Behavior 
{
	Behave behavior;   
	int next_stat;     // 移进是下一个状态，规约是产生式编号
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
	int line_count;                   // 第几行
	int now_level;                    // 当前分析行所在的语句块级次
	vector<Production>productions;    // 产生式集合
	DFA dfa;                          // 句柄识别器（项目集构造）
	map<GOTO, Behavior> SLR1_table;   // 由产生式构造出的SLR1表
	map<Symbol, set<Symbol> >first;   // 由产生式构造出的first集
	map<Symbol, set<Symbol> >follow;  // 由产生式构造出的follow集
	stack<Symbol*> symbol_stack;      // 符号栈
	stack<int> state_stack;           // 状态栈
	vector<Var> var_table;            // 变量表
	vector<Func> func_table;          // 函数表
	IntermediateCode code;            // 生成的四元式
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