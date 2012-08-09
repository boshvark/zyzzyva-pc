//---------------------------------------------------------------------------
// WordEngine.cpp
//
// A class to handle the loading and searching of words.
//
// Copyright 2004-2012 Boshvark Software, LLC.
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

#include "WordEngine.h"
#include "LetterBag.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QFile>
#include <QRegExp>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QVector>

using namespace Defs;

const int LIMIT_RANGE_MAX = 999999;

//---------------------------------------------------------------------------
//  clearCache
//
//! Clear the word information cache for a lexicon.
//
//! @param lexicon the name of the lexicon
//---------------------------------------------------------------------------
void
WordEngine::clearCache(const QString& lexicon) const
{
    if (!lexiconData.contains(lexicon))
        return;

    lexiconData[lexicon]->wordCache.clear();
}

//---------------------------------------------------------------------------
//  connectToDatabase
//
//! Initialize the database connection for a lexicon.
//
//! @param lexicon the name of the lexicon
//! @param filename the name of the database file
//! @param errString returns the error string in case of error
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::connectToDatabase(const QString& lexicon, const QString& filename,
                              QString* errString)
{
    if (!lexiconData.contains(lexicon))
        return false;

    Rand rng;
    rng.srand(QDateTime::currentDateTime().toTime_t(), Auxil::getPid());
    unsigned int r = rng.rand();
    QString dbConnectionName = "WordEngine_" + lexicon + "_" +
        QString::number(r);

    QSqlDatabase* db = new QSqlDatabase(
        QSqlDatabase::addDatabase("QSQLITE", dbConnectionName));
    db->setDatabaseName(filename);
    bool ok = db->open();

    if (!ok) {
        dbConnectionName = QString();
        if (errString)
            *errString = db->lastError().text();
        // delete db?
        return false;
    }

    LexiconData* data = lexiconData[lexicon];
    data->db = db;
    data->dbConnectionName = dbConnectionName;
    return true;
}

//---------------------------------------------------------------------------
//  disconnectFromDatabase
//
//! Remove the database connection for a lexicon.
//
//! @param lexicon the name of the lexicon
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::disconnectFromDatabase(const QString& lexicon)
{
    if (!lexiconData.contains(lexicon))
        return true;

    QSqlDatabase* db = lexiconData[lexicon]->db;
    QString dbConnectionName = lexiconData[lexicon]->dbConnectionName;
    if (!db || !db->isOpen() || dbConnectionName.isEmpty())
        return true;

    delete db;
    lexiconData[lexicon]->db = 0;
    QSqlDatabase::removeDatabase(dbConnectionName);
    lexiconData[lexicon]->dbConnectionName.clear();
    return true;
}

//---------------------------------------------------------------------------
//  databaseIsConnected
//
//! Determine whether a lexicon database is connected.
//
//! @param lexicon the name of the lexicon
//! @return true if the database is connected, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::databaseIsConnected(const QString& lexicon) const
{
    return (lexiconData.contains(lexicon) && lexiconData[lexicon]->db);
}

//---------------------------------------------------------------------------
//  importTextFile
//
//! Import words from a text file.  The file is assumed to be in plain text
//! format, containing one word per line.
//
//! @param lexicon the name of the lexicon
//! @param filename the name of the file to import
//! @param loadDefinitions whether to load word definitions
//! @param errString returns the error string in case of error
//! @return the number of words imported
//---------------------------------------------------------------------------
int
WordEngine::importTextFile(const QString& lexicon, const QString& filename,
                           bool loadDefinitions, QString* errString)
{
    // Delete old word graph if it exists
    if (lexiconData.contains(lexicon))
        delete lexiconData[lexicon]->graph;
    else
        lexiconData[lexicon] = new LexiconData;

    WordGraph* graph = new WordGraph;
    lexiconData[lexicon]->graph = graph;
    lexiconData[lexicon]->lexiconFile = filename;

    QFile file (filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errString) {
            *errString = "Can't open file '" + filename + "': " +
                file.errorString();
        }
        return 0;
    }

    int imported = 0;
    char* buffer = new char[MAX_INPUT_LINE_LEN];
    while (file.readLine(buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);
        line = line.simplified();
        if (!line.length() || (line.at(0) == '#'))
            continue;
        QString word = line.section(' ', 0, 0).toUpper();

        if (!graph->containsWord(word)) {
            QString alpha = Auxil::getAlphagram(word);
            ++lexiconData[lexicon]->numAnagramsMap[alpha];
        }

        graph->addWord(word);
        if (loadDefinitions) {
            QString definition = line.section(' ', 1);
            addDefinition(lexicon, word, definition);
        }
        ++imported;
    }

    delete[] buffer;
    return imported;
}

//---------------------------------------------------------------------------
//  importDawgFile
//
//! Import words from a DAWG file as generated by Graham Toal's dawgutils
//! programs: http://www.gtoal.com/wordgames/dawgutils/
//
//! @param lexicon the name of the lexicon
//! @param filename the name of the DAWG file to import
//! @param reverse whether the DAWG contains reversed words
//! @param errString returns the error string in case of error
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::importDawgFile(const QString& lexicon, const QString& filename,
                           bool reverse, QString* errString, quint16*
                           expectedChecksum)
{
    if (!lexiconData.contains(lexicon)) {
        lexiconData[lexicon] = new LexiconData;
        lexiconData[lexicon]->graph = new WordGraph;
    }

    WordGraph* graph = lexiconData[lexicon]->graph;
    bool ok = graph->importDawgFile(filename, reverse, errString,
                                    expectedChecksum);
    return ok;
}

//---------------------------------------------------------------------------
//  importStems
//
//! Import stems for a lexicon from a file.  The file is assumed to be in
//! plain text format, containing one stem per line.  The file is also assumed
//! to contain stems of equal length.  All stems of different length than the
//! first stem will be discarded.
//
//! @param lexicon the name of the lexicon
//! @param filename the name of the file to import
//! @param errString returns the error string in case of error
//! @return the number of stems imported
//---------------------------------------------------------------------------
int
WordEngine::importStems(const QString& lexicon, const QString& filename,
                        QString* errString)
{
    if (!lexiconData.contains(lexicon))
        return 0;

    QFile file (filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errString) {
            *errString = "Can't open file '" + filename + "': " +
                file.errorString();
        }
        return -1;
    }

    // XXX: At some point, may want to consider allowing words of varying
    // lengths to be in the same file?
    QStringList words;
    QSet<QString> alphagrams;
    int imported = 0;
    int length = 0;
    char* buffer = new char[MAX_INPUT_LINE_LEN];
    while (file.readLine(buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);
        line = line.simplified();
        if (!line.length() || (line.at(0) == '#'))
            continue;
        QString word = line.section(' ', 0, 0);

        if (!length)
            length = word.length();

        if (length != int(word.length()))
            continue;

        words << word;
        alphagrams.insert(Auxil::getAlphagram(word));
        ++imported;
    }
    delete[] buffer;

    // Insert the stem list into the map, or append to an existing stem list
    LexiconData* data = lexiconData[lexicon];
    data->stems[length] += words;
    data->stemAlphagrams[length].unite(alphagrams);
    return imported;
}

