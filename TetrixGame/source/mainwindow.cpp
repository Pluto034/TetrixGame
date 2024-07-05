#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include "tetrixboard.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);//无边框，无法移动
    setAttribute(Qt::WA_TranslucentBackground,true);
    ui->frameTetrixBoard->setNextPieceLabel(ui->labelNextPiece);
    connect(ui->pbStart,&QPushButton::clicked,
            ui->frameTetrixBoard,&TetrixBoard::start);
    connect(ui->pbPause,&QPushButton::clicked,
            ui->frameTetrixBoard,&TetrixBoard::pause);

    connect(ui->frameTetrixBoard,&TetrixBoard::scoreChanged,
            ui->lcdScore,qOverload<int>(&QLCDNumber::display));
    connect(ui->frameTetrixBoard,&TetrixBoard::levelChanged,
            ui->lcdLevel,qOverload<int>(&QLCDNumber::display));
    connect(ui->frameTetrixBoard,&TetrixBoard::linesRemovedChanged,
            ui->lcdRemoved,qOverload<int>(&QLCDNumber::display));
    ui->pbStart->setFocusPolicy(Qt::NoFocus);
    ui->pbPause->setFocusPolicy(Qt::NoFocus);
    ui->pbExit->setFocusPolicy(Qt::NoFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::mousePressEvent(QMouseEvent *event)
{
   QPoint mousePos=event->globalPosition().toPoint();//鼠标相对于整个屏幕
   QPoint topLeft=geometry().topLeft();//窗口相对于整个屏幕
   winPos=mousePos-topLeft;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    winPos=QPoint();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos=event->globalPosition().toPoint();//鼠标相对于整个屏幕
    QPoint endPos=mousePos-winPos;
    move(endPos);
}

void MainWindow::on_pbPause_clicked()
{
    if(ui->pbPause->text()==QString("暂 停"))
        ui->pbPause->setText(QString("恢 复"));
    else
        ui->pbPause->setText(QString("暂 停"));
}

