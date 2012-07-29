//---------------------------------------------------------------------------
// CreateDatabaseThread.cpp
//
// A class for creating a database in the background.
//
// Copyright 2006-2012 Boshvark Software, LLC.
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

#include "CreateDatabaseThread.h"
#include "LetterBag.h"
#include "MainSettings.h"
#include "WordEngine.h"
#include "Auxil.h"
#include "Defs.h"
#include <QtSql>

const int MAX_DEFINITION_LINKS = 3;
const int PROGRESS_STEP = 1000;
const QString DB_CONNECTION_NAME = "CreateDatabaseThread";

using namespace Defs;

//---------------------------------------------------------------------------
//  run
//
//! Create the database.
//---------------------------------------------------------------------------
void
CreateDatabaseThread::run()
{
    runPrivate();
}

//---------------------------------------------------------------------------
//  runPrivate
//
//! Create the database.
//---------------------------------------------------------------------------
void
CreateDatabaseThread::runPrivate()
{
    int numSteps = 0;

    {
        // Create empty database
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE",
                                                    DB_CONNECTION_NAME);
        db.setDatabaseName(dbFilename);
        if (!db.open()) {
            error = QString("Unable to open database file '%1':\n%2").arg(
                dbFilename).arg(db.lastError().text());
            cancel();
        }

        // Start at 1% progress
        emit(steps(100));
        emit(progress(1));

        // Total number of progress steps is number of words times the number
        // of lines that increment stepNum in all the code that is called
        // below.
        int stepNumIncs = 8;
        int numWords = wordEngine->getNumWords(lexiconName);
        int baseProgress = numWords * stepNumIncs / 99;
        numSteps = numWords * stepNumIncs + baseProgress + 1;
        emit steps(numSteps);
        int stepNum = baseProgress;
        emit progress(stepNum);

        createTables(db);
        createIndexes(db);
        // insertWords increments stepNum 2 times
        insertWords(db, stepNum);
        // updateProbabilityOrder increments stepNum 4 times for each word
        // because of 0, 1, 2 blanks and playability
        updateProbabilityOrder(db, stepNum);
        updateDefinitions(db, stepNum);
        updateDefinitionLinks(db, stepNum);
    }

    cleanup();
    emit progress(numSteps);
}

//---------------------------------------------------------------------------
//  createTables
//
//! Create the database tables.
//
//! @param db the database
//---------------------------------------------------------------------------
void
CreateDatabaseThread::createTables(QSqlDatabase& db)
{
    QSqlQuery query (db);

    query.exec("CREATE TABLE words (word text, length integer, "
        "playability integer, playability_order integer, "
        "min_playability_order integer, max_playability_order integer, "
        "combinations0 integer, probability_order0 integer, "
        "min_probability_order0 integer, max_probability_order0 integer, "
        "combinations1 integer, probability_order1 integer, "
        "min_probability_order1 integer, max_probability_order1 integer, "
        "combinations2 integer, probability_order2 integer, "
        "min_probability_order2 integer, max_probability_order2 integer, "
        "alphagram text, num_anagrams integer, "
        "num_unique_letters integer, num_vowels integer, "
        "point_value integer, front_hooks text, "
        "back_hooks text, is_front_hook integer, "
        "is_back_hook integer, lexicon_symbols text, "
        "definition text)");

    query.exec("CREATE TABLE db_version (version integer)");
    query.exec("INSERT into db_version (version) VALUES (" +
               QString::number(CURRENT_DATABASE_VERSION) + ")");

    query.exec("CREATE TABLE lexicon_date (date date)");
    query.prepare("INSERT into lexicon_date (date) VALUES (?)");
    query.bindValue(0, Auxil::lexiconToDate(lexiconName));
    query.exec();

    query.exec("CREATE TABLE lexicon_file (file text)");
    query.prepare("INSERT into lexicon_file (file) VALUES (?)");
    query.bindValue(0, wordEngine->getLexiconFile(lexiconName));
    query.exec();
}