//---------------------------------------------------------------------------
//  databaseSearch
//
//! Search the database for words matching the conditions in a search spec.
//! If a word list is provided, also ensure that result words are in that
//! list.
//
//! @param lexicon the name of the lexicon
//! @param optimizedSpec the search spec
//! @param wordList optional list of words that results must be in
//! @return a list of words matching the search spec
//---------------------------------------------------------------------------
QStringList
WordEngine::databaseSearch(const QString& lexicon, const SearchSpec&
                           optimizedSpec, const QStringList* wordList) const
{
    if (!lexiconData.contains(lexicon) || !lexiconData[lexicon]->db)
        return QStringList();

    // Build SQL query string
    QSet<QString> tables;
    QString whereStr;
    bool foundCondition = false;
    QListIterator<SearchCondition> cit (optimizedSpec.conditions);
    while (cit.hasNext()) {
        SearchCondition condition = cit.next();
        if (getConditionPhase(condition) != DatabasePhase)
            continue;

        if (foundCondition)
            whereStr += " AND";
        foundCondition = true;
        whereStr += " (";

        switch (condition.type) {
            case SearchCondition::PatternMatch: {
                tables.insert("words");
                QString str =
                    condition.stringValue.replace("?", "_").replace("*", "%");
                whereStr += " words.word";
                if (condition.negated)
                    whereStr += " NOT";
                whereStr += " LIKE '" + str + "'";
            }
            break;

            case SearchCondition::PartOfSpeech:
            case SearchCondition::Definition: {
                tables.insert("words");

                // Escape % and _ characters when preceded by an even number
                // of backslashes
                QString str = condition.stringValue.replace(
                    QRegExp("((?:\\\\\\\\)*)([%%_'\\\\])"), "\\1\\\\2");

                // ### replace * with % and ? with _ for more flexible search
                // tricky to get right

                str.replace("'", "''");
                str.replace(";", "\\;");

                QString notStr;
                QString conjStr = " OR";
                if (condition.negated) {
                    conjStr = " AND";
                    notStr = " NOT";
                }

                QString whereSecondStr;
                if (condition.type == SearchCondition::PartOfSpeech) {
                    whereSecondStr = conjStr +
                        " words.definition" + notStr +
                        " LIKE '\%[" + str + "]\%' ESCAPE '\\'";
                    str = "[" + str + " ";
                }

                whereStr +=
                    " words.definition" + notStr +
                    " LIKE '\%" + str + "\%' ESCAPE '\\'" + whereSecondStr;
            }
            break;

            case SearchCondition::ProbabilityOrder:
            case SearchCondition::PlayabilityOrder: {
                tables.insert("words");
                QString col;
                if (condition.type == SearchCondition::ProbabilityOrder) {
                    col = QString("probability_order%1").arg(
                        condition.intValue);
                }
                else if (condition.type == SearchCondition::PlayabilityOrder) {
                    col = "playability_order";
                }

                // Lax boundaries
                if (condition.boolValue) {
                    whereStr += QString(" words.max_%1>=").arg(col) +
                        QString::number(condition.minValue) +
                        QString(" AND words.min_%1<=").arg(col) +
                        QString::number(condition.maxValue);
                }
                // Strict boundaries
                else {
                    whereStr += QString(" words.%1").arg(col);
                    if (condition.minValue == condition.maxValue) {
                        whereStr += "=" + QString::number(condition.minValue);
                    }
                    else {
                        whereStr += ">=" +
                            QString::number(condition.minValue) +
                            QString(" AND words.%1<=").arg(col) +
                            QString::number(condition.maxValue);
                    }
                }
            }
            break;

            case SearchCondition::Length:
            case SearchCondition::NumVowels:
            case SearchCondition::NumUniqueLetters:
            case SearchCondition::PointValue:
            case SearchCondition::NumAnagrams: {
                tables.insert("words");
                QString column;
                if (condition.type == SearchCondition::Length)
                    column = "words.length";
                if (condition.type == SearchCondition::NumVowels)
                    column = "words.num_vowels";
                if (condition.type == SearchCondition::NumUniqueLetters)
                    column = "words.num_unique_letters";
                if (condition.type == SearchCondition::PointValue)
                    column = "words.point_value";
                if (condition.type == SearchCondition::NumAnagrams)
                    column = "words.num_anagrams";

                whereStr += " " + column;
                if (condition.minValue == condition.maxValue) {
                    whereStr += "=" + QString::number(condition.minValue);
                }
                else {
                    whereStr += ">=" + QString::number(condition.minValue) +
                        " AND " + column + "<=" +
                        QString::number(condition.maxValue);
                }
            }
            break;

            case SearchCondition::IncludeLetters: {
                tables.insert("words");
                QString str = condition.stringValue;
                QMap<QChar, int> letters;
                for (int i = 0; i < str.length(); ++i) {
                    ++letters[str.at(i)];
                }

                QMapIterator<QChar, int> it (letters);
                for (int i = 0; it.hasNext(); ++i) {
                    it.next();
                    QChar c = it.key();
                    if (i)
                        whereStr += " AND";
                    whereStr += " word";
                    if (condition.negated)
                        whereStr += " NOT";
                    whereStr += " LIKE '%";
                    int count = condition.negated ? 1 : it.value();
                    for (int j = 0; j < count; ++j) {
                        whereStr += QString(c) + "%";
                    }
                    whereStr += "'";
                }
            }
            break;

            case SearchCondition::BelongToGroup: {
                tables.insert("words");
                SearchSet searchSet =
                    Auxil::stringToSearchSet(condition.stringValue);
                int target = condition.negated ? 0 : 1;
                switch (searchSet) {
                    case SetFrontHooks:
                    whereStr += " words.is_front_hook=" +
                        QString::number(target);
                    break;

                    case SetBackHooks:
                    whereStr += " words.is_back_hook=" +
                        QString::number(target);
                    break;

                    case SetHookWords:
                    if (condition.negated) {
                        whereStr += " (words.is_front_hook=0 AND "
                            "words.is_back_hook=0)";
                    }
                    else {
                        whereStr += " (words.is_front_hook=1 OR "
                            "words.is_back_hook=1)";
                    }
                    break;

                    default:
                    break;
                }
            }
            break;

            case SearchCondition::InWordList: {
                tables.insert("words");
                whereStr += " words.word";
                if (condition.negated)
                    whereStr += " NOT";
                whereStr += " IN (";
                QStringList words = condition.stringValue.split(QChar(' '));
                QStringListIterator it (words);
                bool firstWord = true;
                while (it.hasNext()) {
                    QString word = it.next();
                    if (!firstWord)
                        whereStr += ",";
                    firstWord = false;
                    whereStr += "'" + word + "'";
                }
                whereStr += ")";
            }
            break;

            default:
            break;
        }

        whereStr += ")";
    }

    // Make sure results are in the provided word list
    QMap<QString, QString> upperToLower;
    if (wordList) {
        tables.insert("words");
        whereStr += " AND words.word IN (";
        QStringListIterator it (*wordList);
        bool firstWord = true;
        while (it.hasNext()) {
            QString word = it.next();
            QString wordUpper = word.toUpper();
            upperToLower[wordUpper] = word;
            if (!firstWord)
                whereStr += ",";
            firstWord = false;
            whereStr += "'" + wordUpper + "'";
        }
        whereStr += ")";
    }

    QStringList tablesList = tables.toList();
    QString tableStr = " " + tablesList.join(", ");
    QString selectColStr = "word";
    if (tables.contains("words")) {
        selectColStr = " words.word";
    }

    QString queryStr = "SELECT" + selectColStr + " FROM" + tableStr +
        " WHERE" + whereStr;

    //qDebug("Query str: |%s|", queryStr.toUtf8().constData());

    // Query the database
    QStringList resultList;
    QSqlDatabase* db = lexiconData[lexicon]->db;
    QSqlQuery query (queryStr, *db);
    while (query.next()) {
        QString word = query.value(0).toString();
        if (!upperToLower.isEmpty() && upperToLower.contains(word)) {
            word = upperToLower[word];
        }
        resultList.append(word);
    }

    return resultList;
}

