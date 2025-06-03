#pragma once
#include"Common.h"

class LexicalAnalyser 
{
private:
	ifstream src;
	list<Token> token_list;
	int line_count;

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
