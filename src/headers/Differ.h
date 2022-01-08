#include "Node.h"

#include <stdio.h>

#define MAX_FILE_SIZE 1024

/**
 * @brief Основной класс для работы с выражениями. Часть операция над деревом производятся классом Node.
 * 
 */
class Differ{

    Node* root = nullptr;
    int cur_pos = 0;
    char* line = nullptr;
    bool error_has_occured = false;

    Node* get_root_expr();
    Node* get_expr();
    Node* get_div_mul();
    Node* get_pow();
    Node* get_args();
    Node* get_unary_op();
    Node* get_var();
    Node* get_num();
    Node* get_const();

    bool check_tree();
public:

    Differ(FILE* file);
    Differ(const Differ&) = delete;
    Differ(Differ&&) = delete;
    ~Differ();

    Differ& operator=(const Differ&) = delete;
    Differ& operator=(Differ&&) = delete;

    void dump_graphiz(FILE* graph_file);
    void dump_latex(FILE* latex_file);

    Node* differentiate();
    Node* optimize();
};