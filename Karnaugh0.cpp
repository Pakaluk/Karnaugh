#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <cmath>

/*
This program simplifies boolean functions using a Karnaugh map.
To compile it, try:

make Karnaugh
or
g++ Karnaugh.cpp -o Kmap
then
./Kmap

*/

/*
To parse a boolean function and return an easy to evaluate postfix expression.

Operators:
(, ), +, *, '

Precedence:
( = ), ', *, +

ASCII Code:
' -> 39
( -> 40
) -> 41
* -> 42
+ -> 43

Two adjacent alphabetical characters XY will be parsed as X*Y
*/

std::string reverse (std::string s) {
    std::string t = "";
    for (int i = 0; i < s.length() + t.length(); i++) {
        t += s.back();
        s.pop_back();
    }
    return t;
}
std::string merge(std::string a, std::string b){
    int as = a.length(), bs = b.length();
    std::string temp(as + bs, ' ');
    int counter[] = {0, 0};
    for(int i = 0; i < as + bs; i++){
        if(counter[0] == as){
            temp[i] = b[counter[1]++];
            continue;
        }
        else if(counter[1] == bs){
            temp[i] = a[counter[0]++];
            continue;
        }
        temp[i] = a[counter[0]] < b[counter[1]] ? a[counter[0]++] : b[counter[1]++];
    }
    return temp;
}
std::string sort(std::string a){
    int s = a.length();
    if(s < 2)
        return a;
    if(s == 2){
        if(a[0] > a[1]){
            std::string t = "  ";
            t[0] = a[1];
            t[1] = a[0];
            return t;
        }
        return a;
    }
    std::string x = a.substr(0, s / 2),
                y = a.substr(s / 2);
    /*char *x = new char[s / 2];
    char *y = new char[s & 1 ? s / 2 + 1 : s / 2];
    for(int i = 0; i < s; i++)
        i < s / 2 ? x[i] = a[i] : y[i - s / 2] = a[i];*/
    return merge(sort(x), sort(y));
}
std::string palette (std::string function) {
    std::string s = "";
    for (int i = 0; i < function.length(); i++)
        if (std::isalpha(function[i]) && (s.find_first_of(function[i]) == -1))
            s += function[i];
    s = sort(s);
    return s;
}
std::string parse (std::string infix) {
    infix = '(' + infix + ')';
    std::string postfix = "";
    std::string ops = "";
    ops.reserve(infix.length() >> 1);
    char c, l = '(';
    for (int i = 0; i < infix.length(); i++) {
        c = infix[i];
        if (c == '\'')
            postfix += c;
        else if (std::isalnum(c)) {
            if (std::isalnum(l) || l == ')' || l == '\'') {
                while (ops.back() <= '*' && ops.back() != '(') {
                    postfix += ops.back();
                    ops.pop_back();
                }
                ops.push_back('*');
            }
            postfix += c;
        }
        else if (c == '(') {
            if (std::isalnum(l) || l == ')' || l == '\'') {
                while (ops.back() <= '*' && ops.back() != '(') {
                    postfix += ops.back();
                    ops.pop_back();
                }
                ops.push_back('*');
            }
            ops.push_back(c);
        }
        else if (c == '*' || c == '+') {
            while (ops.back() <= c && ops.back() != '(') {
                postfix += ops.back();
                ops.pop_back();
            }
            ops.push_back(c);
        }
        else if (c == ')') {
            while (ops.back() > c) {
                postfix += ops.back();
                ops.pop_back();
            }
            ops.pop_back();
        }
        else
            continue;
        l = c;
    }
    return postfix;
}
char evaluate (std::string postfix, std::string vars, int value) {
    int ops = 0;
    for (int i = 0; i < postfix.length(); i++) {
        char c = postfix[i];
        if (std::isalpha(c))
            ops = (ops << 1) | ((value >> vars.find_first_of(c)) & 1);
        else if (std::isdigit(c))
            ops = (ops << 1) | static_cast<bool>(c - 48);
        else if (c == '\'')
            ops = ops ^ 1;
        else if (c == '*')
            ops = ((ops >> 2) << 1) | (((ops >> 1) & 1) & (ops & 1));
        else if (c == '+')
            ops = (ops >> 1) | (ops & 1);
    }
    return ops;
}

struct box {
    int a, b, score;
    box (int x, int y) {
        a = x;
        b = y;
        score = 0;
    }
    box () {
        a = 0;
        b = 0;
        score = 0;
    }
    bool operator == (box rect) {
        return (a ^ b) == (rect.a ^ rect.b);
    }
};

struct Kmap {
    char * map;
    int size;
    Kmap (int n) {
        size = n;
        map = new char[1 << n];
    }
    Kmap (std::string f) {
        std::string comp = parse(f);
        std::string vars = palette(f);
        size = vars.length();
        map = new char[1 << size];
        for (int i = 0; i < (1 << size); i++)
            map[i] = evaluate(comp, vars, i);
    }
    void unvisit () {
        for (int i = 0; i < (1 << size); i++)
            map[i] = map[i] & 1;
    }
    void set (int index, char value) {
        map[index] = value;
    }
    char at (int index) {
        return map[index];
    }
    char neighbor (int index, int n) {
        return at(index ^ (1 << n));
    }
    void fill (std::string function) {
        std::string comp = parse(function);
        std::string vars = palette(function);
        int n = vars.length();
        for (int i = 0; i < n; i++)
            map[i] = evaluate(comp, vars, i);
    }
};

