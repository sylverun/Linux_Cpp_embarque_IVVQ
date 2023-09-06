#include "replacebox.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QTextCursor>

Replacebox::Replacebox(QTextEdit* currentTextEdit, QWidget *parent) : QDialog(parent)
{     setWindowTitle("Remplacer ");
      To_replace = new QLineEdit(this);
      Replace_by= new QLineEdit(this);
      Valider = new QPushButton("Remplacer", this);


      QVBoxLayout *layout = new QVBoxLayout;
      layout->addWidget(new QLabel("Remplacer :"));
      layout->addWidget(To_replace);
      layout->addWidget(new QLabel("Par : "));
      layout->addWidget(Replace_by);
      layout->addWidget(Valider);

      this->currentTextEdit = currentTextEdit->toPlainText();
      connect(Valider, &QPushButton::clicked, this, &Replacebox::Remplacer_);

      setLayout(layout);

}

Replacebox::~Replacebox()
{
}

void Replacebox::Remplacer_()
{
    QString toreplaceString = To_replace->text();
    QString replaceByString = Replace_by->text();
    QTextEdit *currentTextEdit = new QTextEdit(this->currentTextEdit, nullptr);

    // Vérifiez si le texte à rechercher n'est pas vide
    if (!toreplaceString.isEmpty()) {
        if (currentTextEdit) {
            qDebug() << currentTextEdit->toPlainText();
            QTextCursor cursor = currentTextEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);

            // Effectuez la recherche
            while (!cursor.isNull() && !cursor.atEnd()) {
                cursor.select(QTextCursor::WordUnderCursor);
                QString g = cursor.selectedText();
                if (g == toreplaceString) {
                    currentTextEdit->setTextCursor(cursor);
                    qDebug() << g;

                    return;
                }
                cursor.movePosition(QTextCursor::NextWord);
            }

            // Si rien n'a été trouvé, avertissez l'utilisateur
            QMessageBox::information(this, "Recherche", "Aucune correspondance trouvée.");
        }
    }

    delete (currentTextEdit);
}
