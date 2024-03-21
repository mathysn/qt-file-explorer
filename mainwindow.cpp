#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileSystemModel>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

    // Créer un modèle pour afficher le contenu du répertoire racine dans le QListView
    QFileSystemModel *listModel = new QFileSystemModel(this);
    listModel->setRootPath(QDir::rootPath());

    // Afficher le contenu du répertoire racine dans le QListView
    ui->listView->setModel(listModel);
    ui->listView->setRootIndex(listModel->index(QDir::rootPath()));

    // Ajouter des actions pour les opérations sur les fichiers et dossiers dans la barre d'outils
    QAction *createFileAction = new QAction(QIcon(":/icons/create_file.png"), tr("Create File"), this);
    connect(createFileAction, &QAction::triggered, this, &MainWindow::createFile);
    ui->toolBar->addAction(createFileAction);

    QAction *createFolderAction = new QAction(QIcon(":/icons/create_folder.png"), tr("Create Folder"), this);
    connect(createFolderAction, &QAction::triggered, this, &MainWindow::createFolder);
    ui->toolBar->addAction(createFolderAction);

    QAction *deleteAction = new QAction(QIcon(":/icons/delete.png"), tr("Delete"), this);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteItem);
    ui->toolBar->addAction(deleteAction);
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

void MainWindow::createFile()
{
    // Récupérer le chemin du répertoire actuellement sélectionné dans le QListView
    QModelIndex index = ui->listView->currentIndex();
    QString path = ui->listView->model()->data(index, QFileSystemModel::FilePathRole).toString();

    // Demander à l'utilisateur de saisir le nom du nouveau fichier
    QString fileName = QInputDialog::getText(this, tr("Create File"), tr("Enter file name:"));

    if (!fileName.isEmpty()) {
        QString filePath = QDir(path).filePath(fileName);
        QFile file(filePath);
        if (!file.exists()) {
            if (file.open(QIODevice::WriteOnly)) {
                file.close();
                QMessageBox::information(this, tr("Success"), tr("File created successfully."));
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Failed to create file."));
            }
        } else {
            QMessageBox::warning(this, tr("Error"), tr("A file with the same name already exists."));
        }
    }
}

void MainWindow::createFolder()
{
    // Récupérer le chemin du répertoire actuellement sélectionné dans le QListView
    QModelIndex index = ui->listView->currentIndex();
    QString path = ui->listView->model()->data(index, QFileSystemModel::FilePathRole).toString();

    // Demander à l'utilisateur
    QString folderName = QInputDialog::getText(this, tr("Create Folder"), tr("Enter folder name:"));

    if (!folderName.isEmpty()) {
        QString folderPath = QDir(path).filePath(folderName);
        QDir dir;
        if (dir.mkpath(folderPath)) {
            QMessageBox::information(this, tr("Success"), tr("Folder created successfully."));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to create folder."));
        }
    }
}

void MainWindow::deleteItem()
{
    // Récupérer le chemin de l'élément actuellement sélectionné dans le QListView
    QModelIndex index = ui->listView->currentIndex();
    QString filePath = ui->listView->model()->data(index, QFileSystemModel::FilePathRole).toString();
    // Afficher une boîte de dialogue de confirmation pour supprimer l'élément
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Delete"), tr("Are you sure you want to delete this item?"),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QFileInfo fileInfo(filePath);
        if (fileInfo.isDir()) {
            QDir dir(filePath);
            if (dir.removeRecursively()) {
                QMessageBox::information(this, tr("Success"), tr("Folder deleted successfully."));
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Failed to delete folder."));
            }
        } else {
            QFile file(filePath);
            if (file.remove()) {
                QMessageBox::information(this, tr("Success"), tr("File deleted successfully."));
            } else {
                QMessageBox::warning(this, tr("Error"), tr("Failed to delete file."));
            }
        }
    }
}





