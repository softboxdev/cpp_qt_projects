#include <QApplication>
#include <QPushButton>
#include <QVector>
#include <QList>
#include <QDebug>
#include <algorithm>
using namespace std;
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QVector<int> heap;
    // insert
    heap.push_back(5002);
    push_heap(heap.begin(), heap.end());
    heap.push_back(3824);
    push_heap(heap.begin(), heap.end());
    heap.push_back(1223);
    push_heap(heap.begin(), heap.end());
    heap.push_back(9832);
    push_heap(heap.begin(), heap.end());
    heap.push_back(3231);
    push_heap(heap.begin(), heap.end());
    // print
    qDebug() << "Current heap state:" << heap;
    qDebug() << "Max heap element:" << heap.first();

    //delete
    pop_heap(heap.begin(), heap.end());

    int maxElement = heap.last();
    heap.pop_back();
    qDebug() << "Deleted element:" << maxElement;
    qDebug() << "Heap after deletion: " << heap;


    //---------------GUI-------------------------
    QPushButton button("Heap exemple!");

    // lambda
    QObject::connect(&button, &QPushButton::clicked, [&heap]() {
        qDebug() << "\n====BUTTON CLICKED";

        // adding element in a heap

        int newValue = rand() % 100;
        heap.push_back(newValue);
        push_heap(heap.begin(), heap.end()) ;
        qDebug() << "Current heap" << heap;
        qDebug() << "Heap maximum" << heap.first();


        // delete
        // 1. max element preparation
        pop_heap(heap.begin(), heap.end());
        // 2. value for deletion
        int removed = heap.last();
        // 3. save last element == maximum
        heap.pop_back();
        qDebug() << "Maximum deletion" << removed;
        qDebug() << "Heap after deletion" << heap;
    });
    button.show();
    button.resize(300,50);
    qDebug() << "\n Push the button!";

    return app.exec();

}
