#include <iostream>
#include <string>

using namespace std;

#define MAX_HASH 997


enum ClassName {
    Eof,  // Special type meaning the token is last in the source
    Add, Sub, Mul, Divide,
    Til, Apper, Dot, Comma, Semicolon, Pipe, LefBr, LefSqBr, LefFigBr,
    Assign, Pow, Eq, NoEq, Lat, Les, Great, LesOrEq, GreatOrEq, TwoDot, Colon, RightBr, RightSqBr, 
    RightFigBr, Arr, Import, Begin, By, Case, Const, Div, Do, Else, Elif, End, False, If, In,
    Is, Mod, Modul, Nil, Of, Or, Pointer, Proc, Rec, Rep, Return, Then, To, True, Type, Until, Var,
    While, For,
    IntDec, IntHex, IntExp, Real, Str, StrHex,
    Ident,
    NONE, // Special type meaning the symbol table does not contain such identifier
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
    Node* nodes[MAX_HASH];

public:
    SymbolTable() {
       for (int i = 0; i < MAX_HASH; i++) 
            nodes[i] = NULL;
    }

    int hashf (string node_value) {
        int hash_sum = 0;
       
        for (char sym : node_value) {
            hash_sum += 29 * (sym + 281);
            hash_sum %= MAX_HASH;
        }

        return hash_sum;
    }

    bool insert (string value, ClassName class_name) {
        int hash_id = hashf(value);
        Node* node = new Node(value, class_name);
        if (nodes[hash_id] == NULL) {
            nodes[hash_id] = node;
            
            return true;
        }else {
            Node* start = nodes[hash_id];
            if (start->value == value) 
                return false;
            while (start->next != NULL) 
                start = start->next;
            start->next = node;
            return true;
        }
        return false;
    }

    ClassName find (string value) {
        int hash_id = hashf(value);
        Node* start = nodes[hash_id];

        if (start == NULL) {
            return ClassName::NONE;
        }

        while (start != NULL) {
            if (start->value == value) {
                return start->class_name;
            }
            start = start->next;
        }
        return ClassName::NONE;
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

        createSymbolTable();
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



private:
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
            "ABS", "ASR", "ASSERT", "BOOLEAN", "BYTE", "CHAR", "CHR",
            "DEC", "EXCL", "FLOOR", "FLT", "INC", "INTEGER", "LEN",
            "LSL", "NEW", "ODD", "ORD", "PACK", "REAL", "ROR", "SET",
            "UNPK", 
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

        while (isLetter(*src_iter) || isDigit(*src_iter)) {
            value += *src_iter;
            src_iter++;
        }

        ClassName class_name = symbol_table.find(value);

        if (class_name == ClassName::NONE) {
            class_name = ClassName::Ident;
            symbol_table.insert(value, class_name);
        }

        token.class_name = class_name;
        token.value = value;

        return token;
    }
};


int main() {
    string src = "123 1A123H (* 123 *) 0123.E+10 0ABCD123X 228X \"Hey there\" 123 Abba LOLKEK123 A1B2 hello hello";

    Lexer lexer = Lexer(src);
    Token token = lexer.next();

    while (token.class_name != ClassName::Eof) {
        cout << "Token " << token.class_name << " " << token.value << endl;
        token = lexer.next();
    }

    return 0;
}
