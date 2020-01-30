#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>  // unique_ptr
#include <QtCore/QObject>  // Q_OBJECT
#include <QtGui/QMainWindow>

#include "bowlingitemmodel.h"  // BowlingItemModel

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();  // = default
    void init();

private:
    void showStatusbarMessage(const QString& message) const;

    // button handlers
    Q_SLOT void on_addButton_clicked();
    Q_SLOT void on_removeButton_clicked();
    Q_SLOT void on_newButton_clicked();
    Q_SLOT void on_openButton_clicked();
    Q_SLOT void on_saveButton_clicked();

    const int kMaxPlayerCount = 5;

	// score model
    BowlingItemModel model;

    std::unique_ptr<Ui::MainWindow> ui;
};

#endif  // MAINWINDOW_H
