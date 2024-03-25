#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileSystemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFileSystemModel *treeModel = new QFileSystemModel(this);
    treeModel->setRootPath(QDir::rootPath());

    ui->treeView->setModel(treeModel);
    ui->treeView->setRootIndex(treeModel->index(QDir::rootPath()));

    connect(ui->treeView, &QTreeView::activated, this, &MainWindow::updateListView);

    connect(ui->createFileButton, &QPushButton::clicked, this, &MainWindow::createFile);
    connect(ui->createFolderButton, &QPushButton::clicked, this, &MainWindow::createFolder);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::deleteItem);
    connect(ui->renameButton, &QPushButton::clicked, this, &MainWindow::renameItem);
    connect(ui->moveButton, &QPushButton::clicked, this, &MainWindow::moveItem);
    connect(ui->copyButton, &QPushButton::clicked, this, &MainWindow::copyItem);
    connect(ui->listView, &QListView::doubleClicked, this, &MainWindow::openItem);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateListView(const QModelIndex &index)
{
    QString path = ui->treeView->model()->data(index, QFileSystemModel::FilePathRole).toString();

    QFileSystemModel *listModel = new QFileSystemModel(this);
    listModel->setRootPath(path);

    ui->listView->setModel(listModel);
    ui->listView->setRootIndex(listModel->index(path));
}

void MainWindow::createFile()
{
    QModelIndex index = ui->treeView->currentIndex();
    QString currentPath = ui->treeView->model()->data(index, QFileSystemModel::FilePathRole).toString();

    bool ok;
    QString fileName = QInputDialog::getText(this, tr("Create File"), tr("Enter file name:"), QLineEdit::Normal, "", &ok);
    if (ok && !fileName.isEmpty()) {
        QFile file(currentPath + "/" + fileName);
        if (!file.exists()) {
            if (file.open(QIODevice::WriteOnly)) {
                file.close();
                updateListView(index);
            } else {
                QMessageBox::critical(this, "Error", "Could not create file.");
            }
        } else {
            QMessageBox::information(this, "Info", "File already exists.");
        }
    }
}

void MainWindow::createFolder()
{
    QModelIndex index = ui->treeView->currentIndex();
    QString currentPath = ui->treeView->model()->data(index, QFileSystemModel::FilePathRole).toString();

    bool ok;
    QString folderName = QInputDialog::getText(this, tr("Create Folder"), tr("Enter folder name:"), QLineEdit::Normal, "", &ok);
    if (ok && !folderName.isEmpty()) {
        QDir dir;
        if (dir.mkdir(currentPath + "/" + folderName)) {
            updateListView(index);
        } else {
            QMessageBox::critical(this, "Error", "Could not create folder.");
        }
    }
}


void MainWindow::deleteItem()
{
    QModelIndex index = ui->listView->currentIndex();
    QString itemPath = ui->listView->model()->data(index, QFileSystemModel::FilePathRole).toString();

    if (index.isValid()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirmation", "Are you sure you want to delete this item?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            if (QFile::exists(itemPath)) {
                QFile::remove(itemPath);
            } else if (QDir(itemPath).exists()) {
                if (QDir().rmdir(itemPath)) {
                    updateListView(index.parent());
                } else {
                    QMessageBox::critical(this, "Error", "Could not delete folder.");
                }
            }
        } else {
            qDebug() << "Delete operation canceled.";
        }
    } else {
        QMessageBox::information(this, "Info", "No item selected.");
    }
}

void MainWindow::renameItem()
{
    QModelIndex index = ui->listView->currentIndex();
    QString itemPath = ui->listView->model()->data(index, QFileSystemModel::FilePathRole).toString();

    if (index.isValid()) {
        bool ok;
        QString newName = QInputDialog::getText(this, tr("Rename Item"), tr("Enter new name:"), QLineEdit::Normal, "", &ok);
        if (ok && !newName.isEmpty()) {
            QString newPath = itemPath.section('/', 0, -2) + "/" + newName;
            if (QFile::rename(itemPath, newPath)) {
                updateListView(index.parent());
            } else {
                QMessageBox::critical(this, "Error", "Could not rename item.");
            }
        }
    } else {
        QMessageBox::information(this, "Info", "No item selected.");
    }
}

void MainWindow::moveItem()
{
    QModelIndex sourceIndex = ui->listView->currentIndex();
    QString sourcePath = ui->listView->model()->data(sourceIndex, QFileSystemModel::FilePathRole).toString();

    if (sourceIndex.isValid()) {
        QString destPath = QFileDialog::getExistingDirectory(this, tr("Select Destination Folder"), QDir::homePath());
        if (!destPath.isEmpty()) {
            QString fileName = sourcePath.section('/', -1);
            QString destFilePath = destPath + "/" + fileName;

            if (QFile::exists(destFilePath)) {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "Confirmation", "A file or folder with the same name already exists in the destination folder. Do you want to overwrite it?",
                                              QMessageBox::Yes|QMessageBox::No);
                if (reply == QMessageBox::No) {
                    return;
                }
            }

            if (QFile::rename(sourcePath, destFilePath)) {
                updateListView(sourceIndex.parent());
            } else {
                QMessageBox::critical(this, "Error", "Could not move item.");
            }
        }
    } else {
        QMessageBox::information(this, "Info", "No item selected.");
    }
}

void MainWindow::copyItem()
{
    QModelIndex sourceIndex = ui->listView->currentIndex();
    QString sourcePath = ui->listView->model()->data(sourceIndex, QFileSystemModel::FilePathRole).toString();

    if (sourceIndex.isValid()) {
        QString destPath = QFileDialog::getExistingDirectory(this, tr("Select Destination Folder"), QDir::homePath());
        if (!destPath.isEmpty()) {
            QString fileName = sourcePath.section('/', -1); // Obtenez le nom du fichier ou du dossier
            QString destFilePath = destPath + "/" + fileName;

            if (QFile::exists(destFilePath)) {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "Confirmation", "A file or folder with the same name already exists in the destination folder. Do you want to overwrite it?",
                                              QMessageBox::Yes|QMessageBox::No);
                if (reply == QMessageBox::No) {
                    return; // Sortie de la fonction si l'utilisateur ne veut pas écraser le fichier/dossier existant
                }
            }

            QFileInfo fileInfo(sourcePath);
            if (fileInfo.isDir()) {
                QDir sourceDir(sourcePath);
                QDir destDir(destFilePath);

                if (!destDir.exists() && !destDir.mkpath(".")) {
                    QMessageBox::critical(this, "Error", "Could not create destination directory.");
                    return;
                }

                if (sourceDir.exists() && destDir.exists()) {
                    if (!QFile::copy(sourcePath, destFilePath)) {
                        QMessageBox::critical(this, "Error", "Could not copy folder.");
                    }
                } else {
                    QMessageBox::critical(this, "Error", "Could not copy folder: Source or destination directory does not exist.");
                }
            } else {
                if (!QFile::copy(sourcePath, destFilePath)) {
                    QMessageBox::critical(this, "Error", "Could not copy file.");
                }
            }

            // Mettre à jour la vue après la copie
            updateListView(sourceIndex.parent());
        }
    } else {
        QMessageBox::information(this, "Info", "No item selected.");
    }
}

void MainWindow::openItem(const QModelIndex &index)
{
    QString filePath = ui->listView->model()->data(index, QFileSystemModel::FilePathRole).toString();

    QFileInfo fileInfo(filePath);
    if (fileInfo.isFile()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}

