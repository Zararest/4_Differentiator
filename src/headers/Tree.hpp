#include "Knots.hpp"
 
class  Tree{

private:

    Knot root;

public:

    Tree(FILE* input_file);

    Tree(const Knot& new_root);

    Tree(const Tree& old_obj) = delete;

    Tree(Tree&& tmp_obj) = delete;

    ~Tree();

    void print();

    Tree& operator = (const Tree& old_obj) = delete;     

    Tree& operator = (Tree&& tmp_obj) = delete;  
};  