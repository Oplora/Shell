#include <iostream>
#include <cstring>

#define NUM_OF_LEX 31
#define MAX_STRING_SIZE 255
#define MAX_NUM_OF_ID 100
#define MAX_NUM_OF_GOTO 10
#define SIZE_OF_POLIZ 1000
#define CUR_DIR "/home/oplora/Desktop/PracSP/additional/"


using namespace std;


/*  _______________________________________|    ОТЛАДКА    |_______________________________________     */

const bool z1 = false;    // Лексический анализатор
const bool z2 = true;    // TGO
const bool z3 = false;    // POLIZ
const bool z4 = false;    // TID

/*  _______________________________________|    МОИ ПЕРСОНАЛЬНЫЕ ФУНКЦИИ    |_______________________________________     */

char* newstr(const char* s) {
    if (s == NULL) return NULL;
    char *n = new char[strlen(s) + 1];
    strcpy(n,s);
    return n;
}

char* sumstr(const char* a, const char* b) {
    if (b == NULL) return newstr(a);
    if (a == NULL) return newstr(b);
    int i = strlen(a);
    char *n = new char[i + strlen(b) + 1];
    strcpy(n,a);
    strcpy(n+i,b);
    return n;
}

/*  _______________________________________|    ЛЕКСЕМЫ    |_______________________________________     */

enum type_of_lex
{
    
    LEX_NULL,         // 0
    
    LEX_AND,          // 1
    LEX_ELSE,         // 2
    LEX_IF,           // 3
    LEX_FOR,          // 4
    LEX_GOTO,         // 5
    LEX_INT,          // 6
    LEX_NOT,          // 7
    LEX_OR,           // 8
    LEX_PROGRAM,      // 9
    LEX_READ,         // 10
    LEX_STRING,       // 11
    LEX_WHILE,        // 12
    LEX_WRITE,        // 13
    
    LEX_SEMICOLON,    // 14
    LEX_COMMA,        // 15
    LEX_COLON,        // 16
    LEX_ASSIGN,       // 17
    LEX_LBRACE,       // 18
    LEX_RBRACE,       // 19
    LEX_LPAREN,       // 20
    LEX_RPAREN,       // 21
    LEX_EQ,           // 22
    LEX_LSS,          // 23
    LEX_GTR,          // 24
    LEX_PLUS,         // 25
    LEX_MINUS,        // 26
    LEX_ASTERISK,     // 27
    LEX_SLASH,        // 28
    LEX_NEQ,          // 29
    LEX_LEQ,          // 30
    LEX_GEQ,          // 31
    
    LEX_ID,           // 32
    LEX_NUM,          // 33
    LEX_STR,          // 34
    LEX_UMINUS,       // 35
    POLIZ_ADDRESS,    // 36
    POLIZ_GO,         // 37
    POLIZ_FGO,        // 38
    LEX_EOF           // 39
    
};

const char * TL[] = {
    "НЕОПОЗНАННАЯ_ЛЕКСЕМА",    // 0

    "and",                     // 1
    "else",                    // 2
    "if",                      // 3
    "for",                     // 4
    "goto",                    // 5
    "int",                     // 6
    "not",                     // 7
    "or",                      // 8
    "program",                 // 9
    "read",                    // 10
    "string",                  // 11
    "while",                   // 12
    "write",                   // 13
    
    ";",                       // 14
    ",",                       // 15
    ":",                       // 16
    "=",                       // 17
    "{",                       // 18
    "}",                       // 19
    "(",                       // 20
    ")",                       // 21
    "==",                      // 22
    "<",                       // 23
    ">",                       // 24
    "+",                       // 25
    "-",                       // 26
    "*",                       // 27
    "/",                       // 28
    "!=",                      // 29
    "<=",                      // 30
    ">=",                      // 31
    
    "ИДЕНТИФИКАТОР",           // 32
    "ЧИСЛОВАЯ_КОНСТАНТА",      // 33
    "СТРОКОВАЯ_КОНСТАНТА",     // 34
    "у-",                      // 35
    "ADDRESS",                 // 36
    "GO",                      // 37
    "FGO",                     // 38
    "EOF"                      // 39
};

struct Lex {
    
    type_of_lex type;
    int value;
    char *string;
    
    Lex (type_of_lex t = LEX_NULL, int v = 0) : type(t), value(v), string(NULL) {}
    Lex (type_of_lex t, const char *s) : type(t), value(0) { string = newstr(s); }
    Lex (const Lex & l) {
        type = l.type;
        value = l.value;
        string = newstr(l.string);
    }
    Lex & operator=(const Lex & l) {
        if (this != &l) {
            type = l.type;
            value = l.value;
            if (string != NULL) delete[] string;
            string = newstr(l.string);
        }
        return *this;
    }
    ~Lex () {
        if (string != NULL) delete[] string;
    }
    friend ostream& operator << (ostream &s, Lex &l) {
        return s << TL[l.type];
    }
    void print () {
        cout << TL[type] << '[' << value;
        if (string != NULL) cout << "|" << string ;
        cout << ']';
    }
    
};

