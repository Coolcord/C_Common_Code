#include <QCoreApplication>
#include <Text_Insertion_Buffer.h>
#include <QDebug>
#include <assert.h>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Text_Insertion_Buffer textInsertionBuffer;
    assert(textInsertionBuffer.Is_Empty());

    textInsertionBuffer.Insert_At_Beginning("Hello");
    textInsertionBuffer.Insert_After_Current_Line("World");
    textInsertionBuffer.Insert_After_Current_Line("");
    textInsertionBuffer.Insert_After_Current_Line("This");
    textInsertionBuffer.Insert_At_End("a");
    textInsertionBuffer.Insert_Before_Current_Line("is");
    textInsertionBuffer.Insert_At_End("test");

    qInfo().noquote() << textInsertionBuffer.Get_First_Line().trimmed();
    while (!textInsertionBuffer.At_End()) {
        qInfo().noquote() << textInsertionBuffer.Get_Next_Line().trimmed();
    }

    return a.exec();
}
