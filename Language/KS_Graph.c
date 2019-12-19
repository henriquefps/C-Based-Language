#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 10000
#define true 1
#define false 0
#define incomplete_arguments 1
#define unknown_argument 2

#define S -1
#define K -18
#define I -2
#define B -3
#define C -4
#define D -5
#define E -6
#define F -7
#define AT -8
#define LT -9
#define GT -10
#define SUM -11
#define MINUS -12
#define DIV -13
#define MUL -14
#define EQ -15
#define TR -16
#define FA -17
#define Y -19


struct node{
    int op;
    struct node *pe;
    struct node *pd;
};

int euristica = 100;
int fy_calls = 0;
struct node nodes1[N/2];
struct node nodes2[N/2];
int current_heap = 1;
int current = 0;
int size_s = 0;
int arg_value = 0;

int get_op_value(char c);
char get_op_char(int c);
void print_tree(struct node* no);

struct node* new_node(int c, struct node* pe, struct node* pd);
void order_process(struct node* root);
void order(struct node* root);

int findArgument(int i, char* a, int len_a);
int count_arguments(char* s, int start_pos);
struct node* node_or_branch(char* s, int current_arg, int last_arg_end_pos);
struct node* create_graph(char* s, int start_pos);

void reduce_k(struct node* root, int is_root);
void reduce_i(struct node* root, int is_root);
void reduce_s(struct node* root, int is_root);
void reduce_b(struct node* root, int is_root);
void reduce_c(struct node* root, int is_root);
void reduce_d(struct node* root, int is_root);
void reduce_e(struct node* root, int is_root);
void reduce_f(struct node* root, int is_root);
void reduce_y(struct node* root, int is_root);

void reduce_equals(struct node* root, int is_root, int rec_level);
void reduce_true(struct node* root, int is_root);
void reduce_false(struct node* root, int is_root);
struct node* reduce_sum(struct node* root, int is_root, int rec_level);
struct node* reduce_minus(struct node* root, int is_root, int rec_level);
struct node* reduce_mul(struct node* root, int is_root, int rec_level);
struct node* reduce_div(struct node* root, int is_root, int rec_level);
void reduce_lt(struct node* root, int is_root, int rec_level);
void reduce_gt(struct node* root, int is_root, int rec_level);

struct node* reduce(struct node* root, int rec_level);

void end_program(struct node* root, int error, int op);

struct node* rec_p(struct node* root, int rec_level);

void flip_heap(){
    current_heap = (current_heap % 2) + 1;
    current = 0;
}

struct node* fy_recursion(struct node* curr){
    struct node* new = new_node(curr->op, NULL, NULL);

    struct node* pe = curr->pe;
    struct node* pd = curr->pd;

    curr->pe = new;
    curr->pd = 0;

    if(pe){
        if(pe->pe && !pe->pd) //Is FP
            new->pe = pe->pe;
        else new->pe = fy_recursion(pe);
    }

    if(pd){
        if(pd->pe && !pd->pd) new->pd = pd->pe;
        else new->pd = fy_recursion(pd);
    }

    return new;
}

struct node* fy(struct node* root){
    fy_calls++;
    flip_heap();
    return fy_recursion(root);
}

struct node* rec_p(struct node* root, int rec_level){
    while(root != NULL && root->op == AT){
        if(current > (N/2)-euristica) {
            if(rec_level > 0)
                return root;
            root = fy(root);
            if(current > (N/2)-euristica){
                printf("\nNOT ENOUGH MEMORY");
                exit(0);
            }
        }
        root = reduce(root, rec_level);
    }
    return root;
}