/*  _______________________________________|    ОШИБКИ    |_______________________________________*/

void err(int n = -1, int i = -1, const char* s = NULL, Lex l = Lex()) {
    
    cout << endl << "ОШИБКА" << endl;
    if (i != -1) cout << "Строка " << i << ": ";
    
    switch (n) {
            
        case 0:  cout << "Ошибка открытия файла"; break;
        case 1:  cout << "Превышен максимальный размер строки"; break;
        case 2:  cout << "Превышено максимально допустимое количество переменных"; break;
        case 3:  cout << "Превышено максимально допустимое количество меток"; break;
        case 4:  cout << "ПОЛИЗ переполнен"; break;
        case 5:  cout << "Попытка деления на ноль"; break;
        case 6:  cout << "Тип константы не соответствует типу переменной"; break;
        case 7:  cout << "Попытка присвоить значение структуре"; break;
        case 8:  cout << "Тип второго выражения в for должен быть int"; break;
        case 9: cout << "Тип выражения в while должен быть int"; break;
        case 10: cout << "Тип выражения в if должен быть int"; break;

        case 11: cout << "Слева от = не переменная"; break;
        case 12: cout << "Попытка присвоения переменной значения другого типа"; break;

        case 13: cout << "Идентификатор " << s << " уже описан"; break;
        case 14: cout << "Идентификатор " << s << " не описан"; break;
        case 15: cout << "Метка " << s << " не описана"; break;
        case 16: cout << "Значение " << s << " не определено"; break;
        case 17: cout << "Значения разных типов при операции " << l; break;
        case 18: cout << "Ошибка в записи выражения, получено " << l; break;
        case 19: cout << "Операция " << l << " применима только к типу " << s; break;
        case 20: cout << "Ожидалось " << s << " ,но получено " << l; break;
        case 66: cout << "ЭТО Я ВЫЛЕТАЮ "  << l << endl; break;
        default: cout << "Непредвиденная ошибка работы интерпретатора";
            
    }
    
    cout << endl;
    throw n;
    
}

/*  _______________________________________|    ЛЕКСИЧЕСКИЙ АНАЛИЗАТОР   |_______________________________________     */

class Scanner {
    
    enum state { H, IDENT, NUMB, STR, CMP, COM };
    FILE * fp;
    char c;
    char buf[MAX_STRING_SIZE + 1];
    int buf_top, count;
    
    void clear () { buf_top = 0; *buf = '\0'; }
    void add () {
        if (buf_top > MAX_STRING_SIZE - 1) err(1);
        buf[buf_top] = c;
        buf[++buf_top] = '\0';
    }
    type_of_lex search_buf () {
        for (int i = 1; i <= NUM_OF_LEX; ++i)
            if (!strcmp(buf, TL[i])) return (type_of_lex) i;
        return LEX_NULL;
    }
    void gc () { c = fgetc (fp); }
    
public:
    Scanner (const char * program) : count(1) {
        fp = fopen(program, "r");
        if (fp == NULL) err(0);
        clear();
        gc();
    }
    ~Scanner () { fclose(fp); }
    
    int get_count() { return count; }
    
