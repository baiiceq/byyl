#include"LexicalAnalyser.h"
#include "ParserAndSemanticAnalyser.h"
#include "ObjectCodeGenerator.h"
#include "Optimizer.h"
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

	Optimizer optimizer(*code);
	optimizer.optimize();
	IntermediateCode optimized_code = optimizer.get_optimization_result();
	optimized_code.outputBlocks("optimized_block.txt");

	/*ObjectCodeGenerator objectCodeGenerator;
	objectCodeGenerator.analyseBlock(code->getFuncBlock());
	objectCodeGenerator.outputIBlocks();*/
	//objectCodeGenerator.generateCode();
	//objectCodeGenerator.outputObjectCode();
	//objectCodeGenerator.outputObjectCode("program.asm");

	return 0;
}