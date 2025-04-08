#include "Includes.h"

std::map<Exp,int> m_subtree_size;

std::set<std::string> collect_variables(Program parse_tree){
    std::set<std::string> variables;
    ListStmt current = parse_tree->u.prog_.liststmt_;

    for(;current;current=current->liststmt_){
        if(current->stmt_->kind==Stmt_::is_SAss){
            if(current->stmt_->u.sass_.ident_==nullptr)
                std::cout<<"Ident is null"<<std::endl;
            
            std::string indentifier = current->stmt_->u.sass_.ident_;
            if(!variables.count(indentifier))
                variables.insert(indentifier);
        }
    }

    return variables;
}

std::string generate_pushed_number(int value){
    if(value <=5 && value >=0)
        return "iconst_"+std::to_string(value);
    else if(value <=127 && value >=-128)
        return "bipush "+std::to_string(value);
    else if(value<=32767 && value>=-32768)
        return std::string("sipush ")+std::to_string(value);
    else
        return std::string("ldc ")+std::to_string(value);
}

std::string generate_store_command(int value){
    if(value <=3 && value >=0)
        return "istore_"+std::to_string(value);
    else
        return std::string("istore ")+std::to_string(value);
}

std::string generate_load_command(int value){
    if(value <=3 && value >=0)
        return "iload_"+std::to_string(value);
    else
        return std::string("iload ")+std::to_string(value);
}

// OK
// void Data::pushed(){
//     stack_size++;
//     if(stack_size>max_stack_size)
//         max_stack_size=stack_size;
// }
// OK

// OK 
// void Data::popped(){stack_size--;}
// OK

// OK
// void Data::add_error(size_t line, const std::string &msg){
//     m_errors.push_back(new Error{line,msg});
// }
// OK


// Data::~Data(){
//     for(size_t i=0;i<m_errors.size();i++)
//         delete m_errors[i];
// }

int calculate_subtree_size(Exp node){
    if(node==nullptr)
        return 0;

    int value=0;
    switch (node->kind)
    {
    case Exp_::is_ExpAdd:
        value = calculate_subtree_size(node->u.expadd_.exp_1)+calculate_subtree_size(node->u.expadd_.exp_2);
        break;
    case Exp_::is_ExpSub:
        value = calculate_subtree_size(node->u.expsub_.exp_1)+calculate_subtree_size(node->u.expsub_.exp_2);
        break;
    case Exp_::is_ExpDiv:
        value = calculate_subtree_size(node->u.expdiv_.exp_1)+calculate_subtree_size(node->u.expdiv_.exp_2);
        break;
    case Exp_::is_ExpMul:
        value = calculate_subtree_size(node->u.expmul_.exp_1)+calculate_subtree_size(node->u.expmul_.exp_2);
        break;
    case Exp_::is_ExpLit:
    case Exp_::is_ExpVar:
        value = 0;
        break;
    default:
        break;
    }
    // plus 1 with root
    m_subtree_size[node] = value+1;

    return value+1;
}

