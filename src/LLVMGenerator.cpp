#include "Includes.h"

std::string get_child_index(Exp node,DataLLVM &data){
    if(data.m_nodes_to_id.count(node)==1)
        return "%"+std::to_string(data.m_nodes_to_id[node]);
    else
        return std::to_string(node->u.explit_.integer_);
}

void rec_post_order_llvm(Exp node,DataLLVM &data){
    class StackEntry{
    public:
        Exp m_node;
        StackEntry(Exp node):m_node{node}{}
        bool are_children_processed=false;
    };

    std::string code;
    std::list<StackEntry> stack;
    
    stack.push_back(node);
    while(!stack.empty()){
        StackEntry &current_entry = stack.back();

        switch(current_entry.m_node->kind){
            case Exp_::is_ExpAdd:{
                if(!current_entry.are_children_processed){
                    current_entry.are_children_processed=true;
                    stack.push_back(current_entry.m_node->u.expadd_.exp_2);
                    stack.push_back(current_entry.m_node->u.expadd_.exp_1);
                    break;
                }else{
                    Exp right_child = current_entry.m_node->u.expadd_.exp_2;
                    Exp left_child = current_entry.m_node->u.expadd_.exp_1;

                    std::string op_1 = get_child_index(left_child,data);
                    std::string op_2 = get_child_index(right_child,data);
                    code = fmt::format("%{} = add nsw i32 {}, {}",data.m_node_current_index,op_1,op_2);
                    data.m_nodes_to_id[current_entry.m_node]=data.m_node_current_index;
                    data.m_lines.push_back(code);
                    data.increase();
                    stack.pop_back();
                    break;
                }
            }
            case Exp_::is_ExpDiv:{
                if(!current_entry.are_children_processed){
                    current_entry.are_children_processed=true;
                    stack.push_back(current_entry.m_node->u.expdiv_.exp_2);
                    stack.push_back(current_entry.m_node->u.expdiv_.exp_1);
                    break;
                }else{
                    Exp left_child = current_entry.m_node->u.expdiv_.exp_1;
                    Exp right_child = current_entry.m_node->u.expdiv_.exp_2;

                    std::string op_1 = get_child_index(left_child,data);
                    std::string op_2 = get_child_index(right_child,data);
                    code = fmt::format("%{} = sdiv i32 {}, {}",data.m_node_current_index,op_1,op_2);
                    data.m_nodes_to_id[current_entry.m_node]=data.m_node_current_index;
                    data.m_lines.push_back(code);
                    data.increase();   
                    stack.pop_back();
                    break;
                }
            }
            case Exp_::is_ExpMul:{
                if(!current_entry.are_children_processed){
                    current_entry.are_children_processed=true;
                    stack.push_back(current_entry.m_node->u.expmul_.exp_2);
                    stack.push_back(current_entry.m_node->u.expmul_.exp_1);
                    break;
                }else{
                    Exp left_child = current_entry.m_node->u.expmul_.exp_1;
                    Exp right_child = current_entry.m_node->u.expmul_.exp_2;

                    std::string op_1 = get_child_index(left_child,data);
                    std::string op_2 = get_child_index(right_child,data);
                    code = fmt::format("%{} = mul nsw i32 {}, {}",data.m_node_current_index,op_1,op_2);
                    data.m_nodes_to_id[current_entry.m_node]=data.m_node_current_index;
                    data.m_lines.push_back(code);
                    data.increase();   
                    stack.pop_back();
                    break;
                }
            }
            case Exp_::is_ExpSub:{
                 if(!current_entry.are_children_processed){
                    current_entry.are_children_processed=true;
                    stack.push_back(current_entry.m_node->u.expsub_.exp_2);
                    stack.push_back(current_entry.m_node->u.expsub_.exp_1);
                    break;
                }else{
                    Exp left_child = current_entry.m_node->u.expsub_.exp_1;
                    Exp right_child = current_entry.m_node->u.expsub_.exp_2;

                    std::string op_1 = get_child_index(left_child,data);
                    std::string op_2 = get_child_index(right_child,data);
                    code = fmt::format("%{} = sub nsw i32 {}, {}",data.m_node_current_index,op_1,op_2);
                    data.m_nodes_to_id[current_entry.m_node]=data.m_node_current_index;
                    data.m_lines.push_back(code);
                    data.increase();   
                    stack.pop_back();
                    break;
                }
            }
            case Exp_::is_ExpLit:{
                stack.pop_back();
                break;
            }
            case Exp_::is_ExpVar:{
                code = fmt::format("%{} = load i32, ptr %{}, align 4",data.m_node_current_index,data.m_var_to_slots.at(current_entry.m_node->u.expvar_.ident_));
                data.m_nodes_to_id[current_entry.m_node]=data.m_node_current_index;
                data.m_lines.push_back(code);
                data.increase();
                stack.pop_back();
                break;
            }
            default:
                std::cout<<"error"<<std::endl;
                break;
        }
    }
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
        std::string code = fmt::format("%{} = alloca i32, align 4",data.m_node_current_index);
        data.increase();
        data.m_lines.push_back(code);
    }

    for(;current;current=current->liststmt_){
        switch (current->stmt_->kind)
        {
        case Stmt_::is_SAss:{
            rec_post_order_llvm(current->stmt_->u.sexp_.exp_,data);
            std::string right_value = get_child_index(current->stmt_->u.sexp_.exp_,data);
            std::string line = fmt::format("store i32 {}, ptr %{}, align 4",right_value,data.m_var_to_slots.at(current->stmt_->u.sass_.ident_));
            data.m_lines.push_back(line);
            break;   
        }
        case Stmt_::is_SExp:{
            rec_post_order_llvm(current->stmt_->u.sexp_.exp_,data);
            std::string variable = get_child_index(current->stmt_->u.sexp_.exp_,data);
            std::string line = fmt::format("%{}= call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef {})",data.m_node_current_index,variable);
            data.increase();
            data.m_lines.push_back(line);
            break;
        }
        default:
            break;
        }
    }
}