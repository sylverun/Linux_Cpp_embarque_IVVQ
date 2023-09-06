#include "searchbox.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QTextCursor>



Searchbox::Searchbox(QTextEdit* currentTextEdit, QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Rechercher");
    searchLineEdit = new QLineEdit(this);
    searchButton = new QPushButton("Rechercher", this);
    nextButton = new QPushButton("Suivant", this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel("Rechercher un mot :"));
    layout->addWidget(searchLineEdit);
    layout->addWidget(searchButton);
    layout->addWidget(nextButton);

    this->currentTextEdit = currentTextEdit->toPlainText();

    connect(searchButton, &QPushButton::clicked, this, &Searchbox::research);
    connect(nextButton, &QPushButton::clicked, this, &Searchbox::findNext);

    setLayout(layout);
}

Searchbox::~Searchbox()
{
}


void Searchbox::research()
{
    QString searchString = searchLineEdit->text();
    QTextEdit *currentTextEdit = new QTextEdit(this->currentTextEdit, nullptr);

    // Vérifiez si le texte à rechercher n'est pas vide
    if (!searchString.isEmpty()) {
        if (currentTextEdit) {
            qDebug() << currentTextEdit->toPlainText();
            QTextCursor cursor = currentTextEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);

            // Effectuez la recherche
            while (!cursor.isNull() && !cursor.atEnd()) {
                cursor.select(QTextCursor::WordUnderCursor);
                QString g = cursor.selectedText();
                if (g == searchString) {
                    currentTextEdit->setTextCursor(cursor);
                    qDebug() << g;
                    FoundText = g;
                    return; // Arrêtez la recherche lorsque vous avez trouvé une correspondance
                }
                cursor.movePosition(QTextCursor::NextWord);
            }

            // Si rien n'a été trouvé, avertissez l'utilisateur
            QMessageBox::information(this, "Recherche", "Aucune correspondance trouvée.");
        }
    }

    delete (currentTextEdit);
}
void Searchbox::findNext()
{

}
