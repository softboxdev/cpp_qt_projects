#include <QApplication>
#include <QPushButton>
#include <QVector>
#include <QList>
#include <QDebug>
#include <algorithm>
#include <iostream>

using namespace std;

vector<int> stack;

void push(int value) {
    stack.push_back(value);
    qDebug() << "Added value : " << value;
}

void pop() {
    if(stack.empty()) {
        qDebug() << "Stack is empty!";
        return;
    }
    qDebug() << "Deleted value " << stack.back();
    stack.pop_back();
}

int top() {
    if (stack.empty()) {
        qDebug() << "Stack is empty!";
        return -1;
    }
    return stack.back();
}

void display() {
    if(stack.empty()) {
        return;
    }
    QString displayStr = "Stack: ";
    for (int i = 0; i < stack.size(); i++) {
        displayStr += QString::number(stack[i]) + " ";
    }
    displayStr += "| Top = " + QString::number(stack.back());
    qDebug() << displayStr;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //---------------GUI-------------------------
    QPushButton button("Stack exemple!");
    button.resize(300,50);

    QObject::connect(&button, &QPushButton::clicked, [&]() {
        //random value
        int num = rand() % 100;
        push(num);
        qDebug() << "Added: " << num;

        qDebug() << "Top:" << top();

        qDebug() << "Stack: ";
        for(int x : stack) qDebug() << x;
        qDebug() << "---";

        if (stack.size() > 10) {
            qDebug() << "Deleted: " << stack.back();
            pop();
        }
        qDebug() << "Stack size: " << stack.size();
        qDebug() << "===================";

    });

    // lambda

    button.show();
    qDebug() << "\n Push the button!";
    return app.exec();
}
