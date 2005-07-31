//---------------------------------------------------------------------------
// WordVariationDialog.cpp
//
// A dialog for displaying variations of a word, such as hooks, anagrams, etc.
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
#include "WordEngine.h"
#include "WordListView.h"
#include "WordListViewItem.h"
#include <qapplication.h>
#include <qlayout.h>
#include <set>

using namespace Defs;
using std::set;

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
                                          QWidget* parent, const char* name,
                                          bool modal, WFlags f)
    : QDialog (parent, name, modal, f), wordEngine (we)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* labelHlay = new QHBoxLayout (SPACING, "labelHlay");
    Q_CHECK_PTR (labelHlay);
    mainVlay->addLayout (labelHlay);

    labelHlay->addStretch (1);

    wordLabel = new DefinitionLabel (this, "wordLabel");
    Q_CHECK_PTR (wordLabel);
    labelHlay->addWidget (wordLabel);

    labelHlay->addStretch (1);

    QHBoxLayout* mainHlay = new QHBoxLayout (SPACING, "mainHlay");
    Q_CHECK_PTR (mainHlay);
    mainVlay->addLayout (mainHlay);

    QVBoxLayout* leftVlay = new QVBoxLayout (SPACING, "leftVlay");
    Q_CHECK_PTR (leftVlay);
    mainHlay->addLayout (leftVlay, 1);

    leftLabel = new QLabel (this, "leftLabel");
    Q_CHECK_PTR (leftLabel);
    leftVlay->addWidget (leftLabel);

    leftList = new WordListView (wordEngine, this, "leftList");
    Q_CHECK_PTR (leftList);
    leftList->setShowSortIndicator(true);
    leftVlay->addWidget (leftList);

    // Only add the right-hand list if necessary
    if (needsRightList (variation)) {
        QVBoxLayout* rightVlay = new QVBoxLayout (SPACING, "rightVlay");
        Q_CHECK_PTR (rightVlay);
        mainHlay->addLayout (rightVlay, 1);

        rightLabel = new QLabel (this, "rightLabel");
        Q_CHECK_PTR (rightLabel);
        rightVlay->addWidget (rightLabel);

        rightList = new WordListView (wordEngine, this, "rightList");
        Q_CHECK_PTR (rightList);
        rightList->setShowSortIndicator(true);
        rightVlay->addWidget (rightList);
    }

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    closeButton = new QPushButton ("&Close", this, "closeButton");
    Q_CHECK_PTR (closeButton);
    closeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setAutoDefault (false);
    closeButton->setFocus();
    connect (closeButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (closeButton);

    setWordVariation (word, variation);
    resize (minimumSizeHint().width() * 2, 500);
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
    QString title, leftTitle, rightTitle;
    SearchSpec spec;
    SearchCondition condition;
    QValueList<SearchSpec> leftSpecs;
    QValueList<SearchSpec> rightSpecs;
    switch (variation) {

        case VariationAnagrams:
        title = "Anagrams for: " + word;
        condition.type = SearchCondition::AnagramMatch;
        condition.stringValue = word;
        spec.conditions << condition;
        leftSpecs << spec;
        leftTitle = "Anagrams";
        break;

        case VariationSubanagrams:
        title = "Subanagrams for: " + word;
        condition.type = SearchCondition::SubanagramMatch;
        condition.stringValue = word;
        spec.conditions << condition;
        leftSpecs << spec;
        leftTitle = "Subanagrams";
        break;

        case VariationHooks:
        title = "Hooks for: " + word;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "?" + word;
        spec.conditions << condition;
        leftSpecs << spec;
        condition.stringValue = word + "?";
        spec.conditions.clear();
        spec.conditions << condition;
        rightSpecs << spec;
        leftTitle = "Front Hooks";
        rightTitle = "Back Hooks";
        break;

        case VariationAnagramHooks:
        title = "Anagram Hooks for: " + word;
        condition.type = SearchCondition::AnagramMatch;
        condition.stringValue = "?" + word;
        spec.conditions << condition;
        leftSpecs << spec;
        leftTitle = "Anagram Hooks";
        break;

        case VariationBlankAnagrams:
        title = "Blank Anagrams for: " + word;
        condition.type = SearchCondition::AnagramMatch;
        for (int i = 0; i < int (word.length()); ++i) {
            condition.stringValue = word.left (i) + "?" +
                word.right (word.length() - i - 1);
            spec.conditions.clear();
            spec.conditions << condition;
            leftSpecs << spec;
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
            spec.conditions << condition;
            leftSpecs << spec;
        }
        leftTitle = "Blank Matches";
        break;

        case VariationExtensions:
        title = "Extensions for: " + word;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "*?" + word;
        spec.conditions << condition;
        leftSpecs << spec;
        condition.stringValue = word + "?*";
        spec.conditions.clear();
        spec.conditions << condition;
        rightSpecs << spec;
        leftTitle = "Front Extensions";
        rightTitle = "Back Extensions";
        break;

        case VariationTranspositions:
        title = "Transpositions for: " + word;
        condition.type = SearchCondition::PatternMatch;
        for (int i = 0; i < int (word.length()); ++i) {
            condition.stringValue = word.left (i) + word.mid (i + 1, 1) +
                word.mid (i, 1) + word.right (word.length() - i - 2);
            spec.conditions.clear();
            spec.conditions << condition;
            leftSpecs << spec;
        }
        leftTitle = "Transpositions";
        break;

        default: break;
    }


    if (!wordEngine->isAcceptable (word))
        title += "*";
    setCaption (title);
    wordLabel->setText (title);

    QApplication::setOverrideCursor (Qt::waitCursor);

    set<QString> wordSet;
    QStringList wordList;
    QStringList::iterator wit;
    QValueList<SearchSpec>::iterator sit;
    for (sit = leftSpecs.begin(); sit != leftSpecs.end(); ++sit) {
        wordList = wordEngine->search (*sit, true);
        for (wit = wordList.begin(); wit != wordList.end(); ++wit) {
            if (wordSet.find (*wit) == wordSet.end())
                leftList->addWord (*wit);
            wordSet.insert (*wit);
        }
    }
    int leftWords = leftList->childCount();
    leftTitle += " : " + QString::number (leftWords) + " word";
    if (leftWords != 1)
        leftTitle += "s";
    leftLabel->setText (leftTitle);

    if (!rightSpecs.empty()) {
        wordSet.clear();
        for (sit = rightSpecs.begin(); sit != rightSpecs.end(); ++sit) {
            wordList = wordEngine->search (*sit, true);
            for (wit = wordList.begin(); wit != wordList.end(); ++wit) {
                if (wordSet.find (*wit) == wordSet.end())
                    rightList->addWord (*wit);
                wordSet.insert (*wit);
            }
        }
        int rightWords = rightList->childCount();
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
