#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QString>

class Searchbox : public QDialog
{ Q_OBJECT
public:
    explicit Searchbox(QTextEdit*, QWidget *parent = nullptr);
    ~Searchbox();

public slots:
    void research();
    void findNext();

public:
    QLineEdit *searchLineEdit;
    QPushButton *searchButton;
    QPushButton *nextButton;
    QString currentTextEdit;
    QString FoundText;
    void text_find();
};

#endif // SEARCHBOX_H
