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
    ui->action_newrecent->setVisible(false);

    // Associez des raccourcis clavier aux actions
    ui->action_New->setShortcut(QKeySequence::New);
    ui->action_Ouvrir->setShortcut(QKeySequence::Open);
    ui->action_Sauver->setShortcut(QKeySequence::Save);
    ui->action_Chercher->setShortcut(QKeySequence::Find);
    ui->action_Remplacer->setShortcut(QKeySequence::Print);

    //Action sur ouvrir et sauver
    connect(ui->action_New,SIGNAL(triggered()),this,SLOT(new_file()));
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

void MainWindow::new_file(){
            // Ouvrir un nouvel onglet
            QTextEdit *textEdit = new QTextEdit(ui->tabWidget);
            // Ajoutez l'onglet avec le texte du fichier
            QString new_doc= "Nouveau Document";
            int tabIndex = ui->tabWidget->addTab(textEdit, new_doc);
            // Initialisez la propriété "TabIndex" avec l'index de l'onglet
            textEdit->setProperty("TabIndex", tabIndex);


            // Connectez le signal textChanged au slot asterisk
            connect(textEdit, &QTextEdit::textChanged, this, &MainWindow::asterisk);
            // connecter le curseur au slot updatecursorposition
            connect(textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
            // connecter le curseur au slot Find & Replace
            connect(ui->action_Chercher, &QAction::triggered, this, &MainWindow::openSearch);
            connect(ui->action_Remplacer, &QAction::triggered, this, &MainWindow::openReplace);

            ui->tabWidget->show();
}


void MainWindow::open_file(){
    QString filePath = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(),"Fichiers texte (*.txt);;Tous les fichiers (*)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            // Ouvrir nouvel onglet avec le fichier voulu
            QTextEdit *textEdit = new QTextEdit(ui->tabWidget);
            // Initier le texte original pour savoir si il y a modification
            QString originalText = in.readAll();
            textEdit->setPlainText(originalText);
            // Ajoutez l'onglet avec le texte du fichier
            int tabIndex = ui->tabWidget->addTab(textEdit, QFileInfo(filePath).fileName());
            // Initialisez la propriété "TabIndex" avec l'index de l'onglet
            textEdit->setProperty("TabIndex", tabIndex);
            // Initialisez la propriété "OriginalText" avec le texte original
            textEdit->setProperty("OriginalText", originalText);
            // Fermer le fichier d''origine
            file.close();

            // Connectez le signal textChanged au slot asterisk
            connect(textEdit, &QTextEdit::textChanged, this, &MainWindow::asterisk);
            // connecter le curseur au slot updatecursorposition
            connect(textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
            // connecter le curseur au slot Find & Replace
            connect(ui->action_Chercher, &QAction::triggered, this, &MainWindow::openSearch);
            connect(ui->action_Remplacer, &QAction::triggered, this, &MainWindow::openReplace);

            // Ajouter le chemin du fichier ouvert à la liste des fichiers récents
            recentFiles.prepend(filePath);

            // Limiter la liste à 10 fichiers récents
            if (recentFiles.size() > 10) {
                recentFiles.removeLast();
            }
            // Mettre à jour le sous-menu "Recents"
            this->updateRecentMenu();


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

void MainWindow::asterisk() {
    QTextEdit *textEdit = qobject_cast<QTextEdit*>(sender()); // Obtenez le QTextEdit émetteur du signal
    if (textEdit) {
        int tabIndex = textEdit->property("TabIndex").toInt(); // Obtenez le numéro de l'onglet
        QString tabText = ui->tabWidget->tabText(tabIndex); // Obtenez le texte actuel de l'onglet
        QString originalText = textEdit->property("OriginalText").toString(); // Récupérez le texte d'origine

        if (textEdit->toPlainText() == originalText) {
            // Retirez l'astérisque seulement si le texte actuel est le même que le texte d'origine
            if (tabText.endsWith('*')) {
                tabText.chop(1); // Supprimez l'astérisque à la fin
                ui->tabWidget->setTabText(tabIndex, tabText);
            }
        } else {
            // Ajoutez un astérisque si le texte actuel est différent du texte d'origine
            if (!tabText.endsWith('*')) {
                tabText += '*';
                ui->tabWidget->setTabText(tabIndex, tabText);
            }
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


void MainWindow::updateRecentMenu() {
    QMenu* recentsMenu = ui->menu_R_cent;
    if (recentsMenu) {
        // Effacer tous les éléments actuels du sous-menu "Recents"
        recentsMenu->clear();

        // Ajouter les fichiers récents en tant qu'actions dans le sous-menu
        for (int i = 0; i < recentFiles.size(); ++i) {
            QAction* recentFileAction = new QAction(recentFiles.at(i), this);
            connect(recentFileAction, &QAction::triggered, this, &MainWindow::openRecentFile);
            recentsMenu->addAction(recentFileAction);
        }
    }
}

void MainWindow::openRecentFile() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString filePath = action->text();
        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                // Ouvrir nouvel onglet avec le fichier voulu
                QTextEdit *textEdit = new QTextEdit(ui->tabWidget);
                // Initier le texte original pour savoir si il y a modification
                QString originalText = in.readAll();
                textEdit->setPlainText(originalText);
                // Ajoutez l'onglet avec le texte du fichier
                int tabIndex = ui->tabWidget->addTab(textEdit, QFileInfo(filePath).fileName());
                // Initialisez la propriété "TabIndex" avec l'index de l'onglet
                textEdit->setProperty("TabIndex", tabIndex);
                // Initialisez la propriété "OriginalText" avec le texte original
                textEdit->setProperty("OriginalText", originalText);
                // Fermer le fichier d''origine
                file.close();

                // Connectez le signal textChanged au slot asterisk
                connect(textEdit, &QTextEdit::textChanged, this, &MainWindow::asterisk);
                // connecter le curseur au slot updatecursorposition
                connect(textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
                // connecter le curseur au slot Find & Replace
                connect(ui->action_Chercher, &QAction::triggered, this, &MainWindow::openSearch);
                connect(ui->action_Remplacer, &QAction::triggered, this, &MainWindow::openReplace);

                // Ajouter le chemin du fichier ouvert à la liste des fichiers récents
                recentFiles.prepend(filePath);

                // Limiter la liste à 10 fichiers récents
                if (recentFiles.size() > 10) {
                    recentFiles.removeLast();
                }
                // Mettre à jour le sous-menu "Recents"
                this->updateRecentMenu();


            }ui->tabWidget->show();
        }
    }
 }
