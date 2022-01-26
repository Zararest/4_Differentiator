#include "Node.h"

#include <stdio.h>

#define MAX_FILE_SIZE 1024
#define MAX_PHRASE_SIZE 2048 

#define NUM_PHRASES_COS   13
#define NUM_PHRASES_SIN   7
#define NUM_PHRASES_DIV   6
#define NUM_PHRASES_LN    15
#define NUM_PHRASES_MUL   20
#define NUM_PHRASES_POWER 10

#define COS_PATH   "../bin/templates/cos.txt"
#define SIN_PATH   "../bin/templates/sin.txt"
#define DIV_PATH   "../bin/templates/div.txt"
#define LN_PATH    "../bin/templates/ln.txt"
#define MUL_PATH   "../bin/templates/mul.txt"
#define POWER_PATH "../bin/templates/power.txt"

/**
 * @brief Основной класс для работы с выражениями. Часть операция над деревом производятся классом Node.
 * 
 */
class Differ{

    NodeData* arr_of_vars = nullptr;
    NodeData cur_var = nullptr;
    int num_of_vars = 0;
    
    Node* initial_expr = nullptr;
    Node* cur_diff_root = nullptr;
    Node* root = nullptr;
    int cur_pos = 0;
    char* line = nullptr;
    bool error_has_occured = false;

    int num_of_expr_optimizations = 0;

    FILE* phrases_file = nullptr;
    int phrases_stack = 0;

    Node* get_root_expr();
    Node* get_expr();
    Node* get_div_mul();
    Node* get_pow();
    Node* get_args();
    Node* get_unary_op();
    Node* get_var();
    Node* get_num();
    Node* get_const();

    void add_var(const NodeData new_var);
    bool check_tree();
    void diff_node(Node* cur_node);
    void diff_unary_op(Node* cur_node);
    void optimize_expr(Node* cur_root);
    void find_expr(Node* cur_node);
    int calc_binary_op(Node* cur_root);
    int calc_unary_op(Node* cur_root);
    int sum_optimizations(Node* cur_node);
    int sub_optimizations(Node* cur_node);
    int mul_optimizations(Node* cur_node);
    int div_optimizations(Node* cur_node);
    int pow_optimizations(Node* cur_node);
    void generate_phrases(const char* template_file, int num_of_phrases);
    void print_cur_stage();

public:

    Differ(FILE* file);
    Differ(const Differ&) = delete;
    Differ(Differ&&) = delete;
    ~Differ();

    Differ& operator=(const Differ&) = delete;
    Differ& operator=(Differ&&) = delete;

    void dump_graphiz(FILE* graph_file);
    void dump_latex(FILE* latex_file);

    void differentiate();
    void optimize();
};