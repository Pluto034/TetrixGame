#include "tetrixboard.h"
#include <QLabel>
#include <QPainter>
#include <QKeyEvent>
TetrixBoard::TetrixBoard(QWidget *parent):QFrame(parent)
{
    nextPiece.setRandomShape();
    clearBoard();
    setFocusPolicy(Qt::StrongFocus);

    bgMusic.setAudioOutput(&audioOutput);
    audioOutput.setVolume(0.4);
    bgMusic.setSource(QUrl("qrc:/sounds/tetrix.mp3"));
    bgMusic.setLoops(QMediaPlayer::Infinite);

    removeSound.setSource(QUrl("qrc:/sounds/remove.wav"));
    downSound.setSource(QUrl("qrc:/sounds/down.wav"));
    gameOverSound.setSource(QUrl("qrc:/sounds/gameover.wav"));
}

void TetrixBoard::setNextPieceLabel(QLabel *label)
{
    nextPieceLabel=label;
}

void TetrixBoard::showNextPiece()
{
    if(!nextPieceLabel) return;
    //单位：小块
    int dx=nextPiece.maxX()-nextPiece.minX()+1;
    int dy=nextPiece.maxY()-nextPiece.minY()+1;
    //单位：像素
    QPixmap pixmap(dx*squareWidth(),dy*squareHeight());
    QPainter painter(&pixmap);
    painter.fillRect(pixmap.rect(),QColor(3, 54, 73));
    //绘制四个小块
    for(int i=0;i<4;++i){
        int x=nextPiece.x(i)-nextPiece.minX();
        int y=nextPiece.y(i)-nextPiece.minY();
        drawSquare(painter,x*squareWidth(),y*squareHeight()
                   ,nextPiece.shape());
    }
    nextPieceLabel->setPixmap(pixmap);
}

void TetrixBoard::newPiece()
{
    curPiece=nextPiece;
    nextPiece.setRandomShape();
    showNextPiece();
    curX=BoardWidth/2;
    curY=BoardHeight-1+curPiece.minY();

    //尝试把newPiece（curPiece）摆放到起始位置（中间，上面）
    if(!tryMove(curPiece,curX,curY)){
        //游戏结束
        timer.stop();
        isStarted=false;
        bgMusic.stop();
        gameOverSound.play();
    }
}

void TetrixBoard::start()
{
    if(isStarted||isPaused)
        return;

    isStarted=true;
    bgMusic.play();
    numLinesRemoved=0;
    numPiecesDropped=0;
    score=0;
    level=1;
    clearBoard();

    emit linesRemovedChanged(numLinesRemoved);
    emit scoreChanged(score);
    emit levelChanged(level);

    newPiece();
    timer.start(timeoutTime(),this);
}

void TetrixBoard::pause()
{
    if(!isStarted)
        return;

    isPaused=!isPaused;
    if(isPaused){
        timer.stop();
        bgMusic.stop();
    }
    else{
        timer.start(timeoutTime(),this);
        bgMusic.play();
    }

    update();
}

bool TetrixBoard::tryMove(const TetrixPiece &newPiece, int newX, int newY)
{
    //判定是否可以移动
    for(int i=0; i<4; ++i){
        int x=newX+newPiece.x(i);
        int y=newY-newPiece.y(i);
        if(x<0||x>=BoardWidth||y<0||y>=BoardHeight)
            return false;
        //是否有其他方块
        if(shapeAt(x,y)!=NoShape)
            return false;
    }
    //确定可以移动，更新当前的方块
    curPiece=newPiece;//旋转时，需要
    curX=newX;
    curY=newY;
    update();
    return true;
}

void TetrixBoard::clearBoard()
{
    for(int i=0;i<BoardWidth*BoardHeight;++i)
        boardBBlocks[i]=NoShape;
}

void TetrixBoard::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    QRect rect=contentsRect();

    if(isPaused){
        painter.drawText(rect,Qt::AlignCenter,"暂停中...");
        return;
    }
    int boardTop=rect.bottom()-BoardHeight*squareHeight();
    //绘制现有的方块，把整个TetrixBoard绘制一下
    for(int i=0;i<BoardHeight;++i)
        for(int j=0;j<BoardWidth;++j){
            TetrixShape shape=shapeAt(j,i/*BoardHeight-i-1*/);
            if(shape!=NoShape)
              drawSquare(painter,rect.left()+j*squareWidth(),
                        rect.bottom()-(i+1)*squareHeight()/*rect.top()+i*squareHeight()*/,shape );
        }
    //绘制正在下落的方块，newPiece,curPiece
    if(curPiece.shape()!=NoShape){
        for(int i=0;i<4;++i){
            int x=curX+curPiece.x(i);
            int y=curY-curPiece.y(i);
            //x,y单位是小块，draw里用的是像素
            drawSquare(painter,rect.left()+x*squareWidth()
                       ,boardTop+(BoardHeight-y-1)*squareHeight()
                       ,curPiece.shape());
        }
    }
}

