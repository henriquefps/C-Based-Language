#bin/bash
lex calc.l
yacc calc.y -d -v  -Wno-empty-rule
gcc y.tab.c lex.yy.c -o main
./main
rm -r lex.yy.c y.tab.c y.tab.h main