    Lex get_lex () {
        int d;
        type_of_lex t;
        state CS = H;
        while (true) {
            switch (CS) {
                    
                case H:
                    if (c == EOF) return Lex(LEX_EOF);
                    if (c == '\t' || c == ' ' || c== '\r') { if (z1) cout << c; gc(); break; } 
                    if (c == '\n') { ++count; if (z1) cout << c; gc(); break; }
                    if (isalpha(c) || c == '_') {
                        clear();
                        add();
                        gc();
                        CS = IDENT;
                        break;
                    }
                    if (isdigit(c)) {
                        d = c - '0';
                        gc();
                        CS = NUMB;
                        break;
                    }
                    if (c == '"') {
                        clear();
                        gc();
                        CS = STR;
                        break;
                    }
                    if (c == '<' || c == '>' || c == '=' || c == '!') {
                        clear();
                        add();
                        gc();
                        CS = CMP;
                        break;
                    }
                    clear();
                    add();
                    gc();
                    if (*buf == '/' && c == '*') {
                        gc();
                        CS = COM;
                        break;
                    }
                    return Lex(search_buf());
                    
                case IDENT:
                    if (isalpha(c) || isdigit(c) || c == '_') {
                        add();
                        gc();
                        break;
                    }
                    if ((t = search_buf()) != LEX_NULL) return Lex(t);
                    return Lex(LEX_ID, buf);
                    break;
                    
                case NUMB:
                    if (isdigit(c)) {
                        d = d * 10 + (c - '0');
                        gc();
                    } else return Lex(LEX_NUM, d);
                    break;
                    
                case STR:
                    if (c == EOF) err(20,count,"\"",Lex(LEX_EOF));
                    if (c == '\n') ++count;
                    if (c == '"') {
                        gc();
                        return Lex(LEX_STR, buf);
                        break;
                    }
                    if (buf[buf_top-1] == '\\') {
                        switch (c) {
                            case 'n': buf[buf_top-1] = '\n'; break;
                            case 'r': buf[buf_top-1] = '\r'; break;
                            case 't': buf[buf_top-1] = '\t'; break;
                            default: add();
                        }
                        gc();
                        break;
                    }
                    add();
                    gc();
                    break;
                    
                case CMP:
                    if (c == '=') {
                        add();
                        gc();
                    }
                    return Lex(search_buf());
                    break;
                    
                case COM:
                    if (c == EOF) err(20,count,"*/",Lex(LEX_EOF));
                    if (c == '\n') ++count;
                    if (*buf == '*' && c == '/') {
                        gc();
                        CS = H;
                        break;
                    }
                    *buf = c;
                    gc();
                    break;
                    
            }
        }
    }
    
};

/*  _______________________________________|    ТАБЛИЦЫ   |_______________________________________     */

class Ident {
    
    struct block {
        char *name;
        Lex ident;
        bool declared;
        block *next;
    };
    
    block *b;
    
public:
    Ident () : b(NULL) {}
    ~Ident () {
        block *p = b;
        while (b != NULL) {
            p = b->next;
            delete[] b->name;
            delete b;
            b = p;
        }
    }
    Ident & operator=(Ident & l) {
        if (this != &l) {
            b = l.b;
            l.b = NULL;
        }
        return *this;
    }
    void put (Lex &, Lex &, Lex);
    void dec (const char*);
    void print () {
        block *p = b;
        while (p != NULL) {
            cout << endl << '\t' << p->name << '\t' << p->declared << "   ";
            (p->ident).print();
            p = p->next;
        }
    }
    
};

template <class T, int max_size > class Stack {
    T s[max_size];
    int top;
public:
    Stack() : top(0) {}
    void reset() { top = 0; }
    void push(T i) {
        if (!is_full()) {
            s[top] = i;
            ++top;
        } else err();
    }
    T pop() {
        if (!is_empty()) {
            --top;
            return s[top];
        } 
        else err();
    }
    void print() {
        for (int i = 0; i < top; ++i)
            cout << s[i] << ' ';
    }
    bool is_empty() { return top == 0; }
    bool is_full() { return top == max_size; }
};

template <class A, class B, B s, int E> class Tabl {
    
    A *a;
    B *b;
    char **name;
    int size;
    int top;
    
public:
    Tabl (int max_size) : size(max_size), top(0) {
        a = new A[size];
        b = new B[size];
        name = new char*[size];
    }
    ~Tabl () {
        delete[] a;
        delete[] b;
        for (int i = 0; i < top; ++i)
            if (name[i] != NULL) delete[] name[i];
        delete[] name;
    }
    A & operator[] (int i) { return a[i]; }
    int search (const char *n) {
        for (int i = 0; i < top; ++i)
            if (!strcmp(n, name[i])) return i;
        return -1;
    }
    int put (const char *n) {
        int i;
        if ((i = search(n)) != -1) return i;
        if (top >= size) err(E);
        name[top] = newstr(n);
        b[top] = s;
        return top++;
    }
    void set (int i, B x) { b[i] = x; }
    B get (int i) { return b[i]; }
    char* get_name (int i) { return name[i]; }
    int get_size () { return top; }
    void print () {
        for (int i = 0; i < top; ++i) {
            cout << i << '\t' << name[i] << '\t' << b[i] << "   ";
            a[i].print();
            cout << endl;
        }
    }
    
};

Tabl < Lex, bool, false, 2 > TID(MAX_NUM_OF_ID);
Tabl < Stack < int, MAX_NUM_OF_GOTO >, int, -1, 3 > TGO(MAX_NUM_OF_GOTO);

bool id_is_dec (const char* n) {
    
    if (TID.search(n) != -1) return true;
    int i;
    if ((i = TGO.search(n)) != -1)
        if (TGO.get(i) != -1) return true;
    return false;
    
}

