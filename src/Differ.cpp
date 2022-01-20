#include "headers/Differ.h"
#include "headers/DSL.h"

#include <stdlib.h>
#include <assert.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#define MAX_OP_SIZE 2
#define MAX_UNARY_OP_SIZE 8
#define MAX_VAR_SIZE 8
#define MAX_NUM_SIZE 16
#define MAX_CONST_SIZE 4
#define WAS_OPTIMIZED 1
#define BUF_SIZE 64

/**
 * @brief Передача данных из одного файла в другой.
 * 
 * @param read_fd 
 * @param write_fd 
 */
void transfer_data(int read_fd, int write_fd){

    char child_buf[BUF_SIZE];
    int read_ret = -1;
    
    while ((read_ret = read(read_fd, child_buf, BUF_SIZE)) != 0){
        
        if (read_ret == -1){

            printf("Problems with pipe\n");
            exit(0);
        }
    
        write(write_fd, child_buf, read_ret);
    }
}

/**
 * @brief G::= E '\\n'
 * 
 * @return Node* 
 */
Node* Differ::get_root_expr(){

    Node* new_root = get_expr();
    
    SYNTAX_ERROR(REQUIRE('\n'), PRINT_ERROR_POS; exit(0));

    return new_root;
}

/**
 * @brief E::= T {[+-] T}*
 * 
 * @return Node* 
 */
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

/**
 * @brief T::= P {[*] P}
 * 
 * @return Node* 
 */
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

/**
 * @brief P::= B {^ B}?
 * 
 * @return Node* 
 */
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

/**
 * @brief B::= {'(' E ')'} | {N} | {C} | {F} | {V}
 * 
 * @return Node* 
 */
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

/**
 * @brief Копирование последовательности маленьких латинских букв.
 * 
 * @param line 
 * @param buf 
 * @param max_size 
 * @return int 
 */
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

/**
 * @brief Копирование числа.
 * 
 * @param line 
 * @param buf 
 * @param max_size 
 * @return int 
 */
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

/**
 * @brief F::= [sin cos tg ln] '(' E ')'
 * 
 * @return Node* 
 */
Node* Differ::get_unary_op(){

    Node* fir_arg = nullptr;
    Node* sec_arg = nullptr;
    char cur_unary_op[MAX_UNARY_OP_SIZE];
    int cur_unary_op_size = copy_word(line + cur_pos, cur_unary_op, MAX_UNARY_OP_SIZE);

    if ((cur_unary_op_size > 0) 
        && (find_in_unary_op(cur_unary_op))){

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

/**
 * @brief Добавление переменной в массив переменных.
 * 
 * @param new_var 
 */
void Differ::add_var(const NodeData new_var){

    for (int i = 0; i < num_of_vars; i++){

        if (strcmp(new_var, arr_of_vars[i]) == 0){ return; }
    }

    NodeData* tmp_arr = new NodeData[num_of_vars + 1];
    memcpy(tmp_arr, arr_of_vars, num_of_vars  * sizeof(NodeData));
    delete[] arr_of_vars;
    arr_of_vars = tmp_arr;

    arr_of_vars[num_of_vars] = new char[MAX_VAR_SIZE];
    memcpy(arr_of_vars[num_of_vars], new_var, MAX_VAR_SIZE);
    num_of_vars++;
}

/**
 * @brief V::= {[a..z]}*
 * 
 * @return Node* 
 */
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

        add_var(cur_var);

        return fir_arg; 
    } else{

        error_has_occured = true;
        return nullptr;
    }
}

/**
 * @brief N::= {[- 0..9]}+
 * 
 * @return Node* 
 */
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

/**
 * @brief C::= {e} | {pi}
 * 
 * @return Node* 
 */
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

/**
 * @brief Construct a new Differ:: Differ object
 * 
 * @param file 
 */
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

/**
 * @brief Destroy the Differ:: Differ object
 * 
 */
Differ::~Differ(){

    delete line;
    delete root;
    delete arr_of_vars;
}

/**
 * @brief Вывод дерева в файл Graphviz.
 * 
 * @param graph_file 
 */
void Differ::dump_graphiz(FILE* graph_file){

    assert(graph_file != nullptr);
    assert((root == nullptr) || root->check_tree());

    root->dump_graphviz(graph_file);
}

/**
 * @brief Дифференцирование выражения по всем переменным.
 * 
 */
