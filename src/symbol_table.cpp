#include <iostream>
#include <unordered_map>

using namespace std;

class SymbolTable {
public:
    SymbolTable() {
        static_vars_count = 0;
        field_vars_count = 0;
        arg_vars_count = 0;
        local_vars_count = 0;
    }

    void define(string name, string type, string kind) {
        int index = 0;
        if (kind == "static") {
            index = static_vars_count;
            static_vars_count++;
        } else if (kind == "field") {
            index = field_vars_count;
            field_vars_count++;
        } else if (kind == "arg") {
            index = arg_vars_count;
            arg_vars_count++;
        } else if (kind == "var") {
            index = local_vars_count;
            local_vars_count++;
        }
        else {
            cout << "Error: Kind '" << kind << "' not recognized. Expected 'static', 'field', 'arg' or 'var'." << endl;
            return;
        }
        table[name] = make_tuple(type, kind, index);
    }

    int var_count(string kind) {
        if (kind == "static") {
            return static_vars_count;
        } else if (kind == "field") {
            return field_vars_count;
        } else if (kind == "arg") {
            return arg_vars_count;
        } else if (kind == "var") {
            return local_vars_count;
        } else {
            cout << "Error: Kind '" << kind << "' not recognized. Expected 'static', 'field', 'arg' or 'var'." << endl;
            return -1;
        }
    }

    string type_of(string name) {
        if (table.count(name) > 0) {
            return get<0>(table[name]);
        } else {
            cout << "Cannot get type of variable '" << name << "'." << endl;
            cout << "Variable '" << name << "' not found in symbol table." << endl;
            return "";
        }
    }

    string kind_of(string name) {
        if (table.count(name) > 0) {
            return get<1>(table[name]);
        } else {
            cout << "Cannot get kind of variable '" << name << "'." << endl;
            cout << "Error: Variable '" << name << "' not found in symbol table." << endl;
            return "";
        }
    }

    int index_of(string name) {
        if (table.count(name) > 0) {
            return get<2>(table[name]);
        } else {
            cout << "Cannot get index of variable '" << name << "'." << endl;
            cout << "Error: Variable '" << name << "' not found in symbol table." << endl;
            return -1;
        }
    }

    void reset() {
        table.clear();
        static_vars_count = 0;
        field_vars_count = 0;
        arg_vars_count = 0;
        local_vars_count = 0;
    }

    void print() {
        for (const auto& entry : table) {
            string name = entry.first;
            string type = get<0>(entry.second);
            string kind = get<1>(entry.second);
            int index = get<2>(entry.second);

            cout << "Name: " << name << ", Type: " << type << ", Kind: " << kind << ", Index: " << index << endl;
        }
    }

    bool contains(string name) {
        return table.find(name) != table.end();
    }

private:
    unordered_map<string, tuple<string, string, int>> table;
    int static_vars_count;
    int field_vars_count;
    int arg_vars_count;
    int local_vars_count;
};