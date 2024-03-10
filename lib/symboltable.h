#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

struct Symbol
{
    wchar_t *type;
    wchar_t *value;
    int numberLine;
    int numberColumn;
    Symbol *pointer;
};

#endif