int main(void) {
//    char input_s[] = "S(K(SII))(S(S(KS)K)(K(SII)))(S(K(S(S(S(S(K=)I)(K0))(K1))))(S(K(S(S(K+)I)))(S(S(KS)(S(KK)I))(K(S(S(K-)I)(K1))))))#";
//    char input_s[] = "Y(ES(C(F=I0)1)(E(D+)I(FBI(F-I1))))#";
//    char input_s[] = "S(K(SII))(S(S(KS)K)(K(SII)))(S(K(S(S(S(KI)(S(S(K<)I)(K2)))I)))(S(S(KS)(S(K(S(K+)))(S(S(KS)(S(KK)I))(K(S(S(K-)I)(K2))))))(S(S(KS)(S(KK)I))(K(S(S(K-)I)(K1))))))#";
    char input_s[] = "Y(ES(S(F<I2)I)(D(D+)(FBI(F-I1))(FBI(F-I2))))#";
    arg_value = 3;
    size_s = strlen(input_s);
    struct node* root = create_graph(input_s, 0);
    root = rec_p(root, 0);
    printf("Time: %lf s\n", (double)(clock()/((float)(CLOCKS_PER_SEC))));
    printf("Result: ");
    print_tree(root);
    printf("\nFY-Calls: %d\n", fy_calls);
    return 0;
}

struct node* new_node(int c, struct node* pe, struct node* pd){
    struct node* n;
    if(current_heap == 1) n = &nodes1[current];
    else n = &nodes2[current];
    n->op = c;
    n->pe = pe;
    n->pd = pd;
    current++;
    if(current >= N/2){
        printf("OUT OF MEMORY");
        exit(0);
    }
    return n;
}

void order_process(struct node* root){
    printf("%c", get_op_char((*root).op));
    if((*root).pe != NULL) order_process(root->pe);
    if((*root).pd != NULL) order_process(root->pd);
}

void order(struct node* root){
    order_process(root);
    printf("\n\n");
}

// Returns the end position of an argument in the string
int findArgument(int i, char* a, int len_a) {
    int aux = i;
    if (aux < len_a && a[aux] == '(') {
        aux++;
        int count = 1;
        while (count > 0) {
            if (a[aux] == '(')
                count++;
            else if (a[aux] == ')')
                count--;
            aux++;
        }
        return aux;
    } else if (aux < len_a && a[aux] == ')')
        return -1;
    else if (aux < len_a)
        return aux + 1;
    else
        return -1;
}

/*
    * Counts how many arguments ar in the highest level of a string
*/
int count_arguments(char* s, int start_pos){
    int aux = start_pos, len_args = 0;
    while(aux < size_s && aux != -1){
        aux = findArgument(aux, s, size_s);
        if (aux != -1)
            len_args++;
    }
    return len_args;
}

/*
    * Create new branch positioning each argument in place.
    * In case of arg len is more than one, like in (KKK), calls itself for a new branch
*/
struct node* create_graph(char* s, int start_pos){
    int len_args = count_arguments(s, start_pos);
    int i = 0, last_arg_end_pos = start_pos, current_arg = start_pos;

    struct node* root = new_node(AT, NULL, NULL); // Inits the first node for the new branch

    // Finds the end position of the next argument(1st) and position it in the left pontier of the newly created node
    current_arg = findArgument(current_arg, s, size_s);
    root->pe = node_or_branch(s, current_arg, last_arg_end_pos);
    last_arg_end_pos = current_arg;

    // Finds the end position of the next argument(2nd) and position it in the right pontier of the newly created node
    current_arg = findArgument(current_arg, s, size_s);
    root->pd = node_or_branch(s, current_arg, last_arg_end_pos);
    last_arg_end_pos = current_arg;

    /*
        * Iterate through the n possible remaining arguments, creating new nodes where its left
        * pointer reference the last node, which was the root, positions the next argument
        * in the right pointer of this new node, and updates the root referece to be this new node.
    */
    for(i = 0; i < len_args - 2; i++) { // len - 2 because we have already positioned 2 arguments before
        struct node *aux = new_node(AT, root, NULL);
        current_arg = findArgument(current_arg, s, size_s);
        aux->pd = node_or_branch(s, current_arg, last_arg_end_pos);
        last_arg_end_pos = current_arg;
        root = aux;
    }
    return root;
}

/*
    * Auxiliary function to create_graph, either creates a new_node or does
    * another call to create_graph to be returned and attached to the graph.
*/
struct node* node_or_branch(char* s, int current_arg, int last_arg_end_pos){
    struct node* r = NULL;
    if(current_arg - last_arg_end_pos == 1){ // Base Case: if len(arg) = 1, new node with arg and return it
        if(s[current_arg-1] == '#'){
            r = new_node(arg_value, NULL, NULL);
        } else
            r = new_node(get_op_value(s[current_arg-1]), NULL, NULL);
    } else if (current_arg - last_arg_end_pos > 1){ // Recursive Case: if len(arg) > 1, new graph branch and return its root
        r = create_graph(s, last_arg_end_pos+1);
    }
    return r;
}

