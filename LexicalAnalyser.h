#pragma once
#include"Common.h"

class LexicalAnalyser 
{
private:
	ifstream src;                // �ļ�Դ
	list<Token> token_list;      // �ʷ��������
	int line_count;              // �������
	vector<string> I, CI, CF, CT;// ��ʶ�������������������ַ���

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
	vector<string> getI(), getCI(), getCF(), getCT();
};
