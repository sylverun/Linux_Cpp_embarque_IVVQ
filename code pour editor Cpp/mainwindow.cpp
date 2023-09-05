#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include <QPushButton>


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

    //Action sur ouvrir et sauver
    connect(ui->action_Ouvrir,SIGNAL(triggered()),this,SLOT(open_file()));
    connect(ui->action_Sauver,SIGNAL(triggered()),this,SLOT(save_file()));


    //Action sur closable tab
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);

    connect(ui->tabWidget->currentWidget(), SIGNAL(textChanged()),this,SLOT(asterisk()));
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

          //ouvrir nouvel onglet avec le fichier voulu
           QTextEdit *textEdit = new QTextEdit(ui->tabWidget);
           textEdit->setPlainText(in.readAll());
           ui->tabWidget->addTab(textEdit, QFileInfo(filePath).fileName());
           file.close();
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

void MainWindow::asterisk(int index){
    QString tabText = ui->tabWidget->tabText(index);
    if (!tabText.endsWith("*")) {
    ui->tabWidget->setTabText(index,tabText +  "*");
}
}
