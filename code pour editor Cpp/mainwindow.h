#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCursor>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void save_file();
    void asterisk();
    void updateCursorPosition();
    void closeTab(int index);
    void openSearch();
    void openReplace();
    void hideTab();
    void search_in_text();
    void next_in_text();
    void replace_in_text();
    void updateRecentMenu();
    void open_file();
    void openRecentFile();
    void new_file();
private:
    Ui::MainWindow *ui;
    QTextCursor lastFoundCursor;
    QStringList recentFiles;
};
#endif // MAINWINDOW_H
