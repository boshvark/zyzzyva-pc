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
#include "Auxil.h"

class WordEngineTest : public QObject
{
    Q_OBJECT
    public:
    WordEngineTest() : prepared (false) { }

    private slots:
    void testSearch_data();
    void testSearch();

    private:
    void tryImport();

    private:
    WordEngine engine;
    bool prepared;

};

//---------------------------------------------------------------------------
//  tryImport
//
//! Try to import OWL2 lexicon into the WordEngine.
//---------------------------------------------------------------------------
void
WordEngineTest::tryImport()
{
    if (prepared)
        return;
    int numWords = engine.importFile (Auxil::getWordsDir() +
                                      "/north-american/owl2.txt", "Custom");
    if (!numWords)
        return;

    engine.importStems (Auxil::getWordsDir() +
                        "/north-american/6-letter-stems.txt");
    engine.importStems (Auxil::getWordsDir() +
                        "/north-american/7-letter-stems.txt");

    prepared = true;
}

//---------------------------------------------------------------------------
//  testSearch_data
//
//! Set up data files for search tests.
//---------------------------------------------------------------------------
void
WordEngineTest::testSearch_data()
{
    QTest::addColumn<QString>("testName");
    //QTest::addColumn<QString>("resultFilename");

    QTest::newRow ("3s") << "3s";
    QTest::newRow ("7s-type1") << "7s-type1";
    QTest::newRow ("anagram-_aeinst") << "anagram-_aeinst";
    QTest::newRow ("anagram-__aerstw") << "anagram-__aerstw";
    QTest::newRow ("pattern-p_r_s") << "pattern-p_r_s";
    QTest::newRow ("subanagram-aeiprs") << "subanagram-aeiprs";

}

//---------------------------------------------------------------------------
//  testSearch
//
//! Test search results.
//---------------------------------------------------------------------------
void
WordEngineTest::testSearch()
{
    tryImport();

    QFETCH (QString, testName);
    QString testFilename = testName + ".zzs";
    QString resultFilename = testName + ".txt";

    // Create a SearchSpec from a test file
    QFile testFile (Auxil::getRootDir() + "/src/tests/data/" + testFilename);
    if (!testFile.open (QIODevice::ReadOnly | QIODevice::Text)) {
        QFAIL ("Cannot open test file");
        //QFAIL ("Cannot open test file '" + testFilename + "': " +
        //       testFile.errorString());
    }

    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;

    QDomDocument document;
    bool success = document.setContent (&testFile, false, &errorMsg,
                                        &errorLine, &errorColumn);

    if (!success) {
        QFAIL ("Error in test file XML");
        //QFAIL ("Error in test file '" + testFilename + "', line" +
        //       QString::number (errorLine) + ", column " +
        //       QString::number (errorColumn) + ": " + errorMsg);
    }

    SearchSpec spec;
    if (!spec.fromDomElement (document.documentElement())) {
        QFAIL ("Error in test file");
        //QFAIL ("Error in test file '" + testFilename + "', cannot set "
        //       "search spec.");
    }

    // Get a list of expected results
    QFile resultFile (Auxil::getRootDir() + "/src/tests/data/" +
                      resultFilename);
    if (!resultFile.open (QIODevice::ReadOnly | QIODevice::Text)) {
        QFAIL ("Cannot open result file");
        //QFAIL ("Cannot open result file '" + resultFilename + "': " +
        //       resultFile.errorString());
    }

    QStringList expectedResults;
    while (!resultFile.atEnd()) {
        expectedResults.append (resultFile.readLine().trimmed());
    }
    qSort (expectedResults);

    // Do the search and test the results
    QStringList foundResults = engine.search (spec, true);
    qSort (foundResults);

    QCOMPARE (foundResults, expectedResults);
}

// Create a main function for a standalone executable
QTEST_MAIN (WordEngineTest);
#include "WordEngineTest.moc"
