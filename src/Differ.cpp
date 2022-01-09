#include "headers/Differ.h"
#include "headers/DSL.h"

#include <stdlib.h>
#include <assert.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_OP_SIZE 2
#define MAX_UNARY_OP_SIZE 8
#define MAX_VAR_SIZE 8
#define MAX_NUM_SIZE 16
#define MAX_CONST_SIZE 4

Node* Differ::get_root_expr(){

    Node* new_root = get_expr();
    
    SYNTAX_ERROR(REQUIRE('\n'), PRINT_ERROR_POS; exit(0));

    return new_root;
}

Node* Differ::get_expr(){

    Node* fir_arg = get_div_mul();
    char cur_op[MAX_OP_SIZE] = {'\0'};

    while (REQUIRE('+') || REQUIRE('-')){

        cur_op[0] = line[cur_pos];
        cur_pos++;
        fir_arg = new Node(fir_arg, cur_op, get_div_mul());
    }

    return fir_arg;
}

Node* Differ::get_div_mul(){

    Node* fir_arg = get_pow();
    char cur_op[MAX_OP_SIZE] = {'\0'};

    while (REQUIRE('*') || REQUIRE('/')){

        cur_op[0] = line[cur_pos];
        cur_pos++;
        fir_arg = new Node(fir_arg, cur_op, get_pow());
    }

    return fir_arg;
}

Node* Differ::get_pow(){

    Node* fir_arg = get_args();
    char cur_op[MAX_OP_SIZE] = {'\0'};

    if (REQUIRE('^')){ 

        cur_op[0] = '^';
        cur_pos++;
        fir_arg = new Node(fir_arg, cur_op, get_args());
    }

    return fir_arg;
}

Node* Differ::get_args(){

    Node* fir_arg = nullptr;

    if (REQUIRE('(')){

        cur_pos++;
        fir_arg = get_expr();
        SYNTAX_ERROR(REQUIRE(')'), PRINT_ERROR_POS; exit(0));
        cur_pos++;

        return fir_arg;
    }

    fir_arg = get_num();
    IS_REG_EXPR_TRUTHFUL;

    fir_arg = get_const();
    IS_REG_EXPR_TRUTHFUL;

    fir_arg = get_unary_op();
    IS_REG_EXPR_TRUTHFUL;

    fir_arg = get_var();
    return fir_arg;
}

int copy_word(char* line, char* buf, int max_size){

    int i = 0;

    while ((line[i] >= 'a') && (line[i] <= 'z')){ 

        buf[i] = line[i];
        i++;

        if (i >= max_size){

            return -1;
        }
    }
    buf[i] = '\0';

    return i;
}

int copy_num(char* line, char* buf, int max_size){

    int i = 0;

    while (((line[i] >= '0') && (line[i] <= '9')) 
            || (line[i] == '-') 
                || (line[i] == '.')){ 

        buf[i] = line[i];
        i++;

        if (i >= max_size){

            return -1;
        }
    }
    buf[i] = '\0';

    return i;
}

Node* Differ::get_unary_op(){

    Node* fir_arg = nullptr;
    Node* sec_arg = nullptr;
    char cur_unary_op[MAX_UNARY_OP_SIZE];
    int cur_unary_op_size = copy_word(line + cur_pos, cur_unary_op, MAX_UNARY_OP_SIZE);

    if ((cur_unary_op_size > 0) 
        && (find_in_unary_op(cur_unary_op))){//надо поправить

        cur_pos += cur_unary_op_size;
        
        SYNTAX_ERROR(REQUIRE('('), PRINT_ERROR_POS; exit(0));
        cur_pos++;
        fir_arg = get_expr();
        SYNTAX_ERROR(REQUIRE(')'), PRINT_ERROR_POS; exit(0));
        cur_pos++;
        sec_arg = new Node(nullptr, nullptr, nullptr);

        fir_arg = new Node(sec_arg, cur_unary_op, fir_arg);
    
        return fir_arg;
    } else{
        
        error_has_occured = true;
        return nullptr;
    }
}

Node* Differ::get_var(){

    Node* fir_arg = nullptr;
    char cur_var[MAX_VAR_SIZE];
    int cur_var_size = copy_word(line + cur_pos, cur_var, MAX_VAR_SIZE);

    if ((cur_var_size > 0) 
        && (!find_in_consts(cur_var)) 
            && (!find_in_unary_op(cur_var)) 
                && (is_var(cur_var))){
        
        cur_pos += cur_var_size;
        fir_arg = new Node(nullptr, cur_var, nullptr);

        return fir_arg; 
    } else{

        error_has_occured = true;
        return nullptr;
    }
}

Node* Differ::get_num(){

    Node* fir_arg = nullptr;
    char cur_num[MAX_NUM_SIZE];
    int cur_num_size = copy_num(line + cur_pos, cur_num, MAX_NUM_SIZE);

    if ((cur_num_size > 0) 
        && (is_number(cur_num))){ 

        cur_pos += cur_num_size;
        fir_arg = new Node(nullptr, cur_num, nullptr);

        return fir_arg;
    } else{

        error_has_occured = true;
        return nullptr;
    }
}

Node* Differ::get_const(){

    Node* fir_arg = nullptr;
    char cur_const[MAX_CONST_SIZE];
    int cur_const_size = copy_word(line + cur_pos, cur_const, MAX_CONST_SIZE);

    if ((cur_const_size > 0) 
        && (!find_in_unary_op(cur_const))
            && (find_in_consts(cur_const))){ 
        
        cur_pos += cur_const_size;
        fir_arg = new Node(nullptr, cur_const, nullptr);

        return fir_arg;
    } else{

        error_has_occured = true;
        return nullptr;
    }
}

Differ::Differ(FILE* file){
    
    struct stat info;
    
    if (file == nullptr){ 

        printf("Can't open file\n");
        exit(0);
    }

    fstat(fileno(file), &info);    

    line = new char[info.st_size];
    
    if (line == nullptr){

        printf("Can't read data from file\n");
        exit(0);
    }

    fread(line, sizeof(char), info.st_size, file);

    root = get_root_expr();
}

Differ::~Differ(){

    delete line;
    delete root;
}

void Differ::dump_graphiz(FILE* graph_file){

    assert(graph_file != nullptr);
    assert(root->check_tree());

    root->dump_graphviz(graph_file);
}

void Differ::differentiate(){

    diff_node(root);
    //optimize();
}

void Differ::diff_unary_op(Node* cur_node){

}

void Differ::diff_node(Node* cur_node){

    if (cur_node == nullptr){ return; }

    Node* tmp_left = nullptr;
    Node* tmp_right = nullptr;
    Node* tmp_diff = nullptr;
    Node* tmp_root = nullptr;

    switch (cur_node->get_priority()){ //надо подумать что делать с разными переменными

        case unary_op:
            diff_unary_op(cur_node);
            break;

        case power:
            DIFF_POWER;
            break;

        case const_or_var:

            if (cur_node->is_variable()){ 

                DIFF_VAR;
            } else{

                DIFF_CONST;
            }
            break;

        case mul_or_div:

            if (*cur_node == Node(nullptr, "*", nullptr)){

                DIFF_MUL;
            } else{

                DIFF_DIV;
            }
            break;   

        case sub:
            DIFF_SUB;
            break;

        case sum:
            DIFF_SUM;
            break;
    }
}