void Ident::put (Lex &T, Lex &ID, Lex V = Lex()) {
    block *p = b;
    b = new block;
    b->name = newstr(ID.string);
    b->next = p;
    b->ident = T;
    switch (V.type) {
        case LEX_NUM:
            (b->ident).value = V.value; break;
        case LEX_STR:
            (b->ident).string = newstr(V.string); break;
        default:
            b->declared = false;
            return;
    }
    b->declared = true;
}

void Ident::dec (const char* str = NULL) {
    char *buf = new char[MAX_STRING_SIZE + 1];
    int j;
    block *p = b;
    while (p != NULL) {
        strcpy(buf, p->name);
        if (id_is_dec(buf)) err(13,-1,buf);
        j = TID.put(buf);
        TID[j] = p->ident;
        TID.set(j, p->declared);
        p = p->next;
    }
    delete[] buf;
    
}

/*  _______________________________________|    ПОЛИЗ   |_______________________________________     */

class Poliz {
    Lex *p;
    int size;
    int top;
    
public:
    Poliz (int max_size) : size(max_size), top(0) {
        p = new Lex[size];
    }
    ~Poliz () { delete []p; }
    Lex & operator[] (int k) { return p[k]; }
    int put (const Lex & l) {
        if (top >= size) err(4);
        p[top] = l;
        return top++;
    }
    int get_top () { return top; }
    void address () { p[top-1].type = POLIZ_ADDRESS; }
    void print () {
        for (int i = 0; i < top; ++i) {
            cout << i << '\t';
            p[i].print();
            cout << endl;
        }
    }
};

/*  _______________________________________|   СИНТАКСИЧЕСКИЙ АНАЛИЗАТОР   |_______________________________________     */

class Parser {
    
    Scanner scan;
    Lex curr_lex, prev_lex;
    int loops;
    bool ident;
    
    void P();  
    void C(); 
    void D(Ident &); 
    void S(); 
    void E(); 
    void E1(); 
    void E2(); 
    void E3(); 
    void E4(); 
    void T(); 
    void M(); 
    void F();
    
    int size_of_type() {
        if (curr_lex.type == LEX_INT || curr_lex.type == LEX_STRING) return 1;
        else return 0;
    };
    void fill_goto (Poliz &);
    
    void gl () {
        curr_lex = scan.get_lex();
        if (z1) curr_lex.print();
    }
    
public:
    Poliz prog;
    Parser (const char *program) : scan(program), prog(SIZE_OF_POLIZ), loops(0) {}
    void analyze ();
    
};

void Parser::analyze () {
    gl();
    P();

    if (z2) cout << endl << "----------TGO----------" << endl;
    if (z2) TGO.print();
    fill_goto(prog);

    if (z3) cout << endl << "---------POLIZ---------" << endl;
    if (z3) prog.print();

    if (z4) cout << endl << "----------TID----------" << endl;
    if (z4) TID.print();
}

void Parser::P () {
    
    if (curr_lex.type != LEX_PROGRAM) err(20, scan.get_count(), TL[LEX_PROGRAM], curr_lex);
    gl();
    if (curr_lex.type != LEX_LBRACE) err(20, scan.get_count(), TL[LEX_LBRACE], curr_lex);
    gl();
    C();
    if (curr_lex.type != LEX_RBRACE) err(20, scan.get_count(), TL[LEX_RBRACE], curr_lex);
    
}

void Parser::C () {
    
    Ident ID_list;
    while (curr_lex.type != LEX_RBRACE) {
        if (size_of_type()) {
            D(ID_list);
            ID_list.dec();
            ID_list.~Ident();
            continue;
        }
        S();
    }
    
}

void Parser::D (Ident & ID_list) {
    
    Lex var_type, var_name;
    int i;
    while (size_of_type()) {
        var_type = curr_lex;
        do {
            gl();
            if (curr_lex.type != LEX_ID) err(20, scan.get_count(), TL[LEX_ID], curr_lex);
            var_name = curr_lex;
            gl();
            if (curr_lex.type == LEX_ASSIGN) {
                gl();
                switch (var_type.type) {
                    case LEX_INT:
                        switch (curr_lex.type) {
                            case LEX_NUM: break;
                            case LEX_PLUS:
                                gl();
                                if (curr_lex.type != LEX_NUM) err(20, scan.get_count(), TL[LEX_ID], curr_lex);
                                break;
                            case LEX_MINUS:
                                gl();
                                if (curr_lex.type != LEX_NUM) err(20, scan.get_count(), TL[LEX_ID], curr_lex);
                                curr_lex.value = -curr_lex.value;
                                break;
                            default: err(6, scan.get_count());
                        }
                        break;
                        
                    case LEX_STRING:
                        if (curr_lex.type != LEX_STR) err(6, scan.get_count());
                        break;
                        
                    default: err(7, scan.get_count());
                }
                ID_list.put(var_type,var_name,curr_lex);
                gl();
            } 
			else ID_list.put(var_type,var_name);
        } while (curr_lex.type == LEX_COMMA);
        if (curr_lex.type != LEX_SEMICOLON) err(20, scan.get_count(), TL[LEX_SEMICOLON], curr_lex);
        gl();
    }
    
}

