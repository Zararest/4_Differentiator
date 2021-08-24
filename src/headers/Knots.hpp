#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Knot{

private:

    Knot* next_L;
    Knot* next_R;
    Knot* prev;

    int hash_of_class_type;

public:

    Knot();

    Knot(int type_hash);

    Knot(const Knot& old_obj);

    Knot(const Knot&& tmp_obj);

    ~Knot();

    int return_type();

    void add_R_brunch();

    void add_L_brunch();

    Knot* get_R_brunch();

    Knot* get_L_brunch();

    Knot* get_prev();

    //virtual void differentiate();

    Knot& operator = (const Knot& old_obj) = delete;     

    Knot& operator = (Knot&& tmp_obj) = delete;  

    friend void define_plus(Knot& cur_knot);

    friend void define_minus(Knot& cur_knot);

    friend void define_num(Knot& cur_knot, char* lexem);

    friend void define_var(Knot& cur_knot, char* lexem);
};


class Undef_knot: public Knot{

public:

    void differentiate(){}   
};

class Empty_knot: public Knot{

public:

    void differentiate(){}
};

class Plus_knot: public Knot{

public:

    Plus_knot(){}

    ~Plus_knot(){}

    void differentiate(){}
};

class Minus_knot: public Knot{

public:

    void differentiate(){}
};

class Var_knot: public Knot{

public:

    Var_knot(char* lexem){}

    void differentiate(){}
};

class Num_knot: public Knot{

public:

    Num_knot(char* lexem){}

    void differentiate(){}
};
