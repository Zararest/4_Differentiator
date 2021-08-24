#define CAT(x, y) x##_##y
#define TEMPLATE(x, y) CAT(x, y)

#define HASH_OF_PLUS  53
#define HASH_OF_EMPTY 55
#define HASH_OF_MINUS 51
#define HASH_OF_MULT  54
#define HASH_OF_DIV   49
#define HASH_OF_NUM   -1
#define HASH_OF_SIN   21664

#define INSERT_KNOT if (cur_knot.prev->next_L == &cur_knot){\
                                                            \
                        cur_knot.prev->next_L = &new_knot;  \
                    } else{                                 \
                                                            \
                        cur_knot.prev->next_R = &new_knot;  \
                    }                                       \
                                                            \
                    new_knot.next_L = cur_knot.next_L;      \
                    new_knot.next_R = cur_knot.next_R;      \
                                                            \
                    cur_knot.~Knot();                       