void Parser::S () {
    
    int i, j, k;
    char *s;
    
    switch (curr_lex.type) {
            
        case LEX_IF:
            gl();
            if (curr_lex.type != LEX_LPAREN) err(20, scan.get_count(), TL[LEX_LPAREN], curr_lex);
            gl();
            E();
            if (curr_lex.type != LEX_RPAREN) err(20, scan.get_count(), TL[LEX_RPAREN], curr_lex);
            if (prev_lex.type != LEX_NUM) err(10, scan.get_count());  
            i = prog.put(Lex(POLIZ_FGO));
            gl();
            S();
            if (curr_lex.type != LEX_ELSE) err(20, scan.get_count(), TL[LEX_ELSE], curr_lex);
            j = prog.put(Lex(POLIZ_GO));
            prog[i].value = j + 1;
            gl();
            S( );
            prog[j].value = prog.get_top();
            break;
            
        case LEX_FOR:
            gl();
            if (curr_lex.type != LEX_LPAREN) err(20, scan.get_count(), TL[LEX_LPAREN], curr_lex);
            gl();
            if (curr_lex.type != LEX_SEMICOLON) {
                E();
                if (curr_lex.type != LEX_SEMICOLON) err(20, scan.get_count(), TL[LEX_SEMICOLON], curr_lex);
            }
            i = prog.get_top();
            gl();
            if (curr_lex.type != LEX_SEMICOLON) {
                E();
                if (curr_lex.type != LEX_SEMICOLON) err(20, scan.get_count(), TL[LEX_SEMICOLON], curr_lex);
                if (prev_lex.type != LEX_NUM) err(8, scan.get_count());
            } else prog.put(Lex(LEX_NUM, 1));      
            j = prog.put(Lex(POLIZ_FGO));
            prog.put(Lex(POLIZ_GO));
            gl();
            if (curr_lex.type != LEX_RPAREN) {
                E();
                if (curr_lex.type != LEX_RPAREN) err(20, scan.get_count(), TL[LEX_RPAREN], curr_lex);
            }
            i = prog.put(Lex(POLIZ_GO, i));
            prog[j+1].value = ++i;  
            s = new char[10];  
            sprintf(s, "#F_%d", loops++); 
            k = TGO.put(s);    
            delete[] s;
            gl();
            S();
            i = prog.put(Lex(POLIZ_GO, j+2)); 
            prog[j].value = ++i;
            TGO.set(k, i);
            break;
            
        case LEX_WHILE:
            gl();
            if (curr_lex.type != LEX_LPAREN) err(20, scan.get_count(), TL[LEX_LPAREN], curr_lex);
            i = prog.get_top();
            gl();
            E();
            if (curr_lex.type != LEX_RPAREN) err(20, scan.get_count(), TL[LEX_RPAREN], curr_lex);
            if (prev_lex.type != LEX_NUM) err(9, scan.get_count());
            j = prog.put(Lex(POLIZ_FGO));
            s = new char[10];
            sprintf(s, "#W_%d", loops++);
            k = TGO.put(s);
            delete[] s;
            gl();
            S();
            i = prog.put(Lex(POLIZ_GO, i));
            TGO.set(k, ++i);
            prog[j].value = i;
            break;
            
        case LEX_GOTO:
            gl();
            if (curr_lex.type != LEX_ID) err(20, scan.get_count(), TL[LEX_ID], curr_lex);
            i = TGO.put(curr_lex.string);
            j = prog.put(Lex(POLIZ_GO));
            TGO[i].push(j);
            gl();
            if (curr_lex.type != LEX_SEMICOLON) err(20, scan.get_count(), TL[LEX_SEMICOLON], curr_lex);
            gl();
            break;
            
        case LEX_READ:
            gl();
            if (curr_lex.type != LEX_LPAREN) err(20, scan.get_count(), TL[LEX_LPAREN], curr_lex);
            gl();
            if (curr_lex.type != LEX_ID) err(20, scan.get_count(), TL[LEX_ID], curr_lex);
            if ((i = TID.search(curr_lex.string)) == -1) err(14, scan.get_count(), curr_lex.string);
            prog.put(Lex(POLIZ_ADDRESS,i));
            prog.put(Lex(LEX_READ));
            gl();
            if (curr_lex.type != LEX_RPAREN) err(20, scan.get_count(), TL[LEX_RPAREN], curr_lex);
            gl();
            if (curr_lex.type != LEX_SEMICOLON) err(20, scan.get_count(), TL[LEX_SEMICOLON], curr_lex);
            gl();
            break;
            
        case LEX_WRITE:
            gl();
            if (curr_lex.type != LEX_LPAREN) err(20, scan.get_count(), TL[LEX_LPAREN], curr_lex);
            do {
                gl();
                E();
                prog.put(Lex(LEX_WRITE));
            } while (curr_lex.type == LEX_COMMA);
            if (curr_lex.type != LEX_RPAREN) err(20, scan.get_count(), TL[LEX_RPAREN], curr_lex);
            gl();
            if (curr_lex.type != LEX_SEMICOLON) err(20, scan.get_count(), TL[LEX_SEMICOLON], curr_lex);
            gl();
            break;
            
        case LEX_LBRACE:
            gl();
            while (curr_lex.type != LEX_RBRACE) S();
            gl();
            break;
            
        case LEX_ID:
            if (!id_is_dec(curr_lex.string)) {
                s = newstr(curr_lex.string);
                i = TGO.put(s);
                j = prog.get_top();
                TGO.set(i, j);
                gl();
                if (curr_lex.type != LEX_COLON) err(14, scan.get_count(), s);
                delete[] s;
                gl();
                S();
                break;
            }
            
        case LEX_PLUS:
        case LEX_MINUS:
        case LEX_NUM:
        case LEX_STR:
        case LEX_NOT:
        case LEX_LPAREN:
            E();
            if (curr_lex.type != LEX_SEMICOLON) err(20, scan.get_count(), TL[LEX_SEMICOLON], curr_lex);
            gl();
            break;
            
        default:
            err(20, scan.get_count(), "ОПЕРАТОР", curr_lex);
            
    }
    
}

