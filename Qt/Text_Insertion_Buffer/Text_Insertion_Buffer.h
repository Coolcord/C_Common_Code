#ifndef TEXT_INSERTION_BUFFER_H
#define TEXT_INSERTION_BUFFER_H

/* Text_Insertion_Buffer
 *
 * This class works around the limitations of QFile and QTextStream.
 * QFile has no way of inserting into a file without rewriting everything after the insertion.
 * QTextStream has no way of editing text after it has been written.
 * Text_Insertion_Buffer allows for lines to be inserted or removed before being written to a file.
 *
 */

#include <QFile>
#include <QLinkedList>
#include <QString>

class Text_Insertion_Buffer {
public:
    Text_Insertion_Buffer();
    ~Text_Insertion_Buffer();

    //Clear buffers
    void Clear_Buffers(); //this will delete all data in the buffer
    void Clear_Text_Insertion_Buffer();
    void Clear_Line_Buffer();

    //Status checks
    bool At_Beginning();
    bool At_End();
    bool Is_Empty();

    //Seek without reading text
    void Seek_To_Beginning();
    void Seek_To_Next();
    void Seek_To_Previous();
    void Seek_To_End();

    //Read text without seeking
    QString Peek_First_Line();
    QString Peek_Last_Line();
    QString Peek_Next_Line();
    QString Peek_Previous_Line();

    //Read text and seek
    QString Get_First_Line();
    QString Get_Last_Line();
    QString Get_Current_Line();
    QString Get_Previous_Line();
    QString Get_Next_Line();
    QString Read_All();
    QString Read_All_Remaining();

    //Insert text and seek
    void Insert_At_Beginning(const QString &line);
    void Insert_After_Current_Line(const QString &line);
    void Insert_Before_Current_Line(const QString &line);
    void Insert_At_End(const QString &line);

    //Insert text and seek
    //Anything in the line buffer will be prepended when an insert is performed
    //E.g. lineBuffer + line
    void Insert_At_Beginning_Without_Seeking(const QString &line);
    void Insert_After_Current_Line_Without_Seeking(const QString &line);
    void Insert_Before_Current_Line_Without_Seeking(const QString &line);
    void Insert_At_End_Without_Seeking(const QString &line);

    //Remove text and seek
    void Remove_Current_Line();
    void Remove_First_Line();
    void Remove_Last_Line();

    //Line Buffer
    //Anything in the line buffer will be prepended when an insert is performed
    void Add_To_Line_Buffer(const QString &line);

    //Write to file
    bool Write_To_File(QFile *file);
    bool Write_To_File(const QString &fileLocation);

private:
    QLinkedList<QString> *buffer; //use a linked list instead of a vector for quick insertions
    QLinkedList<QString>::iterator iter;
    QString lineBuffer;
};

#endif // TEXT_INSERTION_BUFFER_H
