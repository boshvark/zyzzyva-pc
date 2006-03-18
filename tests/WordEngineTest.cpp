//---------------------------------------------------------------------------
// WordEngineTest.cpp
//
// A class for testing the WordEngine class.
//
// Copyright 2006 Michael W Thelen <mike@pietdepsi.com>.
//
// This file is part of Zyzzyva.
//
// Zyzzyva is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Zyzzyva is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include <QtTest/QtTest>

#include "WordEngine.h"

class WordEngineTest : public QObject
{
    Q_OBJECT

    private slots:
    void testBasic();

};

void
WordEngineTest::testBasic()
{
    WordEngine engine;
    engine.importFile ("/home/mthelen/dev/zyzzyva/data/words"
                       "/north-american/owl2-new-words.txt", "Custom");

    QVERIFY (engine.isAcceptable ("FEEB"));
    QVERIFY (!engine.isAcceptable ("NEW"));

}


QTEST_MAIN (WordEngineTest);
#include "WordEngineTest.moc"