void Parser::E () {
    
    E1();
    Lex l = prev_lex;
    if (curr_lex.type == LEX_ASSIGN) {
        if (!ident) err(11, scan.get_count());
        prog.address();
        gl();
        E();
        if (prev_lex.type != l.type) err(12, scan.get_count());
        prog.put(Lex(LEX_ASSIGN));
    }
    
}

void Parser::E1 () {
    
    E2();
    if (curr_lex.type == LEX_OR && prev_lex.type != LEX_NUM) err(19, scan.get_count(), "int", Lex(LEX_OR));
    while (curr_lex.type == LEX_OR) {
        gl();
        E2();
        if (prev_lex.type != LEX_NUM) err(19, scan.get_count(), "int", Lex(LEX_OR));
        prog.put(Lex(LEX_OR));
        ident = false;
    }
    
}

void Parser::E2 () {
    
    E3();
    if (curr_lex.type == LEX_AND && prev_lex.type != LEX_NUM) err(19, scan.get_count(), "int", Lex(LEX_AND));
    while (curr_lex.type == LEX_AND) {
        gl();
        E3();
        if (prev_lex.type != LEX_NUM) err(19, scan.get_count(), "int", Lex(LEX_AND));
        prog.put(Lex(LEX_AND));
        ident = false;
    }
    
}

void Parser::E3 () {
    
    E4();
    type_of_lex e = prev_lex.type, t = curr_lex.type;
    switch (t) {
        case LEX_LSS:
        case LEX_GTR:
        case LEX_EQ:
        case LEX_NEQ:
            if (e != LEX_INT && e != LEX_STRING) err(19, scan.get_count(), "int и string", Lex(t));
            gl();
            E4();
            if (prev_lex.type != e) err(17, scan.get_count(), NULL, Lex(t));
            prog.put(Lex(t));
            prev_lex.type = LEX_NUM;
            ident = false;
            break;
        case LEX_LEQ:
        case LEX_GEQ:
            if (e != LEX_INT) err(19, scan.get_count(), "int", Lex(t));
            gl();
            E4();
            if (prev_lex.type != e) err(17, scan.get_count(), NULL, Lex(t));
            prog.put(Lex(t));
            prev_lex.type = LEX_NUM;
            ident = false;
            break;
    }
    
}

void Parser::E4 () {
    
    T();
    type_of_lex e = prev_lex.type, t;
    while (curr_lex.type == LEX_PLUS || curr_lex.type == LEX_MINUS) {
        t = curr_lex.type;
        switch (t) {
            case LEX_PLUS:
                if (e != LEX_INT && e != LEX_STRING) err(19, scan.get_count(), "int и string", Lex(t));
                break;
            case LEX_MINUS:
                if (e != LEX_INT) err(19, scan.get_count(), "int", Lex(t));
                break;
        }
        gl();
        T();
        if (prev_lex.type != e) err(17, scan.get_count(), NULL, Lex(t));
        prog.put(Lex(t));
        ident = false;
    }
    
}

