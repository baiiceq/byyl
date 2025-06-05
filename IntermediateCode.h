#pragma once
#include "Common.h"

// 第几行
class NewIndex
{
public:
	NewIndex();
	string newIndex();
private:
	int index;
};

class IntermediateCode
{
public:
	void emit(Quaternary q);
	void emit(string op, string src1, string src2, string des);
	void back_patch(list<int>nextList,int quad);
	void output();
	void output(const char* fileName);
	void divideBlocks(vector<pair<int, string> > funcEnter);
	void outputBlocks();
	void outputBlocks(const char* fileName);
	map<string, vector<Block> >*getFuncBlock();
	int nextQuad();

private:
	vector<Quaternary> code;                  // 四元式代码
	map<string, vector<Block> >funcBlocks;    // 
	NewIndex nl;                              // 

	void output(ostream& out);
	void outputBlocks(ostream& out);
};