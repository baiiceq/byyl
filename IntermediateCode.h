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
	void emit(Quaternary q); // 添加一个四元式
	void emit(string op, string src1, string src2, string des);// 另一种emit接口
	void back_patch(list<int>nextList,int quad);// 回填跳转目标
	void output();// 输出所有四元式到控制台
	void output(const char* fileName);// 输出所有四元式到文件
	void divideBlocks(vector<pair<int, string> > funcEnter);// 划分基本块
	void outputBlocks();// 输出基本块到控制台
	void outputBlocks(const char* fileName);// 输出基本块到文件
	map<string, vector<Block> >*getFuncBlock();
	int nextQuad();

private:
	vector<Quaternary> code;                  // 四元式代码
	map<string, vector<Block> >funcBlocks;    // 每个函数名对应的基本块集合
	NewIndex nl;                              // 用于生成唯一的标签名

	void output(ostream& out);
	void outputBlocks(ostream& out);
};