//---------------------------------------------------------------------------
//  applyPostConditions
//
//! Limit search results by search conditions that cannot be easily used to
//! limit word graph or database searches.
//
//! @param optimizedSpec the search spec
//! @param wordList optional list of words that results must be in
//! @return a list of words matching the search spec
//---------------------------------------------------------------------------
QStringList
WordEngine::applyPostConditions(const QString& lexicon,
    const SearchSpec& optimizedSpec, const QStringList& wordList) const
{
    QStringList returnList = wordList;

    // Check special postconditions
    QStringList::iterator wit;
    for (wit = returnList.begin(); wit != returnList.end();) {
        if (matchesPostConditions(lexicon, *wit, optimizedSpec.conditions))
            ++wit;
        else
            wit = returnList.erase(wit);
    }
    if (returnList.isEmpty())
        return returnList;

    // Handle Limit by Probability/Playability Order conditions
    bool legacyProbCondition = false;
    const int MIN_INDEX = 0;
    const int MAX_INDEX = 1;
    const int MIN_LAX_INDEX = 2;
    const int MAX_LAX_INDEX = 3;
    QMap<QPair<SearchCondition::SearchType, int>, QVector<int> > limits;
    QListIterator<SearchCondition> cit (optimizedSpec.conditions);
    while (cit.hasNext()) {
        SearchCondition condition = cit.next();
        if ((condition.type == SearchCondition::LimitByProbabilityOrder) ||
            (condition.type == SearchCondition::LimitByPlayabilityOrder))
        {
            int probNumBlanks = 0;
            if (condition.type == SearchCondition::LimitByProbabilityOrder) {
                probNumBlanks = condition.intValue;
                if (condition.legacy)
                    legacyProbCondition = true;
            }

            QPair<SearchCondition::SearchType, int> typePair =
                qMakePair(condition.type, probNumBlanks);

            // Initialize limits
            if (!limits.contains(typePair)) {
                limits[typePair] = (QVector<int>() << 0 <<
                    LIMIT_RANGE_MAX << 0 << LIMIT_RANGE_MAX);
            }

            if (condition.boolValue) {
                if (condition.minValue > limits[typePair][MIN_LAX_INDEX])
                    limits[typePair][MIN_LAX_INDEX] = condition.minValue;
                if (condition.maxValue < limits[typePair][MAX_LAX_INDEX])
                    limits[typePair][MAX_LAX_INDEX] = condition.maxValue;
            }
            else {
                if (condition.minValue > limits[typePair][MIN_INDEX])
                    limits[typePair][MIN_INDEX] = condition.minValue;
                if (condition.maxValue < limits[typePair][MAX_INDEX])
                    limits[typePair][MAX_INDEX] = condition.maxValue;
            }
        }
    }

    // Keep only words in the limit ranges
    if (!limits.isEmpty()) {
        QSet<QString> returnSet = returnList.toSet();
        QMap<int, QMap<QString, QString> > probValueMap;
        QMap<QString, QString> playValueMap;

        QMapIterator<QPair<SearchCondition::SearchType, int>, QVector<int> >
            it (limits);
        while (it.hasNext()) {
            it.next();
            SearchCondition::SearchType searchType = it.key().first;
            int probNumBlanks = it.key().second;
            const QVector<int> limitValues = it.value();
            bool probCondition =
                (searchType == SearchCondition::LimitByProbabilityOrder);

            if ((limitValues[MIN_INDEX] > limitValues[MAX_INDEX]) ||
                (limitValues[MIN_INDEX] > returnList.size()) ||
                (limitValues[MIN_LAX_INDEX] > returnList.size()))
            {
                return QStringList();
            }

            // Convert from 1-based to 0-based offset
            int limitMin = limitValues[MIN_INDEX] - 1;
            int limitMax = limitValues[MAX_INDEX] - 1;
            int limitMinLax = limitValues[MIN_LAX_INDEX] - 1;
            int limitMaxLax = limitValues[MAX_LAX_INDEX] - 1;

            // Snap limits to endpoints
            if (limitMin < 0)
                limitMin = 0;
            if (limitMinLax < 0)
                limitMinLax = 0;
            if (limitMax > returnList.size() - 1)
                limitMax = returnList.size() - 1;
            if (limitMaxLax > returnList.size() - 1)
                limitMaxLax = returnList.size() - 1;

            // Use the higher of the min values as working min
            int min = (limitMin > limitMinLax) ? limitMin : limitMinLax;

            // Use the lower of the max values as working max
            int max = (limitMax < limitMaxLax) ? limitMax : limitMaxLax;

            // Sort the words according to probability order
            if (probCondition) {
                QMap<QString, QString>& valueMap = probValueMap[probNumBlanks];
                if (valueMap.isEmpty()) {
                    LetterBag bag;
                    foreach (const QString& word, returnList) {
                        QString radix;
                        QString wordUpper = word.toUpper();
                        int combinations =
                            bag.getNumCombinations(wordUpper, probNumBlanks);
                        radix.sprintf("%018lld", 999999999999999999LL -
                            combinations);
                        // Legacy probability order limits are sorted
                        // alphabetically, not by alphagram
                        if (!legacyProbCondition)
                            radix += Auxil::getAlphagram(wordUpper) + ":";
                        radix += wordUpper;
                        valueMap.insert(radix, word);
                    }
                }
            }

            // Sort the words according to playability order
            else if (playValueMap.isEmpty()) {
                LexiconData* lexData = lexiconData.value(lexicon);
                if (!lexData)
                    return returnList;

                QSqlDatabase* db = lexData->db;
                if (!db || !db->isOpen())
                    return returnList;

                QMap<QString, QString> origCase;
                QString qstr = "SELECT word, playability FROM words "
                    "WHERE word IN (";
                QStringListIterator it (returnList);
                for (int i = 0; it.hasNext(); ++i) {
                    const QString& word = it.next();
                    origCase[word.toUpper()] = word;
                    if (i)
                        qstr += ", ";
                    qstr += "'" + word.toUpper() + "'";
                }
                qstr += ")";

                QSqlQuery query (*db);
                query.prepare(qstr);
                query.exec();

                while (query.next()) {
                    QString word = origCase[query.value(0).toString()];
                    qint64 playability = query.value(1).toLongLong();
                    QString radix;
                    QString wordUpper = word.toUpper();
                    radix.sprintf("%018lld", 999999999999999999LL - playability);
                    radix += Auxil::getAlphagram(wordUpper) + ":";
                    radix += wordUpper;
                    playValueMap.insert(radix, word);
                }
            }

            QMap<QString, QString>& valueMap = probCondition ?
                probValueMap[probNumBlanks] : playValueMap;
            QStringList keys = valueMap.keys();

            // Allow Lax matches only up to hard Min limit
            QString minRadix = keys[min];
            QString minValue = minRadix.left(18);
            while ((min > 0) && (min > limitMin)) {
                if (minValue != keys[min - 1].left(18))
                    break;
                --min;
            }

            // Allow Lax matches only up to hard Max limit
            QString maxRadix = keys[max];
            QString maxValue = maxRadix.left(18);
            while ((max < keys.size() - 1) && (max < limitMax)) {
                if (maxValue != keys[max + 1].left(18))
                    break;
                ++max;
            }

            if (min > max)
                return QStringList();

            // Only keep candidates that matched constraints
            returnSet &= (valueMap.values().mid(min, max - min + 1)).toSet();
        }

        returnList = returnSet.toList();
    }

    return returnList;
}

