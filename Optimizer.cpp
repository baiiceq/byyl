#include "Optimizer.h"

void Optimizer::optimize()
{
	map<string, vector<Block> >* funcBlocks = code.getFuncBlock();
	for (map<string, vector<Block> >::iterator iter = funcBlocks->begin(); iter != funcBlocks->end(); iter++)
	{
		for (vector<Block>::iterator bIter = iter->second.begin(); bIter != iter->second.end(); bIter++)
		{
            optimize_block(bIter->codes);
		}
	}
}

void Optimizer::optimize_block(vector<Quaternary>& qs)
{
	vector<shared_ptr<DAG_node>> dag_nodes;
	map<string, shared_ptr<DAG_node>> variable_map; // 变量 -> DAG节点
	map<string, string> var_to_main;

	for (auto& q : qs)
	{
		const string& op = q.op;
		const string& src1 = q.src1;
		const string& src2 = q.src2;
		const string& result = q.des;

		if (op == "=")
		{
            bind_variable_to_node(src1, result, variable_map, dag_nodes, "=");
		}
        else if (op == "+" || op == "-" || op == "*" || op == "/")
        {
            if (src2 == "_")
            {

            }
            else
            {
                string val1 = get_constant_value(src1, variable_map);
                string val2 = get_constant_value(src2, variable_map);

                if (!val1.empty() && !val2.empty())
                {
                    string compute_result = compute(val1, val2, op[0]);
                    bind_variable_to_node(compute_result, result, variable_map, dag_nodes, "=");
                }
                else
                {
                    shared_ptr<DAG_node> left, right;

                    if (variable_map.count(src1))
                    {
                        left = variable_map[src1];
                    }
                    else
                    {
                        left = make_shared<DAG_node>(true, src1);
                        left->variables.push_back(src1);
                        dag_nodes.push_back(left);
                        variable_map[src1] = left;
                    }

                    if (variable_map.count(src2))
                    {
                        right = variable_map[src2];
                    }
                    else
                    {
                        right = make_shared<DAG_node>(true, src2);
                        right->variables.push_back(src2);
                        dag_nodes.push_back(right);
                        variable_map[src2] = right;
                    }

                    if (variable_map.count(result))
                    {
                        auto& old_node = variable_map[result];
                        auto& vars = old_node->variables;
                        vars.erase(std::remove(vars.begin(), vars.end(), result), vars.end());

                        if (old_node->main_variable == result) 
                        {
                            if (!vars.empty())
                            {
                                old_node->main_variable = vars.front();
                            }
                        }
                    }

                    // 查找是否已有相同子表达式
                    bool found = false;
                    for (auto& node : dag_nodes)
                    {
                        if (!node->is_leaf && node->op == op && node->left == left && node->right == right)
                        {
                            if (!is_constant(result) && (node->main_variable == "" || is_temp_variable(node->main_variable)))
                            {
                                node->main_variable = result;
                            }
                            variable_map[result] = node;
                            node->variables.push_back(result);
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                    {
                        auto new_node = make_shared<DAG_node>(false, result);
                        new_node->left = left;
                        new_node->right = right;
                        new_node->op = op;
                        new_node->variables.push_back(result);
                        variable_map[result] = new_node;
                        dag_nodes.push_back(new_node);
                    }
                }
            }
        }
	}

    std::vector<Quaternary> new_qs;
    std::set<std::shared_ptr<DAG_node>> visited;

    bool is_optimized = false;

    for (auto q : qs)
    {
        if (is_control_statement(q))
        {
            new_qs.push_back(q);
        }
        else
        {
            if (!is_optimized)
            {
                is_optimized = true;
                for (auto& node : dag_nodes) 
                {
                    generate_from_dag_node(node, new_qs, visited);
                }
            }
            
        }
    }

    qs = new_qs;
}

string Optimizer::get_constant_value(const string& var, const map<string, shared_ptr<DAG_node>>& variable_map)
{
    if(is_constant(var))
        return var;
    if (variable_map.count(var) && is_constant(variable_map.at(var)->main_variable))
        return variable_map.at(var)->main_variable;
    return ""; 
}

void Optimizer::bind_variable_to_node(const std::string& src1, const std::string& result, std::map<std::string, std::shared_ptr<DAG_node>>& variable_map, std::vector<std::shared_ptr<DAG_node>>& dag_nodes, const std::string& op)
{
    std::shared_ptr<DAG_node> child;

    // 查找 src1 对应的节点或新建节点
    if (variable_map.count(src1)) 
    {
        child = variable_map[src1];

        if (!is_constant(src1) && (child->main_variable == "" || is_temp_variable(child->main_variable)))
        {
            child->main_variable = src1;
        }
    }
    else 
    {
        child = std::make_shared<DAG_node>(true, src1);
        child->variables.push_back(src1);
        child->op = op;
        dag_nodes.push_back(child);
        variable_map[src1] = child;
    }

    // 删除 result 旧节点中的映射和变量名
    if (variable_map.count(result))
    {
        auto& old_node = variable_map[result];
        auto& vars = old_node->variables;
        vars.erase(std::remove(vars.begin(), vars.end(), result), vars.end());

        if (old_node->main_variable == result) 
        {
            if (!vars.empty())
            {
                old_node->main_variable = vars.front();
            }
        }
    }

    // 绑定 result 到新节点
    variable_map[result] = child;
    child->variables.push_back(result);
    if (!is_constant(result) && (child->main_variable == "" || is_temp_variable(child->main_variable)))
    {
        child->main_variable = result;
    }
}

void Optimizer::generate_from_dag_node(const std::shared_ptr<DAG_node>& node, std::vector<Quaternary>& qs, std::set<std::shared_ptr<DAG_node>>& visited)
{
    if (!node || visited.count(node)) return;
    visited.insert(node);

    if (!node->is_leaf) 
    {
        // 先处理左右子树
        generate_from_dag_node(node->left, qs, visited);
        generate_from_dag_node(node->right, qs, visited);

        std::string op_str = node->op;

        std::string src1 = node->left->main_variable;
        std::string src2 = node->right ? node->right->main_variable : "";

        std::string result = node->main_variable;
        if (result.empty() && !node->variables.empty())
            result = node->variables.front();

        qs.push_back({ op_str, src1, src2, result });
        
        for (const std::string& var : node->variables)
        {
            if (var != result && !is_temp_variable(var))
            {
                qs.push_back({ "=", result, "_", var });
            }
        }
    }
    else 
    {
        std::string val = node->main_variable;
        for (const std::string& var : node->variables) 
        {
            if (var != val && !is_temp_variable(var))
            {
                qs.push_back({ "=", val, "_", var });
            }
        }
    }
}
