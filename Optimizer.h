#pragma once

#include "Common.h"
#include "IntermediateCode.h"

struct DAG_node
{
	bool is_leaf;
	shared_ptr<DAG_node> left;
	shared_ptr<DAG_node> right;
	string main_variable;
	vector<string> variables;
	string op;

	DAG_node(bool leaf = false, const string& var = "")
		: is_leaf(leaf), left(nullptr), right(nullptr), main_variable(var) {}
};

class Optimizer
{
public:
	Optimizer(IntermediateCode code)
	{
		this->code = code;
	}

	void optimize();

	void optimize_block(vector<Quaternary>& qs);

	IntermediateCode get_optimization_result()
	{
		return code;
	}

private:
	IntermediateCode code;

private:

	string get_constant_value(const string& var, const map<string, shared_ptr<DAG_node> >& variable_map);

	void bind_variable_to_node
	(
		const std::string& src1,
		const std::string& result,
		std::map<std::string, std::shared_ptr<DAG_node>>& variable_map,
		std::vector<std::shared_ptr<DAG_node>>& dag_nodes,
		const std::string& op
	);

	bool is_constant(const string& s)
	{
		return isdigit(s[0]);
	}

	string compute(const string& src1, const string& src2, const char& op)
	{
		double num1 = std::stod(src1);
		double num2 = std::stod(src2);
		double ret;
		switch (op)
		{
		case '+':
			ret = num1 + num2;
			break;
		case '-':
			ret = num1 - num2;
			break;
		case '*':
			ret = num1 * num2;
			break;
		case '/':
			if (num2 == 0)
			{
				ret = 9999999999999;
			}
			else
			{
				ret = num1 / num2;
			}
			break;
		default:
			break;
		}

		std::ostringstream oss;
		oss << std::fixed << std::setprecision(10) << ret;

		std::string result = oss.str();
		result.erase(result.find_last_not_of('0') + 1, std::string::npos);
		if (result.back() == '.') 
		{
			result.pop_back();
		}

		return result;
	}

	bool is_control_statement(const Quaternary& q)
	{
		if (q.op[0] == 'j' || q.op[0] == 'c' || q.op[0] == 'p' || q.op[0] == 'r' || q.op[0] == 'g') return true;

		return false;
	}

	bool is_temp_variable(const std::string& var)
	{
		return !var.empty() && var[0] == '$';
	}

	void generate_from_dag_node(
		const std::shared_ptr<DAG_node>& node,
		std::vector<Quaternary>& qs,
		std::set<std::shared_ptr<DAG_node>>& visited
	);
};