//---------------------------------------------------------------------------
//  lexiconIsLoaded
//
//! Determine whether a lexicon is loaded.
//
//! @param lexicon the name of the lexicon
//! @return true if the lexicon is loaded, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::lexiconIsLoaded(const QString& lexicon) const
{
    return lexiconData.contains(lexicon);
}

//---------------------------------------------------------------------------
//  isAcceptable
//
//! Determine whether a word is acceptable in a lexicon.
//
//! @param lexicon the name of the lexicon
//! @param word the word to look up
//! @return true if acceptable, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::isAcceptable(const QString& lexicon, const QString& word) const
{
    if (!lexiconData.contains(lexicon))
        return false;

    return lexiconData[lexicon]->graph->containsWord(word);
}

//---------------------------------------------------------------------------
//  search
//
//! Search for acceptable words matching a search specification.
//
//! @param lexicon the name of the lexicon
//! @param spec the search specification
//! @param allCaps whether to ensure the words in the list are all caps
//! @return a list of acceptable words
//---------------------------------------------------------------------------
QStringList
WordEngine::search(const QString& lexicon, const SearchSpec& spec, bool
                   allCaps) const
{
    if (!lexiconData.contains(lexicon))
        return QStringList();

    SearchSpec optimizedSpec = spec;
    optimizedSpec.optimize(lexicon);

    // Discover which kinds of search conditions are present
    QMap<ConditionPhase, int> phaseCounts;
    int lengthConditions = 0;
    QListIterator<SearchCondition> cit (optimizedSpec.conditions);
    while (cit.hasNext()) {
        SearchCondition condition = cit.next();
        ConditionPhase phase = getConditionPhase(condition);
        ++phaseCounts[phase];
        if (condition.type == SearchCondition::Length)
            ++lengthConditions;
    }

    // Do not search the database based on Length conditions that were only
    // added by SearchSpec::optimize to optimize word graph searches
    if ((phaseCounts.contains(WordGraphPhase)) && lengthConditions &&
        (lengthConditions == phaseCounts.value(DatabasePhase)))
    {
        --phaseCounts[DatabasePhase];
    }

    // Search the word graph if necessary
    QStringList resultList;
    if (phaseCounts.value(WordGraphPhase) ||
        !phaseCounts.value(DatabasePhase))
    {
        resultList = wordGraphSearch(lexicon, optimizedSpec);
        if (resultList.isEmpty())
            return resultList;
    }

    // Search the database if necessary, passing word graph results
    if (phaseCounts.value(DatabasePhase)) {
        resultList = databaseSearch(lexicon, optimizedSpec,
            phaseCounts.contains(WordGraphPhase) ? &resultList : 0);
        if (resultList.isEmpty())
            return resultList;
    }

    // Check post conditions if necessary
    if (phaseCounts.value(PostConditionPhase)) {
        resultList = applyPostConditions(lexicon, optimizedSpec, resultList);
    }

    // Convert to all caps if necessary
    if (allCaps) {
        QStringList::iterator it;
        for (it = resultList.begin(); it != resultList.end(); ++it)
            *it = (*it).toUpper();
    }

    if (!resultList.isEmpty()) {
        clearCache(lexicon);
        addToCache(lexicon, resultList);
    }

    return resultList;
}

//---------------------------------------------------------------------------
//  wordGraphSearch
//
//! Search the word graph for words matching the conditions in a search spec.
//
//! @param lexicon the name of the lexicon
//! @param optimizedSpec the search spec
//! @return a list of words
//---------------------------------------------------------------------------
QStringList
WordEngine::wordGraphSearch(const QString& lexicon, const SearchSpec&
                            optimizedSpec) const
{
    if (!lexiconData.contains(lexicon))
        return QStringList();

    return lexiconData[lexicon]->graph->search(optimizedSpec);
}

//---------------------------------------------------------------------------
//  alphagrams
//
//! Transform a list of strings into a list of alphagrams of those strings.
//! The created list may be shorter than the original list.
//
//! @param list the list of strings
//! @return a list of alphagrams
//---------------------------------------------------------------------------
QStringList
WordEngine::alphagrams(const QStringList& strList) const
{
    // Insert into a set to remove duplicates
    QSet<QString> alphaSet;
    foreach (const QString& str, strList) {
        alphaSet.insert(Auxil::getAlphagram(str));
    }

    QStringList alphaList = alphaSet.toList();
    qSort(alphaList.begin(), alphaList.end(),
          Auxil::localeAwareLessThanQString);
    return alphaList;
}

//---------------------------------------------------------------------------
//  getWordInfo
//
//! Get information about a word from the database.  Also cache the
//! information for future queries.  Fail if the information is not in the
//! cache and the database is not open.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return information about the word from the database
//---------------------------------------------------------------------------
WordEngine::WordInfo
WordEngine::getWordInfo(const QString& lexicon, const QString& word) const
{
    if (word.isEmpty())
        return WordInfo();

    if (!lexiconData.contains(lexicon))
        return WordInfo();

    if (lexiconData[lexicon]->wordCache.contains(word)) {
        //qDebug("Cache HIT: |%s|", word.toUtf8().data());
        return lexiconData[lexicon]->wordCache[word];
    }
    //qDebug("Cache MISS: |%s|", word.toUtf8().data());

    addToCache(lexicon, QStringList(word));
    return lexiconData[lexicon]->wordCache.value(word);
}

//---------------------------------------------------------------------------
//  getNumWords
//
//! Return a word count for the current lexicon.
//
//! @param lexicon the name of the lexicon
//! @return the word count
//---------------------------------------------------------------------------
int
WordEngine::getNumWords(const QString& lexicon) const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    QSqlDatabase* db = lexiconData[lexicon]->db;
    if (db && db->isOpen()) {
        QString qstr = "SELECT count(*) FROM words";
        QSqlQuery query (qstr, *db);
        if (query.next())
            return query.value(0).toInt();
    }
    else
        return lexiconData[lexicon]->graph->getNumWords();

    return 0;
}

//---------------------------------------------------------------------------
//  getLexiconFile
//
//! Get the text file associated with a lexicon, if applicable.  This is
//! normally only meaningful for custom lexicons.
//
//! @param lexicon the name of the lexicon
//! @return the lexicon file
//---------------------------------------------------------------------------
QString
WordEngine::getLexiconFile(const QString& lexicon) const
{
    if (!lexiconData.contains(lexicon))
        return QString();

    return lexiconData[lexicon]->lexiconFile;
}

