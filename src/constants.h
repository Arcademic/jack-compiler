#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <list>
#include <unordered_map>

using namespace std;

static const size_t NPOS = string::npos;

static const string SINGLE_LINE_COMMENT_STR = "//";
static const string MULTI_LINE_COMMENT_START_STR = "/*";
static const string MULTI_LINE_COMMENT_END_STR = "*/";

static const list<char> SYMBOLS = {
    '{',
    '}',
    '(',
    ')',
    '[',
    ']',
    '.',
    ',',
    ';',
    '+',
    '-',
    '*',
    '/',
    '&',
    '|',
    '<',
    '>',
    '=',
    '~'
};

static const list<string> KEYWORDS = {
    "class",
    "constructor",
    "function",
    "method",
    "field",
    "static",
    "var",
    "int",
    "char",
    "boolean",
    "void",
    "true",
    "false",
    "null",
    "this",
    "let",
    "do",
    "if",
    "else",
    "while",
    "return"
};

static const unordered_map<char, string> XML_SYMBOLS = {
    {'<', "&lt;"},
    {'>', "&gt;"},
    {'"', "&quot;"},
    {'&', "&amp;"},
};

#endif // CONSTANTS_H