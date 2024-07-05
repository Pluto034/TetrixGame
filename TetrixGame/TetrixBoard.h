#pragma once

#include<QFrame>
#include <QLabel>
#include<QPointer>
#include<QPainter>
#include"tetrixpiece.h"
#include<qbasictimer.h>
class TetrixBoard  : public QFrame
{
	Q_OBJECT

public:
	TetrixBoard(QWidget*parent = nullptr);
	void setNextPieceLabel(QLabel* label);
	void showNextPiece();
	void newPiece();
	~TetrixBoard();
public slots:
	void start();
	void pause();
signals:
	void levelChanged(int level);
	void scoreChanged(int socre);
	void linesRemoved(int numLines);
protected:
	void paintEvent(QPaintEvent* event) override;
	void timerEvent(QTimerEvent* event) override;
private:
	int squareWidth() {
		return contentsRect().width() / BoardWidth;
	}
	int squareHeight() {
		return contentsRect().height() / BoardHeight;
	}
	//TetrixShape& shapeAt(int x, int y) {
	//	return m_boardBlocks[x][y];
	//}
	TetrixShape& shapeAt(int x, int y) {
		return m_boardBlocks[y * BoardWidth + x];
	}
	int timeoutTime() {
		return 300 / (1 + m_level);
	}
	void removeFullLines();
	void clearBoard();
	void drawSquare(QPainter& painter, int x, int y, TetrixShape shape);
	void onLineDown();
	bool tryMove(const TetrixPiece &newPiece,int newX,int newY);
	void pieceDropped(int dropHeight);
private:
	enum {
		DropSCore = 7, BoardWidth = 12,
		BoardHeight = 22, NextLevelNumber = 25
	};
	QPointer<QLabel> m_nextPieceLabel;
	TetrixPiece m_nextPiece;
	TetrixPiece m_curPiece;
	int m_curX;
	int m_curY;
	TetrixShape m_boardBlocks[BoardWidth * BoardHeight];
	bool m_isStarted = false;
	bool m_isPaused = false;
	int m_numLineRemoved = 0;
	int m_numPieceDropped = 0;
	int m_score = 0;
	int m_level = 1;
	QBasicTimer timer;

};
