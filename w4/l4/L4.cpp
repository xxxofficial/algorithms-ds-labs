#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <cctype>
#include <clocale>

using namespace std;

void trim(string& s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char c) {return !isspace(c); }));
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char c) {return !isspace(c); }).base(), s.end());
}

class Graph {
private:
    map<string, vector<string>> adj;
    map<string, vector<string>> rev_adj;
    set<string> vertices;

    void addEdgeInternal(const string& u, const string& v) {
        adj[u].push_back(v);
        rev_adj[v].push_back(u);
        vertices.insert(u);
        vertices.insert(v);
    }

public:
    void addEdge(const string& u, const string& v) {
        addEdgeInternal(u, v);
    }

    void addVertex(const string& v) {
        vertices.insert(v);
        if (adj.find(v) == adj.end()) adj[v] = {};
        if (rev_adj.find(v) == rev_adj.end()) rev_adj[v] = {};
    }

    bool loadFromFile(const string& filename) {
        ifstream fin(filename);
        if (!fin.is_open()) return false;
        string line;
        while (getline(fin, line)) {
            trim(line);
            if (line.empty()) continue;
            vector<string> tokens;
            size_t pos = 0, start = 0;
            while ((pos = line.find(' ', start)) != string::npos) {
                string tok = line.substr(start, pos - start);
                trim(tok);
                if (!tok.empty()) tokens.push_back(tok);
                start = pos + 1;
            }
            string last = line.substr(start);
            trim(last);
            if (!last.empty()) tokens.push_back(last);
            if (tokens.size() == 1) addVertex(tokens[0]);
            else if (tokens.size() == 2) addEdge(tokens[0], tokens[1]);
        }
        fin.close();
        for (const string& v : vertices) {
            if (adj.find(v) == adj.end()) adj[v] = {};
            if (rev_adj.find(v) == rev_adj.end()) rev_adj[v] = {};
        }
        return true;
    }

    void dfs1(const string& v, map<string, int>& color, stack<string>& finishStack) {
        color[v] = 1;
        for (const string& u : adj[v])
            if (color[u] == 0) dfs1(u, color, finishStack);
        color[v] = 2;
        finishStack.push(v);
    }

    void dfs2(const string& v, map<string, int>& color, vector<string>& comp) {
        color[v] = 1;
        comp.push_back(v);
        for (const string& u : rev_adj[v])
            if (color[u] == 0) dfs2(u, color, comp);
        color[v] = 2;
    }

    vector<vector<string>> kosaraju() {
        map<string, int> color;
        for (const string& v : vertices) color[v] = 0;
        stack<string> finishStack;
        for (const string& v : vertices)
            if (color[v] == 0) dfs1(v, color, finishStack);
        for (auto& p : color) p.second = 0;
        vector<vector<string>> sccs;
        while (!finishStack.empty()) {
            string v = finishStack.top();
            finishStack.pop();
            if (color[v] == 0) {
                vector<string> comp;
                dfs2(v, color, comp);
                sort(comp.begin(), comp.end());
                sccs.push_back(comp);
            }
        }
        sort(sccs.begin(), sccs.end(), [](const vector<string>& a, const vector<string>& b) {
            if (a.size() != b.size()) return a.size() > b.size();
            return a[0] < b[0];
            });
        return sccs;
    }

    void printSCCs(const vector<vector<string>>& sccs) {
        int idx = 1;
        for (const auto& comp : sccs) {
            set<string> compSet(comp.begin(), comp.end());
            cout << "\n--- Компонента сильной связности #" << idx << " ---\n";
            cout << "Вершины: ";
            for (size_t i = 0; i < comp.size(); ++i) {
                if (i > 0) cout << ", ";
                cout << comp[i];
            }
            cout << "\nДуги:\n";
            bool hasEdges = false;
            for (const string& u : comp)
                for (const string& v : adj[u])
                    if (compSet.find(v) != compSet.end()) {
                        cout << u << " " << v << "\n";
                        hasEdges = true;
                    }
            if (!hasEdges) cout << "(внутренних дуг нет)\n";
            idx++;
        }
    }
};

int main() {
    setlocale(LC_ALL, "");
    Graph g;
    cout << "Выберите ввод данных:\n1 - из файла\n2 - вручную\nВыбор: ";
    string choice;
    getline(cin, choice);
    trim(choice);

    if (choice == "1") {
        cout << "Введите имя файла: ";
        string filename;
        getline(cin, filename);
        trim(filename);
        if (!g.loadFromFile(filename)) {
            cout << "Не удалось открыть файл.\n";
            return 1;
        }
    }
    else if (choice == "2") {
        cout << "Введите список дуг (пустая строка - конец):\n";
        string line;
        while (true) {
            getline(cin, line);
            trim(line);
            if (line.empty()) break;
            vector<string> tokens;
            size_t pos = 0, start = 0;
            while ((pos = line.find(' ', start)) != string::npos) {
                string tok = line.substr(start, pos - start);
                trim(tok);
                if (!tok.empty()) tokens.push_back(tok);
                start = pos + 1;
            }
            string last = line.substr(start);
            trim(last);
            if (!last.empty()) tokens.push_back(last);
            if (tokens.size() == 1) g.addVertex(tokens[0]);
            else if (tokens.size() == 2) g.addEdge(tokens[0], tokens[1]);
        }
    }
    else {
        cout << "Неверный выбор.\n";
        return 1;
    }

    auto components = g.kosaraju();
    g.printSCCs(components);
    return 0;
}
