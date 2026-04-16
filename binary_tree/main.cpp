//libs
#include <QApplication>
#include <QPushButton>
#include <QDebug>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;


struct Node {
    int data;
    Node* left; // left pointer
    Node* right; // right pointer

    // Class constructor
    Node(int value) {
        data = value;
        left = nullptr;
        right = nullptr;
    }
};

class BinaryTree {
private:
    Node* root;
    // recursion add
    Node* addRecursive(Node* current, int value) {
        if (current == nullptr) {
            // null pointer
            return new Node(value);
        }
        if (value < current->data) {
            current->left = addRecursive(current->left, value);
        } else if (value > current->data) {
            current->right = addRecursive(current->right, value);
        }
        return current;
    }
    void printRecursive(Node* node, QString& result) {
        if (node != nullptr) {
            // left subtree
            printRecursive(node->left, result);
            // current node
            result += QString::number(node->data) + " ";
            // right subtree
            printRecursive(node->right, result);
        }
    }
    // maximum element
    int findMax(Node* node) {
        while(node->right != nullptr) {
            node = node->right;
        }
        return node->data;
    }
    void visualizeToTextRecursive(Node* node, QString& output, int level, QString prefix) {
        if (node == nullptr) return;

        // Правый ребенок
        visualizeToTextRecursive(node->right, output, level + 1, prefix + "    ");

        // Текущий узел
        output += prefix;
        if (level > 0) output += "|--";
        output += QString("0x%1 [%2]\n").arg((quintptr)node, 0, 16).arg(node->data);

        // Указатели детей
        output += prefix + "| |-- left-> " +
                  (node->left ? "0x" + QString::number((quintptr)node->left, 16) : "null") + "\n";
        output += prefix + "| |-- right-> " +
                  (node->right ? "0x" + QString::number((quintptr)node->right, 16) : "null") + "\n";

        // Левый ребенок
        visualizeToTextRecursive(node->left, output, level + 1, prefix + "    ");
    }
public:
    BinaryTree() {
        root = nullptr;
    }
    void add(int value) {
        root = addRecursive(root, value);
        qDebug() << "Added: " << value;
    }
    void print() {
        if (root == nullptr) {
            qDebug() << "Tree is Empty!";
            return;
        }
        QString result = "Tree (sorted): ";
        printRecursive(root, result);
        qDebug() << result;
    }
    int getMax() {
        if (root == nullptr) {
            qDebug() << "Tree is empty";
            return -1;
        }
        return findMax(root);
    }
    int getRoot() {
        if (root == nullptr) return -1;
        return root->data;

    }
    // Добавьте эту функцию для вывода в текстовое окно
    void visualizeToText(QString& output) {
        output = "Binary Tree Visualization:\n\n";
        visualizeToTextRecursive(root, output, 0, "");
    }




};



int main(int argc, char *argv[]){
    //--------------------------------GUI---------------------------------------
    QApplication app(argc, argv);
    srand(time(nullptr));

    BinaryTree tree;

    QPushButton button("Binary Tree Demo");
    button.resize(300, 50);

    QObject::connect(&button, &QPushButton::clicked, [&tree](){
        qDebug() << "===============================";

        int num = rand() % 100; // 0-99
        qDebug() << "Generated number:" << num;

        tree.add(num);

        tree.print();

        qDebug() << "Root:" <<tree.getRoot();
        qDebug() << "Maximum:" << tree.getMax();
        qDebug() << "=============================";


        // =================================Vizualisation=========================
        QString visualization;
        tree.visualizeToText(visualization);
        qDebug().noquote() << visualization;

        qDebug() << "===============================================";

    });

    button.show();

    qDebug() << "Click the button to binary tree";

    return app.exec();
}
