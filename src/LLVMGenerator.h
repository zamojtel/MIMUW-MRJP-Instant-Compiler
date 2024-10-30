#ifndef LLVM_GENERATOR_H
#define LLVM_GENERATOR_H

class DataLLVM{
private:
public:

    std::map<Exp,int> m_nodes_to_id;
    std::map<std::string,int> m_var_to_slots; 
    size_t stack_size=0;
    size_t max_stack_size=0;
    int m_node_current_index=1;
    std::vector<std::string> m_lines;
    void increase(){ m_node_current_index++;}
};

void generate_code_llvm(Program parse_tree,DataLLVM &data);
#endif