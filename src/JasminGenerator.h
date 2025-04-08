#ifndef JASMIN_GENERATOR_H
#define JASMIN_GENERATOR_H

class Data{
public:
    std::map<std::string,int> var_to_slots;
    std::set<std::string> m_assigned_vars;
    std::vector<Error*> m_errors;
    size_t stack_size=0;
    size_t max_stack_size=0;
    void pushed();
    void popped();
    void add_error(size_t line,const std::string &msg);
    size_t get_error_count() const { return m_errors.size();};
    ~Data();
};

std::set<std::string> collect_variables(Program parse_tree);
std::string generate_code_jasmin(Program parse_tree,Data &data);
std::map<std::string,int> create_map(const std::set<std::string> &variables);

#endif 