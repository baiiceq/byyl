#pragma once
#include"Common.h"

class LexicalAnalyser 
{
private:
	ifstream src;                // 文件源
	list<Token> token_list;      // 词法分析结果
	int line_count;              // 结果数量

private:
	void openFile(const char* path);
	char getNextChar();
	Token getNextToken();
	void outputToStream(ostream&out);

public:
	LexicalAnalyser(const char* path);
	~LexicalAnalyser();
	void analyse();
	void outputToScreen();
	void outputToFile(const char *fileName);
	list<Token>&getResult();
};