int score (Kmap map, box * range) {

    range -> score = 0;
    int c = 0, m = range -> a ^ range -> b;
    int * t = new int[map.size];

    for (int i = 0; i < map.size; i++)
        if ((m >> i) & 1)
            t[c++] = i;

    for (int i = 0; i < (1 << c); i++) {
        int o = range -> a;
        for (int j = 0; j < c; j++)
            o = o ^ (((i >> j) & 1) << t[j]);

        if ((map.at(o) & 1) ^ 1) {
            range -> score = 0;
            break;
        }

        range -> score += ((map.at(o) >> 1) & 1) ^ 1;
    }

    delete [] t;
    return range -> score;
}
std::string implicant (box * range, std::string vars) {
    std::string imp = "";
    int m = range -> a ^ range -> b;
    for (int i = 0; i < vars.length(); i++) {
        if (((m >> i) & 1) ^ 1) {
            imp += vars[i];
            if ((m >> i) & 1)
                imp += '\'';
        }
    }
    return imp.length() > 0 ? imp : "1";
}
box * search (Kmap map, box * range) {
    /*
    Optimization methods in consideration to prevent redundant box checks:
    1. Optimization be damned!               | Good
    2. Store box scores in each K-map place  | Doesn't work
    3. Pass box scores around with boxes     | Done, not perfect
    4. Keep central list of boxes and scores | Best?
    5. Use my conjectures to create an efficient iterative algorithm!!!
    */

    // find out how many neighboring boxes there are
    int m = range -> a ^ range -> b, n = 0;
    for (int i = 0; i < map.size; i++) {
        n += ((m >> i) & 1) ^ 1;
    }

    // for each neighboring box, test and run again
    //int * box = new int[2];
    box * rect = new box();
    for (int i = 0; i < map.size; i++) {
        if ((m >> i) & 1) continue;

        rect -> a = range -> a ^ (1 << i);
        rect -> b = range -> b ^ (1 << i);

        if (score(map, rect)) {
            rect -> a = range -> a;
            //box = search(map, box);
        }




    }

    // return largest box
    return rect;
}



std::string sop (std::string function) {
    // compile input, declare variables
    std::string comp = parse(function);
    std::string vars = palette(function);
    int n = vars.length();
    std::string simplified = "";

    // build a Karnaugh map
    Kmap map(function);

    // allocate array for boxes

    // for each 1 in map, find prime implicant, add it to new function


    // scan simplified for unnecessary terms, remove them

    // return simplified
    return simplified;
}
std::string pos (Kmap map, std::string vars) {
    return "";
}
bool equal(std::string a, std::string b) {
    std::string pala = palette(a), palb = palette(b);
    if (pala.compare(palb) != 0)
        return 0;
    std::string posta = parse(a), postb = parse(b);
    for (int i = 0; i < (1 << pala.length()); i++)
        if (evaluate(posta, pala, i) != evaluate(postb, palb, i))
            return 0;
    return 1;
}
void table (std::string function) {
    std::string parsed = parse(function);
    std::string vars = palette(function);
    for (int i = 0; i < vars.length(); i++)
        std::cout << vars[i] << ' ';
    std::cout << "| F " << std::endl;
    for (int i = 0; i < vars.length(); i++)
        std::cout << "--";
    std::cout << "+---" << std::endl;
    vars = reverse(vars);
    for (int i = 0; i < (1 << vars.length()); i++) {
        for (int j = 0; j < vars.length(); j++)
            std::cout << ((i >> (vars.length() - j - 1)) & 1) << ' ';
        std::cout << "| " << (evaluate(parsed, vars, i) & 1) << std::endl;
    }
}

int main () {
    /*int n = 2;
    Kmap test(n);
    test.map[0] = 1;
    test.map[2] = 1;
    for (int i = 0; i < (1 << n); i++) {
        std::cout << test.at(i);
    }
    std::cout << std::endl;
    std::cout << test.at(0b00) << test.neighbor(0b00, 1) << std::endl;
    std::cout << test.at(0b01) << test.neighbor(0b01, 1) << std::endl;*/

    //int test = 2;
    //std::string a = "A'B + AB'";
    //a = parse(a);
    //b = parse(b);
    //std::string post = parse(in);

    std::string F = "(A + B)(B + C)";
    table(F);

    Kmap test(F);

    box * b = new box(0b100, 0b111);
    std::cout << score(test, b) << std::endl;
    std::cout << implicant(b, "ABC") << std::endl;

    //std::cout << in + '\n' << post + '\n' << palette(in) << std::endl;
    //std::cout << equal(a, b) << std::endl;
    //std::cout << evaluate(a, palette(a), test) << evaluate(b, palette(b), test) << std::endl;

    //std::cout << "---------------" << std::endl;
    return 0;
}
