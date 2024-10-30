#ifndef JASMIN_GENERATOR_H
#define JASMIN_GENERATOR_H

class Data{
public:
    std::map<std::string,int> var_to_slots;
    size_t stack_size=0;
    size_t max_stack_size=0;
    void pushed();
    void popped();
};

std::set<std::string> collect_variables(Program parse_tree);
std::string generate_code_jasmin(Program parse_tree,Data &data);
std::map<std::string,int> create_map(const std::set<std::string> &variables);

#endif 