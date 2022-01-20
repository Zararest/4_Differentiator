#include "headers/Differ.h"

int main(int argc, char **argv){

    FILE* input_file = nullptr;
    if (argc == 2){

        input_file = fopen(argv[1], "r");
    } else{

        input_file = fopen("../bin/input.txt", "r");
    }
    
    Differ new_differ(input_file);
    FILE* dump_file = fopen("../bin/graph.dot", "w");
    FILE* latex_file = fopen("../bin/latex.tex", "w");

    new_differ.differentiate();

    new_differ.dump_graphiz(dump_file);
    new_differ.dump_latex(latex_file);
}