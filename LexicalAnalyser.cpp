#include "LexicalAnalyser.h"
#include <string>

string token_to_string(Token t) 
{
	const char* LexicalTypeStr[] =
	{
	"ENDFILE", "ERROR",
	"IF", "ELSE", "INT", "RETURN", "VOID", "WHILE", "CHAR", "FLOAT",
	"ID", "NUM_INT", "NUM_FLOAT", "CHAR_VAL",
	"LBRACE", "RBRACE", "GTE", "LTE", "NEQ", "EQ", "ASSIGN", "LT", "GT", "PLUS", "MINUS", "MULT", "DIV", "LPAREN", "RPAREN", "SEMI", "COMMA",
	"LCOMMENT", "PCOMMENT", "NEXTLINE"
	};

	string res;
	res += LexicalTypeStr[t.first];
	if (t.first == NEXTLINE)
	{
		res += " \\n";
	}
	else
	{
		res += string("  ") + t.second;
	}
	return res;
}

LexicalAnalyser::LexicalAnalyser(const char* path)
{
	line_count = 0;
	openFile(path);
}

LexicalAnalyser::~LexicalAnalyser() 
{
	if (src.is_open()) 
	{
		src.close();
	}
}

void LexicalAnalyser::openFile(const char* path) 
{
	src.open(path, ios::in);
	if (!src.is_open())
	{
		cerr << "file " << path << " open error" << endl;
		exit(-1);
	}
}

//��ȡ��һ���ַ������Կհ׷�
char LexicalAnalyser::getNextChar()
{
	char c;
	while (src.get(c)) 
	{
		if (c == ' '||c=='\t') 
		{
			continue;
		}
		else if (c == '\n') 
		{
			line_count++;
			return '\n';
		}
		break;
	}
	if (src.eof())
		return 0;
	else
		return c;
}

