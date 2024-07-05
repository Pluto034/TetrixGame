#include "TetrixBoard.h"
#include<QPainter>
TetrixBoard::TetrixBoard(QWidget *parent)
	: QFrame(parent)
{
	m_nextPiece.setRandomShape();
}

void TetrixBoard::setNextPieceLabel(QLabel * label)
{
	m_nextPieceLabel = label;
}

TetrixBoard::~TetrixBoard()
{}

void TetrixBoard::showNextPiece()
{
	if (m_nextPieceLabel == nullptr) return;

	int dx = m_nextPiece.maxX() - m_nextPiece.minX() + 1;
	int dy = m_nextPiece.maxY() - m_nextPiece.minY() + 1;

	QPixmap pixmap(dx * squareWidth() , dy * squareHeight() );
	QPainter painter(&pixmap);

	painter.fillRect(pixmap.rect(), QColor(3, 54, 73));

	m_nextPieceLabel->setPixmap(pixmap);
	for (int i = 0; i < 4; i++) {
		int x = m_nextPiece.x(i) - m_nextPiece.minX();
		int y = m_nextPiece.y(i) - m_nextPiece.minY();
		drawSquare(painter,x * squareWidth(), y * squareHeight(),m_nextPiece.shape());
	}
	m_nextPieceLabel->setPixmap(pixmap);
}

void TetrixBoard::removeFullLines()
{
}

void TetrixBoard::clearBoard()
{
	//for (int i = 0; i < BoardWidth; i++) {
	//	for (int j = 0; j < BoardHeight; j++) {
	//		m_boardBlocks[i][j] = NoShape;
	//	}
	//}
	for (int i = 0; i < BoardHeight * BoardWidth; i++) {
		m_boardBlocks[i] = NoShape;
	}
}

void TetrixBoard::newPiece()
{
	m_curPiece = m_nextPiece;
	m_nextPiece.setRandomShape();
	showNextPiece();
	m_curX = BoardWidth / 2;
	m_curY = BoardHeight - 1 + m_curPiece.minY();

	//把newPiece放入起始位置
	if(!tryMove(m_curPiece,m_curX,m_curY)) {
		//游戏结束
		timer.stop();
		m_isStarted = false;

	}
}

void TetrixBoard::drawSquare(QPainter& painter, int x, int y, TetrixShape shape)
{
	static constexpr QRgb colorTable[8]{
		0x000000,0xCC6666,0x66CC66,0x6666CC,
		0xCCCC66,0xCC66CC,0x66CCCC,0xDAAA00
	};
	
	QColor color = colorTable[shape];
	painter.fillRect(x + 1, y + 1, squareWidth() - 2, squareHeight()- 2,color);

	painter.setPen(color.lighter());

	painter.drawLine(x, y, x + squareWidth() - 1, y);
	painter.drawLine(x + squareWidth() - 1, y,x + squareWidth()-1,y + squareHeight()-1);
	painter.drawLine(x + squareWidth()-1, y + squareHeight()-1, x , y + squareHeight()-1);
	painter.drawLine(x, y + squareHeight() - 1,x, y);

}

void TetrixBoard::onLineDown()
{
	if (!tryMove(m_curPiece, m_curX, m_curY - 1)) {
		pieceDropped(0);
	}

}

bool TetrixBoard::tryMove(const TetrixPiece& newPiece, int newX, int newY)
{
	//判断可以移动
	for (int i = 0; i < 4; i++) {
		int x = newX + newPiece.x(i);
		int y = newY - newPiece.y(i);
		if (x < 0 || x >= BoardWidth || y < 0 || y >= BoardHeight) {
			return false;
		}
		if (shapeAt(x, y) != NoShape) {
			return false;
		}
	}
	m_curPiece = newPiece;//旋转时需要
	m_curX = newX;
	m_curY = newY;
	update(); 
	return true;
}
void TetrixBoard::pieceDropped(int dropHeight)
{
	for (int i = 0; i < 4; i++) {
		int x = m_curX + m_curPiece.x(i);
		int y = m_curY - m_curPiece.y(i);
		shapeAt(x, y) = m_curPiece.shape();
	}


	++m_numPieceDropped;
	if (m_numPieceDropped % NextLevelNumber == 0) {
		++m_level;
		timer.start(timeoutTime(), this);
		emit levelChanged(m_level);
	}

	m_score += dropHeight + DropSCore;
	emit scoreChanged(m_score);
	removeFullLines();
	newPiece();
}
void TetrixBoard::start()
{
	if (m_isStarted || m_isPaused) {
		return;
	}
	m_isStarted = true;
	m_numLineRemoved = 0;
	m_numPieceDropped = 0;
	m_score = 0;
	m_level = 1;
	clearBoard();

	emit linesRemoved(m_numLineRemoved);
	emit scoreChanged(m_score);
	emit levelChanged(m_level);

	newPiece();
	timer.start(timeoutTime(),this) ;
}
void TetrixBoard::pause()
{
}

void TetrixBoard::paintEvent(QPaintEvent* event)
{
	QFrame::paintEvent(event);

	QPainter painter(this);
	QRect rect = contentsRect();

	if (m_isPaused) {
		painter.drawText(rect, Qt::AlignCenter, "暂停中...");
		return;
	}
	int boardTop = rect.bottom() - BoardHeight * squareHeight();
	//绘制现有的方块，把整个TetrixBoard绘制一下
	for (int i = 0; i < BoardHeight; ++i)
		for (int j = 0; j < BoardWidth; ++j) {
			TetrixShape shape = shapeAt(j, i/*BoardHeight-i-1*/);
			if (shape != NoShape)
				drawSquare(painter, rect.left() + j * squareWidth(),
					rect.bottom() - (i + 1) * squareHeight()/*rect.top()+i*squareHeight()*/, shape);
		}
	//绘制正在下落的方块，newPiece,curPiece
	if (m_curPiece.shape() != NoShape) {
		for (int i = 0; i < 4; ++i) {
			int x = m_curX + m_curPiece.x(i);
			int y = m_curY - m_curPiece.y(i);
			//x,y单位是小块，draw里用的是像素
			drawSquare(painter, rect.left() + x * squareWidth()
				, boardTop + (BoardHeight - y - 1) * squareHeight()
				, m_curPiece.shape());
		}
	}
}

void TetrixBoard::timerEvent(QTimerEvent* event)
{
	if (m_isStarted) {
		onLineDown();
		update();
	}
}