//---------------------------------------------------------------------------
//  getDefinition
//
//! Return the definition associated with a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word whose definition to look up
//! @param replaceLinks whether to resolve links to other definitions
//! @return the definition, or empty String if no definition
//---------------------------------------------------------------------------
QString
WordEngine::getDefinition(const QString& lexicon, const QString& word,
                          bool replaceLinks) const
{
    if (!lexiconData.contains(lexicon))
        return QString();

    WordInfo info = getWordInfo(lexicon, word);
    //qDebug("WordEngine::getDefinition: lexicon: |%s|, word: |%s|",
    //       lexicon.toUtf8().constData(), word.toUtf8().constData());
    //qDebug("info.isValid: %d, info.word: |%s|, "
    //       "info.numVowels: %d, info.numUniqueLetters: %d, "
    //       "info.numAnagrams: %d, info.pointValue: %d, "
    //       "info.frontHooks: |%s|, info.backHooks: |%s|, "
    //       "info.isFrontHook: %d, info.isBackHook: %d, "
    //       "info.lexiconSymbols: |%s|, info.definition: |%s|",
    //       info.isValid(), info.word.toUtf8().constData(),
    //       info.numVowels, info.numUniqueLetters, info.numAnagrams,
    //       info.pointValue, info.frontHooks.toUtf8().constData(),
    //       info.backHooks.toUtf8().constData(), info.isFrontHook,
    //       info.isBackHook, info.lexiconSymbols.toUtf8().constData(),
    //       info.definition.toUtf8().constData());
    //QMapIterator<int, ProbabilityOrder> it (info.blankProbability);
    //while (it.hasNext()) {
    //    it.next();
    //    int numBlanks = it.key();
    //    const ProbabilityOrder& probOrder = it.value();
    //    qDebug("numBlanks: %d, prob order: %d, min prob order: %d, "
    //           "max prob order: %d", numBlanks, probOrder.probabilityOrder,
    //           probOrder.minProbabilityOrder, probOrder.maxProbabilityOrder);
    //}

    QString definition;
    if (info.isValid()) {
        if (replaceLinks) {
            QStringList defs = info.definition.split(" / ");
            definition = QString();
            foreach (const QString& def, defs) {
                if (!definition.isEmpty())
                    definition += "\n";
                definition += def;
            }
            return definition;
        }
        else {
            return info.definition;
        }
    }

    else {
        if (!lexiconData[lexicon]->definitions.contains(word))
            return QString();

        const QMultiMap<QString, QString>& mmap =
            lexiconData[lexicon]->definitions.value(word);
        QMapIterator<QString, QString> it (mmap);
        while (it.hasNext()) {
            it.next();
            if (!definition.isEmpty()) {
                if (replaceLinks)
                    definition += "\n";
                else
                    definition += " / ";
            }
            definition += it.value();
        }
        return definition;
    }
}

//---------------------------------------------------------------------------
//  getFrontHookLetters
//
//! Get a string of letters that can be added to the front of a word to make
//! other valid words.
//
//! @param lexicon the name of the lexicon
//! @param word the word, assumed to be upper case
//! @return a string containing lower case letters representing front hooks
//---------------------------------------------------------------------------
QString
WordEngine::getFrontHookLetters(const QString& lexicon, const QString& word)
    const
{
    QString ret;

    WordInfo info = getWordInfo(lexicon, word);
    if (info.isValid()) {
        ret = info.frontHooks;
    }

    else {
        SearchSpec spec;
        SearchCondition condition;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "?" + word;
        spec.conditions.append(condition);

        // Get and sort first letters of each word
        QStringList words = search(lexicon, spec, true);
        QList<QChar> letters;
        foreach (const QString& str, words) {
            letters.append(str.at(0).toLower());
        }
        qSort(letters.begin(), letters.end(),
              Auxil::localeAwareLessThanQChar);

        QListIterator<QChar> it (letters);
        while (it.hasNext())
            ret += it.next();
    }

    return ret;
}

//---------------------------------------------------------------------------
//  getBackHookLetters
//
//! Get a string of letters that can be added to the back of a word to make
//! other valid words.
//
//! @param lexicon the name of the lexicon
//! @param word the word, assumed to be upper case
//! @return a string containing lower case letters representing back hooks
//---------------------------------------------------------------------------
QString
WordEngine::getBackHookLetters(const QString& lexicon, const QString& word) const
{
    QString ret;

    WordInfo info = getWordInfo(lexicon, word);
    if (info.isValid()) {
        ret = info.backHooks;
    }

    else {
        SearchSpec spec;
        SearchCondition condition;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = word + "?";
        spec.conditions.append(condition);

        // Get and sort last letters of each word
        QStringList words = search(lexicon, spec, true);
        QList<QChar> letters;
        foreach (const QString& str, words) {
            letters.append(str.at(str.length() - 1).toLower());
        }
        qSort(letters.begin(), letters.end(),
              Auxil::localeAwareLessThanQChar);

        QListIterator<QChar> it (letters);
        while (it.hasNext())
            ret += it.next();
    }

    return ret;
}

//---------------------------------------------------------------------------
//  addToCache
//
//! Add information about a list of words to the cache.
//
//! @param lexicon the name of the lexicon
//! @param words the list of words
//---------------------------------------------------------------------------
void
WordEngine::addToCache(const QString& lexicon, const QStringList& words) const
{
    if (words.isEmpty() || !lexiconData.contains(lexicon))
        return;

    LexiconData* lexData = lexiconData[lexicon];
    QSqlDatabase* db = lexData->db;
    if (!db || !db->isOpen())
        return;

    QString qstr = "SELECT word, num_vowels, "
        "num_unique_letters, num_anagrams, point_value, "
        "front_hooks, back_hooks, is_front_hook, "
        "is_back_hook, lexicon_symbols, definition, playability, "
        "playability_order, min_playability_order, max_playability_order, "
        "probability_order0, min_probability_order0, max_probability_order0, "
        "probability_order1, min_probability_order1, max_probability_order1, "
        "probability_order2, min_probability_order2, max_probability_order2 "
        "FROM words WHERE words.word";

    // Throw out words that are already in the cache
    QStringList needWords;
    foreach (const QString& word, words) {
        if (lexData->wordCache.contains(word))
            continue;
        needWords.append(word);
    }

    // Construct the where clause from the word list
    if (needWords.count() == 1) {
        qstr += "='" + needWords.first() + "'";
    }
    else {
        qstr += " IN (";
        QStringListIterator it (needWords);
        for (int i = 0; it.hasNext(); ++i) {
            if (i)
                qstr += ", ";
            qstr += "'" + it.next().toUpper() + "'";
        }
        qstr += ")";
    }

    QSqlQuery query (*db);
    query.prepare(qstr);
    query.exec();

    while (query.next()) {
        int placeNum = 0;
        WordInfo info;
        info.word                 = query.value(placeNum++).toString();
        info.numVowels            = query.value(placeNum++).toInt();
        info.numUniqueLetters     = query.value(placeNum++).toInt();
        info.numAnagrams          = query.value(placeNum++).toInt();
        info.pointValue           = query.value(placeNum++).toInt();
        info.frontHooks           = query.value(placeNum++).toString();
        info.backHooks            = query.value(placeNum++).toString();
        info.isFrontHook          = query.value(placeNum++).toBool();
        info.isBackHook           = query.value(placeNum++).toBool();
        info.lexiconSymbols       = query.value(placeNum++).toString();
        info.definition           = query.value(placeNum++).toString();
        info.playability          = query.value(placeNum++).toLongLong();

        ValueOrder playOrder;
        playOrder.valueOrder    = query.value(placeNum++).toInt();
        playOrder.minValueOrder = query.value(placeNum++).toInt();
        playOrder.maxValueOrder = query.value(placeNum++).toInt();
        info.playabilityOrder = playOrder;

        for (int numBlanks = 0; numBlanks <= 2; ++numBlanks) {
            ValueOrder probOrder;
            probOrder.valueOrder    = query.value(placeNum++).toInt();
            probOrder.minValueOrder = query.value(placeNum++).toInt();
            probOrder.maxValueOrder = query.value(placeNum++).toInt();
            info.blankProbabilityOrder[numBlanks] = probOrder;
        }

        lexiconData[lexicon]->wordCache[info.word] = info;
    }
}

