#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include <QPushButton>
#include <QDebug>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabsClosable(true);
    ui->tabWidget1->setTabsClosable(true);
    ui->tabWidget1->hide();
    ui->tabWidget->hide();
    ui->tabWidget->removeTab(0);

    // Associez des raccourcis clavier aux actions
    ui->action_Ouvrir->setShortcut(QKeySequence::Open);
    ui->action_Sauver->setShortcut(QKeySequence::Save);
    ui->action_Chercher->setShortcut(QKeySequence::Find);
    ui->action_Remplacer->setShortcut(QKeySequence::Print);

    //Action sur ouvrir et sauver
    connect(ui->action_Ouvrir,SIGNAL(triggered()),this,SLOT(open_file()));
    connect(ui->action_Sauver,SIGNAL(triggered()),this,SLOT(save_file()));

    //Action sur closable tab (texte)
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    //Action sur closable tab (recherche)
    connect(ui->tabWidget1, &QTabWidget::tabCloseRequested, this, &MainWindow::hideTab);


    connect(ui->pushButton_Rech,SIGNAL(clicked()),this,SLOT(search_in_text()));
    connect(ui->pushButton_Suiv,SIGNAL(clicked()),this,SLOT(next_in_text()));
    connect(ui->pushButton_Remplacer,SIGNAL(clicked()),this,SLOT(replace_in_text()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeTab(int index){
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(ui->tabWidget->widget(index));
    // Vérifiez s'il y a des modifications non enregistrées dans le texte
    if (textEdit && textEdit->document()->isModified()) {
        QMessageBox confirmDialog(QMessageBox::Question, "Enregistrer les modifications", "Voulez-vous enregistrer les modifications avant de fermer ?", QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard, this);
                confirmDialog.setButtonText(QMessageBox::Save, "Enregistrer");
                confirmDialog.setButtonText(QMessageBox::Cancel, "Annuler");
                confirmDialog.setButtonText(QMessageBox::Discard, "Ignorer les modifications");
                int boutton = confirmDialog.exec();

                        if (boutton == QMessageBox::Save) {
                            // Enregistrez les modifications
                            save_file();
                        } else if (boutton == QMessageBox::Cancel) {
                            // Annulez la fermeture de l'onglet
                            return;
                        }
    }

    ui->tabWidget->removeTab(index);
}

void MainWindow::open_file(){
    QString filePath = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(),"Fichiers texte (*.txt);;Tous les fichiers (*)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            // Ouvrir nouvel onglet avec le fichier voulu
            QTextEdit *textEdit = new QTextEdit(ui->tabWidget);
            textEdit->setPlainText(in.readAll());
            // Ajoutez l'onglet avec le texte du fichier
            int tabIndex = ui->tabWidget->addTab(textEdit, QFileInfo(filePath).fileName());
            // Initialisez la propriété "TabIndex" avec l'index de l'onglet
            textEdit->setProperty("TabIndex", tabIndex);
            // Fermer le fichier d''origine
            file.close();

            // Connectez le signal textChanged au slot asterisk
            connect(textEdit, &QTextEdit::textChanged, this, &MainWindow::asterisk);
            // connecter le curseur au slot updatecursorposition
            connect(textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
            // connecter le curseur au slot Find & Replace
            connect(ui->action_Chercher, &QAction::triggered, this, &MainWindow::openSearch);
            connect(ui->action_Remplacer, &QAction::triggered, this, &MainWindow::openReplace);
        }ui->tabWidget->show();
    }
}

void MainWindow::save_file(){
QString filePath = QFileDialog::getSaveFileName(this, "Enregistrer sous", QString(), "Fichiers texte (*.txt);;Tous les fichiers (*)");
    if (!filePath.isEmpty()) {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        QTextEdit *currentTextEdit = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
        if (currentTextEdit) {
        out << currentTextEdit->toPlainText();
        file.close();
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), QFileInfo(filePath).fileName());
    }
}
}
}
void MainWindow::asterisk(){
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(sender()); // Obtenez le QTextEdit émetteur du signal
        if (textEdit) {
            int tabIndex = textEdit->property("TabIndex").toInt(); // Obtenez le numéro de l'onglet
            QString tabText = ui->tabWidget->tabText(tabIndex); // Obtenez le texte actuel de l'onglet

            if (!tabText.endsWith('*')) { // Vérifiez si l'astérisque n'est pas déjà présent
                tabText += '*'; // Ajoutez un astérisque
                ui->tabWidget->setTabText(tabIndex, tabText); // Mettez à jour le texte de l'onglet
            }
        }
    }


