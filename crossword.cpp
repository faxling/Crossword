#include "crossword.h"

#include <QEventLoop>
#include <QtDebug>

#define for_i(INDEX, ARG) \
  int INDEX = -1;         \
  for (ARG)               \
    if (++INDEX < 0) {    \
      ;                   \
    } else

CrossWord::CrossWord(int w, int h) {
  m_nW = w;
  m_nH = h;
  d.m_ocVertical = Vec(h, QString(h, BLANK_CHAR));
  d.m_ocHorizontal = Vec(w, QString(w, BLANK_CHAR));
}

CrossWord::~CrossWord() {
  // qDebug() << "~CrossWord()";
}

void CrossWord::SetSeedWordVertical(int x, int y, const QString& c,
                                    WordIndex cI) {
  // AddWord(c);
  UseWord(cI);
  AssignWordVerticalXY(x, y, c, cI);
}

void CrossWord::SetSeedWordHorizontal(int x, int y, const QString& c,
                                      WordIndex cI) {
  // AddWord(c);
  UseWord(cI);
  AssignWordHorizontalXY(x, y, c, cI);
}

CrossWord::Vec2D CrossWord::Get() {
  Vec2D ocRet;
  PInt tMax = {d.m_ocVertical.size(), -1};
  Vec ocRetStr;
  int nFirstLineWithContent = -1;
  for_i(i, auto& oI : d.m_ocHorizontal) {
    PInt tB = BoundsOnLine(oI);
    if (IsEmptyLine(tB))
      continue;

    if (nFirstLineWithContent == -1)
      nFirstLineWithContent = i;

    ocRetStr.append(oI);

    if (tB.first < tMax.first)
      tMax.first = tB.first;

    if (tB.second > tMax.second)
      tMax.second = tB.second;
  }

  for (auto& oI : ocRetStr) {
    Vec1D ocRow;
    for (auto& oJ : oI.mid(tMax.first, tMax.second - tMax.first + 1))
      ocRow.append(oJ);
    ocRet.first.append(ocRow);
  }

  // Strip out not used squares
  int wDiff = tMax.first;
  int hDiff = nFirstLineWithContent;

  for (auto oI = d.m_ocQuestion.begin(); oI != d.m_ocQuestion.end(); ++oI) {
    //  x                      y
    ocRet.second[{oI.key().first - wDiff, oI.key().second - hDiff}] =
        oI.value();
  }

  return ocRet;
}

bool CrossWord::IsEmptyLine(const PInt& s) {
  if (s.first == -1)
    return true;
  return false;
}

CrossWord::PInt CrossWord::BoundsOnLine(const QString& s) {
  PInt tRet{-1, -1};

  for_i(i, auto oI : s) {
    if (tRet.first == -1 && IsNotEmpty(oI))
      tRet.first = i;

    if (IsChar(oI))
      tRet.second = i;
  }
  return tRet;
}

void CrossWord::AddWord(const QString& c) {
  WordIndex nIndex = m_ocWordList.size();
  m_ocWordList.push_back(c);
  m_ocWordIndexList.insert(c);

  for_i(i, auto oJ : c) {
    // Last index
    m_ocCharPosMap[{nIndex, oJ}] = i;
    m_ocWordMap[oJ].insert(nIndex);
  }
}

void CrossWord::AssignWordList(const Vec& oc) {
  for (auto& oI : oc)
    AddWord(oI);
}

QString& CrossWord::VerticalRowAtX(int x) {
  return d.m_ocVertical[x];
}

QString& CrossWord::HorizontalRowAtY(int y) {
  return d.m_ocHorizontal[y];
}

QChar CrossWord::AtXY(int x, int y) {
  return d.m_ocVertical[x][y];
}

bool CrossWord::IsChar(QChar c) {
  return (c != BLANK_CHAR && c != RESERVED_CHAR);
}

bool CrossWord::IsFree(QChar c) {
  return (c == BLANK_CHAR || c == RESERVED_CHAR);
}

