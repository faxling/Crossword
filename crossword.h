#ifndef CROSSWORD_H
#define CROSSWORD_H
#include <QMap>
#include <QSet>
#include <QString>
#include <QVector>

class CrossWord {
public:
  ~CrossWord();
  using WordIndex = int;
  using PInt = std::pair<int, int>;
  using Vec = QVector<QString>;

  // Second Index of word in m_ocWordList first horizontal question  second
  // vertical
  struct QuestionSquare {
    QuestionSquare() {
      HorizontalQ = -1;
      VerticalQ = -1;
    }
    WordIndex HorizontalQ;
    WordIndex VerticalQ;
  };
  using QuestionMapType = QMap<PInt, QuestionSquare>;

  using Set = QSet<WordIndex>;
  using Vec1D = QVector<QChar>;
  using Vec2D = std::pair<QVector<Vec1D>, QuestionMapType>;

  CrossWord(int nW, int nH);
  void AssignWordList(const Vec&);
  void SetSeedWordHorizontal(int x, int y, const QString& c, WordIndex cI);
  void SetSeedWordVertical(int x, int y, const QString& c, WordIndex cI);
  void Regenerate();

  // first container of lines(horizontal rows) ,  second container of questions
  Vec2D Get();
  bool IterateArea(int x1, int y1, int x2, int y2);
  void Print();
  bool IsChar(QChar c);
  bool IsFree(QChar c);

private:
  enum class CheckOrderType {HORIZONTAL_FIRST,VERTICAL_FIRST };
  bool IterateArea(int x1, int x2, int y1, int y2, CheckOrderType ePrio);

  QChar AtXY(int x, int y);
  bool IsNotEmpty(QChar c);
  bool IsEmptyLine(const PInt& s);
  PInt BoundsOnLine(const QString& s);
  QString WordAtPos(int i, const QString& s, int& nFirst);
  QString& VerticalRowAtX(int x);
  QString& HorizontalRowAtY(int y);

  struct D {
    Vec m_ocHorizontal;
    Vec m_ocVertical;
    QuestionMapType m_ocQuestion;
    QSet<WordIndex> m_ocUsedWords;
  };

  // Checks all possible words on a Char at X,Y.
  // If a match is found that is left in the state d.
  //
  bool MatchWordsAtXY_H(int x, int y);
  bool MatchWordsAtXY_V(int x, int y);
  bool IsHorizontalWord(int x, int y);
  bool IsVerticalWord(int x, int y);
  bool IsCrossWord(int x, int y);
  void UseWord(WordIndex tI);
  void AddWord(const QString& c);

  bool CheckWordVerticalXY(int x, int y, const QString& c);
  bool CheckWordHorizontalXY(int x, int y, const QString& c);
  bool AssignWordVerticalXY(int x, int y, const QString& c, WordIndex cI);
  bool AssignWordHorizontalXY(int x, int y, const QString& c, WordIndex cI);
  bool AssignXY(int x, int y, QChar c);

  Set WorkingSet(QChar c);
  Vec m_ocWordList;

  int m_nW = 0;
  int m_nH = 0;

  QSet<QString> m_ocWordIndexList;
  QMap<QChar, Set> m_ocWordMap;
  using ChPosT = std::pair<WordIndex, QChar>;
  QMap<ChPosT, int> m_ocCharPosMap;
  D d;
  const QChar BLANK_CHAR = '.';

  // Question
  const QChar RESERVED_CHAR = '*';
};

#endif  // CROSSWORD_H
