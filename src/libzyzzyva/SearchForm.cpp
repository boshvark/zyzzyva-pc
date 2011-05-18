//---------------------------------------------------------------------------
// SearchForm.cpp
//
// A form for searching for words, patterns, anagrams, etc.
//
// Copyright 2004-2011 Michael W Thelen <mthelen@gmail.com>.
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

#include "SearchForm.h"
#include "LexiconSelectWidget.h"
#include "MainSettings.h"
#include "SearchSpecForm.h"
#include "WordEngine.h"
#include "WordTableModel.h"
#include "WordTableView.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTimer>

using namespace Defs;

const QString TITLE_PREFIX = "Search";

//---------------------------------------------------------------------------
//  SearchForm
//
//! Constructor.
//
//! @param e the word engine
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
SearchForm::SearchForm(WordEngine* e, QWidget* parent, Qt::WFlags f)
    : ActionForm(SearchFormType, parent, f), wordEngine(e)
{
    QHBoxLayout* mainHlay = new QHBoxLayout(this);
    mainHlay->setMargin(MARGIN);
    mainHlay->setSpacing(SPACING);

    QVBoxLayout* specVlay = new QVBoxLayout;
    specVlay->setSpacing(SPACING);
    mainHlay->addLayout(specVlay);

    QHBoxLayout* lexiconHlay = new QHBoxLayout;
    lexiconHlay->setSpacing(SPACING);
    specVlay->addLayout(lexiconHlay);

    lexiconHlay->addStretch(1);

    lexiconWidget = new LexiconSelectWidget;
    connect(lexiconWidget->getComboBox(), SIGNAL(activated(const QString&)),
        SLOT(lexiconActivated(const QString&)));
    lexiconHlay->addWidget(lexiconWidget);

    lexiconHlay->addStretch(1);

    specForm = new SearchSpecForm;
    connect(specForm, SIGNAL(returnPressed()), SLOT(search()));
    connect(specForm, SIGNAL(contentsChanged()), SLOT(specChanged()));
    specVlay->addWidget(specForm);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing(SPACING);
    specVlay->addLayout(buttonHlay);

    searchButton = new ZPushButton("&Search");
    searchButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(searchButton, SIGNAL(clicked()), SLOT(search()));
    buttonHlay->addWidget(searchButton);

    resultView = new WordTableView(wordEngine);
    specVlay->addWidget(resultView, 1);

    resultModel = new WordTableModel(wordEngine, this);
    connect(resultModel, SIGNAL(wordsChanged()),
            resultView, SLOT(resizeItemsToContents()));
    resultView->setModel(resultModel);

    lexiconActivated(lexiconWidget->getCurrentLexicon());

    specChanged();
    QTimer::singleShot(0, this, SLOT(selectInputArea()));
}

//---------------------------------------------------------------------------
//  getIcon
//
//! Returns the current icon.
//
//! @return the current icon
//---------------------------------------------------------------------------
QIcon
SearchForm::getIcon() const
{
    return QIcon(":/search-icon");
}

//---------------------------------------------------------------------------
//  getTitle
//
//! Returns the current title string.
//
//! @return the current title string
//---------------------------------------------------------------------------
QString
SearchForm::getTitle() const
{
    return TITLE_PREFIX;
}

//---------------------------------------------------------------------------
//  getStatusString
//
//! Returns the current status string.
//
//! @return the current status string
//---------------------------------------------------------------------------
QString
SearchForm::getStatusString() const
{
    return statusString;
}

//---------------------------------------------------------------------------
//  getDetailsString
//
//! Returns the current details string.
//
//! @return the current details string
//---------------------------------------------------------------------------
QString
SearchForm::getDetailsString() const
{
    return detailsString;
}

//---------------------------------------------------------------------------
//  isSaveEnabled
//
//! Determine whether the save action should be enabled for this form.
//
//! @return true if save should be enabled, false otherwise
//---------------------------------------------------------------------------
bool
SearchForm::isSaveEnabled() const
{
    return (resultModel->rowCount() > 0);
}

//---------------------------------------------------------------------------
//  saveRequested
//
//! Called when a save action is requested.
//
//! @param saveAs true if the user should be prompted for a filename
//---------------------------------------------------------------------------
void
SearchForm::saveRequested(bool)
{
    resultView->exportRequested();
}

//---------------------------------------------------------------------------
//  selectInputArea
//
//! Give focus to the input area and select its contents.
//---------------------------------------------------------------------------
void
SearchForm::selectInputArea()
{
    specForm->selectInputArea();
}

