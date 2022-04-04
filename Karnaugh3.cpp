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
    std::string x = a.substr(0, s / 2), y = a.substr(s / 2);
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
    char size;
    box * next;
    box (int x, int y) {
        a = x;
        b = y;
        size = 0;
        next = nullptr;
    }
    box (int x, int y, char s) {
        a = x;
        b = y;
        size = s;
        next = nullptr;
    }
    box (int x, int y, char s, box * o) {
        a = x;
        b = y;
        size = s;
        next = o;
    }
    box () {
        a = 0;
        b = 0;
        size = 0;
        next = nullptr;
    }
    bool insert (box * imp) {
        box * temp = this;
        while (temp->next != nullptr) {
            if (((temp->a) ^ (temp->b)) == ((imp->a) ^ (imp->b)))
                return 0;
            temp = temp->next;
        }
        if (((temp->a) ^ (temp->b)) == ((imp->a) ^ (imp->b)))
            return 0;
        temp->next = imp;
        return 1;
    }
    static void remove (box * imp) {
        if (imp -> next != nullptr)
            box::remove(imp -> next);
        delete imp;
    }
    static int length (box * imp) {
        if (imp == nullptr)
            return 0;
        return box::length(imp -> next) + 1;
    }
};
struct Kmap {
    short * map;
    int size;
    Kmap (int n) {
        size = n;
        map = new short[1 << n];
    }
    Kmap (std::string f) {
        std::string comp = parse(f);
        std::string vars = palette(f);
        size = vars.length();
        map = new short[1 << size];
        for (int i = 0; i < (1 << size); i++)
            map[i] = evaluate(comp, vars, i);
    }
    Kmap (std::string t, std::string v) {
        size = 0;
        for (int i = t.length(); i > 1; i >>= 1)
            size++;
        map = new short[1 << size];
        for (int i = 0; i < t.length(); i++) {
            if (t[i] == 88 || t[i] == 120)
                map[i] = 2;
            else
                map[i] = t[i] - 48;
        }
    }
    short & operator [] (int i) {
        return map[i];
    }
    void unvisit () {
        for (int i = 0; i < (1 << size); i++)
            map[i] = map[i] & 127;
    }
};

char score (Kmap map, box range) {
    int c = 0, m = range.a ^ range.b;
    int t[map.size];

    for (int i = 0; i < map.size; i++)
        if ((m >> i) & 1)
            t[c++] = i;

    for (int i = 0; i < (1 << c); i++) {
        int o = range.a;
        for (int j = 0; j < c; j++)
            o = o ^ (((i >> j) & 1) << t[j]);

        if (!(map[o] & 3))
            return 0;
    }
    return c + 1;
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

        if (!(((map[o] >> 2) & 31) >> 1))
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

        map[o] = map[o] + (1 << 2);
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

        if ((map[o] >> 2) & 63)
            map[o] = map[o] - (1 << 2);
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
box * search (Kmap map, box * start) {
    /*
    Optimization methods in consideration to prevent redundant box checks:
    1. Optimization be damned!               | Good
    2. Store box scores in each K-map place  | Done
    3. Pass box scores around with boxes     | Done
    4. Keep central list of boxes and scores | Nope
    5. All of the above?                     | Kind (of)
    */

    // perform a kind of depth-first search
    // (probably more like backtracking)
    int a = start -> a, b = start -> b;
    box * primes = start;
    for (int i = 0; i < map.size; i++) {
        if ((((a ^ b) >> i) & 1) ^ 1) {
            // Generate new box
            box * temp = new box(a ^ (1 << i), b ^ (1 << i));

            // Check its current score
            short p = map[temp -> b] >> 7;
            char q;
            bool t = p & 1;
            if (t)
                p >>= 1;
            else {
                // combine scores
                p = score(map, *temp);
                q = p + 1;
            }

            if (!p) continue;

            delete temp;
            temp = new box(a, b ^ (1 << i), t ? p : q);

            map[temp -> b] = (temp -> size << 8) | 128 | map[temp -> b];

            temp = search(map, temp);

            // Compare with current best box, if better, replace
            if (temp -> size == primes -> size) {
                temp -> next = primes;
                primes = temp;
            }
            else if(temp -> size > primes -> size) {
                box::remove(primes);
                primes = temp;
            }
        }
    }

    // "cover" the elements of best box
    //cover(map, best);
    //map[best.b] = (best.score << 8) | 4 | map[best.b];

    // return best scoring box
    return primes;
}

std::string sop (Kmap map, std::string vars) {
    // declare variables
    int n = vars.length();

    // initiate linked list for implicants
    box * implicants = nullptr;

    // for each 1 in map, find prime implicant, add it to new function
    for (int i = 0; i < (1 << n); i++) {
        if (!(map[i] & 1) || (map[i] & 252))//(!(map[i] & 1) || (map[i] & 124))
            continue;
        box * imps = search(map, new box(i, i, 1));


        do {

            box * temp = imps;
            imps = imps->next;
            temp->next = nullptr;

            if (implicants == nullptr) {
                implicants = temp;
                cover(map, *temp);
            }
            else {
                if (!(implicants->insert(temp)))
                    delete temp;
                else
                    cover(map, *temp);
            }

        } while (imps != nullptr);

        /*if (!(implicants->insert(imps)))
            delete imps;
        else
            cover(map, *imps);*/

        map.unvisit();
    }

    // fill and return simplified
    // Create new loop that fills simplified without redundant boxes.
    // Boxes need to be deleted anyway, so delete boxes along the way.
    //
    box * temp = implicants;
    if (temp != nullptr) {
        std::string simplified = "";

        do {
            box * temp = implicants;
            implicants = implicants->next;
            temp->next = nullptr;

            if (necessary(map, *temp)) {
                simplified += implicant(*temp, vars) + " + ";
            }
            else {
                uncover(map, *temp);
            }
            delete temp;

        } while (implicants != nullptr);

        return simplified.substr(0, simplified.length() - 3);

        /*while (temp -> next != nullptr) {
            if (necessary(map, *temp)) {
                std::string s = implicant(*temp, vars);
                simplified += s + " + ";
            }
            temp = temp -> next;
        }
        return simplified + implicant(*temp, vars);*/
    }
    else
        return "0";
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

    std::string F = "AB + C";
    // Problematic:  0111111000111010 (No longer)
    std::string s = "0100100000111111", vars = "ABCD";

    Kmap map = generate(s);
    //table(F);
    table(map, vars);
    //std::cout << map[1] << std::endl;
    //std::cout << evaluate(parse(F), palette(F), 1) << std::endl;

    std::cout << sop(map, vars) << std::endl;

    return 0;
}
