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
std::string bin(int n) {
    std::string s = "";
    for (int i = 0; i < 32; i++)
        s += (n >> i & 1) + 48;
    return reverse(s);
}
std::string bin(long n) {
    std::string s = "";
    for (int i = 0; i < 64; i++)
        s += (n >> i & 1) + 48;
    return reverse(s);
}
std::string bin(char n) {
    std::string s = "";
    for (int i = 0; i < 8; i++)
        s += (n >> i & 1) + 48;
    return reverse(s);
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
bool evaluate (std::string postfix, std::string vars, int value) {
    long ops = 0;
    for (int i = 0; i < postfix.length(); i++) {
        char c = postfix[i];
        if (std::isalpha(c))
            ops = (ops << 1) | ((value >> vars.find_first_of(c)) & 1);
        else if (std::isdigit(c))
            ops = (ops << 1) | static_cast<bool>(c - 48);
        else if (c == '\'')
            ops = ops ^ 1;
        else if (c == '*')
            ops = ((ops >> 2) << 1) | ((ops >> 1) & (ops & 1)); /* Note: May Be Problematic */
        else if (c == '+')
            ops = (ops >> 1) | (ops & 1);
    }
    return ops & 1;
}

struct box {
    int a, b;
    long score;
    box (int x, int y) {
        a = x;
        b = y;
        score = 0;
    }
    box (int x, int y, long s) {
        a = x;
        b = y;
        score = s;
    }
    box () {
        a = 0;
        b = 0;
        score = 0;
    }
    bool operator == (box rect) {
        return (a ^ b) == (rect.a ^ rect.b);
    }
    void operator = (box r) {
        a = r.a;
        b = r.b;
        score = r.score;
    }
};
struct Kmap {
    long * map;
    int size;
    Kmap (int n) {
        size = n;
        map = new long[1 << n];
    }
    Kmap (std::string f) {
        std::string comp = parse(f);
        std::string vars = palette(f);
        size = vars.length();
        map = new long[1 << size];
        for (int i = 0; i < (1 << size); i++)
            map[i] = evaluate(comp, vars, i);
    }
    Kmap (std::string t, std::string v) {
        size = 0;
        for (int i = t.length(); i > 1; i >>= 1)
            size++;
        map = new long[1 << size];
        for (int i = 0; i < t.length(); i++) {
            if (t[i] == 88 || t[i] == 120)
                map[i] = 2;
            else
                map[i] = t[i] - 48;
        }
    }
    long & operator [] (int i) {
        return map[i];
    }
    void reset () {
        for (int i = 0; i < (1 << size); i++)
            map[i] = map[i] & 251;
    }
};

long score (Kmap map, box range) {
    range.score = 0;
    int c = 0, m = range.a ^ range.b;
    int t[map.size];

    for (int i = 0; i < map.size; i++)
        if ((m >> i) & 1)
            t[c++] = i;

    for (int i = 0; i < (1 << c); i++) {
        int o = range.a;
        for (int j = 0; j < c; j++)
            o = o ^ (((i >> j) & 1) << t[j]);

        if (!(map[o] & 3)) {
            range.score = 0;
            return range.score;
        }

        range.score += ((map[o] >> 3) & 31) + ((map[o] >> 1) & 1);
    }
    range.score = (range.score << 5) | (c + 1);
    return range.score;
}
bool necessary (Kmap map, box range) {
    int c = 0, m = range.a ^ range.b;
    int t[map.size];

    for (int i = 0; i < map.size; i++)
        if ((m >> i) & 1)
            t[c++] = i;

    for (int i = 0; i < (1 << c); i++) {
        int o = range.a;
        for (int j = 0; j < c; j++)
            o = o ^ (((i >> j) & 1) << t[j]);

        if (!(((map[o] >> 3) & 31) >> 1))
            return 1;
    }
    return 0;
}
void cover (Kmap map, box range) {
    int c = 0, m = range.a ^ range.b;
    int t[map.size];

    for (int i = 0; i < map.size; i++)
        if ((m >> i) & 1)
            t[c++] = i;

    for (int i = 0; i < (1 << c); i++) {
        int o = range.a;
        for (int j = 0; j < c; j++)
            o = o ^ (((i >> j) & 1) << t[j]);

        map[o] = map[o] + (1 << 3);
    }
}
void uncover (Kmap map, box range) {
    int c = 0, m = range.a ^ range.b;
    int t[map.size];// = new int[map.size];

    for (int i = 0; i < map.size; i++)
        if ((m >> i) & 1)
            t[c++] = i;

    for (int i = 0; i < (1 << c); i++) {
        int o = range.a;
        for (int j = 0; j < c; j++)
            o = o ^ (((i >> j) & 1) << t[j]);

        if ((map[o] >> 3) & 63)
            map[o] = map[o] - (1 << 3);
    }
}
std::string implicant (box range, std::string vars) {
    std::string imp = "";
    int m = range.a ^ range.b;
    for (int i = vars.length() - 1; i >= 0; i--) {
        if (((m >> i) & 1) ^ 1) {
            imp += vars[vars.length() - i - 1];
            if (((range.a >> i) & 1) ^ 1)
                imp += '\'';
        }
    }
    return imp.length() > 0 ? imp : "1";
}
bool greater_than (long a, long b) {
    if ((a & 31) > (b & 31))
        return 1;
    else if ((a & 31) == (b & 31))
        if (a < b)
            return 1;
    return 0;
}
box search (Kmap map, box range) {
    /*
    Optimization methods in consideration to prevent redundant box checks:
    1. Optimization be damned!               | Good
    2. Store box scores in each K-map place  | Done
    3. Pass box scores around with boxes     | Done
    4. Keep central list of boxes and scores | Nope
    5. All of the above?                     | Kind (of)
    */

    // perform a special kind of depth-first search
    box best = range;
    for (int i = 0; i < map.size; i++) {
        if ((((range.a ^ range.b) >> i) & 1) ^ 1) {
            // Generate new box
            box temp(range.a ^ (1 << i), range.b ^ (1 << i));

            // Check its current score
            long p = map[temp.b] >> 2, q;
            bool t = p & 1;
            if (t)
                p >>= 6;
            else {
                // combine scores
                p = score(map, temp);
                q = p + ((range.score >> 5) << 5) + 1;
            }

            if (!p) continue;

            temp = box(range.a, temp.b, t ? p : q);
            map[temp.b] = (temp.score << 8) | 4 | map[temp.b];
            temp = search(map, temp);

            // Compare with current best box, if better, replace
            if (greater_than(temp.score, best.score))
                best = temp;
        }
    }

    // return best scoring box
    return best;
}

std::string sop (Kmap map, std::string vars) {
    // declare variables
    int n = vars.length();
    std::string simplified = "";

    // allocate array for boxes
    int m = 0;
    box boxes[1 << (n - 1)];

    // for each 1 in map, find prime implicant, add it to new function
    for (int i = 0; i < (1 << n); i++) {
        if (!(map[i] & 1) || ((map[i] >> 3) & 31))
            continue;
        boxes[m] = search(map, box(i, i, 1));
        cover(map, boxes[m]);
        m++;
        map.reset();
    }

    // scan boxes for unnecessary terms
    for (int i = 0; i < m; i++) {
        if (!necessary(map, boxes[i])) {
            boxes[i].score = 0;
        }
    }

    // remove unnecessary terms
    int back = 0;
    for (int i = 0; i < m; i++) {
        if (!boxes[i].score) {
            back++;
            continue;
        }
        boxes[i - back] = boxes[i];
    }
    m -= back;

    // fill and return simplified
    for (int i = 0; i < m - 1; i++) {
        std::string s = implicant(boxes[i], vars);
        simplified += s + " + ";
    }
    return simplified + implicant(boxes[m - 1], vars);
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
void table (Kmap map, std::string vars) {
    for (int i = 0; i < vars.length(); i++)
        std::cout << vars[i] << ' ';
    std::cout << "| F " << std::endl;
    for (int i = 0; i < map.size; i++)
        std::cout << "--";
    std::cout << "+---" << std::endl;
    for (int i = 0; i < (1 << map.size); i++) {
        for (int j = 0; j < map.size; j++)
            std::cout << ((i >> (map.size - j - 1)) & 1) << ' ';
        if ((map[i] & 3) == 2)
            std::cout << "| " << 'X' << std::endl;
        else
            std::cout << "| " << (map[i] & 1) << std::endl;
    }
}
Kmap generate (std::string s) {
    if (!s.length())
        return Kmap(0);
    int n = 0;
    for (int i = s.length(); i > 1; i >>= 1)
        n++;
    if ((1 << n) != s.length())
        return Kmap(0);
    Kmap t(n);
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == 88 || s[i] == 120)
            t[i] = 2;
        else
            t[i] = s[i] - 48;
    }
    return t;
}

int main () {

    std::string F = "A'BC + AB'C' + AB'C + ABC' + ABCD";
    // Problematic:  0111111000111010 Correct Result: BD' + B'C + A'C'D
    //std::string s = "X111111011X00101", vars = "ABCD";
    std::string s = "1xxxxxxx", vars = "ABC";

    Kmap map(s, vars);
    table(map, vars);

    //std::string s = "010x01x10100x100";//"010101x10100xx00";
    std::cout << sop(map, vars) << std::endl;
    std::cout << evaluate(parse(F), palette(F), 1) << std::endl;

    /*box t(3, 3, 1);
    t = search(map, t);
    std::cout << "(" << t.a << ", " << t.b << ")" << std::endl;
    std::cout << implicant(t, "ABC") << std::endl;*/

    /*Kmap map(F);
    box t(3, 3, 1);
    box u(5, 5, 1);

    table(map, "ABC");
    t = search(map, t);
    map.reset();
    u = search(map, u);

    std::cout << "(" << t.a << ", " << t.b << "), (" << u.a << ", " << u.b << ")" << std::endl;
    std::cout << implicant(t, "ABC") << " + " << implicant(u, "ABC") << std::endl;*/

    return 0;
}
