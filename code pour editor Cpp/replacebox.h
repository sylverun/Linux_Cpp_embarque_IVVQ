#ifndef REPLACEBOX_H
#define REPLACEBOX_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QString>

class Replacebox : public  QDialog
{  Q_OBJECT
public:
    explicit Replacebox(QTextEdit*, QWidget *parent = nullptr);
    ~Replacebox();
    QLineEdit *To_replace;
    QLineEdit *Replace_by;
    QPushButton *Valider;
    QString currentTextEdit;
private slots:
    void Remplacer_();
};

#endif // REPLACEBOX_H