void MainWindow::updateCursorPosition() {
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(sender());
    if (textEdit) {
        QTextCursor cursor = textEdit->textCursor();
        int line = cursor.blockNumber() + 1; // Pour numérotées à partir de 1
        int column = cursor.columnNumber() + 1; // Pour numérotées à partir de 1

        // Mettez à jour le texte de la barre d'état
        statusBar()->showMessage(QString("Ligne : %1, Colonne : %2").arg(line).arg(column));
    }
}

void MainWindow::openSearch(){
    ui->tabWidget1->setTabVisible(1,false);
    ui->tabWidget1->setTabVisible(0,true);
    ui->tabWidget1->show();
}

void MainWindow::openReplace(){
    ui->tabWidget1->setTabVisible(1,true);
    ui->tabWidget1->setTabVisible(0,false);
    ui->tabWidget1->show();
}

void MainWindow::hideTab(){
    ui->tabWidget1->hide();
}

void MainWindow::search_in_text() {
    QString searchText = ui->lineEdit->text();
    QTextEdit *currentTextEdit = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());

    if (!searchText.isEmpty()) {
        QTextCursor cursor(currentTextEdit->document());
        bool trouve = false;

        while (!cursor.isNull() && !cursor.atEnd()) {
            cursor = currentTextEdit->document()->find(searchText, cursor);

            if (!cursor.isNull()) {
                cursor.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);
                currentTextEdit->setTextCursor(cursor);
                currentTextEdit->setFocus();
                trouve = true;
                lastFoundCursor.setPosition(cursor.selectionEnd());
                break;
            }
        }
        if (!trouve) {
            QMessageBox::information(this, "Recherche", "Aucune correspondance trouvée.");
        }
    }
}

void MainWindow::next_in_text() {
    QString searchText = ui->lineEdit->text();
        QTextEdit *currentTextEdit = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());

        if (!searchText.isEmpty()) {
            QTextCursor cursor(currentTextEdit->document());


            cursor.setPosition(lastFoundCursor.position());

            bool found = false;

            while (!cursor.isNull() && !cursor.atEnd()) {
                cursor = currentTextEdit->document()->find(searchText, cursor);

                if (!cursor.isNull()) {
                    cursor.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);
                    currentTextEdit->setTextCursor(cursor);
                    currentTextEdit->setFocus();
                    lastFoundCursor = cursor;
                    found = true;
                    break;
                }
            }
            if (!found) {
                QMessageBox::information(this, "Recherche", "Fin du document,plus de correspondance trouvée.");
            }
        }
    }

void MainWindow::replace_in_text() {
    QString searchText = ui->lineEdit_rempla->text();
        QString replaceText = ui->lineEdit_Par->text();
        QTextEdit *currentTextEdit = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());

        if (!searchText.isEmpty() && currentTextEdit) {
            QTextCursor cursor(currentTextEdit->document());

            bool replacementMade = false;

            while (!cursor.isNull() && !cursor.atEnd()) {
                cursor = currentTextEdit->document()->find(searchText, cursor);

                if (!cursor.isNull()) {
                    cursor.removeSelectedText();
                    cursor.insertText(replaceText);
                    replacementMade = true;
                }
            }
            if (!replacementMade) {
                QMessageBox::information(this, "Recherche", "Aucune correspondance trouvée.");
            }
        }
    }
