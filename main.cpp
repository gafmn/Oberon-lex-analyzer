#include <iostream>
#include <string>
#define MAX 100
using namespace std;


enum ClassName {
    Eof,  // Special type meaning the token is last in the source
    Add, Sub, Mul, Divide,
    Til, Apper, Dot, Comma, Semicolon, Pipe, LefBr, LefSqBr, LefFigBr,
    Assign, Pow, Eq, NoEq, Lat, Les, Great, LesOrEq, GreatOrEq, TwoDot, Colon, RightBr, RightSqBr, 
    RightFigBr, Arr, Import, Begin, By, Case, Const, Div, Do, Else, Elif, End, False, If, In,
    Is, Mod, Modul, Nil, Of, Or, Pointer, Proc, Rec, Rep, Return, Then, To, True, Type, Until, Var,
    While, For,
    IntDec, IntHex, IntExp, Real, Str, StrHex,
    Identifier,
};

class Node {
public:
    string value;
    ClassName class_name;
    Node* next; 

    Node() {
        next = NULL;    
    }

    Node(string value, ClassName class_name) {
        this->value = value;
        this->class_name = class_name;
        this->next = NULL;
    }
};

class SymbolTable {
public:
    Node* nodes[MAX];

public:
    SymbolTable() {
       for (int i = 0; i < MAX; i++) 
            nodes[i] = NULL;
    }

    int hashf (string node_value) {
        int ascii_sum = 0;
        
        for (int i = 0; i < node_value.length(); i++) 
            ascii_sum += node_value[i];

        return (ascii_sum % 100);
    }

    bool insert (string value, ClassName class_name) {
        int hash_id = hashf(value);
        Node* node = new Node(value, class_name);
        if (nodes[hash_id] == NULL) {
            nodes[hash_id] = node;

            cout << "\n" << hash_id << " inserted";
            
            return true;
        }else {
            Node* start = nodes[hash_id];
            if (start->value == value) 
                return false;
            while (start->next != NULL) 
                start = start->next;
            start->next = node;
            cout << "\n" << hash_id << " inserted";
            return true;
        }
        return false;
    }

    string find (string value) {
        int hash_id = hashf(value);
        Node* start = nodes[hash_id];

        if (start == NULL) {
            return "-1";
        }

        while (start != NULL) {
            if (start->value == value) {
                cout << "\n";
                cout << value << " was found" ;
                return start->value;
            }
            start = start->next;
        }
        cout << value << " was not found";
        return "-1";
    }

};

class Token {
public:
    ClassName class_name;
    string value;

public:
    Token() {
        this->class_name = ClassName::Eof;
    }

    Token(ClassName class_name, string value) {
        this->class_name = class_name;
        this->value = value;
    }

};


class Lexer {
private:
    string src;
    string::iterator src_iter;

public:
    SymbolTable symbol_table;

public:
    Lexer(string src) {
        this->src = src;
        this->src_iter = this->src.begin();
    }

    Token next() {
        Token token;

        while (src_iter != src.end()) {
            if (*src_iter == ' ' || *src_iter == '\n') {
                // Skip spaces and new line characters
            }
            else if (*src_iter == '(') {
                if (src_iter + 1 != src.end() && *(src_iter + 1) == '*') {
                    parseComment();
                }
            }
            else if (isDigit(*src_iter)) {
                return parseNumber();
            }
            else if (isLetter(*src_iter)) {
                return parseIdentifier();
            }
            else if (*src_iter == '"') {
                return parseString();
            }

            src_iter++;
        }

        return token;
    }

    bool createSymbolTable() {
        symbol_table = SymbolTable();
        bool res = true;
        // Feel the table keywords
        string keywords[] = {
            "ARRAY", "BEGIN", "BY", "CASE", "CONST", "DIV", "DO",
            "ELSE", "ELSEIF", "END", "FALSE", "FOR", "IF", "IMPORT",
            "IN", "IS", "MOD", "MODULE", "NIL", "OF", "OR", "POINTER",
            "PROCEDURE", "RECORD", "REPEAT", "RETURN", "THEN", "TO",
            "TRUE", "TYPE", "UNTIL", "VAR", "WHILE",
        };

        ClassName types[] = {
            Arr, Begin, By, Case, Const, Div, Do, Else, Elif, End, False, For, If, Import, 
            In, Is, Mod, Modul, Nil, Of, Of, Or, Pointer, Proc, Rec, Rep, Return, Then, To,
            True, Type, Until, Var, While,
        };

        int length = (sizeof(types)/sizeof(*types));

        for (int i = 0; i < length - 1; i++) {
            res = res && symbol_table.insert(keywords[i], types[i]);
        }
       
        return res;
    }

private:
    bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    bool isHexDigit(char c) {
        return isDigit(c) || (c >= 'A' && c <= 'F');
    }

