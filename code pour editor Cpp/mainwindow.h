#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCursor>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString textofind;


private slots:
    void open_file();
    void closeTab(int index);
    void save_file();
    void asterisk();
    void updateCursorPosition();
    void openSearch();
    void handletext_find();
    void openReplace();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
