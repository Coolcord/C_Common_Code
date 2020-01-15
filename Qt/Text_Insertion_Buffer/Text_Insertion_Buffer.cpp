#include "Text_Insertion_Buffer.h"
#include <QTextStream>
#include <assert.h>

Text_Insertion_Buffer::Text_Insertion_Buffer() {
    this->buffer = new QLinkedList<QString>();
    this->iter = this->buffer->begin();
    this->lineBuffer = QString();
}

Text_Insertion_Buffer::~Text_Insertion_Buffer() {
    this->Clear_Buffers();
}

void Text_Insertion_Buffer::Clear_Buffers() {
    this->buffer->clear();
    this->lineBuffer.clear();
}

void Text_Insertion_Buffer::Clear_Text_Insertion_Buffer() {
    this->buffer->clear();
}

void Text_Insertion_Buffer::Clear_Line_Buffer() {
    this->lineBuffer = QString();
}

bool Text_Insertion_Buffer::At_Beginning() {
    return this->iter == this->buffer->begin();
}

bool Text_Insertion_Buffer::At_End() {
    return this->iter == this->buffer->end()-1;
}

bool Text_Insertion_Buffer::Is_Empty() {
    return this->buffer->isEmpty();
}

void Text_Insertion_Buffer::Seek_To_Beginning() {
    this->iter = this->buffer->begin();
}

void Text_Insertion_Buffer::Seek_To_Next() {
    if (this->iter == this->buffer->end()-1) return;
    ++this->iter;
}

void Text_Insertion_Buffer::Seek_To_Previous() {
    if (this->iter == this->buffer->begin()) return;
    --this->iter;
}

void Text_Insertion_Buffer::Seek_To_End() {
    if (this->buffer->begin() == this->buffer->end()) return;
    this->iter = this->buffer->end()-1;
}

QString Text_Insertion_Buffer::Peek_First_Line() {
    return *this->buffer->begin();
}

QString Text_Insertion_Buffer::Peek_Last_Line() {
    return *(this->buffer->end()-1);
}

QString Text_Insertion_Buffer::Peek_Next_Line() {
    if (this->At_End()) return QString();
    QLinkedList<QString>::iterator tmpIter = this->iter;
    ++tmpIter;
    return *tmpIter;
}

QString Text_Insertion_Buffer::Peek_Previous_Line() {
    QLinkedList<QString>::iterator tmpIter = this->iter;
    if (tmpIter == this->buffer->begin()) return QString();
    --tmpIter;
    return *tmpIter;
}

QString Text_Insertion_Buffer::Get_First_Line() {
    this->iter = this->buffer->begin();
    return this->buffer->first();
}

QString Text_Insertion_Buffer::Get_Last_Line() {
    this->iter = this->buffer->end()-1;
    return *this->iter;
}

QString Text_Insertion_Buffer::Get_Current_Line() {
    return *this->iter;
}

QString Text_Insertion_Buffer::Get_Previous_Line() {
    if (this->iter == this->buffer->begin()) return QString();
    --this->iter;
    return *this->iter;
}

QString Text_Insertion_Buffer::Get_Next_Line() {
    if (this->At_End()) return QString();
    ++this->iter;
    return *this->iter;
}

QString Text_Insertion_Buffer::Read_All() {
    this->Seek_To_Beginning();
    return this->Read_All_Remaining();
}

QString Text_Insertion_Buffer::Read_All_Remaining() {
    QString data;
    while (this->iter != this->buffer->end()) {
        data += *this->iter;
        ++this->iter;
    }
    return data;
}

void Text_Insertion_Buffer::Insert_At_Beginning(const QString &line) {
    this->buffer->prepend(this->lineBuffer+line);
    this->iter = this->buffer->begin();
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_After_Current_Line(const QString &line) {
    QLinkedList<QString>::iterator tmpIter = this->iter;
    ++tmpIter;
    this->iter = this->buffer->insert(tmpIter, this->lineBuffer+line);
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_Before_Current_Line(const QString &line) {
    this->iter = this->buffer->insert(this->iter, this->lineBuffer+line);
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_At_End(const QString &line) {
    this->buffer->append(this->lineBuffer+line);
    this->iter = this->buffer->end()-1;
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_At_Beginning_Without_Seeking(const QString &line) {
    this->buffer->prepend(this->lineBuffer+line);
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_After_Current_Line_Without_Seeking(const QString &line) {
    QLinkedList<QString>::iterator tmpIter = this->iter;
    ++tmpIter;
    this->iter = this->buffer->insert(tmpIter, this->lineBuffer+line);
    --this->iter;
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_Before_Current_Line_Without_Seeking(const QString &line) {
    this->iter = this->buffer->insert(this->iter, this->lineBuffer+line);
    ++this->iter;
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_At_End_Without_Seeking(const QString &line) {
    this->buffer->append(this->lineBuffer+line);
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Remove_Current_Line() {
    this->iter = this->buffer->erase(this->iter);
}

void Text_Insertion_Buffer::Remove_First_Line() {
    bool atBeginning = false;
    if (this->At_Beginning()) atBeginning = true;
    this->buffer->removeFirst();
    if (atBeginning) this->iter = this->buffer->begin();
}

void Text_Insertion_Buffer::Remove_Last_Line() {
    bool atEnd = false;
    if (this->At_End()) atEnd = true;
    this->buffer->removeLast();
    if (atEnd) this->iter = this->buffer->end();
}

void Text_Insertion_Buffer::Add_To_Line_Buffer(const QString &line) {
    this->lineBuffer += line;
}

bool Text_Insertion_Buffer::Write_To_File(QFile *file) {
    assert(file);
    QTextStream stream(file);
    stream << this->Get_First_Line() << endl;
    while (!this->At_End()) stream << this->Get_Next_Line() << endl;
    stream.flush();
    return stream.status() == QTextStream::Ok;
}

bool Text_Insertion_Buffer::Write_To_File(const QString &fileLocation) {
    QFile file(fileLocation);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) return false;
    this->Seek_To_Beginning();
    QTextStream stream(&file);
    stream << this->Get_First_Line() << endl;
    while (!this->At_End()) stream << this->Get_Next_Line() << endl;
    stream.flush();
    file.close();
    return stream.status() == QTextStream::Ok;
}
