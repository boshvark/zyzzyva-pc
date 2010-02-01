//---------------------------------------------------------------------------
// WordVariationDialog.cpp
//
// A dialog for displaying variations of a word, such as hooks, anagrams, etc.
//
// Copyright 2005-2010 Michael W Thelen <mthelen@gmail.com>.
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

#include "WordVariationDialog.h"
#include "Defs.h"
#include "DefinitionLabel.h"
#include "MainSettings.h"
#include "WordEngine.h"
#include "WordTableView.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QVBoxLayout>

using namespace Defs;

//---------------------------------------------------------------------------
//  WordVariationDialog
//
//! Constructor.
//
//! @param we the word engine
//! @param lex the lexicon
//! @param word the word
//! @param variation the variation type
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
WordVariationDialog::WordVariationDialog(WordEngine* we, const QString& lex,
    const QString& word, WordVariationType variation, QWidget* parent,
    Qt::WFlags f)
    : QDialog(parent, f), wordEngine(we), lexicon(lex), topView(0), topModel(0),
    middleView(0), middleModel(0), bottomView(0), bottomModel(0)
{
    int numLists = getNumLists(variation);

    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);

    QHBoxLayout* labelHlay = new QHBoxLayout;
    Q_CHECK_PTR(labelHlay);
    labelHlay->setSpacing(SPACING);
    mainVlay->addLayout(labelHlay);

    labelHlay->addStretch(1);

    wordLabel = new DefinitionLabel;
    Q_CHECK_PTR(wordLabel);
    labelHlay->addWidget(wordLabel);

    labelHlay->addStretch(1);

    topLabel = new QLabel;
    Q_CHECK_PTR(topLabel);
    mainVlay->addWidget(topLabel);

    topView = new WordTableView(wordEngine);
    Q_CHECK_PTR(topView);
    mainVlay->addWidget(topView);

    topModel = new WordTableModel(wordEngine, this);
    Q_CHECK_PTR(topModel);
    topModel->setLexicon(lexicon);
    connect(topModel, SIGNAL(wordsChanged()),
            topView, SLOT(resizeItemsToContents()));
    topView->setModel(topModel);

    // Only add the middle list if necessary
    if (numLists > 1) {
        middleLabel = new QLabel;
        Q_CHECK_PTR(middleLabel);
        mainVlay->addWidget(middleLabel);

        middleView = new WordTableView(wordEngine);
        Q_CHECK_PTR(middleView);
        mainVlay->addWidget(middleView);

        middleModel = new WordTableModel(wordEngine, this);
        Q_CHECK_PTR(middleModel);
        middleModel->setLexicon(lexicon);
        connect(middleModel, SIGNAL(wordsChanged()),
                middleView, SLOT(resizeItemsToContents()));
        middleView->setModel(middleModel);
    }

    // Only add the bottom list if necessary
    if (numLists > 2) {
        bottomLabel = new QLabel;
        Q_CHECK_PTR(bottomLabel);
        mainVlay->addWidget(bottomLabel);

        bottomView = new WordTableView(wordEngine);
        Q_CHECK_PTR(bottomView);
        mainVlay->addWidget(bottomView);

        bottomModel = new WordTableModel(wordEngine, this);
        Q_CHECK_PTR(bottomModel);
        bottomModel->setLexicon(lexicon);
        connect(bottomModel, SIGNAL(wordsChanged()),
                bottomView, SLOT(resizeItemsToContents()));
        bottomView->setModel(bottomModel);
    }

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    Q_CHECK_PTR(buttonHlay);
    buttonHlay->setSpacing(SPACING);
    mainVlay->addLayout(buttonHlay);

    buttonHlay->addStretch(1);

    closeButton = new ZPushButton("&Close");
    Q_CHECK_PTR(closeButton);
    closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setAutoDefault(true);
    connect(closeButton, SIGNAL(clicked()), SLOT(accept()));
    buttonHlay->addWidget(closeButton);

    setWordVariation(word, variation);
    resize(minimumSizeHint().width() * 5, 500);

    closeButton->setFocus();
}

//---------------------------------------------------------------------------
//  ~WordVariationDialog
//
//! Destructor.
//---------------------------------------------------------------------------
WordVariationDialog::~WordVariationDialog()
{
}

