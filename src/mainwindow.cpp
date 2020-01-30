#include "mainwindow.h"

#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>

#include "ui_mainwindow.h"  // Ui::MainWindow

MainWindow::MainWindow() :
    ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() = default;

void MainWindow::init()
{
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setModel(&model);
}

void MainWindow::on_newButton_clicked()
{
    model.clear();
    ui->statusbar->showMessage(QObject::tr("Table cleared"), 1000);
}

void MainWindow::showStatusbarMessage(const QString& message) const
{
    ui->statusbar->showMessage(message, 1000);
}

void MainWindow::on_addButton_clicked()
{
    if (model.playerCount() < kMaxPlayerCount)
    {
        showStatusbarMessage(QObject::tr("Maximum number of players reached"));
        return;
    }

    const QString name(QInputDialog::getText(this,
                                             QObject::tr("Add Player"),
                                             QObject::tr("Name")));
    if (!name.isNull())
    {
        model.addPlayer(name);
        showStatusbarMessage(QObject::tr("Added %1").arg(name));
    }
    else
    {
        showStatusbarMessage(QObject::tr("No name specified"));
    }
}

Q_SLOT void MainWindow::on_removeButton_clicked()
{
    model.removePlayer();
    showStatusbarMessage(QObject::tr("Removed last added player"));
}

void MainWindow::on_openButton_clicked()
{
    const QString filename(
        QFileDialog::getOpenFileName(this,
                                     QObject::tr("Open File"),
                                     QCoreApplication::applicationDirPath(),
                                     QObject::tr("CSV Tables (*.csv)")));
    if (!filename.isNull())
    {
        model.readFromCsv(filename);
    }
}

void MainWindow::on_saveButton_clicked()
{
    const QString filename(
        QFileDialog::getSaveFileName(this,
                                     QObject::tr("Open File"),
                                     QCoreApplication::applicationDirPath(),
                                     QObject::tr("CSV Tables (*.csv)")));
    if (!filename.isNull())
    {
        model.saveToCsv(filename);
    }
}
