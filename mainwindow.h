#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void openItem(const QModelIndex &index);

private slots:
    void updateListView(const QModelIndex &index);
    void createFile();
    void createFolder();
    void deleteItem();
    void renameItem();
    void moveItem();
    void copyItem();
    void searchTextChanged(const QString &text);
    void updateTreeView(const QString &text);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
