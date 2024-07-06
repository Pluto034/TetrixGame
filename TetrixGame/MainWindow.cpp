#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint); // 无边框无法移动
    setAttribute(Qt::WA_TranslucentBackground, true); // 隐藏没有空间的窗体
    ui.frameTerixBoard->setNextPieceLabel(ui.labelNextPiece);
    connect(ui.pbStart, &QPushButton::clicked, ui.frameTerixBoard, &TetrixBoard::start);
    connect(ui.pbPause, &QPushButton::clicked, ui.frameTerixBoard, &TetrixBoard::pause);
    connect(ui.frameTerixBoard, &TetrixBoard::scoreChanged, ui.lcdScore,qOverload<int>( & QLCDNumber::display));
    connect(ui.frameTerixBoard, &TetrixBoard::levelChanged, ui.lcdLevel,qOverload<int>( & QLCDNumber::display));
    connect(ui.frameTerixBoard, &TetrixBoard::linesRemoved, ui.lcdRemoved,qOverload<int>( & QLCDNumber::display));

    ui.pbStart->setFocusPolicy(Qt::NoFocus);
    ui.pbPause->setFocusPolicy(Qt::NoFocus);
    ui.pbExit->setFocusPolicy(Qt::NoFocus);

    

}

MainWindow::~MainWindow()
{}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint mousePos = event->globalPos();
        QPoint topLeft = geometry().topLeft();
        m_winPos = mousePos - topLeft;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    m_winPos = QPoint();
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    QPoint mousePos = event->globalPos();
    QPoint mousePosDect = event->pos();

    QPoint endPos = mousePos - m_winPos;
    move(endPos);
}

void MainWindow::on_pbPause_clicked()
{
    if (ui.pbPause->text() == QString(u8"暂 停")) {
        ui.pbPause->setText(QString(u8"恢 复"));
    }
    else {
        ui.pbPause->setText(QString(u8"暂 停"));
    }
}