bool CrossWord::IsNotEmpty(QChar c) {
  return (c != BLANK_CHAR);
}

QString CrossWord::WordAtPos(int i, const QString& s, int& nFirst) {
  nFirst = i;
  int nLast = i;
  while (IsChar(s[--nFirst]))
    ;
  ++nFirst;
  while (IsChar(s[++nLast]))
    ;
  QString sRet = s.mid(nFirst, nLast - nFirst);
  return sRet;
}

bool CrossWord::IsCrossWord(int x, int y) {
  return IsHorizontalWord(x, y) && IsVerticalWord(x, y);
}

bool CrossWord::IsHorizontalWord(int x, int y) {
  auto& r = HorizontalRowAtY(y);
  if (IsFree(r[x - 1]) && IsFree(r[x + 1]))
    return true;

  int nPos = -1;
  QString s = WordAtPos(x, r, nPos);
  if (m_ocWordIndexList.contains(s) == true) {
    return true;
  }

  return false;
}

bool CrossWord::IsVerticalWord(int x, int y) {
  auto& r = VerticalRowAtX(x);
  if (IsFree(r[y - 1]) && IsFree(r[y + 1]))
    return true;

  int nPos = -1;
  QString s = WordAtPos(y, r, nPos);
  if (m_ocWordIndexList.contains(s) == true) {
    return true;
  }

  return false;
}

bool CrossWord::MatchWordsAtXY_V(int x, int y) {
  QChar cCharAtPos = AtXY(x, y);
  const Set oc = WorkingSet(cCharAtPos);
  for (WordIndex iWordI : oc) {
    const QString& rWord = m_ocWordList[iWordI];
    if (CheckWordVerticalXY(x, y - m_ocCharPosMap[{iWordI, cCharAtPos}],
                            rWord)) {
      // Save state
      auto oLastD = d;

      int nY = y - m_ocCharPosMap[{iWordI, cCharAtPos}];

      AssignWordVerticalXY(x, nY, rWord, iWordI);
      bool bAllOk = true;

      for_i(i, auto& oI : rWord) {
        (void)oI;
        if (IsHorizontalWord(x, nY + i) == true)
          continue;

        if (MatchWordsAtXY_H(x, nY + i) == false) {
          bAllOk = false;
          break;
        }
      }

      if (bAllOk == true)
        return true;
      d = oLastD;
    }
  }
  return false;
}

bool CrossWord::MatchWordsAtXY_H(int x, int y) {
  QChar cCharAtPos = AtXY(x, y);
  const Set oc = WorkingSet(cCharAtPos);
  for (WordIndex iWordI : oc) {
    const QString& rWord = m_ocWordList[iWordI];
    if (CheckWordHorizontalXY(x - m_ocCharPosMap[{iWordI, cCharAtPos}], y,
                              rWord)) {
      // Save state
      auto oLastD = d;

      int nX = x - m_ocCharPosMap[{iWordI, cCharAtPos}];

      AssignWordHorizontalXY(nX, y, rWord, iWordI);
      bool bAllOk = true;

      for_i(i, auto oI : rWord) {
        (void)oI;
        if (IsVerticalWord(nX + i, y) == true)
          continue;
        if (MatchWordsAtXY_V(nX + i, y) == false) {
          bAllOk = false;
          break;
        }
      }

      if (bAllOk == true)
        return true;
      d = oLastD;
    }
  }
  return false;
}

void CrossWord::UseWord(WordIndex s) {
  d.m_ocUsedWords.insert(s);
}

void CrossWord::Print() {
  for (auto& oI : d.m_ocHorizontal)
    printf("%ls\n", (wchar_t*)oI.utf16());
}

bool CrossWord::CheckWordVerticalXY(int x, int y, const QString& c) {
  if (y < 1)
    return false;

  if (y + c.length() >= m_nH)
    return false;

  QString& r = VerticalRowAtX(x);
  if (IsChar(r[y - 1]))
    return false;
  for_i(i, auto& oI : c) {
    if (r[y + i] != BLANK_CHAR && r[y + i] != oI)
      return false;
  }

  if (IsChar(r[y + i + 1]))
    return false;

  return true;
}

