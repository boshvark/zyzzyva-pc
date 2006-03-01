//---------------------------------------------------------------------------
// WordVariationDialog.cpp
//
// A dialog for displaying variations of a word, such as hooks, anagrams, etc.
//
// Copyright 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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
#include "WordTableModel.h"
#include "WordTableView.h"
#include "ZPushButton.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QList>
#include <QVBoxLayout>

using namespace Defs;

//---------------------------------------------------------------------------
//  WordVariationDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param modal whether the dialog is modal
//! @param f widget flags
//---------------------------------------------------------------------------
WordVariationDialog::WordVariationDialog (WordEngine* we, const QString& word,
                                          WordVariationType variation,
                                          QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f), wordEngine (we)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this);
    mainVlay->setMargin (MARGIN);
    mainVlay->setSpacing (SPACING);
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* labelHlay = new QHBoxLayout;
    labelHlay->setSpacing (SPACING);
    Q_CHECK_PTR (labelHlay);
    mainVlay->addLayout (labelHlay);

    labelHlay->addStretch (1);

    wordLabel = new DefinitionLabel;
    Q_CHECK_PTR (wordLabel);
    labelHlay->addWidget (wordLabel);

    labelHlay->addStretch (1);

    leftLabel = new QLabel;
    Q_CHECK_PTR (leftLabel);
    mainVlay->addWidget (leftLabel);

    leftView = new WordTableView (wordEngine);
    Q_CHECK_PTR (leftView);
    mainVlay->addWidget (leftView);

    leftModel = new WordTableModel (wordEngine, this);
    Q_CHECK_PTR (leftModel);
    leftView->verticalHeader()->hide();
    connect (leftModel, SIGNAL (wordsChanged()),
             leftView, SLOT (resizeAllColumnsToContents()));
    leftView->setModel (leftModel);

    // Only add the right-hand list if necessary
    if (needsRightList (variation)) {
        rightLabel = new QLabel;
        Q_CHECK_PTR (rightLabel);
        mainVlay->addWidget (rightLabel);

        rightView = new WordTableView (wordEngine);
        Q_CHECK_PTR (rightView);
        mainVlay->addWidget (rightView);

        rightModel = new WordTableModel (wordEngine, this);
        Q_CHECK_PTR (rightModel);
        rightView->verticalHeader()->hide();
        connect (rightModel, SIGNAL (wordsChanged()),
                 rightView, SLOT (resizeAllColumnsToContents()));
        rightView->setModel (rightModel);
    }

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    closeButton = new ZPushButton ("&Close");
    Q_CHECK_PTR (closeButton);
    closeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setAutoDefault (true);
    connect (closeButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (closeButton);

    setWordVariation (word, variation);
    resize (minimumSizeHint().width() * 3, 500);

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
WordVariationDialog::setWordVariation (const QString& word, WordVariationType
                                       variation)
{
    bool forceAlphabetSort = false;
    bool origGroupByAnagrams = MainSettings::getWordListGroupByAnagrams();
    QString title, leftTitle, rightTitle;
    SearchSpec spec;
    SearchCondition condition;
    QList<SearchSpec> leftSpecs;
    QList<SearchSpec> rightSpecs;
    switch (variation) {

        case VariationAnagrams:
        title = "Anagrams for: " + word;
        condition.type = SearchCondition::AnagramMatch;
        condition.stringValue = word;
        spec.conditions.append (condition);
        leftSpecs.append (spec);
        leftTitle = "Anagrams";
        break;

        case VariationSubanagrams:
        title = "Subanagrams for: " + word;
        condition.type = SearchCondition::SubanagramMatch;
        condition.stringValue = word;
        spec.conditions.append (condition);
        leftSpecs.append (spec);
        leftTitle = "Subanagrams";
        forceAlphabetSort = true;
        break;

        case VariationHooks:
        title = "Hooks for: " + word;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "?" + word;
        spec.conditions.append (condition);
        leftSpecs.append (spec);
        condition.stringValue = word + "?";
        spec.conditions.clear();
        spec.conditions.append (condition);
        rightSpecs.append (spec);
        leftTitle = "Front Hooks";
        rightTitle = "Back Hooks";
        forceAlphabetSort = true;
        break;

        case VariationAnagramHooks:
        title = "Anagram Hooks for: " + word;
        condition.type = SearchCondition::AnagramMatch;
        condition.stringValue = "?" + word;
        spec.conditions.append (condition);
        leftSpecs.append (spec);
        leftTitle = "Anagram Hooks";
        break;

        case VariationBlankAnagrams:
        title = "Blank Anagrams for: " + word;
        condition.type = SearchCondition::AnagramMatch;
        for (int i = 0; i < int (word.length()); ++i) {
            condition.stringValue = word.left (i) + "?" +
                word.right (word.length() - i - 1);
            spec.conditions.clear();
            spec.conditions.append (condition);
            leftSpecs.append (spec);
        }
        leftTitle = "Blank Anagrams";
        break;

        case VariationBlankMatches:
        title = "Blank Matches for: " + word;
        condition.type = SearchCondition::PatternMatch;
        for (int i = 0; i < int (word.length()); ++i) {
            condition.stringValue = word.left (i) + "?" +
                word.right (word.length() - i - 1);
            spec.conditions.clear();
            spec.conditions.append (condition);
            leftSpecs.append (spec);
        }
        leftTitle = "Blank Matches";
        forceAlphabetSort = true;
        break;

        case VariationExtensions:
        title = "Extensions for: " + word;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "*" + word;
        spec.conditions.append (condition);
        leftSpecs.append (spec);
        condition.stringValue = word + "*";
        spec.conditions.clear();
        spec.conditions.append (condition);
        rightSpecs.append (spec);
        leftTitle = "Front Extensions";
        rightTitle = "Back Extensions";
        forceAlphabetSort = true;
        break;

        case VariationTranspositions:
        title = "Transpositions for: " + word;
        condition.type = SearchCondition::PatternMatch;
        for (int i = 0; i < int (word.length()); ++i) {
            condition.stringValue = word.left (i) + word.mid (i + 1, 1) +
                word.mid (i, 1) + word.right (word.length() - i - 2);
            spec.conditions.clear();
            spec.conditions.append (condition);
            leftSpecs.append (spec);
        }
        leftTitle = "Transpositions";
        break;

        default: break;
    }


    if (!wordEngine->isAcceptable (word))
        title += "*";
    setWindowTitle (title);
    wordLabel->setText (title);

    QApplication::setOverrideCursor (QCursor (Qt::WaitCursor));


    QMap<QString, QString> wordMap;
    QStringList wordList;
    QListIterator<SearchSpec> lit (leftSpecs);
    while (lit.hasNext()) {
        wordList = wordEngine->search (lit.next(), false);
        QStringListIterator wit (wordList);
        while (wit.hasNext()) {
            QString str = wit.next();
            if ((str == word) && (variation == VariationExtensions))
                continue;
            wordMap.insert (str.toUpper(), str);
        }
    }

    QList<WordTableModel::WordItem> wordItems;
    QMapIterator<QString, QString> mit (wordMap);
    while (mit.hasNext()) {
        mit.next();
        QString value = mit.value();
        QList<QChar> wildcardChars;
        for (int i = 0; i < value.length(); ++i) {
            QChar c = value[i];
            if (c.isLower())
                wildcardChars.append (c);
        }
        QString wildcard;
        if (!wildcardChars.isEmpty()) {
            qSort (wildcardChars);
            QChar c;
            foreach (c, wildcardChars)
                wildcard.append (c.toUpper());
        }
        wordItems.append (WordTableModel::WordItem
                          (mit.key(), WordTableModel::WordNormal, wildcard));
    }

    // FIXME: Probably not the right way to get alphabetical sorting instead
    // of alphagram sorting
    if (forceAlphabetSort)
        MainSettings::setWordListGroupByAnagrams (false);
    leftModel->addWords (wordItems);
    if (forceAlphabetSort)
        MainSettings::setWordListGroupByAnagrams (origGroupByAnagrams);

    int leftWords = wordMap.size();
    leftTitle += " : " + QString::number (leftWords) + " word";
    if (leftWords != 1)
        leftTitle += "s";
    leftLabel->setText (leftTitle);

    if (!rightSpecs.empty()) {
        wordMap.clear();
        QListIterator<SearchSpec> rit (rightSpecs);
        while (rit.hasNext()) {
            wordList = wordEngine->search (rit.next(), false);
            QStringListIterator wit (wordList);
            while (wit.hasNext()) {
                QString str = wit.next();
                if ((str == word) && (variation == VariationExtensions))
                    continue;
                wordMap.insert (str.toUpper(), str);
            }
        }

        wordItems.clear();
        QList<WordTableModel::WordItem> wordItems;
        QMapIterator<QString, QString> mit (wordMap);
        while (mit.hasNext()) {
            mit.next();
            QString value = mit.value();
            QList<QChar> wildcardChars;
            for (int i = 0; i < value.length(); ++i) {
                QChar c = value[i];
                if (c.isLower())
                    wildcardChars.append (c);
            }
            QString wildcard;
            if (!wildcardChars.isEmpty()) {
                qSort (wildcardChars);
                QChar c;
                foreach (c, wildcardChars)
                    wildcard.append (c.toUpper());
            }
            wordItems.append (WordTableModel::WordItem
                              (mit.key(), WordTableModel::WordNormal,
                               wildcard));
        }

        // FIXME: Probably not the right way to get alphabetical sorting
        // instead of alphagram sorting
        if (forceAlphabetSort)
            MainSettings::setWordListGroupByAnagrams (false);
        rightModel->addWords (wordItems);
        if (forceAlphabetSort)
            MainSettings::setWordListGroupByAnagrams (origGroupByAnagrams);

        int rightWords = wordMap.size();
        rightTitle += " : " + QString::number (rightWords) + " word";
        if (rightWords != 1)
            rightTitle += "s";
        rightLabel->setText (rightTitle);
    }

    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  needsRightList
//
//! Determine whether the right-hand list is needed for a word variation type.
//
//! @param variation the variation type
//! @return true if the variation type requires a right-hand list, false
//! otherwise
//---------------------------------------------------------------------------
bool
WordVariationDialog::needsRightList (WordVariationType variation)
{
    switch (variation) {
        case VariationHooks:
        case VariationExtensions: return true;
        default: return false;
    }
}