/*
    * Does the K reduction operation Kab = a
*/
void reduce_k(struct node* root, int is_root){
    if(!is_root)
        root->pe = root->pe->pe->pd;
    else{
        root->pe = root->pe->pd;
    }
}

/*
    * Does the I reduction operation Ia = a
*/
void reduce_i(struct node* root, int is_root){
    if(!is_root)
        root->pe = root->pe->pd;
    else{
        root->pe = root->pd;
    }
}

/*
    * Does the S reduction operation Sabc = ac(bc)
*/
void reduce_s(struct node* root, int is_root){
    struct node* a = NULL;
    struct node* b = NULL;
    struct node* c = NULL;

    if(!is_root){
        a = root->pe->pe->pe->pd;
        b = root->pe->pe->pd;
        c = root->pe->pd;
    } else {
        a = root->pe->pe->pd;
        b = root->pe->pd;
        c = root->pd;
    }
    struct node *at1 = new_node(AT, a, c);
    struct node *at2 = new_node(AT, b, c);
    struct node *at3 = new_node(AT, at1, at2);

    root->pe = at3;
}

/*
    * Does the B reduction operation Babc = a(bc)
*/
void reduce_b(struct node* root, int is_root){
    struct node* a = NULL;
    struct node* b = NULL;
    struct node* c = NULL;

    if(!is_root){
        a = root->pe->pe->pe->pd;
        b = root->pe->pe->pd;
        c = root->pe->pd;
    } else {
        a = root->pe->pe->pd;
        b = root->pe->pd;
        c = root->pd;
    }

    struct node* at1 = new_node(AT, b, c);
    struct node* branch = new_node(AT, a, at1);
    root->pe = branch;
}

/*
    * Does the C reduction operation Cabc = (ac)b
*/
void reduce_c(struct node* root, int is_root){
    struct node* a = NULL;
    struct node* b = NULL;
    struct node* c = NULL;

    if(!is_root){
        a = root->pe->pe->pe->pd;
        b = root->pe->pe->pd;
        c = root->pe->pd;
    } else {
        a = root->pe->pe->pd;
        b = root->pe->pd;
        c = root->pd;
    }

    struct node* at1 = new_node(AT, a, c);

    struct node* branch = new_node(AT, at1, b);
    root->pe = branch;
}


/*
    * Does the D reduction operation Dabcd = a(bd)(cd)
*/
void reduce_d(struct node* root, int is_root){
    struct node* a = NULL;
    struct node* b = NULL;
    struct node* c = NULL;
    struct node* d = NULL;

    if(!is_root){
        a = root->pe->pe->pe->pe->pd;
        b = root->pe->pe->pe->pd;
        c = root->pe->pe->pd;
        d = root->pe->pd;
    } else {
        a = root->pe->pe->pe->pd;
        b = root->pe->pe->pd;
        c = root->pe->pd;
        d = root->pd;
    }

    struct node* at1 = new_node(AT, b, d);
    struct node* at2 = new_node(AT, a, at1);
    struct node* at3 = new_node(AT, c, d);
    struct node* branch = new_node(AT, at2, at3);
    root->pe = branch;
}

/*
    * Does the E reduction operation Dabcd = (ab)(cd)
*/
void reduce_e(struct node* root, int is_root){
    struct node* a = NULL;
    struct node* b = NULL;
    struct node* c = NULL;
    struct node* d = NULL;

    if(!is_root){
        a = root->pe->pe->pe->pe->pd;
        b = root->pe->pe->pe->pd;
        c = root->pe->pe->pd;
        d = root->pe->pd;
    } else {
        a = root->pe->pe->pe->pd;
        b = root->pe->pe->pd;
        c = root->pe->pd;
        d = root->pd;
    }

    struct node* at1 = new_node(AT, a, b);
    struct node* at2 = new_node(AT, c, d);
    struct node* branch = new_node(AT, at1, at2);
    root->pe = branch;
}

