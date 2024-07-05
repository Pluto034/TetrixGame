#ifndef TETRIXBOARD_H
#define TETRIXBOARD_H

#include <QFrame>
#include <QPointer>
#include "tetrixpiece.h"
#include <QBasicTimer>
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QAudioOutput>

class QLabel;

class TetrixBoard : public QFrame
{
    Q_OBJECT
public:
    TetrixBoard(QWidget* parent = nullptr);
    void setNextPieceLabel(QLabel* label);
public slots:
    void start();
    void pause();
signals:
    void levelChanged(int level);
    void scoreChanged(int score);
    void linesRemovedChanged(int numLines);

private:
    enum{
        DropScore=7,BoardWidth=12,BoardHeight=22,NextLevelNumber=25
    };
    int squareWidth(){return contentsRect().width()/BoardWidth;}
    int squareHeight(){return contentsRect().height()/BoardHeight;}

    bool tryMove(const TetrixPiece &newPiece,int newX, int newY);
    TetrixShape& shapeAt(int x,int y){
        return boardBBlocks[y*BoardWidth+x];
    }
    void clearBoard();
    void showNextPiece();
    //绘制小块
    void drawSquare(QPainter& painter,int x, int y, TetrixShape shape);
    void oneLineDown();
    void pieceDropped(int dropHeight);
    int timeoutTime(){return 300/(1+level);}
    void removeFullLines();
    void dropDown();
    void newPiece();
private:
    QPointer<QLabel> nextPieceLabel;
    TetrixPiece nextPiece;
    TetrixPiece curPiece;
    int curX;
    int curY;
    TetrixShape boardBBlocks[BoardWidth*BoardHeight];
    bool isStarted=false;
    bool isPaused=false;
    int numLinesRemoved=0;
    int numPiecesDropped=0;
    int score=0;
    int level=1;
    QBasicTimer timer;

    QMediaPlayer bgMusic;
    QAudioOutput audioOutput;
    QSoundEffect removeSound;
    QSoundEffect downSound;
    QSoundEffect gameOverSound;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // TETRIXBOARD_H
