#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parser.h"
#include "Printer.h"
#include "Absyn.h"

void print_tree(Exp e){
    switch (e->kind)
    {
    case Exp_::is_ExpAdd:
        printf("+\n");
        print_tree(e->u.expdiv_.exp_1);
        print_tree(e->u.expdiv_.exp_2);
        break;
    case Exp_::is_ExpSub:
        printf("-\n");
        print_tree(e->u.expdiv_.exp_1);
        print_tree(e->u.expdiv_.exp_2);
        break;
    case Exp_::is_ExpMul:
        printf("*\n");
        print_tree(e->u.expdiv_.exp_1);
        print_tree(e->u.expdiv_.exp_2);
        break;
    case Exp_::is_ExpDiv:
        printf("/\n");
        print_tree(e->u.expdiv_.exp_1);
        print_tree(e->u.expdiv_.exp_2);
        break;
    case Exp_::is_ExpLit:
        printf("%d",e->u.explit_.integer_);
        break;
    case Exp_::is_ExpVar:
        printf("%s",e->u.expvar_.ident_);
        break;
    default:
        break;
    }
}

void my_show_tree(Program p){
    printf("Start Program \n");
    ListStmt current = p->u.prog_.liststmt_;
    for(;current;current=current->liststmt_){
        switch(current->stmt_->kind){
            case Stmt_::is_SAss:
                printf("%s ",current->stmt_->u.sass_.ident_);
                print_tree(current->stmt_->u.sass_.exp_);
                break;
            case Stmt_::is_SExp:
                print_tree(current->stmt_->u.sexp_.exp_);
                break; 
        }
    }
    printf("End Program \n");
}