/*
    * Does the F reduction operation Fabcd = a(bd)c
*/
void reduce_f(struct node* root, int is_root){
    struct node* a = NULL;
    struct node* b = NULL;
    struct node* c = NULL;
    struct node* d = NULL;

    if(!is_root){
        a = root->pe->pe->pe->pe->pd;
        b = root->pe->pe->pe->pd;
        c = root->pe->pe->pd;
        d = root->pe->pd;
    } else {
        a = root->pe->pe->pe->pd;
        b = root->pe->pe->pd;
        c = root->pe->pd;
        d = root->pd;
    }

    struct node* at1 = new_node(AT, b, d);
    struct node* at2 = new_node(AT, a, at1);
    struct node* branch = new_node(AT, at2, c);
    root->pe = branch;
}

void reduce_y(struct node* root, int is_root){
    struct node* aux = root;
    struct node* arroba1 = new_node(AT, NULL, NULL);
    struct node* arroba2 = new_node(AT, NULL, NULL);

    struct node* argumentoA = aux->pe->pd;

    arroba1->pd = arroba2;
    arroba1->pe = argumentoA;

    arroba2->pd = argumentoA;
    arroba2->pe = new_node(Y, NULL, NULL);
    aux->pe = arroba1;
//    struct node* at1 = new_node(AT, root->pe, NULL);
//    at1->pd = at1;
}

void reduce_equals(struct node* root, int is_root, int rec_level){
    struct node* aux = root;
    struct node* a;
    if(!is_root)
        a = aux->pe->pe->pd;
    else
        a = aux->pe->pd;
    struct node* b;
    if(!is_root)
        b = aux->pe->pd;
    else
        b = aux->pd;

    if(b->op == AT)
        b = rec_p(b, rec_level + 1);

    if(a->op == AT)
        a = rec_p(a, rec_level + 1);

    int ai = a->op;
    int bi = b->op;
    struct node* resultado;
    if(ai == bi)
        resultado = new_node(TR, NULL, NULL);
    else
        resultado = new_node(FA, NULL, NULL);

    aux->pe = resultado;
}

void reduce_true(struct node* root, int is_root){
    struct node* aux = root;
    struct node* argumentoA;
    if(!is_root)
        argumentoA = aux->pe->pe->pd;
    else  argumentoA = aux->pe->pd;
    aux->pe = argumentoA;
}

void reduce_false(struct node* root, int is_root){
    struct node* aux = root;
    struct node* argumentoA;
    if(!is_root)
        argumentoA = aux->pe->pd;
    else  argumentoA = aux->pd;
    aux->pe = argumentoA;
}

struct node* reduce_sum(struct node* root, int is_root, int rec_level){
    struct node* aux = root;
    struct node* a;
    struct node* b;
    if(!is_root) {
        a = aux->pe->pe->pd;
        b = aux->pe->pd;
    }else {
        a = aux->pe->pd;
        b = aux->pd;
    }

    if(b->op == AT)
        b = rec_p(b, rec_level + 1);

    if(a->op == AT)
        a = rec_p(a, rec_level + 1);

    if(a->op == AT || b->op == AT) {
        if(!is_root) {
            aux->pe->pe->pd = a;
            aux->pe->pd = b;
        }else{
            aux->pe->pd = a;
            aux->pd = b;
        }
        return aux;
    }

    int ai = a->op;
    int bi = b->op;
//    printf("%d + %d = %d\n", ai, bi , ai+bi);
    struct node* resultado = new_node(ai+bi, NULL, NULL);

//    aux->pe = resultado;
    return resultado;
}

struct node* reduce_minus(struct node* root, int is_root, int rec_level){
    struct node* aux = root;
    struct node* a;
    if(!is_root)
        a = aux->pe->pe->pd;
    else
        a = aux->pe->pd;
    struct node* b;
    if(!is_root)
        b = aux->pe->pd;
    else
        b = aux->pd;

    if(b->op == AT)
        b = rec_p(b, rec_level + 1);

    if(a->op == AT)
        a = rec_p(a, rec_level + 1);

    if(a->op == AT || b->op == AT) {
        if(!is_root) {
            aux->pe->pe->pd = a;
            aux->pe->pd = b;
        }else{
            aux->pe->pd = a;
            aux->pd = b;
        }
        return aux;
    }

