#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileSystemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Créer un modèle pour l'arborescence des répertoires
    QFileSystemModel *treeModel = new QFileSystemModel(this);
    treeModel->setRootPath(QDir::rootPath());

    // Afficher l'arborescence des répertoires dans le QTreeView
    ui->treeView->setModel(treeModel);
    ui->treeView->setRootIndex(treeModel->index(QDir::rootPath()));
    ui->treeView->setHeaderHidden(true); // Masquer l'en-tête du QTreeView

    // Connecter le signal itemActivated du QTreeView à la fonction updateListView
    connect(ui->treeView, &QTreeView::activated, this, &MainWindow::updateListView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateListView(const QModelIndex &index)
{
    // Récupérer le chemin du répertoire sélectionné dans le QTreeView
    QString path = ui->treeView->model()->data(index, QFileSystemModel::FilePathRole).toString();

    // Créer un nouveau modèle pour afficher le contenu du répertoire sélectionné
    QFileSystemModel *listModel = new QFileSystemModel(this);
    listModel->setRootPath(path);

    // Afficher le contenu du répertoire sélectionné dans le QListView
    ui->listView->setModel(listModel);
    ui->listView->setRootIndex(listModel->index(path));
}
