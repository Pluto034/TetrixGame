#pragma once
#
enum TetrixShape {
	NoShape, ZShape, SShape, LineShape, TShape, SquareShape, LShape, MirroredLShape
};
class TetrixPiece
{
public:
	TetrixPiece();
	void setShape(TetrixShape shape);
	void setRandomShape();
	int minX() const;
	int maxX() const;
	int minY() const;
	int maxY() const;
	int x(int index) const {
		return m_coords[index][0];
	}
	int y(int index) const {
		return m_coords[index][1];
	}
	TetrixShape shape() const {
		return m_pieceShape;
	}
	TetrixPiece rotatedLeft() const;
	TetrixPiece rotatedRight() const;
private:
	void setX(int index, int value) {
		m_coords[index][0] = value;
	}
	void setY(int index, int value) {
		m_coords[index][1] = value;
	}

	TetrixShape m_pieceShape;
	int m_coords[4][2];

};

