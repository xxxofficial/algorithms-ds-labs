#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <cctype>
#include <sstream>
#include <vector>

using namespace std;

struct Node {
    string value;
    Node* left;
    Node* right;

    Node(string val) : value(val), left(nullptr), right(nullptr) {}
};

int precedence(const string& op) {
    if (op == "^") return 3;
    if (op == "*" || op == "/") return 2;
    if (op == "+" || op == "-") return 1;
    return 0;
}

bool isRightAssociative(const string& op) {
    return op == "^";
}

bool isOperator(const string& token) {
    return token == "+" || token == "-" ||
        token == "*" || token == "/" ||
        token == "^";
}

vector<string> tokenize(const string& expr) {
    vector<string> tokens;

    for (size_t i = 0; i < expr.size();) {
        if (isspace(expr[i])) {
            i++;
            continue;
        }

        if (isalpha(expr[i]) || isdigit(expr[i])) {
            string id;
            while (i < expr.size() && (isalnum(expr[i]) || expr[i] == '_')) {
                id += expr[i++];
            }
            tokens.push_back(id);
        }
        else {
            tokens.push_back(string(1, expr[i]));
            i++;
        }
    }

    return tokens;
}

Node* buildTree(const string& expr) {
    vector<string> tokens = tokenize(expr);
    stack<Node*> output;
    stack<string> ops;

    for (size_t i = 0; i < tokens.size(); ++i) {
        string token = tokens[i];

        if (isalnum(token[0])) {
            output.push(new Node(token));
        }
        else if (isOperator(token)) {

            if (i == 0 || tokens[i - 1] == "(" || isOperator(tokens[i - 1])) {
                output.push(new Node("0"));
            }

            while (!ops.empty() && ops.top() != "(" &&
                (precedence(ops.top()) > precedence(token) ||
                    (precedence(ops.top()) == precedence(token) &&
                        !isRightAssociative(token))))
            {
                string op = ops.top();
                ops.pop();

                Node* right = output.top(); output.pop();
                Node* left = output.top(); output.pop();

                Node* newNode = new Node(op);
                newNode->left = left;
                newNode->right = right;
                output.push(newNode);
            }

            ops.push(token);
        }
        else if (token == "(") {
            ops.push(token);
        }
        else if (token == ")") {

            while (!ops.empty() && ops.top() != "(") {
                string op = ops.top();
                ops.pop();

                Node* right = output.top(); output.pop();
                Node* left = output.top(); output.pop();

                Node* newNode = new Node(op);
                newNode->left = left;
                newNode->right = right;
                output.push(newNode);
            }

            if (!ops.empty()) ops.pop(); 
        }
    }

    while (!ops.empty()) {
        string op = ops.top();
        ops.pop();

        Node* right = output.top(); output.pop();
        Node* left = output.top(); output.pop();

        Node* newNode = new Node(op);
        newNode->left = left;
        newNode->right = right;
        output.push(newNode);
    }

    return output.top();
}

void printTree(Node* node, const string& prefix = "", bool isLeft = false) {
    if (!node) return;

    cout << prefix;

    if (!prefix.empty())
        cout << (isLeft ? "|-- [L] " : "+-- [R] ");

    cout << node->value << "\n";

    bool hasLeft = node->left != nullptr;
    bool hasRight = node->right != nullptr;

    if (!hasLeft && !hasRight) return;

    string newPrefix = prefix + (isLeft ? "|   " : "    ");

    printTree(node->left, newPrefix, true);
    printTree(node->right, newPrefix, false);
}

void printTreeIndented(Node* node, int level = 0) {
    if (!node) return;

    // Выводим отступ: level точек
    for (int i = 0; i < level; ++i) {
        cout << ".";
    }
    cout << node->value << "\n";

    // Рекурсивный обход: сначала левое поддерево, потом правое
    // (для бинарного дерева — стандартный прямой обход)
    if (node->left)  printTreeIndented(node->left, level + 1);
    if (node->right) printTreeIndented(node->right, level + 1);
}

void freeTree(Node* root) {
    if (!root) return;

    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

int main() {
    system("chcp 65001");
    system("cls");
    setlocale(LC_ALL, "Russian");

    while (true) {
        cout << "\nВведите имя файла с выражением (или 'exit' для выхода): ";
        string filename;
        getline(cin, filename);

        if (filename == "exit") break;

        ifstream fin(filename);
        if (!fin.is_open()) {
            cout << "Не удалось открыть файл!\n";
            continue;
        }

        string expression;
        getline(fin, expression);
        fin.close();

        cout << "Выражение: " << expression << "\n";

        Node* root = buildTree(expression);

        cout << "\nДерево выражения:\n";
        printTreeIndented(root);

        freeTree(root);
    }

    return 0;
}
