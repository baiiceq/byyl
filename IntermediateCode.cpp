#include "IntermediateCode.h"

NewLabeler::NewLabeler() {
	index = 1;
}

string NewLabeler::newLabel () {
	return string("Label") + to_string(index++);
}

void IntermediateCode::divideBlocks(vector<pair<int, string> > funcEnter) {
	for (vector<pair<int, string> >::iterator iter = funcEnter.begin(); iter != funcEnter.end(); iter++) {//��ÿһ��������
		vector<Block>blocks;
		priority_queue<int, vector<int>, greater<int> >block_enter;//��¼���л��������ڵ�
		block_enter.push(iter->first);

		int endIndex = iter + 1 == funcEnter.end()? code.size(): (iter + 1)->first;
		for (int i = iter->first; i != endIndex; i++) {
			if (code[i].op[0] == 'j') {
				if (code[i].op == "j") {//����������j
					block_enter.push(atoi(code[i].des.c_str()));
				}
				else {//������������j=-,,j!=.j>=��j>��j<=��j<
					if (i + 1 < endIndex) {
						block_enter.push(i + 1);
					}
					block_enter.push(atoi(code[i].des.c_str()));
				}
			}
			else if (code[i].op == "return" || code[i].op == "call") {
				if (i + 1 < endIndex) {
					block_enter.push(i + 1);
				}
			}
		}

		//���ֻ�����	
		Block block;
		map<int, string>labelEnter;//��ڵ�ͱ�ǩ�Ķ�Ӧ��ϵ
		map<int, int>enter_block;//������ڵ��block�Ķ�Ӧ��ϵ
		int firstFlag = true;//�������һ���ǣ��ÿ�����Ϊ������
		int enter;
		int lastEnter = block_enter.top();
		block_enter.pop();
		while (!block_enter.empty()) {
			//������Ԫʽ��block��
			enter = block_enter.top();
			block_enter.pop();

			if (enter == lastEnter) {
				continue;
			}

			for (int i = lastEnter; i != enter; i++) {
				block.codes.push_back(code[i]);
			}

			if (!firstFlag) {//�û����鲻�Ǻ�����ĵ�һ�������
				block.name = nl.newLabel();
				labelEnter[lastEnter] = block.name;
			}
			else {//�û������Ǻ�����ĵ�һ�������
				block.name = iter->second;
				firstFlag = false;
			}
			enter_block[lastEnter] = blocks.size();
			blocks.push_back(block);
			lastEnter = enter;
			block.codes.clear();
		}
		if (!firstFlag) {//�û����鲻�Ǻ�����ĵ�һ�������
			block.name = nl.newLabel();
			labelEnter[lastEnter] = block.name;
		}
		else {//�û������Ǻ�����ĵ�һ�������
			block.name = iter->second;
			firstFlag = false;
		}
		if (iter + 1 != funcEnter.end()) {//���������������֮��
			for (int i = lastEnter; i != (iter+1)->first; i++) {
				block.codes.push_back(code[i]);
			}
		}
		else {//�����һ���������м����ĩβ
			for (int i = lastEnter; i != code.size(); i++) {
				block.codes.push_back(code[i]);
			}
		}
		enter_block[lastEnter] = blocks.size();
		blocks.push_back(block);

		int blockIndex = 0;
		for (vector<Block>::iterator bIter = blocks.begin(); bIter != blocks.end(); bIter++, blockIndex++) {
			vector<Quaternary>::reverse_iterator lastCode = bIter->codes.rbegin();
			if (lastCode->op[0] == 'j') {
				if (lastCode->op == "j") {//����������j
					bIter->next1 = enter_block[atoi(lastCode->des.c_str())];
					bIter->next2 = -1;
				}
				else {//������������j=-,,j!=.j>=��j>��j<=��j<
					bIter->next1 = blockIndex + 1;
					bIter->next2 = enter_block[atoi(lastCode->des.c_str())];
					bIter->next2 = bIter->next1 == bIter->next2 ? -1 : bIter->next2;
				}
				lastCode->des = labelEnter[atoi(lastCode->des.c_str())];
			}
			else if (lastCode->op == "return") {
				bIter->next1 = bIter->next2 = -1;
			}
			else {
				bIter->next1 = blockIndex + 1;
				bIter->next2 = -1;
			}
			
			
		}

		funcBlocks[iter->second] = blocks;
	}
}

void IntermediateCode::output(ostream& out) {
	int i = 0;
	for (vector<Quaternary>::iterator iter = code.begin(); iter != code.end(); iter++, i++) {
		out << setw(4) << i;
		out << "( " << iter->op << " , ";
		out << iter->src1 << " , ";
		out << iter->src2 << " , ";
		out << iter->des << " )";
		out << endl;
	}
}

void IntermediateCode::outputBlocks(ostream& out) {
	for (map<string, vector<Block> >::iterator iter = funcBlocks.begin(); iter != funcBlocks.end(); iter++) {
		out << "[" << iter->first << "]" << endl;
		for (vector<Block>::iterator bIter = iter->second.begin(); bIter != iter->second.end(); bIter++) {
			out << bIter->name << ":" << endl;
			for (vector<Quaternary>::iterator cIter = bIter->codes.begin(); cIter != bIter->codes.end(); cIter++) {
				out <<"    "<< "(" << cIter->op << "," << cIter->src1 << "," << cIter->src2 << "," << cIter->des << ")" << endl;
			}
			out << "    " << "next1 = " << bIter->next1 << endl;
			out << "    " << "next2 = " << bIter->next2 << endl;
		}
		cout << endl;
	}
}

void IntermediateCode::emit(Quaternary q) {
	code.push_back(q);
}

void IntermediateCode::emit(string op, string src1, string src2, string des) {
	emit(Quaternary{ op,src1,src2,des });
}

void IntermediateCode::back_patch(list<int>nextList, int quad) {
	for (list<int>::iterator iter = nextList.begin(); iter != nextList.end(); iter++) {
		code[*iter].des = to_string(quad);
	}
}

void IntermediateCode::output() {
	output(cout);
}

void IntermediateCode::output(const char* fileName) {
	ofstream fout;
	fout.open(fileName);
	if (!fout.is_open()) {
		cerr << "file " << fileName << " open error" << endl;
		return;
	}
	output(fout);

	fout.close();
}

void IntermediateCode::outputBlocks() {
	outputBlocks(cout);
}

void IntermediateCode::outputBlocks(const char* fileName) {
	ofstream fout;
	fout.open(fileName);
	if (!fout.is_open()) {
		cerr << "file " << fileName << " open error" << endl;
		return;
	}
	outputBlocks(fout);

	fout.close();
}

int IntermediateCode::nextQuad() {
	return code.size();
}

map<string, vector<Block> >* IntermediateCode::getFuncBlock() {
	return &funcBlocks;
}