    int ai = a->op;
    int bi = b->op;
//    printf("%d - %d = %d\n", ai, bi , ai-bi);
    struct node* resultado = new_node(ai-bi, NULL, NULL);

//    aux->pe = resultado;
    return resultado;
}

void end_program(struct node* root, int error, int op){
    if(error == incomplete_arguments)
        printf("Incomplete arguments quantity for the current operator %c\n", get_op_char(op));
    else if(error == unknown_argument)
        printf("Unknown operator %c fault\n", get_op_char(op));
    exit(0);
}

struct node* reduce_mul(struct node* root, int is_root, int rec_level){
    struct node* aux = root;
    struct node* a;
    if(!is_root)
        a = aux->pe->pe->pd;
    else
        a = aux->pe->pd;
    struct node* b;
    if(!is_root)
        b = aux->pe->pd;
    else
        b = aux->pd;

    if(b->op == AT)
        b = rec_p(b, rec_level + 1);

    if(a->op == AT)
        a = rec_p(a, rec_level + 1);

    if(a->op == AT || b->op == AT) {
        if(!is_root) {
            aux->pe->pe->pd = a;
            aux->pe->pd = b;
        }else{
            aux->pe->pd = a;
            aux->pd = b;
        }
        return aux;
    }
    int ai = a->op;
    int bi = b->op;
//    printf("%d - %d = %d\n", ai, bi , ai-bi);
    struct node* resultado = new_node(ai*bi, NULL, NULL);

//    aux->pe = resultado;
    return resultado;
}

struct node* reduce_div(struct node* root, int is_root, int rec_level){
    struct node* aux = root;
    struct node* a;
    if(!is_root)
        a = aux->pe->pe->pd;
    else
        a = aux->pe->pd;
    struct node* b;
    if(!is_root)
        b = aux->pe->pd;
    else
        b = aux->pd;

    if(b->op == AT)
        b = rec_p(b, rec_level + 1);

    if(a->op == AT)
        a = rec_p(a, rec_level + 1);

    if(a->op == AT || b->op == AT) {
        if(!is_root) {
            aux->pe->pe->pd = a;
            aux->pe->pd = b;
        }else{
            aux->pe->pd = a;
            aux->pd = b;
        }
        return aux;
    }

    int ai = a->op;
    int bi = b->op;
//    printf("%d - %d = %d\n", ai, bi , ai-bi);
    struct node* resultado = new_node(ai/bi, NULL, NULL);

//    aux->pe = resultado;
    return resultado;
}

void reduce_lt(struct node* root, int is_root, int rec_level){
    struct node* aux = root;
    struct node* a;
    if(!is_root)
        a = aux->pe->pe->pd;
    else
        a = aux->pe->pd;
    struct node* b;
    if(!is_root)
        b = aux->pe->pd;
    else
        b = aux->pd;

    if(b->op == AT)
        b = rec_p(b, rec_level + 1);

    if(a->op == AT)
        a = rec_p(a, rec_level + 1);

    if(a->op == AT || b->op == AT) {
        if(!is_root) {
            aux->pe->pe->pd = a;
            aux->pe->pd = b;
        }else{
            aux->pe->pd = a;
            aux->pd = b;
        }
        return;
    }

    int ai = a->op;
    int bi = b->op;
    struct node* resultado;
    if(ai < bi)
        resultado = new_node(TR, NULL, NULL);
    else
        resultado = new_node(FA, NULL, NULL);

    aux->pe = resultado;
}

void reduce_gt(struct node* root, int is_root, int rec_level){
    struct node* aux = root;
    struct node* a;
    if(!is_root)
        a = aux->pe->pe->pd;
    else
        a = aux->pe->pd;
    struct node* b;
    if(!is_root)
        b = aux->pe->pd;
    else
        b = aux->pd;

    if(b->op == AT)
        b = rec_p(b, rec_level + 1);

    if(a->op == AT)
        a = rec_p(a, rec_level + 1);

    if(a->op == AT || b->op == AT) {
        if(!is_root) {
            aux->pe->pe->pd = a;
            aux->pe->pd = b;
        }else{
            aux->pe->pd = a;
            aux->pd = b;
        }
        return;
    }

    int ai = a->op;
    int bi = b->op;
    struct node* resultado;
    if(ai > bi)
        resultado = new_node(TR, NULL, NULL);
    else
        resultado = new_node(FA, NULL, NULL);

    aux->pe = resultado;

}

