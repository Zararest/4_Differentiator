#include "headers/Differ.h"

int main(){

    FILE* input_file = fopen("../bin/input.txt", "r");
    Differ new_differ(input_file);

    FILE* dump_file = fopen("../bin/graph.dot", "w");
    new_differ.differentiate();
    new_differ.dump_graphiz(dump_file);
}