//---------------------------------------------------------------------------
//  matchesPostConditions
//
//! Test whether a word matches certain conditions.  Not all conditions in the
//! list are tested.  Only the conditions that cannot be easily tested in
//! WordGraph::search are tested here.
//
//! @param lexicon the name of the lexicon
//! @param word the word to be tested
//! @param conditions the list of conditions to test
//! @return true if the word matches all special conditions, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::matchesPostConditions(const QString& lexicon, const QString& word,
                                  const QList<SearchCondition>& conditions) const
{
    if (!lexiconData.contains(lexicon))
        return false;

    QString wordUpper = word.toUpper();
    QListIterator<SearchCondition> it (conditions);
    while (it.hasNext()) {
        const SearchCondition& condition = it.next();
        if (getConditionPhase(condition) != PostConditionPhase)
            continue;

        switch (condition.type) {

            case SearchCondition::Prefix:
            if ((!isAcceptable(lexicon, condition.stringValue + wordUpper))
                ^ condition.negated)
                return false;
            break;

            case SearchCondition::Suffix:
            if ((!isAcceptable(lexicon, wordUpper + condition.stringValue))
                ^ condition.negated)
                return false;
            break;

            case SearchCondition::BelongToGroup: {
                SearchSet searchSet =
                    Auxil::stringToSearchSet(condition.stringValue);
                if (searchSet == UnknownSearchSet)
                    continue;
                if (!isSetMember(lexicon, wordUpper, searchSet)
                    ^ condition.negated)
                    return false;
            }
            break;

            case SearchCondition::InLexicon:
            if ((!isAcceptable(condition.stringValue, wordUpper))
                ^ condition.negated)
                return false;
            break;

            default: break;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
//  isSetMember
//
//! Determine whether a word is a member of a set.  Assumes the word has
//! already been determined to be acceptable.
//
//! @param lexicon the name of the lexicon
//! @param word the word to look up
//! @param ss the search set
//! @return true if a member of the set, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::isSetMember(const QString& lexicon, const QString& word,
                        SearchSet ss) const
{
    if (!lexiconData.contains(lexicon))
        return false;

    static QString typeTwoChars = "AAADEEEEGIIILNNOORRSSTTU";
    static int typeTwoCharsLen = typeTwoChars.length();
    static LetterBag letterBag("A:9 B:2 C:2 D:4 E:12 F:2 G:3 H:2 I:9 J:1 "
                               "K:1 L:4 M:2 N:6 O:8 P:2 Q:1 R:6 S:4 T:6 "
                               "U:4 V:2 W:2 X:1 Y:2 Z:1 _:2");

    // ### For backward compatibility, set membership is calculated using
    // probability calculated with two blanks - should make this customizable
    static double typeThreeSevenCombos
        = letterBag.getNumCombinations("HUNTERS", 2);
    static double typeThreeEightCombos
        = letterBag.getNumCombinations("NOTIFIED", 2);

    switch (ss) {
        case SetHookWords:
        return (isAcceptable(lexicon, word.left(word.length() - 1)) ||
                isAcceptable(lexicon, word.right(word.length() - 1)));

        case SetFrontHooks:
        return isAcceptable(lexicon, word.right(word.length() - 1));

        case SetBackHooks:
        return isAcceptable(lexicon, word.left(word.length() - 1));

        case SetHighFives: {
            if (word.length() != 5)
                return false;

            bool ok = false;
            for (int i = 0; i < word.length(); ++i) {
                int value = letterBag.getLetterValue(word[i]);
                if (value > 5)
                    return false;
                if (((value == 4) || (value == 5)) && ((i == 0) || (i == 4)))
                    ok = true;
            }
            return ok;
        }

        case SetTypeOneSevens: {
            if (word.length() != 7)
                return false;

            if (!lexiconData[lexicon]->stemAlphagrams.contains(word.length() - 1))
                return false;

            QString agram = Auxil::getAlphagram(word);
            const QSet<QString>& alphaSet =
                lexiconData[lexicon]->stemAlphagrams[word.length() - 1];

            for (int i = 0; i < int(agram.length()); ++i) {
                if (alphaSet.contains(agram.left(i) +
                                      agram.right(agram.length() - i - 1)))
                {
                    return true;
                }
            }
            return false;
        }

        case SetTypeOneEights: {
            if (word.length() != 8)
                return false;

            if (!lexiconData[lexicon]->stemAlphagrams.contains(word.length() - 2))
                return false;

            // Compare the letters of the word with the letters of each
            // alphagram, ensuring that no more than two letters in the word
            // are missing from the alphagram.
            QString agram = Auxil::getAlphagram(word);
            const QSet<QString>& alphaSet =
                lexiconData[lexicon]->stemAlphagrams[word.length() - 2];

            QSetIterator<QString> it (alphaSet);
            while (it.hasNext()) {
                QString setAlphagram = it.next();
                int missing = 0;
                int saIndex = 0;
                for (int i = 0; (i < int(agram.length())) &&
                                (saIndex < setAlphagram.length()); ++i)
                {
                    if (agram.at(i) == setAlphagram.at(saIndex))
                        ++saIndex;
                    else
                        ++missing;
                    if (missing > 2)
                        break;
                }
                if (missing <= 2)
                    return true;
            }
            return false;
        }

        case SetTypeTwoSevens:
        case SetTypeTwoEights:
        {
            if (((ss == SetTypeTwoSevens) && (word.length() != 7)) ||
                ((ss == SetTypeTwoEights) && (word.length() != 8)))
                return false;

            bool ok = false;
            QString alphagram = Auxil::getAlphagram(word);
            int wi = 0;
            QChar wc = alphagram[wi];
            for (int ti = 0; ti < typeTwoCharsLen; ++ti) {
                QChar tc = typeTwoChars[ti];
                if (tc == wc) {
                    ++wi;
                    if (wi == alphagram.length()) {
                        ok = true;
                        break;
                    }
                    wc = alphagram[wi];
                }
            }
            return (ok && !isSetMember(lexicon, word,
                (ss == SetTypeTwoSevens ? SetTypeOneSevens : SetTypeOneEights)));
        }

        case SetTypeThreeSevens: {
            if (word.length() != 7)
                return false;

            // ### For backward compatibility, set membership is calculated
            // using probability calculated with two blanks - should make this
            // customizable
            double combos = letterBag.getNumCombinations(word, 2);
            return ((combos >= typeThreeSevenCombos) &&
                    !isSetMember(lexicon, word, SetTypeOneSevens) &&
                    !isSetMember(lexicon, word, SetTypeTwoSevens));
        }

        case SetTypeThreeEights: {
            if (word.length() != 8)
                return false;

            // ### For backward compatibility, set membership is calculated
            // using probability calculated with two blanks - should make this
            // customizable
            double combos = letterBag.getNumCombinations(word, 2);
            return ((combos >= typeThreeEightCombos) &&
                    !isSetMember(lexicon, word, SetTypeOneEights) &&
                    !isSetMember(lexicon, word, SetTypeTwoEights));
        }

        case SetEightsFromSevenLetterStems: {
            if (word.length() != 8)
                return false;

            if (!lexiconData[lexicon]->stemAlphagrams.contains(word.length() - 1))
                return false;

            QString agram = Auxil::getAlphagram(word);
            const QSet<QString>& alphaSet =
                lexiconData[lexicon]->stemAlphagrams[word.length() - 1];

            for (int i = 0; i < int(agram.length()); ++i) {
                if (alphaSet.contains(agram.left(i) +
                                      agram.right(agram.length() - i - 1)))
                {
                    return true;
                }
            }
            return false;
        }

        default: return false;
    }
}

//---------------------------------------------------------------------------
//  getNumAnagrams
//
//! Determine the number of valid anagrams of a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return the number of valid anagrams
//---------------------------------------------------------------------------
int
WordEngine::getNumAnagrams(const QString& lexicon, const QString& word) const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    if (info.isValid()) {
        return info.numAnagrams;
    }
    else {
        QString alpha = Auxil::getAlphagram(word);
        return lexiconData[lexicon]->numAnagramsMap.value(alpha);
    }
}

//---------------------------------------------------------------------------
//  getPlayabilityValue
//
//! Get the playability value for a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return the playability value
//---------------------------------------------------------------------------
qint64
WordEngine::getPlayabilityValue(const QString& lexicon, const QString& word)
    const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ? info.playability : 0;
}