struct node* reduce(struct node* root, int rec_level){

    struct node* curr = root;
    struct node* aux1 = NULL;
    struct node* i = NULL;
    struct node* k = NULL;
    struct node* s = NULL;
    struct node* d = NULL;


    while(curr->pe != NULL){
        d = s;
        s = k;
        k = i;
        i = aux1;
        aux1 = curr;
        curr = curr->pe;
    }


    //getchar();
    switch(curr->op){
        case K:
            if(i == NULL) end_program(root, incomplete_arguments, curr->op);
            if(k != NULL) reduce_k(k, false);
            else {
//                reduce_k(root, true);
//                root = root->pe;
                root = root->pe->pd;
            }
            break;
        case S:
            if(k == NULL) end_program(root, incomplete_arguments, curr->op);
            if(s != NULL) reduce_s(s, false);
            else {
                reduce_s(root, true);
                root = root->pe;
            }
            break;
        case I:
            if(aux1 == NULL) end_program(root, incomplete_arguments, curr->op);
            if(i != NULL) {
                reduce_i(i, false);
            } else {
//                reduce_i(root, true);
//                root = root->pe;
                root = root->pd;
            }
            break;
        case B:
            if(k == NULL) end_program(root, incomplete_arguments, curr->op);
            if(s != NULL) reduce_b(s, false);
            else {
                reduce_b(root, true);
                root = root->pe;
            }
            break;
        case C:
            if(k == NULL) end_program(root, incomplete_arguments, curr->op);
            if(s != NULL) reduce_c(s, false);
            else {
                reduce_c(root, true);
                root = root->pe;
            }
            break;
        case D:
            if(s == NULL) end_program(root, incomplete_arguments, curr->op);
            if(d != NULL) reduce_d(d, false);
            else {
                reduce_d(root, true);
                root = root->pe;
            }
            break;
        case E:
            if(s == NULL) end_program(root, incomplete_arguments, curr->op);
            if(d != NULL) reduce_e(d, false);
            else {
                reduce_e(root, true);
                root = root->pe;
            }
            break;
        case F:
            if(s == NULL) end_program(root, incomplete_arguments, curr->op);
            if(d != NULL) reduce_f(d, false);
            else {
                reduce_f(root, true);
                root = root->pe;
            }
            break;
        case EQ:
            if(i == NULL) end_program(root, incomplete_arguments, curr->op);
            if(k != NULL) {
                struct node* aux = root->pd;
                reduce_equals(k, false, rec_level);
                root->pd = aux;
            }
            else {
                reduce_equals(root, true, rec_level);
                root = root->pe;
            }
            break;
        case TR:
            if(i == NULL) end_program(root, incomplete_arguments, curr->op);
            if(k != NULL) reduce_true(k, false);
            else {
                reduce_true(root, true);
                root = root->pe;
            }
            break;
        case FA:
            if(i == NULL) end_program(root, incomplete_arguments, curr->op);
            if(k != NULL) reduce_false(k, false);
            else {
                reduce_false(root, true);
                root = root->pe;
            }
            break;
        case SUM:
            if(i == NULL) end_program(root, incomplete_arguments, curr->op);
            if(k != NULL) root = reduce_sum(k, false, rec_level);
            else {
                root = reduce_sum(root, true, rec_level);
//                root = root->pe;
            }
            break;
        case MINUS:
            if(i == NULL) end_program(root, incomplete_arguments, curr->op);
            if(k != NULL) root = reduce_minus(k, false, rec_level);
            else {
                root = reduce_minus(root, true, rec_level);
//                root = root->pe;
            }
            break;
        case MUL:
            if(i == NULL) end_program(root, incomplete_arguments, curr->op);
            if(k != NULL) root = reduce_mul(k, false, rec_level);
            else {
                root = reduce_mul(root, true, rec_level);
            }
            break;
        case DIV:
            if(i == NULL) end_program(root, incomplete_arguments, curr->op);
            if(k != NULL) root = reduce_div(k, false, rec_level);
            else {
                root = reduce_div(root, true, rec_level);
            }
            break;
        case Y:
            if(aux1 == NULL) end_program(root, incomplete_arguments, curr->op);
            if(i != NULL) reduce_y(i, false);
            else {
                reduce_y(root, true);
                root = root->pe;
            }
            break;
        case LT:
            if(i == NULL) end_program(root, incomplete_arguments, curr->op);
            if(k != NULL) reduce_lt(k, false, rec_level);
            else {
                reduce_lt(root, true, rec_level);
                root = root->pe;
            }
            break;
        case GT:
            if(i == NULL) end_program(root, incomplete_arguments, curr->op);
            if(k != NULL) reduce_gt(k, false, rec_level);
            else {
                reduce_gt(root, true, rec_level);
                root = root->pe;
            }
            break;
        default:
            printf("%d ..  \n", (curr->op));
            end_program(0, unknown_argument, curr->op);
            break;
    }
    return root;
}