void Differ::differentiate(){

    if (num_of_vars == 0){

        if (root != nullptr){

            root->add_branches(nullptr, nullptr);
            root->change_data("0");
        }
        
        return;
    }

    initial_expr = Node::copy_tree(root);

    Node* cur_diff = Node::copy_tree(root);
    Node* tmp_root = nullptr;
    Node* new_root = nullptr;

    cur_var = arr_of_vars[0];
    diff_node(cur_diff);

    new_root = new Node(cur_diff, "|", new Node(nullptr, cur_var, nullptr)); 

    for (int i = 1; i < num_of_vars; i++){

        cur_diff = Node::copy_tree(root);
        cur_var = arr_of_vars[i];
        diff_node(cur_diff);

        tmp_root = new Node(cur_diff, "|", new Node(nullptr, cur_var, nullptr)); 
        new_root = new Node(new_root, "+", tmp_root);
    }

    root = new_root;
    
    optimize();
}

/**
 * @brief Дифференцирование унарных операций.
 * 
 * @param cur_node 
 */
void Differ::diff_unary_op(Node* cur_node){

    DECLARE_DIFF_VARS
    assert(cur_node != nullptr);
    
    if (cur_node->cmp_data("sin")){ DIFF_COMPLEX(DIFF_SIN); return; }
    if (cur_node->cmp_data("cos")){ DIFF_COMPLEX(DIFF_COS); return; }
    if (cur_node->cmp_data("tg")){ DIFF_COMPLEX(DIFF_TAN); return; }
    if (cur_node->cmp_data("ln")){ DIFF_COMPLEX(DIFF_LN); return; }
}

/**
 * @brief Дифференцирование узла в зависимости от его приоритета.
 * 
 * @param cur_node 
 */
