#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>


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
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::closeTab(int index){
            ui->tabWidget->removeTab(index);
}
