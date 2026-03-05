#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
using namespace std;

const int MAX_QUEUE_SIZE = 1000;

struct QueueNode {
    int data;
    QueueNode* next;
};

struct Queue {
    QueueNode* front;
    QueueNode* rear;
    int size;

    Queue() : front(nullptr), rear(nullptr), size(0) {}

    ~Queue() {
        int val;
        while (dequeue(val));
    }

    Queue(const Queue& other) : front(nullptr), rear(nullptr), size(0) {
        QueueNode* cur = other.front;
        while (cur) {
            enqueue(cur->data);
            cur = cur->next;
        }
    }

    Queue& operator=(const Queue& other) {
        if (this == &other) return *this;
        int val;
        while (dequeue(val));
        size = 0;
        QueueNode* cur = other.front;
        while (cur) {
            enqueue(cur->data);
            cur = cur->next;
        }
        return *this;
    }

    void enqueue(int value) {
        if (size >= MAX_QUEUE_SIZE) return;
        QueueNode* node = new QueueNode{ value, nullptr };
        if (!rear) front = rear = node;
        else {
            rear->next = node;
            rear = node;
        }
        size++;
    }

    bool dequeue(int& value) {
        if (!front) return false;
        QueueNode* tmp = front;
        value = front->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete tmp;
        size--;
        return true;
    }

    void print() const {
        QueueNode* cur = front;
        while (cur) {
            cout << cur->data << " ";
            cur = cur->next;
        }
    }
};

struct StackNode {
    Queue queue;
    StackNode* next;
};

struct Stack {
    StackNode* top;

    Stack() : top(nullptr) {}

    ~Stack() {
        while (top) {
            StackNode* tmp = top;
            top = top->next;
            delete tmp;
        }
    }

    void pushQueue(const Queue& q) {
        StackNode* node = new StackNode;
        node->queue = q;
        node->next = top;
        top = node;
    }

    bool popQueue(Queue& q) {
        if (!top) return false;
        q = top->queue;
        StackNode* tmp = top;
        top = top->next;
        delete tmp;
        return true;
    }

    void print() const {
        StackNode* cur = top;
        cout << "Стек очередей:\n";
        while (cur) {
            cur->queue.print();
            cout << " -> ";
            cur = cur->next;
        }
        cout << "NULL\n";
    }

    bool isEmpty() const {
        return top == nullptr;
    }

    Queue* getQueueAt(int index) {
        StackNode* cur = top;
        int i = 0;
        while (cur && i < index) {
            cur = cur->next;
            i++;
        }
        return (cur ? &cur->queue : nullptr);
    }

    int getSize() const {
        int count = 0;
        StackNode* cur = top;
        while (cur) {
            count++;
            cur = cur->next;
        }
        return count;
    }
};

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    Stack s;
    int variant;

    do {
        cout << "\nМеню:\n";
        cout << "1. Загрузить очередь из файла и поместить в стек\n";
        cout << "2. Удалить очередь с вершины стека\n";
        cout << "3. Показать содержимое стека\n";
        cout << "4. Создать очередь вручную и добавить в стек\n";
        cout << "5. Изменить очередь по номеру\n";
        cout << "0. Выход\n";
        cout << "Ваш выбор: ";
        cin >> variant;


        if (variant == 1) {
            ifstream fin("input.txt");
            if (!fin) {
                cout << "Ошибка открытия файла!\n";
                continue;
            }
            Queue q;
            int x;
            while (fin >> x) q.enqueue(x);
            s.pushQueue(q);
            cout << "Очередь добавлена в стек.\n";
        }
        else if (variant == 2) {
            Queue q;
            if (s.popQueue(q)) {
                cout << "Удалена очередь: ";
                q.print();
                cout << endl;
            }
            else cout << "Стек пуст!\n";
        }
        else if (variant == 3) {
            if (s.isEmpty()) cout << "Стек пуст!\n";
            else s.print();
        }
        else if (variant == 4) {
            Queue q;
            int n;
            cout << "Сколько элементов добавить в очередь? (макс. " << MAX_QUEUE_SIZE << "): ";
            cin >> n;
            if (n > MAX_QUEUE_SIZE) {
                cout << "Ошибка: превышен лимит элементов (" << MAX_QUEUE_SIZE << ")\n";
                continue;
            }
            cout << "Введите элементы очереди (введите '$' для досрочного завершения ввода):\n";
            for (int i = 0; i < n; ++i) {
                string input;
                cout << "Элемент " << i + 1 << ": ";
                cin >> input;
                if (input == "$") {
                    cout << "Ввод очереди прерван пользователем.\n";
                    break;
                }
                try {
                    int value = stoi(input);
                    q.enqueue(value);
                }
                catch (...) {
                    cout << "Ошибка: введите целое число или '$' для выхода.\n";
                    i--;
                }
            }
            s.pushQueue(q);
            cout << "Очередь добавлена в стек вручную.\n";
        }
        else if (variant == 5) {
            if (s.isEmpty()) {
                cout << "Стек пуст!\n";
                continue;
            }
            int total = s.getSize();
            cout << "Всего очередей в стеке: " << total << endl;
            cout << "Введите номер очереди для изменения (1 - верхняя, " << total << " - нижняя): ";
            int idx;
            cin >> idx;
            if (idx < 1 || idx > total) {
                cout << "Неверный номер очереди!\n";
                continue;
            }
            Queue* q = s.getQueueAt(idx - 1);
            if (!q) {
                cout << "Ошибка доступа к очереди!\n";
                continue;
            }
            cout << "Вы выбрали очередь " << idx << ": ";
            q->print();
            cout << endl;
            int subChoice;
            do {
                cout << "\nПодменю редактирования очереди:\n";
                cout << "1. Добавить элемент\n";
                cout << "2. Удалить элемент из начала\n";
                cout << "3. Показать очередь\n";
                cout << "0. Выход в главное меню\n";
                cout << "Ваш выбор: ";
                cin >> subChoice;
                if (subChoice == 1) {
                    int val;
                    cout << "Введите значение для добавления: ";
                    cin >> val;
                    q->enqueue(val);
                }
                else if (subChoice == 2) {
                    int val;
                    if (q->dequeue(val)) cout << "Удалён элемент: " << val << endl;
                    else cout << "Очередь пуста!\n";
                }
                else if (subChoice == 3) {
                    cout << "Очередь: ";
                    q->print();
                    cout << endl;
                }
            } while (subChoice != 0);
        }

    } while (variant != 0);

    return 0;
}
