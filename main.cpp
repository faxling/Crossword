#include <QCoreApplication>
#include <QMap>
#include <QSet>
#include <QString>
#include <QVector>
#include <ctime>
#include <iostream>
#include <random>

#include "crossword.h"



int main(int, char**) {
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

    g_ocWordList.append(sWord);
  }
*/
  constexpr int nH = 40;
  constexpr int nW = 40;

  CrossWord oCrossWord(nW, nH);

  CrossWord::Vec ocWordList = {
      "HZOFPOSF",   "KWIWLAOQZO", "ZQKRBWRF",   "ZCYVV",      "JQQQ",
      "UJFYQ",      "EPNUUZHN",   "AMJQK",      "JLSW",       "ITPCXTJ",
      "VNBL",       "JILAL",      "XMDBYCMBVY", "CCVHDWVT",   "TETUBTSA",
      "APLSRR",     "NTUZKQJ",    "FSBNUUP",    "HIJDVZK",    "HTKJX",
      "WLETZ",      "IFSUUXO",    "EXCSVNUH",   "UKVGGUDKY",  "SAJTWYBIN",
      "MPQZCSTZTG", "QYHYPBCFA",  "JFWWVD",     "HBRYHXCQR",  "TAESQ",
      "UCLPGDCP",   "UNITEVL",    "ZWUMGKQV",   "ZFWIMNOZZS", "DCFMZY",
      "MPEQ",       "WPVLAO",     "ZQRGQRJJU",  "YUANQ",      "GFAXGM"};

  oCrossWord.AssignWordList(ocWordList);

  oCrossWord.SetSeedWordHorizontal(10, nH / 2, "FREDRIK");

  while (oCrossWord.IterateArea(10, 10, nW - 10, nH - 10) != 0)
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
    printf("%d %d %ls\n", oI.key().first, oI.key().second,
           (wchar_t*)oI.value().utf16());
  }
  return 1;

  // QCoreApplication a(argc, argv);

  //  return a.exec();
}