void Parser::T () {
    
    M();
    type_of_lex e = prev_lex.type, t;
    while (curr_lex.type == LEX_ASTERISK || curr_lex.type == LEX_SLASH) {
        t = curr_lex.type;
        if (e != LEX_INT) err(19, scan.get_count(), "int", Lex(t));
        gl();
        M();
        if (prev_lex.type != e) err(17, scan.get_count(), NULL, Lex(t));
        prog.put(Lex(t));
        ident = false;
    }
    
}

void Parser::M () {
    
    switch (curr_lex.type) {
        case LEX_PLUS:
            gl();
            F();
            if (prev_lex.type != LEX_INT) err(19, scan.get_count(), "int", Lex(LEX_PLUS));
            ident = false;
            break;
        case LEX_MINUS:
            gl();
            F();
            if (prev_lex.type != LEX_INT) err(19, scan.get_count(), "int", Lex(LEX_MINUS));
            prog.put(Lex(LEX_UMINUS));
            ident = false;
            break;
        default:
            F();
    }
    
}

void Parser::F () {
    
    int i;
    char *s;
    switch (curr_lex.type) {
            
        case LEX_ID:
            s = newstr(curr_lex.string);
            if ((i = TID.search(s)) == -1) err(14, scan.get_count(), s);
            prog.put(Lex(LEX_ID,i));
            prev_lex = TID[i];
            ident = true;
            gl();
            if (curr_lex.type == LEX_COLON) err(13, scan.get_count(), s);
            delete[] s;
            break;
            
        case LEX_NUM:
            prog.put(Lex(LEX_INT,curr_lex.value));
            prev_lex.type = LEX_INT;
            ident = false;
            gl();
            break;
            
        case LEX_STR:
            prog.put(Lex(LEX_STRING,curr_lex.string));
            prev_lex.type = LEX_STRING;
            ident = false;
            gl();
            break;
            
            
        case LEX_NOT:
            gl();
            F();
            if (prev_lex.type != LEX_NUM) err(19, scan.get_count(), "int", Lex(LEX_NOT));
            prog.put(Lex(LEX_NOT));
            ident = false;
            break;
            
        case LEX_LPAREN:
            gl();
            E();
            ident = false;
            if (curr_lex.type != LEX_RPAREN) err(20, scan.get_count(), TL[LEX_RPAREN], curr_lex);
            gl();
            break;
            
        default: err(18, scan.get_count(), NULL, curr_lex);
    }
    
}

void Parser::fill_goto (Poliz & p) {
    
    int j, s = TGO.get_size();
    for (int i = 0; i < s; ++i) {
        if (TGO.get(i) == -1) err(15, -1, TGO.get_name(i));
        while (!TGO[i].is_empty()) {
            j = TGO[i].pop();
            p[j].value = TGO.get(i);
        }
    }
    
}