// Check that we have ok squares in Horizontal i.e Blank chars or matching
// One square needs to be free before the wort to have space for the question
// square
bool CrossWord::CheckWordHorizontalXY(int x, int y, const QString& c) {
  if (x < 1)
    return false;

  if (x + c.length() >= m_nW)
    return false;

  QString& r = HorizontalRowAtY(y);

  if (IsChar(r[x - 1]))
    return false;

  for_i(i, auto& oI : c) {
    if (r[x + i] != BLANK_CHAR && r[x + i] != oI)
      return false;
  }
  if (IsChar(r[x + i + 1]))
    return false;

  return true;
}

bool CrossWord::AssignXY(int x, int y, QChar c) {
  if (d.m_ocHorizontal[y][x] == BLANK_CHAR) {
    d.m_ocHorizontal[y][x] = c;
    d.m_ocVertical[x][y] = c;
    return true;
  }
  if (d.m_ocHorizontal[y][x] == c) {
    return true;
  }

  return false;
}

bool CrossWord::AssignWordVerticalXY(int x, int y, const QString& c,
                                     WordIndex cI) {
  int le = c.length();
  if (y + le >= m_nH)
    return false;

  if (y <= 0)
    return false;

  UseWord(cI);

  for (int i = 0; i < le; ++i)
    AssignXY(x, y + i, c[i]);

  QString& rVerticalRow = VerticalRowAtX(x);

  if (rVerticalRow[y - 1] == BLANK_CHAR)
    AssignXY(x, y - 1, RESERVED_CHAR);

  if (rVerticalRow[y + le] == BLANK_CHAR)
    AssignXY(x, y + le, RESERVED_CHAR);

  d.m_ocQuestion[{x, y - 1}].VerticalQ = cI;
  return true;
}

bool CrossWord::AssignWordHorizontalXY(int x, int y, const QString& c,
                                       WordIndex cI) {
  int le = c.length();
  if (x + le >= m_nW)
    return false;

  if (x <= 0)
    return false;

  UseWord(cI);

  QString& rHorizontalRow = HorizontalRowAtY(x);

  for (int i = 0; i < le; ++i)
    AssignXY(x + i, y, c[i]);

  if (rHorizontalRow[x - 1] == BLANK_CHAR)
    AssignXY(x - 1, y, RESERVED_CHAR);

  if (rHorizontalRow[x + le] == BLANK_CHAR)
    AssignXY(x + le, y, RESERVED_CHAR);

  d.m_ocQuestion[{x - 1, y}].HorizontalQ = cI;
  return true;
}

CrossWord::Set CrossWord::WorkingSet(QChar c) {
  const auto& oc = m_ocWordMap[c];
  Set ocRet;
  for (auto oI : oc) {
    if (d.m_ocUsedWords.contains(oI))
      continue;
    ocRet.insert(oI);
  }
  return ocRet;
}

bool CrossWord::IterateArea(int x1, int y1, int x2, int y2,
                            CheckOrderType ePrio) {
  for (int x = x1; x < x2; ++x) {
    for (int y = y1; y < y2; ++y) {
      QChar c = AtXY(x, y);
      if (IsChar(c)) {
        if (ePrio == CheckOrderType::HORIZONTAL_FIRST) {
          if (MatchWordsAtXY_V(x, y) == true)
            return true;
        } else {
          if (MatchWordsAtXY_H(x, y) == true)
            return true;
        }
      }
    }
  }
  return false;
}

bool CrossWord::IterateArea(int x1, int y1, int x2, int y2) {
  int nUsedAtStart = d.m_ocUsedWords.count();
  IterateArea(x1, y1, x2, y2, CheckOrderType::HORIZONTAL_FIRST);
  IterateArea(x1, y1, x2, y2, CheckOrderType::VERTICAL_FIRST);
  QEventLoop loop;
  loop.processEvents();
  return nUsedAtStart < d.m_ocUsedWords.count();
}
