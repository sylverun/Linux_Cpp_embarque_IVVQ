#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCursor>
#include <QString>
#include <QSettings>

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
    void openReplace();
    void openRecentFile();
private:
    Ui::MainWindow *ui;
    QSettings settings;

};
#endif // MAINWINDOW_H