int get_op_value(char c){
    int r = 0;
    switch(c){
        case 'S':
            r = S;
            break;
        case 'K':
            r = K;
            break;
        case 'I':
            r = I;
            break;
        case 'B':
            r = B;
            break;
        case 'C':
            r = C;
            break;
        case 'D':
            r = D;
            break;
        case 'E':
            r = E;
            break;
        case 'F':
            r = F;
            break;
        case '<':
            r = LT;
            break;
        case '>':
            r = GT;
            break;
        case '+':
            r = SUM;
            break;
        case '-':
            r = MINUS;
            break;
        case '/':
            r = DIV;
            break;
        case '*':
            r = MUL;
            break;
        case '@':
            r = AT;
            break;
        case '=':
            r = EQ;
            break;
        case 'Y':
            r = Y;
            break;
        default:
            r = c - '0';
    }
    return r;
}

char get_op_char(int c){
    char r = 0;
    switch(c){
        case S:
            r = 'S';
            break;
        case K:
            r = 'K';
            break;
        case I:
            r = 'I';
            break;
        case B:
            r = 'B';
            break;
        case C:
            r = 'C';
            break;
        case D:
            r = 'D';
            break;
        case E:
            r = 'E';
            break;
        case F:
            r = 'F';
            break;
        case AT:
            r = '@';
            break;

        case LT:
            r = '<';
            break;
        case GT:
            r = '>';
            break;
        case SUM:
            r = '+';
            break;
        case MINUS:
            r = '-';
            break;
        case DIV:
            r = '/';
            break;
        case MUL:
            r = '*';
            break;
        case EQ:
            r = '=';
            break;
        case TR:
            r = 'T';
            break;
        case FA:
            r = 'F';
            break;
        case Y:
            r = 'Y';
            break;
        default:
            r = '0' + c;
            break;
    }
    return r;
}

void print_tree(struct node* no){
    static int deph = 0;
    static int lista = 0;
    if(no->pe) {
        deph++;
        print_tree(no->pe);
    }
    switch (no->op){
        case AT:
            printf("(");
            break;
        case S:
            printf("S");
            break;
        case K:
            printf("K");
            break;
        case I:
            printf("I");
            break;
        case B:
            printf("B");
            break;
        case C:
            printf("C");
            break;
        case D:
            printf("D");
            break;
        case E:
            printf("E");
            break;
        case F:
            printf("F");
            break;
        case TR:
            printf("TRUE");
            break;
        case FA:
            printf("FALSE");
            break;
        case GT:
            printf(">");
            break;
        case LT:
            printf("<");
            break;
        case EQ:
            printf("==");
            break;
        case SUM:
            printf("+");
            break;
        case MINUS:
            printf("-");
            break;
        case MUL:
            printf("*");
            break;
        case DIV:
            printf("/");
            break;
        case Y:
            printf("Y");
            break;
        default:
            printf("%u", no->op);
    }
    if(no->pd) {
        deph++;
        if(no->pd != no) {
            print_tree(no->pd);
            printf(")");
        }else{
            printf("Y(");
            print_tree(no->pe);
            printf(")");
            return;
        }
    }
    if(deph == 0)
        lista = 0;
}