//---------------------------------------------------------------------------
//  getPlayabilityOrder
//
//! Get the playability order for a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return the playability order
//---------------------------------------------------------------------------
int
WordEngine::getPlayabilityOrder(const QString& lexicon, const QString& word)
    const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ? info.playabilityOrder.valueOrder : 0;
}

//---------------------------------------------------------------------------
//  getMinPlayabilityOrder
//
//! Get the minimum playability order for a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return the playability order
//---------------------------------------------------------------------------
int
WordEngine::getMinPlayabilityOrder(const QString& lexicon, const QString&
                                   word) const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ? info.playabilityOrder.minValueOrder : 0;
}

//---------------------------------------------------------------------------
//  getMaxPlayabilityOrder
//
//! Get the maximum playability order for a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return the playability order
//---------------------------------------------------------------------------
int
WordEngine::getMaxPlayabilityOrder(const QString& lexicon, const QString&
                                   word) const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ? info.playabilityOrder.maxValueOrder : 0;
}

//---------------------------------------------------------------------------
//  getProbabilityOrder
//
//! Get the probability order for a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @param numBlanks the number of blanks
//! @return the probability order
//---------------------------------------------------------------------------
int
WordEngine::getProbabilityOrder(const QString& lexicon, const QString& word,
                                int numBlanks) const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ?
        info.blankProbabilityOrder.value(numBlanks).valueOrder : 0;
}

//---------------------------------------------------------------------------
//  getMinProbabilityOrder
//
//! Get the minimum probability order for a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @param numBlanks the number of blanks
//! @return the probability order
//---------------------------------------------------------------------------
int
WordEngine::getMinProbabilityOrder(const QString& lexicon, const QString&
                                   word, int numBlanks) const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ?
        info.blankProbabilityOrder.value(numBlanks).minValueOrder : 0;
}

//---------------------------------------------------------------------------
//  getMaxProbabilityOrder
//
//! Get the maximum probability order for a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @param numBlanks the number of blanks
//! @return the probability order
//---------------------------------------------------------------------------
int
WordEngine::getMaxProbabilityOrder(const QString& lexicon, const QString&
                                   word, int numBlanks) const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ?
        info.blankProbabilityOrder.value(numBlanks).maxValueOrder : 0;
}

//---------------------------------------------------------------------------
//  getNumVowels
//
//! Get the number of vowels in a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return the number of vowels
//---------------------------------------------------------------------------
int
WordEngine::getNumVowels(const QString& lexicon, const QString& word) const
{
    // No test of lexiconData because we want to calculate if even not cached
    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ? info.numVowels : Auxil::getNumVowels(word);
}

//---------------------------------------------------------------------------
//  getNumUniqueLetters
//
//! Get the number of unique letters in a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return the number of unique letters
//---------------------------------------------------------------------------
int
WordEngine::getNumUniqueLetters(const QString& lexicon, const QString& word) const
{
    // No test of lexiconData because we want to calculate if even not cached
    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ? info.numUniqueLetters
                          : Auxil::getNumUniqueLetters(word);
}

//---------------------------------------------------------------------------
//  getPointValue
//
//! Get the point value for a word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return the point value
//---------------------------------------------------------------------------
int
WordEngine::getPointValue(const QString& lexicon, const QString& word) const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ? info.pointValue : 0;
}

//---------------------------------------------------------------------------
//  getIsFrontHook
//
//! Determine whether a word is a front hook.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return true if the word is a front hook, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::getIsFrontHook(const QString& lexicon, const QString& word) const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ? info.isFrontHook : false;
}

//---------------------------------------------------------------------------
//  getIsBackHook
//
//! Determine whether a word is a back hook.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return true if the word is a back hook, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::getIsBackHook(const QString& lexicon, const QString& word) const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ? info.isBackHook : false;
}

//---------------------------------------------------------------------------
//  getLexiconSymbols
//
//! Get lexicon symbols to be displayed along with this word.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @return a lexicon symbol string
//---------------------------------------------------------------------------
QString
WordEngine::getLexiconSymbols(const QString& lexicon, const QString& word) const
{
    if (!lexiconData.contains(lexicon))
        return 0;

    WordInfo info = getWordInfo(lexicon, word);
    return info.isValid() ? info.lexiconSymbols : QString();
}

