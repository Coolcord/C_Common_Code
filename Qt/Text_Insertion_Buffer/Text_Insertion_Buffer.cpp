#include "Text_Insertion_Buffer.h"
#include <QTextStream>
#include <assert.h>

Text_Insertion_Buffer::Text_Insertion_Buffer() {
    this->buffer = new std::list<QString>();
    this->iter = this->buffer->begin();
    this->lineBuffer = QString();
    this->beforeBeginning = false;
    this->afterEnd = false;
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
    return this->iter == --this->buffer->end();
}

bool Text_Insertion_Buffer::Is_Empty() {
    return this->buffer->empty();
}

void Text_Insertion_Buffer::Seek_To_Before_Beginning() {
    if (this->buffer->empty()) return;
    this->iter = this->buffer->begin();
    this->beforeBeginning = true;
}

void Text_Insertion_Buffer::Seek_To_Beginning() {
    if (this->buffer->empty()) return;
    this->iter = this->buffer->begin();
}

void Text_Insertion_Buffer::Seek_To_Next() {
    if (this->buffer->empty()) return;
    if (this->beforeBeginning) { this->beforeBeginning = false; return; }
    if (this->iter == --this->buffer->end()) return;
    ++this->iter;
}

void Text_Insertion_Buffer::Seek_To_Previous() {
    if (this->buffer->empty()) return;
    if (this->iter == this->buffer->begin()) return;
    --this->iter;
}

void Text_Insertion_Buffer::Seek_To_End() {
    if (this->buffer->empty()) return;
    if (this->buffer->begin() == this->buffer->end()) return;
    this->iter = --this->buffer->end();
}

void Text_Insertion_Buffer::Seek_To_After_End() {
    if (this->buffer->empty()) return;
    this->afterEnd = true;
    if (this->buffer->begin() == this->buffer->end()) return;
    this->iter = --this->buffer->end();
}

QString Text_Insertion_Buffer::Peek_First_Line() {
    if (this->buffer->empty()) return QString();
    return *this->buffer->begin();
}

QString Text_Insertion_Buffer::Peek_Last_Line() {
    if (this->buffer->empty()) return QString();
    return *(--this->buffer->end());
}

QString Text_Insertion_Buffer::Peek_Next_Line() {
    if (this->buffer->empty()) return QString();
    if (this->At_End()) return QString();
    if (this->beforeBeginning) return this->Peek_First_Line();
    std::list<QString>::iterator tmpIter = this->iter;
    ++tmpIter;
    return *tmpIter;
}

QString Text_Insertion_Buffer::Peek_Previous_Line() {
    if (this->buffer->empty()) return QString();
    std::list<QString>::iterator tmpIter = this->iter;
    if (this->afterEnd) return this->Peek_Last_Line();
    if (tmpIter == this->buffer->begin()) return QString();
    --tmpIter;
    return *tmpIter;
}

QString Text_Insertion_Buffer::Get_First_Line() {
    if (this->buffer->empty()) return QString();
    this->iter = this->buffer->begin();
    return this->buffer->front();
}

QString Text_Insertion_Buffer::Get_Last_Line() {
    if (this->buffer->empty()) return QString();
    this->iter = --this->buffer->end();
    return *this->iter;
}

QString Text_Insertion_Buffer::Get_Current_Line() {
    if (this->buffer->empty()) return QString();
    return *this->iter;
}

QString Text_Insertion_Buffer::Get_Previous_Line() {
    if (this->buffer->empty()) return QString();
    if (this->iter == this->buffer->begin()) return QString();
    if (this->afterEnd) { this->afterEnd = false; return *this->iter; }
    --this->iter;
    return *this->iter;
}

QString Text_Insertion_Buffer::Get_Next_Line() {
    if (this->buffer->empty()) return QString();
    if (this->At_End()) return QString();
    if (this->beforeBeginning) { this->beforeBeginning = false; return *this->iter; }
    ++this->iter;
    return *this->iter;
}

QString Text_Insertion_Buffer::Read_All() {
    if (this->buffer->empty()) return QString();
    this->Seek_To_Beginning();
    return this->Read_All_Remaining();
}

QString Text_Insertion_Buffer::Read_All_Remaining() {
    if (this->buffer->empty()) return QString();
    QString data;
    while (this->iter != this->buffer->end()) {
        data += *this->iter;
        ++this->iter;
    }
    return data;
}

void Text_Insertion_Buffer::Insert_At_Beginning(const QString &line) {
    this->buffer->push_front(this->lineBuffer+line);
    this->iter = this->buffer->begin();
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_After_Current_Line(const QString &line) {
    if (this->buffer->empty()) return this->Insert_Before_Current_Line(line);
    std::list<QString>::iterator tmpIter = this->iter;
    ++tmpIter;
    this->iter = this->buffer->insert(tmpIter, this->lineBuffer+line);
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_Before_Current_Line(const QString &line) {
    this->iter = this->buffer->insert(this->iter, this->lineBuffer+line);
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_At_End(const QString &line) {
    this->buffer->push_back(this->lineBuffer+line);
    this->iter = --this->buffer->end();
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_At_Beginning_Without_Seeking(const QString &line) {
    this->buffer->push_front(this->lineBuffer+line);
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Insert_After_Current_Line_Without_Seeking(const QString &line) {
    std::list<QString>::iterator tmpIter = this->iter;
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
    this->buffer->push_back(this->lineBuffer+line);
    this->Clear_Line_Buffer();
}

void Text_Insertion_Buffer::Remove_Current_Line() {
    this->iter = this->buffer->erase(this->iter);
}

void Text_Insertion_Buffer::Remove_First_Line() {
    bool atBeginning = false;
    if (this->At_Beginning()) atBeginning = true;
    this->buffer->pop_front();
    if (atBeginning) this->iter = this->buffer->begin();
}

void Text_Insertion_Buffer::Remove_Last_Line() {
    bool atEnd = false;
    if (this->At_End()) atEnd = true;
    this->buffer->pop_back();
    if (atEnd) this->iter = this->buffer->end();
}

void Text_Insertion_Buffer::Add_To_Line_Buffer(const QString &line) {
    this->lineBuffer += line;
}

bool Text_Insertion_Buffer::Write_To_File(QFile *file) {
    assert(file);
    QTextStream stream(file);
    stream << this->Get_First_Line() << Qt::endl;
    while (!this->At_End()) stream << this->Get_Next_Line() << Qt::endl;
    stream.flush();
    return stream.status() == QTextStream::Ok;
}

bool Text_Insertion_Buffer::Write_To_File(const QString &fileLocation) {
    QFile file(fileLocation);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) return false;
    this->Seek_To_Before_Beginning();
    QTextStream stream(&file);
    while (!this->At_End()) stream << this->Get_Next_Line() << Qt::endl;
    stream.flush();
    file.close();
    return stream.status() == QTextStream::Ok;
}
