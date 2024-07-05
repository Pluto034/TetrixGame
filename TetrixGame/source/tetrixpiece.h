#ifndef TETRIXPIECE_H
#define TETRIXPIECE_H

//这里没写class，需要隐式转换为int
enum TetrixShape { NoShape, ZShape, SShape, LineShape, TShape, SquareShape,
                    LShape,MirroredLShape};

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
    TetrixPiece rotatedLeft() const;
    TetrixPiece rotatedRight() const;
    int x(int index) const {return coords[index][0];}
    int y(int index) const {return coords[index][1];}
    TetrixShape shape(){return pieceShape;}
private:
    void setX(int index,int value){coords[index][0]=value;}
    void setY(int index,int value){coords[index][1]=value;}

    TetrixShape pieceShape;//抽象的描述
    int coords[4][2];//具体的描述
};

#endif // TETRIXPIECE_H
