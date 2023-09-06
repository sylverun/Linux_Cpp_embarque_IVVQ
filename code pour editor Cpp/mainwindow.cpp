#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "searchbox.h"
#include "replacebox.h"

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
    ui->tabWidget->setTabText(0,"Nouveau document");

    // Associez des raccourcis clavier aux actions (Ctrl+O et Ctrl+S)
    ui->action_Ouvrir->setShortcut(QKeySequence::Open);
    ui->action_Sauver->setShortcut(QKeySequence::Save);
    ui->action_Rechercher->setShortcut(QKeySequence::Find);
    ui->action_Remplacer->setShortcut(QKeySequence::Print);

    //Action sur ouvrir et sauver
    connect(ui->action_Ouvrir,SIGNAL(triggered()),this,SLOT(open_file()));
    connect(ui->action_Sauver,SIGNAL(triggered()),this,SLOT(save_file()));


    //Action sur closable tab
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);


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

            // connecter le curseur au slot updatecursorposition
            connect(textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPosition);


            // Ajoutez l'onglet avec le texte du fichier
            int tabIndex = ui->tabWidget->addTab(textEdit, QFileInfo(filePath).fileName());

            // Initialisez la propriété "TabIndex" avec l'index de l'onglet
            textEdit->setProperty("TabIndex", tabIndex);

            file.close();

            // Connectez le signal textChanged au slot asterisk
            connect(textEdit, &QTextEdit::textChanged, this, &MainWindow::asterisk);

            connect(ui->action_Rechercher, &QAction::triggered, this, &MainWindow::openSearch);
            connect(ui->action_Remplacer, &QAction::triggered, this, &MainWindow::openReplace);
        }
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
        int line = cursor.blockNumber() + 1; // Les lignes sont généralement numérotées à partir de 1
        int column = cursor.columnNumber() + 1; // Les colonnes sont généralement numérotées à partir de 1

        // Mettez à jour le texte de la barre d'état
        statusBar()->showMessage(QString("Ligne : %1, Colonne : %2").arg(line).arg(column));
    }
}

void MainWindow::openSearch(){
    QTextEdit *currentTextEdit = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
        if (currentTextEdit) {
            Searchbox *research_ = new Searchbox(currentTextEdit, this);
            research_->exec(); // Ouvrir la fenêtre de recherche
            delete(research_);   // Libérer la mémoire lorsque la fenêtre de recherche est fermée
}
}


void MainWindow::openReplace(){
    QTextEdit *currentTextEdit = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
        if (currentTextEdit) {
            Replacebox *replace_ = new Replacebox(currentTextEdit, this);
            replace_->exec(); // Ouvrir la fenêtre
            delete(replace_);   // Libérer la mémoire lorsque la fenêtre est fermée
}
}
