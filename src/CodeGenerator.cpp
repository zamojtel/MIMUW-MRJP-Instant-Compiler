#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Parser.h"
#include "Printer.h"
#include "Absyn.h"
#include "myShow.cpp"
#include <string>
#include <set>
#include <vector>

std::string rec_post_order(Exp node){
    std::string left;
    std::string right;

    switch(node->kind){
        case Exp_::is_ExpAdd:
            left=rec_post_order(node->u.expadd_.exp_1);
            right=rec_post_order(node->u.expadd_.exp_2);
            return "("+left+")+("+right+")";
        case Exp_::is_ExpDiv:
            left=rec_post_order(node->u.expadd_.exp_1);
            right=rec_post_order(node->u.expadd_.exp_2);
            return "("+left+")/("+right+")";
        case Exp_::is_ExpMul:
            left=rec_post_order(node->u.expadd_.exp_1);
            right=rec_post_order(node->u.expadd_.exp_2);
            return "("+left+")*("+right+")";
        case Exp_::is_ExpSub:
            left=rec_post_order(node->u.expadd_.exp_1);
            right=rec_post_order(node->u.expadd_.exp_2);
            return "("+left+")-("+right+")";
        case Exp_::is_ExpLit:
            return std::to_string(node->u.explit_.integer_);
        case Exp_::is_ExpVar:
            return node->u.expvar_.ident_;
        default:
            std::cout<<"Blad"<<std::endl;
            break;
    }
    return "";
}

void check_undefined_variables(Exp node,const std::set<std::string> &d_v,std::set<std::string> &u_v){

    switch(node->kind){
        case Exp_::is_ExpAdd:
            check_undefined_variables(node->u.expadd_.exp_1,d_v,u_v);
            check_undefined_variables(node->u.expadd_.exp_2,d_v,u_v);
            break;
        case Exp_::is_ExpDiv:
            check_undefined_variables(node->u.expadd_.exp_1,d_v,u_v);
            check_undefined_variables(node->u.expadd_.exp_2,d_v,u_v);
            break;
        case Exp_::is_ExpMul:
            check_undefined_variables(node->u.expadd_.exp_1,d_v,u_v);
            check_undefined_variables(node->u.expadd_.exp_2,d_v,u_v);
            break;
        case Exp_::is_ExpSub:
            check_undefined_variables(node->u.expadd_.exp_1,d_v,u_v);
            check_undefined_variables(node->u.expadd_.exp_2,d_v,u_v);
            break;
        case Exp_::is_ExpLit:
            break;
        case Exp_::is_ExpVar:{
            std::string var = node->u.expvar_.ident_;
            std::string line_number = std::to_string(node->line_number);
            if(d_v.count(var)){
                return;
            }else{
                std::string message = "Undefined Variable: "+var+" line: "+line_number+"\n";
                u_v.insert(message);
            }
            break;
        }
        default:
            std::cout<<"Blad"<<std::endl;
            break;
    }
}

void print_undef_vars(const std::set<std::string> &u_v){
    for(auto &message : u_v){
        std::cout<<message<<std::endl;
    }
}

std::string generate_code(Program parse_tree){
    ListStmt current = parse_tree->u.prog_.liststmt_;
    std::vector<std::string> lines;
    std::set<std::string> def_variables;
    std::set<std::string> undef_variables;
    for(;current;current=current->liststmt_){
        //is_SAss, is_SExp 
        switch (current->stmt_->kind)
        {
        case Stmt_::is_SAss:{
            //First we check if there are no undefined variables in the parse tree
            check_undefined_variables(current->stmt_->u.sass_.exp_,def_variables,undef_variables);
            if(!undef_variables.empty()){
                print_undef_vars(undef_variables);
                // throwing an error, error in this case is an empty string
                return "";
            }
            
            std::string value=rec_post_order(current->stmt_->u.sass_.exp_);
            if(def_variables.count(current->stmt_->u.sass_.ident_)==1){
                std::string code = std::string(current->stmt_->u.sass_.ident_)+" = "+value+";";
                std::cout<<code<<std::endl;
                lines.push_back(code);
            }else{
                std::cout<<"Without identifier"<<std::endl;
                std::string id = std::string(current->stmt_->u.sass_.ident_);
                def_variables.insert(id);
                std::string code = "int "+std::string(id)+" = "+value+";";
                // std::cout<<code<<std::endl;
                lines.push_back(code);
            }
            break;
        }
        case Stmt_::is_SExp:{
            std::string value=rec_post_order(current->stmt_->u.sexp_.exp_);
            std::string final_result = "std::cout<<"+value+"<<std::endl;";
            return final_result;
        }
        default:
            break;
        }
    }

    return "";
}