//---------------------------------------------------------------------------
//  search
//
//! Search for the word or pattern in the edit area, and display the results
//! in the list box.
//---------------------------------------------------------------------------
void
SearchForm::search()
{
    SearchSpec spec = specForm->getSearchSpec();
    if (spec.conditions.empty())
        return;

    QString lexicon = lexiconWidget->getCurrentLexicon();

    searchButton->setEnabled(false);
    resultModel->removeRows(0, resultModel->rowCount());
    resultModel->setLexicon(lexicon);

    statusString = "Searching...";
    emit statusChanged(statusString);
    qApp->processEvents();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QStringList wordList =
        wordEngine->search(lexicon, specForm->getSearchSpec(), false);

    if (!wordList.empty()) {

        // Check for Anagram or Subanagram conditions, and only group by
        // alphagrams if one of them is present
        bool hasAnagramCondition = false;
        bool hasSubanagramCondition = false;
        bool hasProbabilityCondition = false;
        bool hasPlayabilityCondition = false;
        int probNumBlanks = MainSettings::getProbabilityNumBlanks();
        QListIterator<SearchCondition> it (spec.conditions);
        while (it.hasNext()) {
            const SearchCondition& condition = it.next();
            SearchCondition::SearchType type = condition.type;
            if (!condition.negated &&
                ((type == SearchCondition::AnagramMatch) ||
                (type == SearchCondition::SubanagramMatch) ||
                (type == SearchCondition::NumAnagrams)))
            {
                hasAnagramCondition = true;
                if (type == SearchCondition::SubanagramMatch)
                    hasSubanagramCondition = true;
            }

            else if ((type == SearchCondition::ProbabilityOrder) ||
                (type == SearchCondition::LimitByProbabilityOrder))
            {
                // Set number of blanks based on the first probability search
                // condition
                if (!hasProbabilityCondition)
                    probNumBlanks = condition.intValue;
                hasProbabilityCondition = true;
            }

            else if ((type == SearchCondition::PlayabilityOrder) ||
                (type == SearchCondition::LimitByPlayabilityOrder))
            {
                hasPlayabilityCondition = true;
            }
        }

        // Create a list of WordItem objects from the words
        QList<WordTableModel::WordItem> wordItems;
        foreach (const QString& word, wordList) {
            QString wildcard;
            if (hasAnagramCondition) {
                // Get wildcard characters
                QList<QChar> wildcardChars;
                for (int i = 0; i < word.length(); ++i) {
                    QChar c = word[i];
                    if (c.isLower())
                        wildcardChars.append(c);
                }
                if (!wildcardChars.isEmpty()) {
                    qSort(wildcardChars.begin(), wildcardChars.end(),
                          Auxil::localeAwareLessThanQChar);
                    foreach (const QChar& c, wildcardChars)
                        wildcard.append(c.toUpper());
                }
            }

            QString displayWord = word;
            QString wordUpper = word.toUpper();

            // Convert to all caps if necessary
            if (!MainSettings::getWordListLowerCaseWildcards())
                displayWord = wordUpper;

            WordTableModel::WordItem wordItem
                (displayWord, WordTableModel::WordNormal, wildcard);

            // Set probability/playability order for correct sorting
            if (hasProbabilityCondition) {
                int probOrder = wordEngine->getProbabilityOrder(
                    lexicon, wordUpper, probNumBlanks);
                wordItem.setProbabilityOrder(probOrder);
            }
            else if (hasPlayabilityCondition) {
                int playValue = wordEngine->getPlayabilityValue(
                    lexicon, wordUpper);
                int playOrder = wordEngine->getPlayabilityOrder(
                    lexicon, wordUpper);
                wordItem.setPlayabilityValue(playValue);
                wordItem.setPlayabilityOrder(playOrder);
            }

            wordItems.append(wordItem);
        }

        // FIXME: Probably not the right way to get alphabetical sorting instead
        // of alphagram sorting
        bool origGroupByAnagrams = MainSettings::getWordListGroupByAnagrams();
        if (!hasAnagramCondition)
            MainSettings::setWordListGroupByAnagrams(false);
        if (hasSubanagramCondition)
            MainSettings::setWordListSortByReverseLength(true);
        if (hasProbabilityCondition)
            MainSettings::setWordListSortByProbabilityOrder(true);
        else if (hasPlayabilityCondition)
            MainSettings::setWordListSortByPlayabilityOrder(true);
        resultModel->setProbabilityNumBlanks(probNumBlanks);
        resultModel->addWords(wordItems);
        MainSettings::setWordListSortByPlayabilityOrder(false);
        MainSettings::setWordListSortByProbabilityOrder(false);
        if (hasSubanagramCondition)
            MainSettings::setWordListSortByReverseLength(false);
        if (!hasAnagramCondition)
            MainSettings::setWordListGroupByAnagrams(origGroupByAnagrams);
    }

    updateResultTotal(wordList.size());
    emit saveEnabledChanged(!wordList.empty());

    QWidget* focusWidget = QApplication::focusWidget();
    QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(focusWidget);
    if (lineEdit) {
        lineEdit->setSelection(0, lineEdit->text().length());
    }
    else {
        selectInputArea();
    }

    searchButton->setEnabled(true);
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  specChanged
//
//! Called when the contents of the search spec form change.  Enable or
//! disable the Search button appropriately.
//---------------------------------------------------------------------------
void
SearchForm::specChanged()
{
    searchButton->setEnabled(specForm->isValid());
}

//---------------------------------------------------------------------------
//  updateResultTotal
//
//! Display the number of words currently in the search results.
//! @param num the number of words
//---------------------------------------------------------------------------
void
SearchForm::updateResultTotal(int num)
{
    QString wordStr = QString::number(num) + " word";
    if (num != 1)
        wordStr += "s";
    statusString = "Search found " + wordStr;
    emit statusChanged(statusString);
}

//---------------------------------------------------------------------------
//  lexiconActivated
//
//! Called when the lexicon combo box is activated
//! @param lexicon the activated lexicon
//---------------------------------------------------------------------------
void
SearchForm::lexiconActivated(const QString& lexicon)
{
    detailsString = Auxil::lexiconToDetails(lexicon);
    emit detailsChanged(detailsString);
}
