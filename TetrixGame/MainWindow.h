#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include"TetrixBoard.h"
#include<QMouseEvent>
#include<qDebug>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
private slots:
    void on_pbPause_clicked();
private:
    Ui::MainWindowClass ui;
    QPoint m_winPos;
};