void Differ::diff_node(Node* cur_node){

    if (cur_node == nullptr){ return; }

    DECLARE_DIFF_VARS

    switch (cur_node->get_priority()){

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
                
            if (cur_node->cmp_data("*")){

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

/**
 * @brief Поиск выражения для частной производной по каждой из переменных. 
 * 
 * @param cur_node 
 */
void Differ::find_expr(Node* cur_node){

    if (cur_node == nullptr){ return; }

    if (cur_node->cmp_data("|")){ 

        optimize_expr(cur_node->get_left());
        return;
    }

    find_expr(cur_node->get_left());
    find_expr(cur_node->get_right());
}

/**
 * @brief Оптимизация частных производных в дифференциале.
 * 
 */
void Differ::optimize(){

    do{
        num_of_expr_optimizations = 0;

        find_expr(root);

    } while (num_of_expr_optimizations > 0);
}

/**
 * @brief Вычисление бинарных операций.
 * 
 * @param cur_root 
 * @return int 
 */
int Differ::calc_binary_op(Node* cur_root){

    if ((cur_root == nullptr) || (cur_root->is_leaf())){ return 0; }

    Node* cur_left = cur_root->get_left();
    Node* cur_right = cur_root->get_right();

    assert(cur_left != nullptr);
    assert(cur_right != nullptr);    

    int opt_of_sub_tree = calc_binary_op(cur_left) + calc_binary_op(cur_right);

    if (cur_root->get_priority() == sum){

        return sum_optimizations(cur_root) + opt_of_sub_tree;
    }

    if (cur_root->get_priority() == sub){

        return sub_optimizations(cur_root) + opt_of_sub_tree;
    }

    if (cur_root->cmp_data("*")){

        return mul_optimizations(cur_root) + opt_of_sub_tree;
    }

    if (cur_root->cmp_data("/")){

        return div_optimizations(cur_root) + opt_of_sub_tree;
    }

    if (cur_root->get_priority() == power){

        return pow_optimizations(cur_root) + opt_of_sub_tree;
    }

    return opt_of_sub_tree;
}

/**
 * @brief Вычисление унарных операций.
 * 
 * @param cur_root 
 * @return int 
 */
int Differ::calc_unary_op(Node* cur_root){

    if ((cur_root == nullptr) || (cur_root->is_leaf())){ return 0; }

    char new_num[MAX_NUM_SIZE] = {"!"};
    Node* cur_left = cur_root->get_left();
    Node* cur_right = cur_root->get_right();

    assert(cur_left != nullptr);
    assert(cur_right != nullptr);
    
    if ((cur_root->get_priority() == power)
        && (cur_left->get_type() == Const_or_num)
            && (cur_right->get_type() == Const_or_num)){
        
        sprintf(new_num, "%.3lf", pow(cur_left->get_num(), cur_right->get_num()));
    }

    if ((cur_root->get_type() == Op)
        && (cur_right->get_type() == Const_or_num)
            && (cur_left->get_type() == Nothing)){ 
        
        if (cur_root->cmp_data("sin")){

            sprintf(new_num, "%.3lf", sin(cur_right->get_num()));
        }

        if (cur_root->cmp_data("cos")){

            sprintf(new_num, "%.3lf", cos(cur_right->get_num()));
        }

        if (cur_root->cmp_data("tg")){

            sprintf(new_num, "%.3lf", tan(cur_right->get_num()));
        }

        if (cur_root->cmp_data("ln")){

            sprintf(new_num, "%.3lf", log(cur_right->get_num()));
        }
    } else{

        return calc_binary_op(cur_left) + calc_binary_op(cur_right);
    }

    cur_root->change_data(new_num);
    cur_root->add_branches(nullptr, nullptr);

    return WAS_OPTIMIZED;
}

/**
 * @brief Оптимизация узлов суммы.
 * 
 * @param cur_node 
 * @return int 
 */
int Differ::sum_optimizations(Node* cur_node){

    char new_num[MAX_NUM_SIZE] = {"!"};
    Node* cur_left = cur_node->get_left();
    Node* cur_right = cur_node->get_right();
    Node* tmp_right = nullptr;
    Node* tmp_left = nullptr;

    if ((cur_right->get_type() == Const_or_num)
        && (cur_left->get_type() == Const_or_num)){
        
        sprintf(new_num, "%.3lf", cur_left->get_num() + cur_right->get_num());

        cur_node->change_data(new_num);
        cur_node->add_branches(nullptr, nullptr);

        return WAS_OPTIMIZED;
    }

    if (cur_node->get_left()->get_num() == 0){ 
            
        REMOVE_LEFT_OPERAND;
        return WAS_OPTIMIZED;
    }

    if (cur_node->get_right()->get_num() == 0){

        cur_node->swap_branches();
        REMOVE_LEFT_OPERAND;
        return WAS_OPTIMIZED;
    }   

    return 0;
}

/**
 * @brief Оптимизация узлов вычитания.
 * 
 * @param cur_node 
 * @return int 
 */
int Differ::sub_optimizations(Node* cur_node){

    char new_num[MAX_NUM_SIZE] = {"!"};
    Node* cur_left = cur_node->get_left();
    Node* cur_right = cur_node->get_right();
    Node* tmp_right = nullptr;
    Node* tmp_left = nullptr;

    if ((cur_right->get_type() == Const_or_num)
        && (cur_left->get_type() == Const_or_num)){
        
        sprintf(new_num, "%.3lf", cur_left->get_num() - cur_right->get_num());

        cur_node->change_data(new_num);
        cur_node->add_branches(nullptr, nullptr);

        return WAS_OPTIMIZED;
    }

    if (cur_node->get_right()->get_num() == 0){ 

        cur_node->swap_branches();    
        REMOVE_LEFT_OPERAND;
        return WAS_OPTIMIZED;
    }

    return 0;
}

/**
 * @brief Оптимизация узлов умножения.
 * 
 * @param cur_node 
 * @return int 
 */
int Differ::mul_optimizations(Node* cur_node){
    
    char new_num[MAX_NUM_SIZE] = {"!"};
    Node* cur_left = cur_node->get_left();
    Node* cur_right = cur_node->get_right();
    Node* tmp_right = nullptr;
    Node* tmp_left = nullptr;

    if ((cur_right->get_type() == Const_or_num)
        && (cur_left->get_type() == Const_or_num)){
        
        sprintf(new_num, "%.3lf", cur_left->get_num() * cur_right->get_num());

        cur_node->change_data(new_num);
        cur_node->add_branches(nullptr, nullptr);

        return WAS_OPTIMIZED;
    }

    if ((cur_node->get_left()->get_num() == 0)
        || (cur_node->get_right()->get_num() == 0)){
        
        cur_node->change_data("0");
        cur_node->add_branches(nullptr, nullptr);
        return WAS_OPTIMIZED;
    }

    if (cur_node->get_left()->get_num() == 1){

        REMOVE_LEFT_OPERAND;
        return WAS_OPTIMIZED;
    }

    if (cur_node->get_right()->get_num() == 1){

        cur_node->swap_branches();
        REMOVE_LEFT_OPERAND;
        return WAS_OPTIMIZED;
    }

    return 0;
}

/**
 * @brief Оптимизация узлов деления.
 * 
 * @param cur_node 
 * @return int 
 */
int Differ::div_optimizations(Node* cur_node){

    char new_num[MAX_NUM_SIZE] = {"!"};
    Node* cur_left = cur_node->get_left();
    Node* cur_right = cur_node->get_right();
    Node* tmp_right = nullptr;
    Node* tmp_left = nullptr;

    if ((cur_right->get_type() == Const_or_num)
        && (cur_left->get_type() == Const_or_num)){
        
        if (cur_right->get_num() != 0){ 

            sprintf(new_num, "%.3lf", cur_left->get_num() / cur_right->get_num());
        } else{

            sprintf(new_num, "nan");
        }

        cur_node->change_data(new_num);
        cur_node->add_branches(nullptr, nullptr);

        return WAS_OPTIMIZED;
    }

    if ((cur_node->get_left()->get_num() == 0)){
        
        cur_node->change_data("0");
        cur_node->add_branches(nullptr, nullptr);
        return WAS_OPTIMIZED;
    }

    if (cur_node->get_right()->get_num() == 1){

        cur_node->swap_branches();
        REMOVE_LEFT_OPERAND;
        return WAS_OPTIMIZED;
    }

    return 0;
}

/**
 * @brief Оптимизация узлов степени.
 * 
 * @param cur_node 
 * @return int 
 */
int Differ::pow_optimizations(Node* cur_node){

    char new_num[MAX_NUM_SIZE] = {"!"};
    Node* cur_left = cur_node->get_left();
    Node* cur_right = cur_node->get_right();
    Node* tmp_right = nullptr;
    Node* tmp_left = nullptr;

    if ((cur_right->get_type() == Const_or_num)
        && (cur_left->get_type() == Const_or_num)){
        
        sprintf(new_num, "%.3lf", pow(cur_left->get_num(), cur_right->get_num()));

        cur_node->change_data(new_num);
        cur_node->add_branches(nullptr, nullptr);

        return WAS_OPTIMIZED;
    }

    if ((cur_node->get_left()->get_num() == 0)){
        
        cur_node->change_data("0");
        cur_node->add_branches(nullptr, nullptr);
        return WAS_OPTIMIZED;
    }

    if ((cur_node->get_left()->get_num() == 1)){
        
        cur_node->change_data("1");
        cur_node->add_branches(nullptr, nullptr);
        return WAS_OPTIMIZED;
    }

    if (cur_node->get_right()->get_num() == 1){

        cur_node->swap_branches();
        REMOVE_LEFT_OPERAND;
        return WAS_OPTIMIZED;
    }

    if (cur_node->get_right()->get_num() == 0){

        cur_node->change_data("1");
        cur_node->add_branches(nullptr, nullptr);
        return WAS_OPTIMIZED;
    }

    return 0;
}

/**
 * @brief Оптимизация выражения.
 * 
 * @param cur_root 
 */
void Differ::optimize_expr(Node* cur_root){

    int num_of_cur_opt = 0;
    
    do{
        num_of_cur_opt = 0;    

        num_of_cur_opt += calc_binary_op(cur_root);
        num_of_cur_opt += calc_unary_op(cur_root);

    } while (num_of_cur_opt > 0);
}

/**
 * @brief Вывод выражения в латех.
 * 
 * @param latex_file 
 */
void Differ::dump_latex(FILE* latex_file){
    
    assert(latex_file != nullptr);
    assert((root == nullptr) || root->check_tree());

    int TexPattern = open("../bin/TexTemplate.txt", O_RDONLY);
    int i = 0;
    transfer_data(TexPattern, fileno(latex_file));

    Node::print_node_latex(initial_expr, latex_file);
    fprintf(latex_file, "\n\\end{displaymath}\n");

    fprintf(latex_file, "Её дифференциал:\n");
    fprintf(latex_file, "\\begin{displaymath}\n");
    fprintf(latex_file, "df(");
    for (i = 0; i < num_of_vars - 1; i++){

        fprintf(latex_file, "%s,", arr_of_vars[i]);
    }
    fprintf(latex_file, "%s) = ", arr_of_vars[i]);

    Node::print_node_latex(root, latex_file);
    fprintf(latex_file, "\n\\end{displaymath}\n");
    fprintf(latex_file, "\\end{document}\n");
}
