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
    IntDec, IntHex, IntExp, Real,
};

class Node {
public:
    string identifier;
    Node* next;

public:
    Node() {
        next = NULL;
    }

    Node (string identifier) {
        this->identifier = identifier;
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

    int hashf (string identifier) {
        int ascii_sum = 0;
        
        for (int i = 0; i < identifier.length(); i++) 
            ascii_sum += identifier[i];

        return (ascii_sum % 100);
    }

    bool insert (string identifier) {
        int hash_id = hashf(identifier);
        Node* node = new Node(identifier);

        if (nodes[hash_id] == NULL) {
            nodes[hash_id] = node;

            cout << "\n" << hash_id << " inserted";
            
            return true;
        }else {
            Node* start = nodes[hash_id];
            while (start->next != NULL) 
                start = start->next;
            start->next = node;
            cout << "\n" << hash_id << " inserted";
            return true;
        }
        return false;
    }

    string find (string identifier) {
        int hash_id = hashf(identifier);
        Node* start = nodes[hash_id];

        if (start == NULL) {
            cout << "\n" << "Error";
            return "-1";
        }

        while (start != NULL) {
            if (start->identifier == identifier) {
                cout << "\n";
                cout << identifier << " was found" ;
                return start->identifier;
            }
            start = start->next;
        }
        cout << identifier << " was not found";
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
    string src;
    string::iterator src_iter;

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

            src_iter++;
        }

        return token;
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

                if (src_iter == src.end() || *src_iter != 'H') {
                    cerr << "Hex number should be ended with 'H' symbol" << endl;
                    exit(1);
                }

                value += *src_iter;
                src_iter++;

                token.class_name = ClassName::IntHex;
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
        return token;
    }
};


int main() {
    string src = "123 1A123H (* 123 *) 0123.E+10";

    Lexer lexer = Lexer(src);
    Token token = lexer.next();
    SymbolTable symbol_table = SymbolTable();

    int k = symbol_table.hashf("IF");
    int m = symbol_table.hashf("BEGIN");
    cout << k << " IF";
    cout << m << " BEGIN";
    if (symbol_table.insert("IF")) {
        cout << "Everything is ok";
    }
    symbol_table.insert("BEGIN");
    string res = symbol_table.find("BEGIN");
    cout << "\n" << res; 

    // while (token.class_name != ClassName::Eof) {
    //     cout << "Token " << token.class_name << " " << token.value << endl;
    //     token = lexer.next();
    //  }

    return 0;
}