void execute (Poliz & prog) {
    Stack <Lex, SIZE_OF_POLIZ> args;
    Lex l1, l2;
    int i = 0, j, x, y, size = prog.get_top();
    char buf[MAX_STRING_SIZE];
    
    cout << "Выполнение..." << endl;
    while (i < size) {
        switch(prog[i].type) {
                
            case LEX_INT:
            case LEX_STRING:
            case POLIZ_ADDRESS:
                args.push(prog[i]);
                break;
                
            case LEX_ID:
                j = prog[i].value;
                if (!TID.get(j)) err(16, -1, TID.get_name(j));
                args.push(TID[j]);
                break;
                
            case LEX_NOT:
                l1 = args.pop();
                j = !l1.value;
                args.push(Lex(LEX_NUM,j));
                break;
                
            case LEX_OR:
                l2 = args.pop();
                l1 = args.pop();
                j = (l1.value || l2.value);
                args.push(Lex(LEX_NUM,j));
                break;
                
            case LEX_AND:
                l2 = args.pop();
                l1 = args.pop();
                j = (l1.value && l2.value);
                args.push(Lex(LEX_NUM,j));
                break;
                
            case LEX_PLUS:
                l2 = args.pop();
                l1 = args.pop();
                if (l1.type == LEX_STRING) args.push(Lex(LEX_STRING, sumstr(l1.string, l2.string)));
                else args.push(Lex(LEX_INT, l1.value + l2.value));
                break;
                
            case LEX_MINUS:
                l2 = args.pop();
                l1 = args.pop();
                args.push(Lex(LEX_INT, l1.value - l2.value));
                break;
                
            case LEX_UMINUS:
                l1 = args.pop();
                j = -l1.value;
                args.push(Lex(LEX_INT,j));
                break;
                
            case LEX_ASTERISK:
                l2 = args.pop();
                l1 = args.pop();
                args.push(Lex(LEX_INT, l1.value * l2.value));
                break;
                
            case LEX_SLASH:
                l2 = args.pop();
                l1 = args.pop();
                if (!l2.value) err(5);
                args.push(Lex(LEX_INT, l1.value / l2.value));
                break;
                
            case LEX_EQ:
                l2 = args.pop();
                l1 = args.pop();
                if (l1.type == LEX_STRING) args.push(Lex(LEX_NUM, !strcmp(l1.string, l2.string)));
                else args.push(Lex(LEX_NUM, l1.value == l2.value));
                break;
                
            case LEX_NEQ:
                l2 = args.pop();
                l1 = args.pop();
                if (l1.type == LEX_STRING) args.push(Lex(LEX_NUM, strcmp(l1.string, l2.string)));
                else args.push(Lex(LEX_NUM, l1.value != l2.value));
                break;
                
            case LEX_LSS:
                l2 = args.pop();
                l1 = args.pop();
                if (l1.type == LEX_STRING) args.push(Lex(LEX_NUM, strcmp(l1.string, l2.string) < 0 ));
                else args.push(Lex(LEX_NUM, l1.value < l2.value));
                break;
                
            case LEX_GTR:
                l2 = args.pop();
                l1 = args.pop();
                if (l1.type == LEX_STRING) args.push(Lex(LEX_NUM, strcmp(l1.string, l2.string) > 0 ));
                else args.push(Lex(LEX_NUM, l1.value > l2.value));
                break;
                
            case LEX_LEQ:
                l2 = args.pop();
                l1 = args.pop();
                args.push(Lex(LEX_NUM, l1.value <= l2.value));
                break;
                
            case LEX_GEQ:
                l2 = args.pop();
                l1 = args.pop();
                args.push(Lex(LEX_NUM, l1.value >= l2.value));
                break;
                
            case LEX_WRITE:
                l1 = args.pop();
                switch(l1.type) {
                    case LEX_INT: cout << l1.value; break;
                    case LEX_STRING: cout << l1.string; break;
                }
                args.reset();
                break;
                
            case LEX_READ:
                l1 = args.pop();
                j = l1.value;
                cout << endl;
                switch(TID[j].type) {
                    case LEX_INT:
                        cout << "Введите значение для " << TID.get_name(j) << " (int): ";
                        cin >> x;
                        TID[j].value = x;
                        TID.set(j,true);
                        break;
                    case LEX_STRING:
                        cout << "Введите значение для " << TID.get_name(j) << " (string): ";
                        buf[x = 0] = getchar(); y = 0;
                        while ((buf[x] != '\n') || y) {
                            ++x;
                            if (x >= MAX_STRING_SIZE) err(1);
                            buf[x] = getchar();
                            y = 0;
                            if (buf[x-1] == '\\')
                                switch (buf[x]) {
                                    case 'n': buf[--x] = '\n'; y = 1; break;
                                    case 'r': buf[--x] = '\r'; break;
                                    case 't': buf[--x] = '\t';
                                }
                        }
                        buf[x] = '\0';
                        if (TID.get(j)) delete[] TID[j].string;
                        TID[j].string = newstr(buf);
                        TID.set(j,true);
                        break;
                }
                args.reset();
                break;
                
            case POLIZ_GO:
                i = prog[i].value - 1;
                args.reset();
                break;
                
            case POLIZ_FGO:
                l1 = args.pop();
                if (!l1.value) i = prog[i].value - 1;
                args.reset();
                break;
                
            case LEX_ASSIGN:
                l2 = args.pop();
                l1 = args.pop();
                args.push(l2);
                j = l1.value;
                switch(TID[j].type) {
                    case LEX_INT:
                        TID[j].value = l2.value;
                        TID.set(j,true);
                        break;
                    case LEX_STRING:
                        if (TID.get(j)) delete[] TID[j].string;
                        TID[j].string = newstr(l2.string);
                        TID.set(j,true);
                        break;
                }
                break;
                
        }
        ++i;
    }
    cout << endl << "Успешно завершено!" << endl;
}


/*  =========================================================================================================  */

int main(int argc, char **argv) {
    
    if (argc < 2) {
        cout<< argc<< endl;
        cout << "Пример вызова: " << argv[0] << " program.txt" << endl;
        return 0;
    }
    
    try {
        Parser PAR(sumstr(CUR_DIR, argv[1]));
        PAR.analyze();
        execute(PAR.prog);
    }
    catch (...) { cout << endl << "Аварийно завершено" << endl; }

}