//---------------------------------------------------------------------------
//  createIndexes
//
//! Create the database indexes.
//
//! @param db the database
//! @param stepNum the current step number
//---------------------------------------------------------------------------
void
CreateDatabaseThread::createIndexes(QSqlDatabase& db)
{
    QSqlQuery query (db);

    // Indexes on words table
    query.exec("CREATE UNIQUE INDEX word_index on words (word)");
    if (cancelled)
        return;

    query.exec("CREATE INDEX word_length_index on words (length)");
    if (cancelled)
        return;

    query.exec("CREATE UNIQUE INDEX play_index on words "
               "(length, playability_order)");
    if (cancelled)
        return;

    query.exec("CREATE INDEX play_min_max_index on words "
               "(length, min_playability_order, max_playability_order)");
    if (cancelled)
        return;

    query.exec("CREATE UNIQUE INDEX prob0_index on words "
               "(length, probability_order0)");
    if (cancelled)
        return;

    query.exec("CREATE INDEX prob0_min_max_index on words "
               "(length, min_probability_order0, max_probability_order0)");
    if (cancelled)
        return;

    query.exec("CREATE UNIQUE INDEX prob1_index on words "
               "(length, probability_order1)");
    if (cancelled)
        return;

    query.exec("CREATE INDEX prob1_min_max_index on words "
               "(length, min_probability_order1, max_probability_order1)");
    if (cancelled)
        return;

    query.exec("CREATE UNIQUE INDEX prob2_index on words "
               "(length, probability_order2)");
    if (cancelled)
        return;

    query.exec("CREATE INDEX prob2_min_max_index on words "
               "(length, min_probability_order2, max_probability_order2)");
    if (cancelled)
        return;

    query.exec("CREATE INDEX definition_index on words "
               "(definition)");
    if (cancelled)
        return;
}

