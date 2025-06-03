#ifndef COMMON_H
#define COMMON_H
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <utility>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <cstdlib>
#include <cassert>
#include <functional>
using namespace std;


typedef enum _LexicalType //�ʻ�����
{
	// ������־�ʹ����־
	ENDFILE, ERROR,
	// ������
	IF, ELSE, INT, RETURN, VOID, WHILE, CHAR, STRING,
	// �����������֡��ַ����ַ���
	ID, NUM, CHAR_VAL, STRING_VAL,
	// �����ַ�
	// {       }    >=   <=   !=   ==    =    <   >    +      -     *     /     (        )     ;     , 
	LBRACE, RBRACE, GTE, LTE, NEQ, EQ, ASSIGN, LT, GT, PLUS, MINUS, MULT, DIV, LPAREN, RPAREN, SEMI, COMMA,
	// ��ע��  ��ע��
	LCOMMENT, PCOMMENT,
	// ���з�
	NEXTLINE 
} LexicalType;

typedef pair<LexicalType, string> Token;

// ��Ԫʽ
struct Quaternary
{
	string op;       //������
	string src1;     //Դ������1
	string src2;     //Դ������2
	string des;      //Ŀ�Ĳ�����
};

// ������
struct Block 
{
	string name;               //�����������
	vector<Quaternary> codes;  //�������е���Ԫʽ
	int next1;                 //���������һ���ӿ�
	int next2;                 //���������һ���ӿ�
};

// ����
void outputError(string err);
#endif // !COMMON_H
