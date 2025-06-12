#pragma once
#include "Common.h"

// �ڼ���
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
	void emit(Quaternary q); // ���һ����Ԫʽ
	void emit(string op, string src1, string src2, string des);// ��һ��emit�ӿ�
	void back_patch(list<int>nextList,int quad);// ������תĿ��
	void output();// ���������Ԫʽ������̨
	void output(const char* fileName);// ���������Ԫʽ���ļ�
	void divideBlocks(vector<pair<int, string> > funcEnter);// ���ֻ�����
	void outputBlocks();// ��������鵽����̨
	void outputBlocks(const char* fileName);// ��������鵽�ļ�
	map<string, vector<Block> >*getFuncBlock();
	int nextQuad();

private:
	vector<Quaternary> code;                  // ��Ԫʽ����
	map<string, vector<Block> >funcBlocks;    // ÿ����������Ӧ�Ļ����鼯��
	NewIndex nl;                              // ��������Ψһ�ı�ǩ��

	void output(ostream& out);
	void outputBlocks(ostream& out);
};