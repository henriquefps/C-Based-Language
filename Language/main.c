#include "cheney.h"
#include "database.h"

int main(int n, char* args[]) {

    storeFunction("fib", "[f][n](<n2)n(+(f(-n1))(f(-n2)))");
    storeFunction("fab", "[f][n](=n0)1(+(n)(f(-n1)))");
    storeFunction("fat", "[f][n](=n0)1(*(n)(f(-n1)))");
    storeFunction("succ", "[s][n]+n1");
    storeFunction("triple", "[t][n]*n3");
    storeFunction("doble", "[d][n]*n2");

    storeFunction("fib", "[f][n](<n2)n(+(f(-n1))(f(-n2)))");
    storeFunction("fab", "[f][n](=n0)1(+(n)(f(-n1)))");

    printf("\n");
    printFunctions();
    printf("\n");

    //functionCall("fib","5");
    //functionCall("fab","5");
    //functionCall("fat","5");
    //functionCall("succ","5");
    //functionCall("succ(5)");

    int a, b;
    searchParethesis("succ(5)", &a, &b);
    printf("%i %i",a,b);
    return 0;
}