//---------------------------------------------------------------------------
//  nonGraphSearch
//
//! Search for valid words matching conditions that can be matched without
//! searching the word graph.
//
//! @param lexicon the name of the lexicon
//! @param spec the search specification
//! @return a list of acceptable words matching the In Word List conditions
//---------------------------------------------------------------------------
QStringList
WordEngine::nonGraphSearch(const QString& lexicon, const SearchSpec& spec) const
{
    QStringList wordList;
    QSet<QString> finalWordSet;
    int conditionNum = 0;

    const int MAX_ANAGRAMS = 65535;
    int minAnagrams = 0;
    int maxAnagrams = MAX_ANAGRAMS;
    int minNumVowels = 0;
    int maxNumVowels = MAX_WORD_LEN;
    int minNumUniqueLetters = 0;
    int maxNumUniqueLetters = MAX_WORD_LEN;
    int minPointValue = 0;
    int maxPointValue = 10 * MAX_WORD_LEN;

    // Look for InWordList conditions first, to narrow the search as much as
    // possible
    QListIterator<SearchCondition> it (spec.conditions);
    while (it.hasNext()) {
        const SearchCondition& condition = it.next();

        // Note the minimum and maximum number of anagrams from any Number of
        // Anagrams conditions
        if (condition.type == SearchCondition::NumAnagrams) {
            if ((condition.minValue > maxAnagrams) ||
                (condition.maxValue < minAnagrams))
                return wordList;
            if (condition.minValue > minAnagrams)
                minAnagrams = condition.minValue;
            if (condition.maxValue < maxAnagrams)
                maxAnagrams = condition.maxValue;
        }

        // Note the minimum and maximum number of vowels from any Number of
        // Vowels conditions
        else if (condition.type == SearchCondition::NumVowels) {
            if ((condition.minValue > maxNumVowels) ||
                (condition.maxValue < minNumVowels))
                return wordList;
            if (condition.minValue > minNumVowels)
                minNumVowels = condition.minValue;
            if (condition.maxValue < maxNumVowels)
                maxNumVowels = condition.maxValue;
        }

        // Note the minimum and maximum number of unique letters from any
        // Number of Unique Letters conditions
        else if (condition.type == SearchCondition::NumUniqueLetters) {
            if ((condition.minValue > maxNumUniqueLetters) ||
                (condition.maxValue < minNumUniqueLetters))
                return wordList;
            if (condition.minValue > minNumUniqueLetters)
                minNumUniqueLetters = condition.minValue;
            if (condition.maxValue < maxNumUniqueLetters)
                maxNumUniqueLetters = condition.maxValue;
        }

        // Note the minimum and maximum point value from any Point Value
        // conditions
        else if (condition.type == SearchCondition::PointValue) {
            if ((condition.minValue > maxPointValue) ||
                (condition.maxValue < minPointValue))
                return wordList;
            if (condition.minValue > minPointValue)
                minPointValue = condition.minValue;
            if (condition.maxValue < maxPointValue)
                maxPointValue = condition.maxValue;
        }

        // Only InWordList conditions allowed beyond this point - look up
        // words for acceptability and combine the word lists
        if (condition.type != SearchCondition::InWordList)
            continue;

        QStringList words = condition.stringValue.split(QChar(' '));
        QSet<QString> wordSet;
        QStringListIterator wit (words);
        while (wit.hasNext()) {
            QString word = wit.next();
            if (isAcceptable(lexicon, word))
                wordSet.insert(word);
        }

        // Combine search result set with words already found
        if (!conditionNum) {
            finalWordSet = wordSet;
        }

        else if (spec.conjunction) {
            QSet<QString> conjunctionSet = finalWordSet & wordSet;
            if (conjunctionSet.isEmpty())
                return wordList;
            finalWordSet = conjunctionSet;
        }

        else {
            finalWordSet += wordSet;
        }

        ++conditionNum;
    }

    // Now limit the set only to those words containing the requisite number
    // of anagrams.  If some words are already in the finalWordSet, then only
    // test those words.  Otherwise, run through the map of number of anagrams
    // and pull out all words matching the conditions.
    if (!finalWordSet.isEmpty() &&
        ((minAnagrams > 0) || (maxAnagrams < MAX_ANAGRAMS)) ||
        ((minNumVowels > 0) || (maxNumVowels < MAX_WORD_LEN)) ||
        ((minNumUniqueLetters > 0) || (maxNumUniqueLetters < MAX_WORD_LEN)) ||
        ((minPointValue > 0) || (minPointValue < 10 * MAX_WORD_LEN)))
    {
        bool testAnagrams = ((minAnagrams > 0) ||
                             (maxAnagrams < MAX_ANAGRAMS));
        bool testNumVowels = ((minNumVowels > 0) ||
                              (maxNumVowels < MAX_WORD_LEN));
        bool testNumUniqueLetters = ((minNumUniqueLetters > 0) ||
                                     (maxNumUniqueLetters < MAX_WORD_LEN));
        bool testPointValue = ((minPointValue > 0) ||
                               (minPointValue < 10 * MAX_WORD_LEN));

        QSet<QString> wordSet;
        QSetIterator<QString> it (finalWordSet);
        while (it.hasNext()) {
            QString word = it.next();

            if (testAnagrams) {
                int numAnagrams = getNumAnagrams(lexicon, word);
                if ((numAnagrams < minAnagrams) || (numAnagrams > maxAnagrams))
                    continue;
            }

            if (testNumVowels) {
                int numVowels = getNumVowels(lexicon, word);
                if ((numVowels < minNumVowels) || (numVowels > maxNumVowels))
                    continue;
            }

            if (testNumUniqueLetters) {
                int numUniqueLetters = getNumUniqueLetters(lexicon, word);
                if ((numUniqueLetters < minNumUniqueLetters) ||
                    (numUniqueLetters > maxNumUniqueLetters))
                    continue;
            }

            if (testPointValue) {
                int pointValue = getPointValue(lexicon, word);
                if ((pointValue < minPointValue) ||
                    (pointValue > maxPointValue))
                    continue;
            }

            wordSet.insert(word);
        }
        finalWordSet = wordSet;
    }

    return finalWordSet.toList();
}

//---------------------------------------------------------------------------
//  addDefinition
//
//! Add a word with its definition.  Parse the definition and separate its
//! parts of speech.
//
//! @param lexicon the name of the lexicon
//! @param word the word
//! @param definition the definition
//---------------------------------------------------------------------------
void
WordEngine::addDefinition(const QString& lexicon, const QString& word,
                          const QString& definition)
{
    if (word.isEmpty() || definition.isEmpty() ||
        !lexiconData.contains(lexicon))
    {
        return;
    }

    QRegExp posRegex (QString("\\[(\\w+)"));
    QMultiMap<QString, QString> defMap;
    QStringList defs = definition.split(" / ");
    foreach (const QString& def, defs) {
        QString pos;
        if (posRegex.indexIn(def, 0) >= 0) {
            pos = posRegex.cap(1);
        }
        defMap.insert(pos, def);
    }
    lexiconData[lexicon]->definitions.insert(word, defMap);
}

//---------------------------------------------------------------------------
//  getConditionPhase
//
//! Determine the search phase during which a search condition should be
//! considered.
//
//! @param condition the search condition
//! @return the appropriate search phase
//---------------------------------------------------------------------------
WordEngine::ConditionPhase
WordEngine::getConditionPhase(const SearchCondition& condition) const
{
    switch (condition.type) {
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::ConsistOf:
        return WordGraphPhase;

        case SearchCondition::Length:
        case SearchCondition::InWordList:
        case SearchCondition::NumVowels:
        case SearchCondition::IncludeLetters:
        case SearchCondition::ProbabilityOrder:
        case SearchCondition::PlayabilityOrder:
        case SearchCondition::NumUniqueLetters:
        case SearchCondition::PointValue:
        case SearchCondition::NumAnagrams:
        case SearchCondition::PartOfSpeech:
        case SearchCondition::Definition:
        return DatabasePhase;

        case SearchCondition::Prefix:
        case SearchCondition::Suffix:
        case SearchCondition::InLexicon:
        case SearchCondition::LimitByProbabilityOrder:
        case SearchCondition::LimitByPlayabilityOrder:
        return PostConditionPhase;

        case SearchCondition::PatternMatch:
        if (condition.stringValue.startsWith("*") &&
            condition.stringValue.endsWith("*") &&
            !condition.stringValue.contains("["))
        {
            return DatabasePhase;
        }
        else
            return WordGraphPhase;

        case SearchCondition::BelongToGroup: {
            SearchSet searchSet =
                Auxil::stringToSearchSet(condition.stringValue);
            if ((searchSet == SetHookWords) || (searchSet == SetFrontHooks) ||
                (searchSet == SetBackHooks))
            {
                return DatabasePhase;
            }
            else
                return PostConditionPhase;
        }

        default:
        return UnknownPhase;
    }
}
