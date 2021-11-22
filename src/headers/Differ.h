#include "Node.h"

#include <stdio.h>

class Differ{

    Node* root = nullptr;

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