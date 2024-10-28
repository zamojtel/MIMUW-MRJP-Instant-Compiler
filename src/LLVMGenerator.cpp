#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Parser.h"
#include "Printer.h"
#include "Absyn.h"
#include <string>
#include <set>
#include <vector>
#include <map>
#include <format>

int stack_level= 0;
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

std::string get_child_index(Exp node,DataLLVM &data){
    if(data.m_nodes_to_id.count(node)==1)
        return "%"+std::to_string(data.m_nodes_to_id[node]);
    else
        return std::to_string(node->u.explit_.integer_);
}

void rec_post_order_llvm(Exp node,DataLLVM &data){
    std::string code;
    stack_level++;
    switch(node->kind){
        case Exp_::is_ExpAdd:{
            rec_post_order_llvm(node->u.expadd_.exp_1,data);
            rec_post_order_llvm(node->u.expadd_.exp_2,data);
            std::string op_1 = get_child_index(node->u.expadd_.exp_1,data);
            std::string op_2 = get_child_index(node->u.expadd_.exp_2,data);
            code = std::format("%{} = add nsw i32 {}, {}",data.m_node_current_index,op_1,op_2);
            data.m_nodes_to_id[node]=data.m_node_current_index;
            data.m_lines.push_back(code);
            data.increase();
            break;
        }
        case Exp_::is_ExpDiv:{
            rec_post_order_llvm(node->u.expadd_.exp_1,data);
            rec_post_order_llvm(node->u.expadd_.exp_2,data);
            std::string op_1 = get_child_index(node->u.expadd_.exp_1,data);
            std::string op_2 = get_child_index(node->u.expadd_.exp_2,data);
            code = std::format("%{} = sdiv i32 {}, {}",data.m_node_current_index,op_1,op_2);
            data.m_nodes_to_id[node]=data.m_node_current_index;
            data.m_lines.push_back(code);
            data.increase();
            break;
        }
        case Exp_::is_ExpMul:{
            rec_post_order_llvm(node->u.expadd_.exp_1,data);
            rec_post_order_llvm(node->u.expadd_.exp_2,data);
            std::string op_1 = get_child_index(node->u.expadd_.exp_1,data);
            std::string op_2 = get_child_index(node->u.expadd_.exp_2,data);
            code = std::format("%{} = mul nsw i32 {}, {}",data.m_node_current_index,op_1,op_2);
            data.m_nodes_to_id[node]=data.m_node_current_index;
            data.m_lines.push_back(code);
            data.increase();
            break;
        }
        case Exp_::is_ExpSub:{
            rec_post_order_llvm(node->u.expadd_.exp_1,data);
            rec_post_order_llvm(node->u.expadd_.exp_2,data);
            std::string op_1 = get_child_index(node->u.expadd_.exp_1,data);
            std::string op_2 = get_child_index(node->u.expadd_.exp_2,data);
            code = std::format("%{} = sub nsw i32 {}, {}",data.m_node_current_index,op_1,op_2);
            data.m_nodes_to_id[node]=data.m_node_current_index;
            data.m_lines.push_back(code);
            data.increase();
            break;
        }
        case Exp_::is_ExpLit:{
            
            break;
        }
        case Exp_::is_ExpVar:{
            code = std::format("%{} = load i32, ptr %{}, align 4",data.m_node_current_index,data.m_var_to_slots.at(node->u.expvar_.ident_));
            data.m_nodes_to_id[node]=data.m_node_current_index;
            data.m_lines.push_back(code);
            data.increase();
            break;
        }
        default:
            std::cout<<"Blad"<<std::endl;
            break;
    }
    stack_level--;
}

void test_tagging_tree(Exp node,DataLLVM &data2){
    rec_post_order_llvm(node,data2);
}

void generate_code_llvm(Program parse_tree,DataLLVM &data){
    ListStmt current = parse_tree->u.prog_.liststmt_;
    std::vector<std::string> lines;
    std::set<std::string> def_variables;
    std::set<std::string> undef_variables;
    
    for(size_t i=0;i<data.m_var_to_slots.size();i++){
        std::string code = std::format("%{} = alloca i32, align 4",data.m_node_current_index);
        data.increase();
        data.m_lines.push_back(code);
    }

    for(;current;current=current->liststmt_){
        switch (current->stmt_->kind)
        {
        case Stmt_::is_SAss:{
            rec_post_order_llvm(current->stmt_->u.sexp_.exp_,data);
            std::string right_value = get_child_index(current->stmt_->u.sexp_.exp_,data);
            std::string line = std::format("store i32 {}, ptr %{}, align 4",right_value,data.m_var_to_slots.at(current->stmt_->u.sass_.ident_));
            data.m_lines.push_back(line);
            break;   
        }
        case Stmt_::is_SExp:{
            rec_post_order_llvm(current->stmt_->u.sexp_.exp_,data);
            std::string variable = get_child_index(current->stmt_->u.sexp_.exp_,data);
            std::string line = std::format("%{}= call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef {})",data.m_node_current_index,variable);
            data.increase();
            data.m_lines.push_back(line);
            break;
        }
        default:
            break;
        }
    }
}