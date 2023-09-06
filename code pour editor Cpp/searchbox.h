#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>

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
    QTextEdit* currentTextEdit;
};

#endif // SEARCHBOX_H