void TetrixBoard::drawSquare(QPainter &painter, int x, int y, TetrixShape shape)
{
    static constexpr QRgb colorTable[8]{
        0x000000,0xCC6666,0x66CC66,0x6666CC,
        0xCCCC66,0xCC66CC,0x66CCCC,0xDAAA00
    };
    QColor color=colorTable[shape];
    painter.fillRect(x+1,y+1,squareWidth()-2,squareHeight()-2,color);
    //描边
    painter.setPen(color.lighter());
    painter.drawLine(x,y,x+squareWidth(),y);
    painter.drawLine(x+squareWidth()-1,y,x+squareWidth()-1,y+squareHeight());
    painter.setPen(color.darker());
    painter.drawLine(x+squareWidth(),y+squareHeight()-1,x,y+squareHeight()-1);
    painter.drawLine(x,y+squareHeight(),x,y);
}

void TetrixBoard::oneLineDown()
{
    if(!tryMove(curPiece,curX,curY-1))
        pieceDropped(0);
}

void TetrixBoard::pieceDropped(int dropHeight)
{
    downSound.play();
    for(int i=0;i<4;++i){
        int x=curX+curPiece.x(i);
        int y=curY-curPiece.y(i);
        shapeAt(x,y)=curPiece.shape();//更新boardBlocks数组
    }

    ++numPiecesDropped;
    if(numPiecesDropped%NextLevelNumber==0){
        ++level;
        timer.start(timeoutTime(),this);
        emit levelChanged(level);
    }

    score+=dropHeight+DropScore;//打分规则
    emit scoreChanged(score);
    removeFullLines();
    newPiece();
}

void TetrixBoard::keyPressEvent(QKeyEvent *event)
{
    if(!isStarted||isPaused||curPiece.shape()==NoShape){
        QFrame::keyPressEvent(event);
        return;
    }

    switch(event->key()){
    case Qt::Key_Left:
        tryMove(curPiece,curX-1,curY);break;
    case Qt::Key_Right:
        tryMove(curPiece,curX+1,curY);break;
    case Qt::Key_Down:
        tryMove(curPiece.rotatedRight(),curX,curY);break;
    case Qt::Key_Up:
        tryMove(curPiece.rotatedLeft(),curX,curY);break;
    case Qt::Key_D://立即下降一行
        oneLineDown();break;
    case Qt::Key_Space://直接下落
        dropDown();break;
    default:
        QFrame::keyPressEvent(event);
    }
}

void TetrixBoard::removeFullLines()
{
    int numFullLines=0;
    //消除行s
    for(int i=BoardHeight-1;i>=0;--i){
        bool lineIsFull=true;
        //扫描一行
        for(int j=0;j<BoardWidth;++j)
            if(shapeAt(j,i)==NoShape){
                lineIsFull=false;
                break;
            }
        if(lineIsFull){
            //消除一行
            removeSound.play();
            ++numFullLines;
            for(int k=i;k<BoardHeight-1;++k){
                for(int j=0;j<BoardWidth;++j)
                    shapeAt(j,k)=shapeAt(j,k+1);
            }
            //最高处可能原先也有形状
            for(int j=0;j<BoardWidth;++j)
                shapeAt(j,BoardHeight-1)=NoShape;
        }
    }

    //打分
    if(numFullLines>0){
        numLinesRemoved+=numFullLines;
        score+=10*numFullLines;
        emit linesRemovedChanged(numLinesRemoved);
        emit scoreChanged(score);
    }
}

void TetrixBoard::dropDown()
{
    int newY=curY;
    int dropHeight=0;
    while(newY>0){
        if(!tryMove(curPiece,curX,newY-1))
            break;

        --newY;
        ++dropHeight;
    }
    pieceDropped(dropHeight);
}

void TetrixBoard::timerEvent(QTimerEvent *event)
{
    if(isStarted){
        //播放音乐
        oneLineDown();//下移一行
    }
}


