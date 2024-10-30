// #include <stdio.h>
// #include <stdlib.h>
// #include <iostream>

// #include "Parser.h"
// #include "Printer.h"
// #include "Absyn.h"
// #include <string>
// #include <set>
// #include <list>
// #include <vector>
// #include <map>
#include "Includes.h"

// class Data{
// private:
// public:
//     std::map<std::string,int> var_to_slots;
//     size_t stack_size=0;
//     size_t max_stack_size=0;
//     void pushed(){
//         stack_size++;
//         if(stack_size>max_stack_size)
//             max_stack_size=stack_size;
//     }

//     void popped(){
//         stack_size--;
//     }
// };

std::set<std::string> collect_variables(Program parse_tree){
    std::set<std::string> variables;
    ListStmt current = parse_tree->u.prog_.liststmt_;
    
    for(;current;current=current->liststmt_){
        if(current->stmt_->kind==Stmt_::is_SAss && current->stmt_->is_SAss==Stmt_::is_SAss){
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

void Data::pushed(){
    stack_size++;
    if(stack_size>max_stack_size)
        max_stack_size=stack_size;
}

void Data::popped(){stack_size--;}


void rec_post_order(Exp node,Data &data,std::vector<std::string> &lines){
    class StackEntry{
    public:
        Exp m_node;
        StackEntry(Exp node):m_node{node}{}
        bool are_children_processed=false;
    };
    
    std::list<StackEntry> stack;
    stack.push_back(node);

    while(!stack.empty()){
        StackEntry &current_entry = stack.back();

        switch(current_entry.m_node->kind){
        case Exp_::is_ExpAdd:{
            Exp left_child = current_entry.m_node->u.expadd_.exp_1;
            Exp right_child = current_entry.m_node->u.expadd_.exp_2;
            if(!current_entry.are_children_processed){
                current_entry.are_children_processed=true;
                // in case of addition 
                stack.push_back(left_child);
                stack.push_back(right_child);
                break;
            }else{
                lines.push_back("iadd");
                data.popped();
                stack.pop_back();
                break;
            }
        }
        case Exp_::is_ExpDiv:{
            Exp left_child = current_entry.m_node->u.expdiv_.exp_1;
            Exp right_child = current_entry.m_node->u.expdiv_.exp_2;
            if(!current_entry.are_children_processed){
                current_entry.are_children_processed=true;
                stack.push_back(right_child);
                stack.push_back(left_child);
                break;
            }else{
                lines.push_back("idiv");
                data.popped();
                stack.pop_back();
                break;
            }
        }
        case Exp_::is_ExpMul:{
            Exp left_child = current_entry.m_node->u.expmul_.exp_1;
            Exp right_child = current_entry.m_node->u.expmul_.exp_2;
            if(!current_entry.are_children_processed){
                current_entry.are_children_processed=true;
                stack.push_back(right_child);
                stack.push_back(left_child);
                break;
            }else{
                lines.push_back("imul");
                data.popped();
                stack.pop_back();
                break;
            }
        }
        case Exp_::is_ExpSub:{
            Exp left_child = current_entry.m_node->u.expsub_.exp_1;
            Exp right_child = current_entry.m_node->u.expsub_.exp_2;
            if(!current_entry.are_children_processed){
                current_entry.are_children_processed=true;
                stack.push_back(right_child);
                stack.push_back(left_child);
                break;
            }else{
                lines.push_back("isub");
                data.popped();
                stack.pop_back();
                break;
            }
        }
        case Exp_::is_ExpLit:
        {
            std::string generated_code = generate_pushed_number(current_entry.m_node->u.explit_.integer_);
            lines.push_back(generated_code);
            data.pushed();
            stack.pop_back();
            break;
        }
        case Exp_::is_ExpVar:{
            std::string code = generate_load_command(data.var_to_slots.at(current_entry.m_node->u.expvar_.ident_));
            lines.push_back(code);
            data.pushed();
            stack.pop_back();
            break;
        }
        default:
            std::cout<<"Error"<<std::endl;
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

    // getstatic java/lang/System/out Ljava/io/PrintStream;
    for(;current;current=current->liststmt_){
        switch (current->stmt_->kind)
        {
        case Stmt_::is_SAss:{
            rec_post_order(current->stmt_->u.sexp_.exp_,data,lines);
            std::string code = generate_store_command(data.var_to_slots.at(std::string(current->stmt_->u.sass_.ident_)));
            lines.push_back(code);
            break;
        }
        case Stmt_::is_SExp:{
            data.pushed(); //we invoke the virtual method so we need one extra place on the stack for this keyword
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
    int index = 1; //first of all is this

    for(auto &var : variables)
        map[var]=index++;

    return map;
}