Token LexicalAnalyser::getNextToken()
{
	char c = getNextChar();
	switch (c)
	{
		case '\n':
			return Token(NEXTLINE, "\n");
		case '(':
			return Token(LPAREN, "(");
			break;
		case ')':
			return Token(RPAREN, ")");
			break;
		case '{':
			return Token(LBRACE, "{");
			break;
		case '}':
			return Token(RBRACE, "}");
			break;
		case '#':
			return Token(ENDFILE, "#");
			break;
		case '+':
			return Token(PLUS, "+");
			break;
		case '-':
			return Token(MINUS, "-");
			break;
		case '*':
			return Token(MULT, "*");
			break;
		case ',':
			return Token(COMMA, ",");
			break;
		case ';':
			return Token(SEMI, ";");
			break;
		case '=':
			if (src.peek() == '=') 
			{
				src.get();
				return Token(EQ, "==");
			}
			else 
			{
				return Token(ASSIGN, "=");
			}
			break;
		case '>':
			if (src.peek() == '=')
			{
				src.get();
				return Token(GTE, ">=");
			}
			else 
			{
				return Token(GT, ">");
			}
			break;
		case '<':
			if (src.peek() == '=') 
			{
				src.get();
				return Token(LTE, "<=");
			}
			else 
			{
				return Token(LT, "<");
			}
			break;
		case '!':
			if (src.peek() == '=')
			{
				src.get();
				return Token(NEQ, "!=");
			}
			else 
			{
				return Token(ERROR, string("�ʷ�������")+to_string(line_count)+string("�У�δʶ��ķ���!"));
			}
			break;
		case '/':
			//��ע��
			if (src.peek() == '/')
			{
				char buf[1024];
				src.getline(buf, 1024);
				return Token(LCOMMENT, string("/")+buf);
			}
			//��ע��
			else if (src.peek() == '*') 
			{
				src.get();
				string buf = "/*";
				while (src >> c)
				{
					buf += c;
					if (c == '*') 
					{
						src >> c;
						buf += c;
						if (c == '/')
						{
							return Token(PCOMMENT, buf);
							break;
						}
					}
				}
				//�������û�ҵ�*/��������whileѭ�������˳�
				if (src.eof()) 
				{
					return Token(ERROR, string("�ʷ�������")+to_string(line_count)+string("�У���ע��û��ƥ���*/"));
				}
			}
			//����
			else 
			{
				return Token(DIV, "/");
			}
			break;
		default:
			if (c == '0' && (src.peek() == 'x' || src.peek() == 'X'))
			{
				src.get();
				string str = "";
				while (isxdigit(src.peek()))
				{
					str += src.peek();
					src.get();
				}
				long long num = 0;
				for (char a : str)
				{
					num *= 16;
					if (c >= '0' && c <= '9') 
					{
						num += c - '0';
					}
					else if (c >= 'a' && c <= 'f') 
					{
						num += 10 + (c - 'a');
					}
					else if (c >= 'A' && c <= 'F') 
					{
						num += 10 + (c - 'A');
					}
				}
				return Token(NUM_INT, to_string(num));
			}
			else if (c == '0' && (src.peek() == 'o' || src.peek() == 'O'))
			{
				src.get();
				string str = "";
				while (src.peek()>='0'&& src.peek()<='7')
				{
					str += src.peek();
					src.get();
				}
				long long num = 0;
				for (char a : str)
				{
					num *= 8;
					num += a - '0';
				}
				return Token(NUM_INT, to_string(num));
			}
			else if (c == '0' && (src.peek() == 'b' || src.peek() == 'B'))
			{
				src.get();
				string str = "";
				while (src.peek() >= '0' && src.peek() <= '1')
				{
					str += src.peek();
					src.get();
				}
				long long num = 0;
				for (char a : str)
				{
					num *= 2;
					num += a - '0';
				}
				return Token(NUM_INT, to_string(num));
			}
			if (isdigit(c))
			{
				string buf, buf1;
				buf.push_back(c);
				while (c=src.peek())
				{
					if (isdigit(c)) 
					{
						src >> c;
						buf += c;
					}
					else 
					{
						break;
					}
				}
				if (c == 'e' || c == 'E')
				{
					bool flag = false;
					src.get();
					buf1.push_back('e');
					if (src.peek() == '-' && !flag)
					{
						src.get();
						buf1 += '-';
						flag = 1;
					}
					else if (src.peek() == '-' && flag)
					{
						return Token(ERROR, string("��") + to_string(line_count) + string("��ѧ��������ʾ����"));
					}
					while (c = src.peek())
					{
						if (isdigit(c))
						{
							src >> c;
							buf += c;
						}
						else
						{
							break;
						}
					}
					buf += buf1;
					return Token(NUM_FLOAT, buf);
				}
				return Token(NUM_INT, buf);
			}
			else if (isalpha(c)) 
			{
				string buf;
				buf.push_back(c);
				while (c = src.peek()) 
				{
					if (isdigit(c)||isalpha(c)) 
					{
						src >> c;
						buf += c;
					}
					else
					{
						break;
					}
				}
				if (buf == "int") 
				{
					return Token(INT, "int");
				}
				else if (buf == "void") 
				{
					return Token(VOID, "void");
				}
				else if (buf == "if")
				{
					return Token(IF, "if");
				}
				else if (buf == "else")
				{
					return Token(ELSE, "else");
				}
				else if (buf == "float")
				{
					return Token(FLOAT, "float");
				}
				else if (buf == "char")
				{
					return Token(CHAR, "char");
				}
				else if (buf == "while") 
				{
					return Token(WHILE, "while");
				}
				else if (buf == "return") 
				{
					return Token(RETURN, "return");
				}
				else 
				{
					return Token(ID, buf);
				}
			}
			else if (c == '\'')
			{
				char buf;
				src >> buf;
				cout << buf << endl;
				if (src.get() != '\'')
				{
					return Token(ERROR, string("��") + to_string(line_count) + string("�ַ���ʾ����"));
				}
				int a = buf;
				string buf1 = to_string(a);
				return Token(CHAR_VAL, buf1);
			}
			else
			{
				return Token(ERROR, string("�ʷ�������") + to_string(line_count) + string("�У�δʶ��ķ���") + c);
			}
	}
	return Token(ERROR, "UNKOWN ERROR");
}

void LexicalAnalyser::analyse() 
{
	while (1)
	{
		Token t = getNextToken();
		token_list.push_back(t);
		if (t.first == ERROR) 
		{
			outputError(t.second);
		}
		else if (t.first == ENDFILE) 
		{
			break;
		}
	}
}

void LexicalAnalyser::outputToStream(ostream&out)
{
	if (token_list.back().first == ERROR)
	{
		out << token_to_string(token_list.back())<<endl;
	}
	else 
	{
		list<Token>::iterator iter;
		for (iter = token_list.begin(); iter != token_list.end(); iter++)
		{
			out << token_to_string(*iter) << endl;
		}
	}
}

void LexicalAnalyser::outputToScreen() 
{
	outputToStream(cout);
}

void LexicalAnalyser::outputToFile(const char *fileName) 
{
	ofstream fout;
	fout.open(fileName, ios::out);
	if (!fout.is_open()) 
	{
		cerr << "file " << fileName << " open error" << endl;
		return;
	}
	outputToStream(fout);
	fout.close();
}

list<Token>&LexicalAnalyser::getResult() 
{
	return token_list;
}