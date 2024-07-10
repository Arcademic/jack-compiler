#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <list>
#include <unordered_map>
#include <regex>

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
    '~',
    '"'
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

static const unordered_map<string, string> XML_SYMBOLS = {
    {"<", "&lt;"},
    {">", "&gt;"},
    {"\"", "&quot;"},
    {"&", "&amp;"},
};

static const regex IDENTIFIER("[a-zA-Z_][a-zA-Z0-9_]*");

static const regex OP("(\\+|-|\\*|/|\\&|\\||<|>|=)");

static const regex UNARY_OP("(-|~)");

static const regex INTEGER_CONSTANT("\\d{1,5}");

static const regex STRING_CONSTANT("[^\"\n]+");

static const regex KEYWORD_CONSTANT("(true|false|null|this)");

static const unordered_map<string, string> OP_TO_VM = {
    {"+", "add"},
    {"-", "sub"},
    {"=", "eq"},
    {">", "gt"},
    {"<", "lt"},
    {"&", "and"},
    {"|", "or"},
};

static const unordered_map<string, string> UNARY_OP_TO_VM = {
    {"-", "neg"},
    {"~", "not"},
};

static const unordered_map<string, string> KIND_TO_SEGMENT = {
    {"static", "static"},
    {"field", "this"},
    {"arg", "argument"},
    {"var", "local"},
};

#endif // CONSTANTS_H