//---------------------------------------------------------------------------
//  insertWords
//
//! Insert words into the database.
//
//! @param db the database
//! @param stepNum the current step number
//---------------------------------------------------------------------------
void
CreateDatabaseThread::insertWords(QSqlDatabase& db, int& stepNum)
{
    LetterBag letterBag;
    QStringList letters;
    letters << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" <<
        "I" << "J" << "K" << "L" << "M" << "N" << "O" << "P" << "Q" <<
        "R" << "S" << "T" << "U" << "V" << "W" << "X" << "Y" << "Z";

    SearchCondition searchCondition;
    searchCondition.type = SearchCondition::Length;
    SearchSpec searchSpec;
    searchSpec.conditions.append(searchCondition);

    QList<LexiconStyle> lexStyles = MainSettings::getWordListLexiconStyles();
    QMutableListIterator<LexiconStyle> it (lexStyles);
    while (it.hasNext()) {
        const LexiconStyle& style = it.next();
        if ((style.lexicon != lexiconName) ||
            !wordEngine->lexiconIsLoaded(style.compareLexicon))
        {
            it.remove();
        }
    }

    QMap<QString, qint64> playabilityMap;
    QString playabilityFile = Auxil::getWordsDir() +
        Auxil::getLexiconPrefix(lexiconName) + "-Playability.txt";
    importPlayability(playabilityFile, playabilityMap);

    QSqlQuery transactionQuery ("BEGIN TRANSACTION", db);
    QSqlQuery query (db);

    QMap<QString, qint64> numAnagramsMap;

    for (int length = 1; length <= MAX_WORD_LEN; ++length) {
        searchSpec.conditions[0].minValue = length;
        searchSpec.conditions[0].maxValue = length;

        // Do a word graph search because we're still building the database!
        QStringList words = wordEngine->wordGraphSearch(lexiconName,
                                                        searchSpec);

        query.prepare("INSERT INTO words (word, length, playability, "
                      "combinations0, combinations1, combinations2, "
                      "alphagram, num_unique_letters, num_vowels, "
                      "point_value, front_hooks, back_hooks, "
                      "is_front_hook, is_back_hook, lexicon_symbols) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

        // Insert words with length, combinations, hooks
        foreach (const QString& word, words) {
            qint64 playability = playabilityMap.value(word);
            double combinations0 = letterBag.getNumCombinations(word, 0);
            double combinations1 = letterBag.getNumCombinations(word, 1);
            double combinations2 = letterBag.getNumCombinations(word, 2);
            int numUniqueLetters = Auxil::getNumUniqueLetters(word);
            int numVowels = Auxil::getNumVowels(word);

            int pointValue = 0;
            for (int i = 0; i < word.length(); ++i) {
                pointValue += letterBag.getLetterValue(word.at(i));
            }

            QString alphagram = Auxil::getAlphagram(word);
            if (numAnagramsMap.contains(alphagram))
                ++numAnagramsMap[alphagram];
            else
                numAnagramsMap[alphagram] = 1;

            int isFrontHook = wordEngine->isAcceptable(
                lexiconName, word.right(word.length() - 1)) ? 1 : 0;
            int isBackHook = wordEngine->isAcceptable(
                lexiconName, word.left(word.length() - 1)) ? 1 : 0;

            QString front, back;
            foreach (const QString& letter, letters) {
                if (wordEngine->isAcceptable(lexiconName, letter + word))
                    front += letter;
                if (wordEngine->isAcceptable(lexiconName, word + letter))
                    back += letter;
            }

            // Populate words and hooks with symbols
            QString symbolStr;
            if (!lexStyles.isEmpty()) {
                QListIterator<LexiconStyle> it (lexStyles);
                while (it.hasNext()) {
                    const LexiconStyle& style = it.next();
                    bool acceptable =
                        wordEngine->isAcceptable(style.compareLexicon, word);
                    if (!(acceptable ^ style.inCompareLexicon))
                        symbolStr += style.symbol;
                }

                // Populate front hooks with symbols
                for (int i = 0; i < front.length(); ++i) {
                    QChar c = front[i];
                    QString hookWord = c.toUpper() + word;

                    it.toFront();
                    while (it.hasNext()) {
                        const LexiconStyle& style = it.next();
                        bool acceptable = wordEngine->isAcceptable(
                            style.compareLexicon, hookWord);

                        if (!(acceptable ^ style.inCompareLexicon)) {
                            front.insert(i + 1, style.symbol);
                            i += style.symbol.length();
                        }
                    }
                }

                // Populate back hooks with symbols
                for (int i = 0; i < back.length(); ++i) {
                    QChar c = back[i];
                    QString hookWord = word + c.toUpper();

                    it.toFront();
                    while (it.hasNext()) {
                        const LexiconStyle& style = it.next();
                        bool acceptable = wordEngine->isAcceptable(
                            style.compareLexicon, hookWord);

                        if (!(acceptable ^ style.inCompareLexicon)) {
                            back.insert(i + 1, style.symbol);
                            i += style.symbol.length();
                        }
                    }
                }
            }

            int bindNum = 0;
            query.bindValue(bindNum++, word);
            query.bindValue(bindNum++, length);
            query.bindValue(bindNum++, playability);
            query.bindValue(bindNum++, combinations0);
            query.bindValue(bindNum++, combinations1);
            query.bindValue(bindNum++, combinations2);
            query.bindValue(bindNum++, alphagram);
            query.bindValue(bindNum++, numUniqueLetters);
            query.bindValue(bindNum++, numVowels);
            query.bindValue(bindNum++, pointValue);
            query.bindValue(bindNum++, front.toLower());
            query.bindValue(bindNum++, back.toLower());
            query.bindValue(bindNum++, isFrontHook);
            query.bindValue(bindNum++, isBackHook);
            query.bindValue(bindNum++, symbolStr);
            query.exec();

            if ((stepNum % PROGRESS_STEP) == 0) {
                if (cancelled) {
                    transactionQuery.exec("END TRANSACTION");
                    return;
                }
                emit progress(stepNum);
            }
            ++stepNum;
        }

        // Update number of anagrams
        query.prepare("UPDATE words SET num_anagrams=? WHERE word=?");
        foreach (const QString& word, words) {
            QString alphagram = Auxil::getAlphagram(word);
            query.bindValue(0, numAnagramsMap[alphagram]);
            query.bindValue(1, word);
            query.exec();

            if ((stepNum % PROGRESS_STEP) == 0) {
                if (cancelled) {
                    transactionQuery.exec("END TRANSACTION");
                    return;
                }
                emit progress(stepNum);
            }
            ++stepNum;
        }
    }

    transactionQuery.exec("END TRANSACTION");
}

//---------------------------------------------------------------------------
//  updateProbabilityOrder
//
//! Update probability and playability order of words in the database.
//
//! @param db the database
//! @param stepNum the current step number
//---------------------------------------------------------------------------
void
CreateDatabaseThread::updateProbabilityOrder(QSqlDatabase& db, int& stepNum)
{
    QSqlQuery transactionQuery ("BEGIN TRANSACTION", db);

    for (int numBlanks = -1; numBlanks <= 2; ++numBlanks) {
        QString valueCol = (numBlanks < 0 ? "playability"
            : QString("combinations%1").arg(numBlanks));
        QString orderCol = (numBlanks < 0 ? "playability_order"
            : QString("probability_order%1").arg(numBlanks));
        QString minOrderCol = "min_" + orderCol;
        QString maxOrderCol = "max_" + orderCol;

        QSqlQuery updateQuery (db);
        updateQuery.prepare(QString("UPDATE words SET %1=?, %2=?, %3=? "
            "WHERE word=?").arg(orderCol).arg(minOrderCol).arg(maxOrderCol));

        for (int length = 1; length <= MAX_WORD_LEN; ++length) {
            QSqlQuery selectQuery (db);
            selectQuery.prepare(QString("SELECT word, %1 FROM words "
                "WHERE length=? ORDER BY %1 DESC").arg(valueCol));

            selectQuery.bindValue(0, length);
            selectQuery.exec();

            QMap<QString, QString> equalWordMap;
            int order = 1;
            int minOrder = 1;
            double prevValue = 0;

            while (selectQuery.next()) {
                QString word = selectQuery.value(0).toString();
                double value = selectQuery.value(1).toDouble();

                // Update probability ranges
                if ((value != prevValue) && !equalWordMap.empty()) {
                    int maxOrder = minOrder + equalWordMap.size() - 1;
                    QMapIterator<QString, QString> jt (equalWordMap);
                    while (jt.hasNext()) {
                        jt.next();
                        QString equalWord = jt.value();
                        updateQuery.bindValue(0, order);
                        updateQuery.bindValue(1, minOrder);
                        updateQuery.bindValue(2, maxOrder);
                        updateQuery.bindValue(3, equalWord);
                        updateQuery.exec();

                        if ((stepNum % PROGRESS_STEP) == 0) {
                            if (cancelled) {
                                transactionQuery.exec("END TRANSACTION");
                                return;
                            }
                            emit progress(stepNum);
                        }

                        ++order;
                        ++stepNum;
                    }
                    minOrder = order;
                    equalWordMap.clear();
                }

                // Sort words by alphagram
                QString radix = Auxil::getAlphagram(word) + word;
                equalWordMap.insert(radix, word);

                prevValue = value;
            }

            int maxOrder = minOrder + equalWordMap.size() - 1;
            QMapIterator<QString, QString> jt (equalWordMap);
            while (jt.hasNext()) {
                jt.next();
                QString equalWord = jt.value();
                updateQuery.bindValue(0, order);
                updateQuery.bindValue(1, minOrder);
                updateQuery.bindValue(2, maxOrder);
                updateQuery.bindValue(3, equalWord);
                updateQuery.exec();
                ++order;
                ++stepNum;
            }
        }
    }

    transactionQuery.exec("END TRANSACTION");
}

//---------------------------------------------------------------------------
//  updateDefinitions
//
//! Update definitions of words in the database.
//
//! @param db the database
//! @param stepNum the current step number
//---------------------------------------------------------------------------
void
CreateDatabaseThread::updateDefinitions(QSqlDatabase& db, int& stepNum)
{
    QSqlQuery transactionQuery ("BEGIN TRANSACTION", db);

    QSqlQuery query (db);
    query.prepare("UPDATE words SET definition=? WHERE word=?");

    QMap<QString, QString> definitionMap;
    QFile definitionFile (definitionFilename);
    if (definitionFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        bool readNewline = true;
        char* buffer = new char[MAX_INPUT_LINE_LEN * 2 + 1];
        while (definitionFile.readLine(buffer, MAX_INPUT_LINE_LEN) > 0) {
            QString line (buffer);

            // If first line didn't contain newline, skip subsequent reads
            // until we see a newline (effectively truncating long lines)
            bool skip = !readNewline;
            readNewline = (line.right(1) == QString("\n"));
            if (skip)
                continue;

            line = line.simplified();
            if (!line.length() || (line.at(0) == '#')) {
                if ((stepNum % PROGRESS_STEP) == 0) {
                    if (cancelled) {
                        transactionQuery.exec("END TRANSACTION");
                        delete buffer;
                        return;
                    }
                    emit progress(stepNum);
                }
                ++stepNum;
                continue;
            }
            QString word = line.section(' ', 0, 0).toUpper();
            QString definition = line.section(' ', 1);

            query.bindValue(0, definition);
            query.bindValue(1, word);
            query.exec();

            if ((stepNum % PROGRESS_STEP) == 0) {
                if (cancelled) {
                    transactionQuery.exec("END TRANSACTION");
                    delete buffer;
                    return;
                }
                emit progress(stepNum);
            }
            ++stepNum;
        }
        delete buffer;
    }

    transactionQuery.exec("END TRANSACTION");
}

//---------------------------------------------------------------------------
//  cancel
//
//! Cancel the creation of this database.  Set the cancelled flag so the
//! operation will be stopped and the database will be deleted.
//---------------------------------------------------------------------------
void
CreateDatabaseThread::cancel()
{
    cancelled = true;
}

//---------------------------------------------------------------------------
//  cleanup
//
//! Clean up open database handles.
//---------------------------------------------------------------------------
void
CreateDatabaseThread::cleanup()
{
    QSqlDatabase::removeDatabase(DB_CONNECTION_NAME);
}

//---------------------------------------------------------------------------
//  updateDefinitionLinks
//
//! Update links within definitions of words in the database.
//
//! @param db the database
//! @param stepNum the current step number
//---------------------------------------------------------------------------
void
CreateDatabaseThread::updateDefinitionLinks(QSqlDatabase& db, int& stepNum)
{
    getDefinitions(db, stepNum);

    if (cancelled)
        return;

    QSqlQuery updateQuery (db);
    updateQuery.prepare("UPDATE words SET definition=? WHERE word=?");

    QSqlQuery transactionQuery ("BEGIN TRANSACTION", db);

    QSet<QString> alreadyReplaced;
    QMapIterator<QString, QString> it (definitions);
    while (it.hasNext()) {
        it.next();
        QString word = it.key();
        QString definition = it.value();

        QStringList defs = definition.split(" / ");
        QString newDefinition;
        foreach (const QString& def, defs) {
            if (!newDefinition.isEmpty())
                newDefinition += "\n";

            alreadyReplaced.clear();
            alreadyReplaced.insert(word.toUpper());

            newDefinition += replaceDefinitionLinks(def, MAX_DEFINITION_LINKS,
                &alreadyReplaced);
        }

        if (definition != newDefinition) {
            updateQuery.bindValue(0, newDefinition);
            updateQuery.bindValue(1, word);
            updateQuery.exec();
        }

        ++stepNum;

        if ((stepNum % PROGRESS_STEP) == 0) {
            if (cancelled) {
                transactionQuery.exec("END TRANSACTION");
                return;
            }
            emit progress(stepNum);
        }
    }

    transactionQuery.exec("END TRANSACTION");
}

//---------------------------------------------------------------------------
//  getDefinitions
//
//! Get word definitions from the database and put them in the definitions
//! map.  Discard any definitions that consist of only part of speech.
//
//! @param db the database
//! @param stepNum the current step number
//---------------------------------------------------------------------------
void
CreateDatabaseThread::getDefinitions(QSqlDatabase& db, int& stepNum)
{
    QSqlQuery selectQuery (db);
    selectQuery.prepare("SELECT word, definition FROM words");
    selectQuery.exec();

    QRegExp defRegex (QString("^[^[]|\\s+/\\s+[^[]"));

    while (selectQuery.next()) {
        QString word = selectQuery.value(0).toString();
        QString definition = selectQuery.value(1).toString();

        if (defRegex.indexIn(definition, 0) < 0) {
            ++stepNum;
            if ((stepNum % PROGRESS_STEP) == 0) {
                if (cancelled)
                    return;
                emit progress(stepNum);
            }
            continue;
        }

        definitions[word] = definition;
    }
}

//---------------------------------------------------------------------------
//  replaceDefinitionLinks
//
//! Replace links in a definition with the definitions of the words they are
//! linked to.  A string is assumed to have a maximum of one link.  Links may
//! be followed recursively to the maximum depth specified.
//
//! @param definition the definition with links to be replaced
//! @param maxDepth the maximum number of recursive links to replace
//! @param useFollow true if the "follow" replacement should be used
//
//! @return a string with links replaced
//---------------------------------------------------------------------------
QString
CreateDatabaseThread::replaceDefinitionLinks(const QString& definition,
    int maxDepth, QSet<QString>* alreadyReplaced, bool useFollow) const
{
    QRegExp followRegex (QString("\\{(\\w+)=(\\w+)\\}"));
    QRegExp replaceRegex (QString("\\<(\\w+)=(\\w+)\\>"));

    bool createdSet = false;
    if (!alreadyReplaced) {
        alreadyReplaced = new QSet<QString>;
        createdSet = true;
    }

    // Try to match the follow regex and the replace regex.  If a follow regex
    // is ever matched, then the "follow" replacements should always be used,
    // even if the "replace" regex is matched in a later iteration.
    QRegExp* matchedRegex = 0;
    int index = followRegex.indexIn(definition, 0);
    if (index >= 0) {
        matchedRegex = &followRegex;
        useFollow = true;
    }
    else {
        index = replaceRegex.indexIn(definition, 0);
        matchedRegex = &replaceRegex;
    }

    if (index < 0)
        return definition;

    QString modified (definition);
    QString word = matchedRegex->cap(1);
    QString pos = matchedRegex->cap(2);

    QString replacement;
    QString upper = word.toUpper();
    QString failReplacement = useFollow ? word : upper;
    bool cutRecursion = !maxDepth || alreadyReplaced->contains(upper);
    if (cutRecursion) {
        replacement = failReplacement;
    }
    else {
        QString subdef = getSubDefinition(upper, pos);
        if (subdef.isEmpty()) {
            replacement = failReplacement;
            cutRecursion = true;
        }
        else if (useFollow) {
            replacement = (matchedRegex == &followRegex) ?
                word + " (" + subdef + ")" : subdef;
        }
        else {
            replacement = upper + ", " + subdef;
        }
        alreadyReplaced->insert(upper);
    }

    modified.replace(index, matchedRegex->matchedLength(), replacement);
    int lowerMaxDepth = useFollow ? maxDepth - 1 : maxDepth;
    QString newDefinition = cutRecursion ? modified
        : replaceDefinitionLinks(modified, lowerMaxDepth, alreadyReplaced,
            useFollow);

    if (createdSet) {
        delete alreadyReplaced;
    }

    return newDefinition;
}

//---------------------------------------------------------------------------
//  getSubDefinition
//
//! Return the definition associated with a word and a part of speech.  If
//! more than one definition is given for a part of speech, pick the first
//! one.
//
//! @param word the word
//! @param pos the part of speech
//
//! @return the definition substring
//---------------------------------------------------------------------------
QString
CreateDatabaseThread::getSubDefinition(const QString& word, const QString&
                                       pos) const
{
    if (!definitions.contains(word))
        return QString();

    QString definition = definitions[word];
    QRegExp posRegex (QString("\\[(\\w+)"));
    QStringList defs = definition.split(" / ");
    foreach (const QString& def, defs) {
        if ((posRegex.indexIn(def, 0) > 0) &&
            (posRegex.cap(1) == pos))
        {
            QString str = def.left(def.indexOf("[")).simplified();
            if (!str.isEmpty())
                return str;
        }
    }

    return QString();
}

//---------------------------------------------------------------------------
//  importPlayability
//
//! Import playability values for a lexicon from a file.  The file is assumed
//! to be in plain text format, containing one playability value and word per
//! line.
//
//! @param lexicon the name of the lexicon
//! @param filename the name of the file to import
//! @param errString returns the error string in case of error
//! @return the number of playability values imported
//---------------------------------------------------------------------------
int
CreateDatabaseThread::importPlayability(const QString& filename,
    QMap<QString, qint64>& playabilityMap) const
{
    playabilityMap.clear();

    QFile file (filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0;

    QStringList words;
    QSet<QString> alphagrams;
    int imported = 0;
    bool readNewline = true;
    char* buffer = new char[MAX_INPUT_LINE_LEN * 2 + 1];
    while (file.readLine(buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);

        // If first line didn't contain newline, skip subsequent reads
        // until we see a newline (effectively truncating long lines)
        bool skip = !readNewline;
        readNewline = (line.right(1) == QString("\n"));
        if (skip)
            continue;

        line = line.simplified();
        if (line.isEmpty() || (line.at(0) == '#'))
            continue;
        bool ok = false;
        qint64 playability = line.section(' ', 0, 0).toLongLong(&ok);
        if (!ok)
            continue;
        QString word = line.section(' ', 1, 1);
        if (word.isEmpty())
            continue;

        playabilityMap[word] = playability;
        ++imported;
    }
    delete[] buffer;

    return imported;
}