void rec_post_order(Exp node,Data &data,std::vector<std::string> &lines){
    class StackEntry{
    public:
        Exp m_node=nullptr;
        bool m_is_swap=false;
        StackEntry(Exp node):m_node{node},m_is_swap{false}{}
        bool are_children_processed=false;
    };
    
    std::list<StackEntry> container;
    container.push_front(node);
   
    while(!container.empty()){
        StackEntry &current_entry = container.front();
        container.pop_front();
        switch(current_entry.m_node->kind){
            case Exp_::is_ExpAdd:{
                // OK
                Exp left_child = current_entry.m_node->u.expadd_.exp_1;
                Exp right_child = current_entry.m_node->u.expadd_.exp_2;
                if(!current_entry.are_children_processed){
                    current_entry.are_children_processed=true;
                    container.push_front(current_entry);
                    if(m_subtree_size.at(left_child)>=m_subtree_size.at(right_child)){
                        container.push_front(right_child);
                        container.push_front(left_child);
                    }else{
                        container.push_front(left_child);
                        container.push_front(right_child);
                    }
                }else{
                    lines.push_back("iadd");
                    data.popped();
                }
                break;
            }
            case Exp_::is_ExpDiv:{
                Exp left_child = current_entry.m_node->u.expdiv_.exp_1;
                Exp right_child = current_entry.m_node->u.expdiv_.exp_2;
                if(!current_entry.are_children_processed){
                    current_entry.are_children_processed=true;
                    container.push_front(current_entry);
                    if(m_subtree_size.at(left_child)>=m_subtree_size.at(right_child)){
                        container.push_front(right_child);
                        container.push_front(left_child);
                    }else{
                        container.push_front(left_child);
                        container.push_front(right_child);
                        current_entry.m_is_swap=true;
                    }
                }else{
                    if(current_entry.m_is_swap)
                        lines.push_back("swap");

                    lines.push_back("idiv");
                    data.popped();
                }
                break;
            }
            case Exp_::is_ExpMul:{
                Exp left_child = current_entry.m_node->u.expmul_.exp_1;
                Exp right_child = current_entry.m_node->u.expmul_.exp_2;
                if(!current_entry.are_children_processed){
                    current_entry.are_children_processed=true;
                    container.push_front(current_entry);
                    if(m_subtree_size.at(left_child)>=m_subtree_size.at(right_child)){
                        container.push_front(right_child);
                        container.push_front(left_child);
                    }else{
                        container.push_front(left_child);
                        container.push_front(right_child);
                    }
                }else{
                    lines.push_back("imul");
                    data.popped();
                }
                break;
            }
            case Exp_::is_ExpSub:{
                Exp left_child = current_entry.m_node->u.expsub_.exp_1;
                Exp right_child = current_entry.m_node->u.expsub_.exp_2;
                if(!current_entry.are_children_processed){
                    current_entry.are_children_processed=true;
                    container.push_front(current_entry);
                    if(m_subtree_size.at(left_child)>=m_subtree_size.at(right_child)){
                        container.push_front(right_child);
                        container.push_front(left_child);
                    }else{
                        container.push_front(left_child);
                        container.push_front(right_child);
                        current_entry.m_is_swap=true;
                    }
                }else{
                    if(current_entry.m_is_swap)
                        lines.push_back("swap");
                    lines.push_back("isub");
                    data.popped();
                }
                break;
            }
            case Exp_::is_ExpLit:
            {
                std::string generated_code = generate_pushed_number(current_entry.m_node->u.explit_.integer_);
                lines.push_back(generated_code);
                data.pushed();
                break;
            }
            case Exp_::is_ExpVar:{
                if(data.m_assigned_vars.count(current_entry.m_node->u.expvar_.ident_)>0){
                    std::string code = generate_load_command(data.var_to_slots.at(current_entry.m_node->u.expvar_.ident_));
                    lines.push_back(code);
                    data.pushed();
                }else
                    data.add_error((size_t)current_entry.m_node->line_number,fmt::format("Unitialized variable : {} ",current_entry.m_node->u.expvar_.ident_));
                break;
            }
            default:
                std::cout<<"Internal Compiler Error"<<std::endl;
                break;
        }
    }
}

std::string process_lines(const std::vector<std::string> &lines){
    if(lines.size()==0)
        return "";
        
    std::string final_result;
    
    for(auto &line:lines)
        final_result+=line+"\n";

    return final_result;
}

std::string generate_code_jasmin(Program parse_tree,Data &data){
    ListStmt current = parse_tree->u.prog_.liststmt_;
    std::vector<std::string> lines;
    std::set<std::string> def_variables;
    std::set<std::string> undef_variables;

    for(;current;current=current->liststmt_){
        switch (current->stmt_->kind)
        {
        case Stmt_::is_SAss:{
            calculate_subtree_size(current->stmt_->u.sexp_.exp_);

            rec_post_order(current->stmt_->u.sexp_.exp_,data,lines);

            std::string code = generate_store_command(data.var_to_slots.at(std::string(current->stmt_->u.sass_.ident_)));
            lines.push_back(code);

            data.m_assigned_vars.insert(current->stmt_->u.sass_.ident_); 
            data.popped();
            break;
        }
        case Stmt_::is_SExp:{
            calculate_subtree_size(current->stmt_->u.sexp_.exp_);
            data.pushed();
            lines.push_back("getstatic java/lang/System/out Ljava/io/PrintStream;");
            rec_post_order(current->stmt_->u.sexp_.exp_,data,lines);
            lines.push_back("invokevirtual java/io/PrintStream/println(I)V");
            data.popped();
            data.popped();
            break;
        }
        default:
            break;
        }
    }

    return process_lines(lines);
}

std::map<std::string,int> create_map(const std::set<std::string> &variables){
    std::map<std::string,int> map;
    int index = 1;

    for(auto &var : variables)
        map[var]=index++;

    return map;
}