//---------------------------------------------------------------------------
//  setWordVariation
//
//! Display the front and back hooks of a word.
//
//! @param word the word whose hooks are displayed
//! @param variation the variation of the word to display
//---------------------------------------------------------------------------
void
WordVariationDialog::setWordVariation(const QString& word, WordVariationType
                                      variation)
{
    bool forceAlphabetSort = false;
    bool origGroupByAnagrams = MainSettings::getWordListGroupByAnagrams();
    QString title, topTitle, middleTitle, bottomTitle;
    SearchSpec spec;
    SearchCondition condition;
    QList<SearchSpec> topSpecs;
    QList<SearchSpec> middleSpecs;
    QList<SearchSpec> bottomSpecs;
    switch (variation) {

        case VariationAnagrams:
        title = "Anagrams for: " + word;
        condition.type = SearchCondition::AnagramMatch;
        condition.stringValue = word;
        spec.conditions.append(condition);
        topSpecs.append(spec);
        topTitle = "Anagrams";
        break;

        case VariationSubanagrams:
        title = "Subanagrams for: " + word;
        condition.type = SearchCondition::SubanagramMatch;
        condition.stringValue = word;
        spec.conditions.append(condition);
        topSpecs.append(spec);
        topTitle = "Subanagrams";
        forceAlphabetSort = true;
        break;

        case VariationHooks:
        title = "Hooks for: " + word;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "?" + word;
        spec.conditions.append(condition);
        topSpecs.append(spec);
        condition.stringValue = word + "?";
        spec.conditions.clear();
        spec.conditions.append(condition);
        middleSpecs.append(spec);
        topTitle = "Front Hooks";
        middleTitle = "Back Hooks";
        forceAlphabetSort = true;
        break;

        case VariationAnagramHooks:
        title = "Anagram Hooks for: " + word;
        condition.type = SearchCondition::AnagramMatch;
        condition.stringValue = "?" + word;
        spec.conditions.append(condition);
        topSpecs.append(spec);
        topTitle = "Anagram Hooks";
        break;

        case VariationBlankAnagrams:
        title = "Blank Anagrams for: " + word;
        condition.type = SearchCondition::AnagramMatch;
        for (int i = 0; i < int(word.length()); ++i) {
            condition.stringValue = word.left(i) + "?" +
                word.right(word.length() - i - 1);
            spec.conditions.clear();
            spec.conditions.append(condition);
            topSpecs.append(spec);
        }
        topTitle = "Blank Anagrams";
        break;

        case VariationBlankMatches:
        title = "Blank Matches for: " + word;
        condition.type = SearchCondition::PatternMatch;
        for (int i = 0; i < int(word.length()); ++i) {
            condition.stringValue = word.left(i) + "?" +
                word.right(word.length() - i - 1);
            spec.conditions.clear();
            spec.conditions.append(condition);
            topSpecs.append(spec);
        }
        topTitle = "Blank Matches";
        forceAlphabetSort = true;
        break;

        case VariationExtensions:
        title = "Extensions for: " + word;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "*?" + word;
        spec.conditions.append(condition);
        topSpecs.append(spec);
        condition.stringValue = word + "?*";
        spec.conditions.clear();
        spec.conditions.append(condition);
        middleSpecs.append(spec);
        condition.stringValue = "*?" + word + "?*";
        spec.conditions.clear();
        spec.conditions.append(condition);
        bottomSpecs.append(spec);
        topTitle = "Front Extensions";
        middleTitle = "Back Extensions";
        bottomTitle = "Double Extensions";
        forceAlphabetSort = true;
        break;

        case VariationTranspositions:
        title = "Transpositions for: " + word;
        condition.type = SearchCondition::PatternMatch;
        for (int i = 0; i < int(word.length()); ++i) {
            condition.stringValue = word.left(i) + word.mid(i + 1, 1) +
                word.mid(i, 1) + word.right(word.length() - i - 2);
            spec.conditions.clear();
            spec.conditions.append(condition);
            topSpecs.append(spec);
        }
        topTitle = "Transpositions";
        break;

        default: break;
    }

    if (!wordEngine->isAcceptable(lexicon, word))
        title += "*";
    setWindowTitle(title);
    wordLabel->setText(title);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Populate the top list
    QList<WordTableModel::WordItem> wordItems = getWordItems(topSpecs);

    // FIXME: Probably not the right way to get alphabetical sorting instead
    // of alphagram sorting
    if (forceAlphabetSort)
        MainSettings::setWordListGroupByAnagrams(false);
    topModel->addWords(wordItems);
    if (forceAlphabetSort)
        MainSettings::setWordListGroupByAnagrams(origGroupByAnagrams);

    int topWords = topModel->rowCount();
    topTitle += " : " + QString::number(topWords) + " word";
    if (topWords != 1)
        topTitle += "s";
    topLabel->setText(topTitle);

    // Populate the middle list
    if (!middleSpecs.empty()) {
        wordItems = getWordItems(middleSpecs);

        // FIXME: Probably not the right way to get alphabetical sorting
        // instead of alphagram sorting
        if (forceAlphabetSort)
            MainSettings::setWordListGroupByAnagrams(false);
        middleModel->addWords(wordItems);
        if (forceAlphabetSort)
            MainSettings::setWordListGroupByAnagrams(origGroupByAnagrams);

        int middleWords = middleModel->rowCount();
        middleTitle += " : " + QString::number(middleWords) + " word";
        if (middleWords != 1)
            middleTitle += "s";
        middleLabel->setText(middleTitle);
    }

    // Populate the bottom list
    if (!bottomSpecs.empty()) {
        wordItems = getWordItems(bottomSpecs);

        // FIXME: Probably not the right way to get alphabetical sorting
        // instead of alphagram sorting
        if (forceAlphabetSort)
            MainSettings::setWordListGroupByAnagrams(false);
        bottomModel->addWords(wordItems);
        if (forceAlphabetSort)
            MainSettings::setWordListGroupByAnagrams(origGroupByAnagrams);

        int bottomWords = bottomModel->rowCount();
        bottomTitle += " : " + QString::number(bottomWords) + " word";
        if (bottomWords != 1)
            bottomTitle += "s";
        bottomLabel->setText(bottomTitle);
    }

    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  getWordItems
//
//! Construct a list of word items to be inserted into a word list, based on
//! the results of a list of searches.
//
//! @param searchSpecs the list of search specifications
//! @return a list of word items
//---------------------------------------------------------------------------
QList<WordTableModel::WordItem>
WordVariationDialog::getWordItems(const QList<SearchSpec>& searchSpecs) const
{
    QList<WordTableModel::WordItem> wordItems;
    QMap<QString, QString> wordMap;
    QListIterator<SearchSpec> lit (searchSpecs);
    while (lit.hasNext()) {
        QStringList wordList = wordEngine->search(lexicon, lit.next(), false);
        QStringListIterator wit (wordList);
        while (wit.hasNext()) {
            QString str = wit.next();
            wordMap.insert(str.toUpper(), str);
        }
    }

    QMapIterator<QString, QString> mit (wordMap);
    while (mit.hasNext()) {
        mit.next();
        QString value = mit.value();
        QList<QChar> wildcardChars;
        for (int i = 0; i < value.length(); ++i) {
            QChar c = value[i];
            if (c.isLower())
                wildcardChars.append(c);
        }
        QString wildcard;
        if (!wildcardChars.isEmpty()) {
            qSort(wildcardChars.begin(), wildcardChars.end(),
                  Auxil::localeAwareLessThanQChar);
            foreach (const QChar& c, wildcardChars)
                wildcard.append(c.toUpper());
        }
        wordItems.append(WordTableModel::WordItem(
            mit.key(), WordTableModel::WordNormal, wildcard));
    }

    return wordItems;
}

//---------------------------------------------------------------------------
//  getNumLists
//
//! Determine how many lists should be displayed for a word variation type.
//
//! @param variation the variation type
//! @return the number of lists
//---------------------------------------------------------------------------
int
WordVariationDialog::getNumLists(WordVariationType variation)
{
    switch (variation) {
        case VariationHooks: return 2;
        case VariationExtensions: return 3;
        default: return 1;
    }
}
