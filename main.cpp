/* Test of CrossWord generator */

#include <QCoreApplication>
#include <QMap>
#include <QSet>
#include <QString>
#include <QVector>
#include <ctime>
#include <iostream>
#include <random>
#include "crossword.h"

int main(int argc, char** argv) {
  static std::mt19937 gen(time(0));
  static std::uniform_int_distribution<> dis(0, 25);
  static std::uniform_int_distribution<> dis2(4, 10);
  /*
   *
   *
   *   int nIndexMax = 0;
  for (int i = 0; i < 40;++i)
  {
    QString sWord;
    int l = dis2(gen);
    for (int j = 0; j < l;++j)
    {
      sWord.append(QChar(dis(gen) + 'A'));
    }

    if (l > nMax)
    {
      nMax = l;
      nIndexMax = i;
    }

    ocWordList.append(sWord);
  }
*/
  constexpr int nH = 40;
  constexpr int nW = 40;
  QCoreApplication a(argc, argv);

  CrossWord oCrossWord(nW, nH);

  CrossWord::Vec ocWordList = {
      "FREDRIK",   "HZOFPOSF",   "KWIWLAOQZO", "ZQKRBWRF",   "ZCYVV",
      "JQQQ",      "UJFYQ",      "EPNUUZHN",   "AMJQK",      "JLSW",
      "ITPCXTJ",   "VNBL",       "JILAL",      "XMDBYCMBVY", "CCVHDWVT",
      "TETUBTSA",  "APLSRR",     "NTUZKQJ",    "FSBNUUP",    "HIJDVZK",
      "HTKJX",     "WLETZ",      "IFSUUXO",    "EXCSVNUH",   "UKVGGUDKY",
      "SAJTWYBIN", "MPQZCSTZTG", "QYHYPBCFA",  "JFWWVD",     "HBRYHXCQR",
      "TAESQ",     "UCLPGDCP",   "UNITEVL",    "ZWUMGKQV",   "ZFWIMNOZZS",
      "DCFMZY",    "MPEQ",       "WPVLAO",     "ZQRGQRJJU",  "YUANQ",
      "GFAXGM"};

  oCrossWord.AssignWordList(ocWordList);

  oCrossWord.SetSeedWordHorizontal(10, nH / 2, "FREDRIK", 0);

  while (oCrossWord.IterateArea(10, 10, nW - 10, nH - 10) == true)
    ;

  // oCrossWord.Print();
  auto oR = oCrossWord.Get();
  const auto& r0 = oR.first;
  for (const auto& oI : r0) {
    for (const auto& oJ : oI)
      printf("%c", oJ.toLatin1());
    printf("\n");
  }
  const auto& r1 = oR.second;
  for (auto oI = r1.begin(); oI != r1.end(); ++oI) {
    int nT = 0;
    if (oI.value().HorizontalQ > -1)
      nT += 1;
    if (oI.value().VerticalQ > -1)
      nT += 2;
    switch (nT) {
      case 1:
        printf("x%d y%d has Horizontal Question %d\n", oI.key().first,
               oI.key().second, oI.value().HorizontalQ);
        break;
      case 2:
        printf("x%d y%d has Vertical Question %d\n", oI.key().first,
               oI.key().second, oI.value().VerticalQ);
        break;
      case 3:
        printf("x%d y%d has Horizontal and Vertical Question %d %d\n",
               oI.key().first, oI.key().second, oI.value().VerticalQ,
               oI.value().HorizontalQ);
        break;
    }
  }
  return 1;

  //  return a.exec();
}
