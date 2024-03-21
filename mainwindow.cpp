#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileSystemModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Créer un modèle pour l'arborescence des répertoires
    QFileSystemModel *model = new QFileSystemModel(this);
    model->setRootPath(QDir::rootPath());

    // Afficher l'arborescence des répertoires dans le QTreeView
    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(model->index(QDir::rootPath()));
    ui->treeView->setHeaderHidden(true); // Masquer l'en-tête du QTreeView
}

MainWindow::~MainWindow()
{
    delete ui;
}
