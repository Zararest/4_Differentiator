#include "headers/Tree.hpp"
#include "DSL.hpp"

bool check_brackets(FILE* input_file){

    char symbol = fgetc(input_file);
    int stack = 0;

    while ((symbol != '\n') && (symbol != EOF)){

        if (symbol == '('){

            stack++;
        }

        if (symbol == ')'){

            stack--;
        }

        if (stack < 0){

            return false;
        }

        symbol = fgetc(input_file);
    }

    return true;
}

char get_bracket(FILE* input_file){

    char symbol = fgetc(input_file);

    while (symbol == ' '){

        symbol = fgetc(input_file);
    }

    if ((symbol == '(') || (symbol == ')')){

        return symbol;
    } else{

        return '!';
    }
}
/*
int my_hash(char* cur_str, int str_size){

    const int k = 39, mod = 1e5+3;
    int h = 0, m = 1;

    if ((cur_str[0] == '-') && (str_size != 1)){

        return -1;
    }

    if ((cur_str[0] >= '0') && (cur_str[0] <= '9')){

        return - 1;
    }

    for (int i = 0; i < str_size; i++){

        int x = (int) (cur_str[i] - 'a' + 1);
        h = (h + m * x) % mod;
        m = (m * k) % mod;
    }

    return abs(h);
}
*/
int get_lexem_hash(FILE* input_file, char*& lexem_ptr){

    char symbol = fgetc(input_file);
    int lexem_len = 1;

    lexem_ptr = NULL;

    while ((symbol == ' ') && (symbol != EOF) && (symbol != '\n')){

        symbol = fgetc(input_file);
    } 
    fseek(input_file, SEEK_CUR, -1);

    lexem_ptr = (char*) realloc(lexem_ptr, lexem_len * sizeof(char));
    lexem_ptr[0] = symbol;

    while ((symbol != ' ') && (symbol != ')') && (symbol != EOF) && (symbol != '\n')){

        symbol = fgetc(input_file);
        lexem_len++;

        lexem_ptr = (char*) realloc(lexem_ptr, lexem_len * sizeof(char));
        lexem_ptr[lexem_len - 1] = symbol;
    }     

    lexem_ptr = (char*) realloc(lexem_ptr, (lexem_len + 1) * sizeof(char));
    lexem_ptr[lexem_len] = '\0';

    if ((symbol == EOF) || (symbol == '\n')){

        printf("WARNING: get_lexem_hash(): incorrect bracket sequence\n\n");
        return -1;
    }

    if (symbol == ')'){

        fseek(input_file, SEEK_CUR, -1);
    }

    //return my_hash(lexem_ptr, lexem_len);
}

void define_plus(Knot& cur_knot){

    Plus_knot plus_knot;
    Knot& new_knot = plus_knot;

    INSERT_KNOT
}

void define_minus(Knot& cur_knot){

    Minus_knot minus_knot;
    Knot& new_knot = minus_knot;

    INSERT_KNOT
}

void define_num(Knot& cur_knot, char* lexem){

    Num_knot num_knot(lexem);
    Knot& new_knot = num_knot;

    INSERT_KNOT
}

void define_var(Knot& cur_knot, char* lexem){

    Var_knot var_knot(lexem);
    Knot& new_knot = var_knot;

    INSERT_KNOT
}

void replace_knot(Knot& cur_knot, int hash, char* lexem){

    bool knot_defined = false;

    switch (hash){

    case HASH_OF_PLUS:
        
        define_plus(cur_knot);
        knot_defined = true;
        break;
    
    case HASH_OF_MINUS:

        define_minus(cur_knot);
        knot_defined = true;
        break;

    case HASH_OF_NUM:

        define_num(cur_knot, lexem);
        knot_defined = true;
        break;
    }

    if (knot_defined == false){

        define_var(cur_knot, lexem);
    }
}

void create_tree(Knot& cur_knot, FILE* input_file){

    char* lexem_ptr = nullptr;

    if (get_bracket(input_file) == '('){

        cur_knot.add_L_brunch();
        create_tree(*(cur_knot.get_L_brunch()), input_file);

        if (get_bracket(input_file) != ')'){

            printf("WARNING: create_tree(): incorrect bracket sequence(left knot)\n");
        }

        replace_knot(cur_knot, get_lexem_hash(input_file, lexem_ptr), lexem_ptr);
        free(lexem_ptr);

        if (get_bracket(input_file) != '('){

            printf("WARNING: create_tree(): incorrect bracket sequence(right knot(1))\n");
        }

        cur_knot.add_R_brunch();
        create_tree(*(cur_knot.get_R_brunch()), input_file);

        if (get_bracket(input_file) != ')'){

            printf("WARNING: create_tree(): incorrect bracket sequence(right knot(2))\n");
        }
    } else{

        replace_knot(cur_knot, get_lexem_hash(input_file, lexem_ptr), lexem_ptr);
        free(lexem_ptr);
    }
}   

Tree::Tree(FILE* input_file):

    root()
{

    if (check_brackets(input_file) == true){

        create_tree(root, input_file);
    } else{

        printf("WARNING: Tree(): incorrect bracket sequence\n\n");
    }

}