    bool isLetter(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    Token parseString() {
        Token token;
        string value = "";

        src_iter++;

        while (src_iter != src.end() && *src_iter != '"') {
            value += *src_iter;
            src_iter++;
        }

        if (src_iter == src.end()) {
            cerr << "Strings should have quotes in the end" << endl;
            exit(1);
        }

        src_iter++;

        token.class_name = ClassName::Str;
        token.value = value;
        if (symbol_table.find(value) != value)
            if (symbol_table.insert(value, ClassName::Identifier))
                    cout << "\n" << "New value is added: " << value << "\n";
        return token;
    }

    void parseComment() {
        if (src_iter + 2 == src.end()) {
            cerr << "Comment section should be enclosed" << endl;
            exit(1);
        }

        char last_char = ' ';
        while (last_char != '*' && *src_iter != ')') {
            src_iter++;

            if (src_iter == src.end()) {
                cerr << "Comment section should be enclosed" << endl;
                exit(1);
            }
        }
    }

    Token parseNumber() {
        Token token;
        string value = "";    

        // Skip zeros
        while (src_iter != src.end() && *src_iter == '0') { src_iter++; }

        while (true) {
            if (isDigit(*src_iter)) {
                value += *src_iter;
            }
            else if (*src_iter >= 'A' && *src_iter <= 'F') {
                while (src_iter != src.end() && isHexDigit(*src_iter)) {
                    value += *src_iter;
                    src_iter++;
                }

                if (src_iter == src.end() || (*src_iter != 'H' && *src_iter != 'X')) {
                    cerr << "Hex number should be ended with an 'H' or an 'X' symbol" << endl;
                    exit(1);
                }

                value += *src_iter;
                src_iter++;

                if (value.back() == 'H') {
                    token.class_name = ClassName::IntHex;
                }
                else if (value.back() == 'X') {
                    token.class_name = ClassName::StrHex;
                }
                token.value = value;
                return token;
            }
            else if (*src_iter == 'H') {
                value += *src_iter;
                src_iter++;

                token.class_name = ClassName::IntHex;
                token.value = value;
                return token;
            }
            else if (*src_iter == 'X') {
                value += *src_iter;
                src_iter++;

                token.class_name = ClassName::StrHex;
                token.value = value;
                return token;
            }
            else if (*src_iter == '.') {
                value += *src_iter;
                src_iter++;

                while (src_iter != src.end() && isDigit(*src_iter)) {
                    value += *src_iter;
                    src_iter++;
                }

                if (src_iter != src.end() && *src_iter == 'E') {
                    value += *src_iter;
                    src_iter++;

                    if (src_iter != src.end() && (*src_iter == '+' || *src_iter == '-')) {
                        value += *src_iter;
                        src_iter++;
                    }

                    if (src_iter == src.end() || !isDigit(*src_iter)) {
                        cerr << "An exponent notation should be ended with a number" << endl;
                        exit(1);
                    }

                    while (src_iter != src.end() && isDigit(*src_iter)) {
                        value += *src_iter;
                        src_iter++;
                    }
                }

                token.class_name = ClassName::Real;
                token.value = value;
                return token;
            }
            else {
                break;
            }

            src_iter++;
        }

        token.class_name = ClassName::IntDec;
        token.value = value;
        return token;
    }

    Token parseIdentifier() {
        Token token;
        string value = "";
        
        return token;
    }
};


int main() {
    string src = "123 1A123H (* 123 *) 0123.E+10 0ABCD123X 228X \"Hey there\" 123";
    Lexer lexer = Lexer(src);
    Token token = lexer.next();
    lexer.createSymbolTable();
    while (token.class_name != ClassName::Eof) {
        cout << "Token " << token.class_name << " " << token.value << endl;
        token = lexer.next();
    }

    string s = lexer.symbol_table.find("Hey there");
    cout << "\n" << "FOUND" << ' '  << s;

    bool res = lexer.symbol_table.insert("Hey there", ClassName::Identifier);
    cout << "\n" << res;
    return 0;
}
