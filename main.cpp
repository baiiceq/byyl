#include"LexicalAnalyser.h"
#include "ParserAndSemanticAnalyser.h"
#include "ObjectCodeGenerator.h"
int main() 
{
	LexicalAnalyser lexicalAnalyser("test.txt");
	lexicalAnalyser.analyse();
	lexicalAnalyser.outputToFile("token_list.txt");
	
	ParserAndSemanticAnalyser parserAndSemanticAnalyser("productions.txt");
	parserAndSemanticAnalyser.outputDFA("DFA.txt");
	parserAndSemanticAnalyser.analyse(lexicalAnalyser.getResult(), "SLR1_analyse.txt");
	parserAndSemanticAnalyser.outputIntermediateCode("initial_quaternion.txt");

	IntermediateCode* code = parserAndSemanticAnalyser.getIntermediateCode();
	code->divideBlocks(parserAndSemanticAnalyser.getFuncEnter());
	code->outputBlocks("initial_block.txt");

	/*ObjectCodeGenerator objectCodeGenerator;
	objectCodeGenerator.analyseBlock(code->getFuncBlock());
	objectCodeGenerator.outputIBlocks();*/
	//objectCodeGenerator.generateCode();
	//objectCodeGenerator.outputObjectCode();
	//objectCodeGenerator.outputObjectCode("program.asm");

	return 0;
}