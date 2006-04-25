//---------------------------------------------------------------------------
// SearchConditionForm.cpp
//
// A form for specifying a search condition.
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

#include "SearchConditionForm.h"
#include "SearchSet.h"
#include "WordLineEdit.h"
#include "WordListDialog.h"
#include "WordValidator.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QHBoxLayout>
#include <QLabel>

const int MAX_MAX_INT_VALUE = 999999;

using namespace Defs;

//---------------------------------------------------------------------------
//  SearchConditionForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
SearchConditionForm::SearchConditionForm (QWidget* parent, Qt::WFlags f)
    : QWidget (parent, f),
    letterValidator (new WordValidator (this)),
    patternValidator (new WordValidator (this))

{
    patternValidator->setOptions (WordValidator::AllowQuestionMarks |
                                  WordValidator::AllowAsterisks |
                                  WordValidator::AllowCharacterClasses);

    QHBoxLayout* mainHlay = new QHBoxLayout (this);
    Q_CHECK_PTR (mainHlay);
    mainHlay->setMargin (0);
    mainHlay->setSpacing (SPACING);

    negationCbox = new QCheckBox ("Not");
    Q_CHECK_PTR (negationCbox);
    negationCbox->setEnabled (false);
    mainHlay->addWidget (negationCbox);

    QStringList types;
    types << Auxil::searchTypeToString (SearchCondition::AnagramMatch)
          << Auxil::searchTypeToString (SearchCondition::PatternMatch)
          << Auxil::searchTypeToString (SearchCondition::SubanagramMatch)
          << Auxil::searchTypeToString (SearchCondition::Length)
          << Auxil::searchTypeToString (SearchCondition::Prefix)
          << Auxil::searchTypeToString (SearchCondition::Suffix)
          << Auxil::searchTypeToString (SearchCondition::IncludeLetters)
          << Auxil::searchTypeToString (SearchCondition::ConsistOf)
          << Auxil::searchTypeToString (SearchCondition::BelongToGroup)
          << Auxil::searchTypeToString (SearchCondition::InWordList)
          << Auxil::searchTypeToString (SearchCondition::NumAnagrams)
          << Auxil::searchTypeToString (SearchCondition::ProbabilityOrder)
          << Auxil::searchTypeToString
             (SearchCondition::LimitByProbabilityOrder);

    typeCbox = new QComboBox;
    Q_CHECK_PTR (typeCbox);
    typeCbox->addItems (types);
    connect (typeCbox, SIGNAL (activated (const QString&)),
             SLOT (typeChanged (const QString&)));
    mainHlay->addWidget (typeCbox);

    paramStack = new QStackedWidget;
    Q_CHECK_PTR (paramStack);
    mainHlay->addWidget (paramStack);

    // Frame containing just an input line
    paramLineWidget = new QWidget;
    Q_CHECK_PTR (paramLineWidget);
    QHBoxLayout* paramLineHlay = new QHBoxLayout (paramLineWidget);
    Q_CHECK_PTR (paramLineHlay);
    paramLineHlay->setMargin (0);
    paramLineHlay->setSpacing (SPACING);
    paramLine = new WordLineEdit;
    Q_CHECK_PTR (paramLine);
    paramLine->setValidator (patternValidator);
    connect (paramLine, SIGNAL (returnPressed()), SIGNAL (returnPressed()));
    connect (paramLine, SIGNAL (textChanged (const QString&)),
             SIGNAL (contentsChanged()));
    paramLineHlay->addWidget (paramLine);
    paramStack->addWidget (paramLineWidget);

    // Frame containing just spin boxes
    paramSboxWidget = new QWidget;
    Q_CHECK_PTR (paramSboxWidget);
    QHBoxLayout* paramSboxHlay = new QHBoxLayout (paramSboxWidget);
    Q_CHECK_PTR (paramSboxHlay);
    paramSboxHlay->setMargin (0);
    paramSboxHlay->setSpacing (SPACING);

    QLabel* paramMinLabel = new QLabel ("Min:");
    Q_CHECK_PTR (paramMinLabel);
    paramSboxHlay->addWidget (paramMinLabel);

    paramMinSbox = new QSpinBox;
    Q_CHECK_PTR (paramMinSbox);
    paramMinSbox->setMinimum (0);
    connect (paramMinSbox, SIGNAL (valueChanged (int)),
             SIGNAL (contentsChanged()));
    paramSboxHlay->addWidget (paramMinSbox, 1);

    QLabel* paramMaxLabel = new QLabel ("Max:");
    Q_CHECK_PTR (paramMaxLabel);
    paramSboxHlay->addWidget (paramMaxLabel);

    paramMaxSbox = new QSpinBox;
    Q_CHECK_PTR (paramMaxSbox);
    paramMaxSbox->setMinimum (0);
    connect (paramMaxSbox, SIGNAL (valueChanged (int)),
             SIGNAL (contentsChanged()));
    paramSboxHlay->addWidget (paramMaxSbox, 1);

    paramProbCbox = new QCheckBox ("Lax");
    Q_CHECK_PTR (paramProbCbox);
    paramProbCbox->setCheckState (Qt::Checked);
    paramSboxHlay->addWidget (paramProbCbox);

    paramStack->addWidget (paramSboxWidget);

    // Frame containing just a combo box
    paramCboxWidget = new QWidget;
    Q_CHECK_PTR (paramCboxWidget);
    QHBoxLayout* paramCboxHlay = new QHBoxLayout (paramCboxWidget);
    Q_CHECK_PTR (paramCboxHlay);
    paramCboxHlay->setMargin (0);
    paramCboxHlay->setSpacing (SPACING);
    paramCbox = new QComboBox;
    Q_CHECK_PTR (paramCbox);
    connect (paramCbox, SIGNAL (activated (const QString&)),
             SIGNAL (contentsChanged()));
    paramCboxHlay->addWidget (paramCbox);
    paramStack->addWidget (paramCboxWidget);

    // Frame containing spin box and input line
    paramConsistWidget = new QWidget;
    Q_CHECK_PTR (paramConsistWidget);
    QHBoxLayout* paramConsistHlay = new QHBoxLayout (paramConsistWidget);
    Q_CHECK_PTR (paramConsistHlay);
    paramConsistHlay->setMargin (0);
    paramConsistHlay->setSpacing (SPACING);

    QLabel* paramConsistMinLabel = new QLabel ("Min:");
    Q_CHECK_PTR (paramConsistMinLabel);
    paramConsistHlay->addWidget (paramConsistMinLabel);

    paramConsistMinSbox = new QSpinBox;
    Q_CHECK_PTR (paramConsistMinSbox);
    paramConsistMinSbox->setMinimum (0);
    paramConsistMinSbox->setMaximum (100);
    connect (paramConsistMinSbox, SIGNAL (valueChanged (int)),
             SIGNAL (contentsChanged()));
    paramConsistHlay->addWidget (paramConsistMinSbox);

    QLabel* minPctLabel = new QLabel ("%");
    Q_CHECK_PTR (minPctLabel);
    paramConsistHlay->addWidget (minPctLabel);

    QLabel* paramConsistMaxLabel = new QLabel ("Max:");
    Q_CHECK_PTR (paramConsistMaxLabel);
    paramConsistHlay->addWidget (paramConsistMaxLabel);

    paramConsistMaxSbox = new QSpinBox;
    Q_CHECK_PTR (paramConsistMaxSbox);
    paramConsistMaxSbox->setMinimum (0);
    paramConsistMaxSbox->setMaximum (100);
    connect (paramConsistMaxSbox, SIGNAL (valueChanged (int)),
             SIGNAL (contentsChanged()));
    paramConsistHlay->addWidget (paramConsistMaxSbox);

    QLabel* maxPctLabel = new QLabel ("%");
    Q_CHECK_PTR (maxPctLabel);
    paramConsistHlay->addWidget (maxPctLabel);

    paramConsistLine = new WordLineEdit;
    Q_CHECK_PTR (paramConsistLine);
    paramConsistLine->setValidator (letterValidator);
    connect (paramConsistLine, SIGNAL (returnPressed()),
             SIGNAL (returnPressed()));
    connect (paramConsistLine, SIGNAL (textChanged (const QString&)),
             SIGNAL (contentsChanged()));
    paramConsistHlay->addWidget (paramConsistLine);

    paramStack->addWidget (paramConsistWidget);

    // Frame containing disabled input line and push button for getting word
    // lists
    paramWordListWidget = new QWidget;
    Q_CHECK_PTR (paramWordListWidget);
    QHBoxLayout* paramWordListHlay = new QHBoxLayout (paramWordListWidget);
    Q_CHECK_PTR (paramWordListHlay);
    paramWordListHlay->setMargin (0);
    paramWordListHlay->setSpacing (SPACING);

    paramWordListLine = new QLineEdit;
    Q_CHECK_PTR (paramWordListLine);
    paramWordListLine->setText ("0 words");
    paramWordListLine->setReadOnly (true);
    connect (paramWordListLine, SIGNAL (textChanged (const QString&)),
             SIGNAL (contentsChanged()));
    paramWordListHlay->addWidget (paramWordListLine);

    ZPushButton* paramWordListButton = new ZPushButton ("Edit List...");
    Q_CHECK_PTR (paramWordListButton);
    connect (paramWordListButton, SIGNAL (clicked()),
             SLOT (editListClicked()));
    paramWordListHlay->addWidget (paramWordListButton);
    paramStack->addWidget (paramWordListWidget);
}

//---------------------------------------------------------------------------
//  getSearchCondition
//
//! Get a SearchCondition object corresponding to the values in the form.
//
//! @return the search condition
//---------------------------------------------------------------------------
SearchCondition
SearchConditionForm::getSearchCondition() const
{
    SearchCondition condition;
    QString typeStr = typeCbox->currentText();
    condition.type = Auxil::stringToSearchType (typeStr);
    condition.negated = (negationCbox->checkState() == Qt::Checked);

    switch (condition.type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::Prefix:
        case SearchCondition::Suffix:
        case SearchCondition::IncludeLetters:
        condition.stringValue = paramLine->text();
        break;

        case SearchCondition::Length:
        case SearchCondition::NumAnagrams:
        case SearchCondition::Probability:
        condition.minValue = paramMinSbox->value();
        condition.maxValue = paramMaxSbox->value();
        break;

        case SearchCondition::ProbabilityOrder:
        case SearchCondition::LimitByProbabilityOrder:
        condition.minValue = paramMinSbox->value();
        condition.maxValue = paramMaxSbox->value();
        condition.boolValue = (paramProbCbox->checkState() == Qt::Checked);
        break;

        case SearchCondition::ConsistOf:
        condition.stringValue = paramConsistLine->text();
        condition.minValue = paramConsistMinSbox->value();
        condition.maxValue = paramConsistMaxSbox->value();
        break;

        case SearchCondition::BelongToGroup:
        condition.stringValue = paramCbox->currentText();
        break;

        case SearchCondition::InWordList:
        condition.stringValue = paramWordListString;
        break;

        default:
        //qWarning ("Unrecognized search condition: " + typeStr);
        break;
    }

    return condition;
}

//---------------------------------------------------------------------------
//  setSearchCondition
//
//! Set the contents of the form according to the contents of a search
//! condition.
//
//! @param condition the search condition
//---------------------------------------------------------------------------
void
SearchConditionForm::setSearchCondition (const SearchCondition& condition)
{
    reset();
    typeCbox->setCurrentIndex (typeCbox->findText (Auxil::searchTypeToString
                                                   (condition.type)));
    typeChanged (typeCbox->currentText());
    negationCbox->setCheckState (condition.negated ? Qt::Checked
                                                   : Qt::Unchecked);

    switch (condition.type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::Prefix:
        case SearchCondition::Suffix:
        case SearchCondition::IncludeLetters:
        paramLine->setText (condition.stringValue);
        break;

        case SearchCondition::Length:
        case SearchCondition::NumAnagrams:
        case SearchCondition::Probability:
        paramMinSbox->setValue (condition.minValue);
        paramMaxSbox->setValue (condition.maxValue);
        break;

        case SearchCondition::ProbabilityOrder:
        case SearchCondition::LimitByProbabilityOrder:
        paramMinSbox->setValue (condition.minValue);
        paramMaxSbox->setValue (condition.maxValue);
        paramProbCbox->setCheckState (condition.boolValue ? Qt::Checked
                                                          : Qt::Unchecked);
        break;

        case SearchCondition::ConsistOf:
        paramConsistMinSbox->setValue (condition.minValue);
        paramConsistMaxSbox->setValue (condition.maxValue);
        paramConsistLine->setText (condition.stringValue);
        break;

        case SearchCondition::BelongToGroup:
        paramCbox->setCurrentIndex (paramCbox->findText
                                    (condition.stringValue));
        break;

        case SearchCondition::InWordList:
        setWordListString (condition.stringValue);
        break;

        default: break;
    }
}

//---------------------------------------------------------------------------
//  isValid
//
//! Determine whether the input in the form is valid.
//
//! @return true if valid, false otherwise
//---------------------------------------------------------------------------
bool
SearchConditionForm::isValid() const
{
    SearchCondition::SearchType type = Auxil::stringToSearchType
        (typeCbox->currentText());

    switch (type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::IncludeLetters:
        return matchStringIsValid (paramLine->text());

        case SearchCondition::Prefix:
        case SearchCondition::Suffix:
        return !paramLine->text().isEmpty();

        case SearchCondition::Length:
        return ((paramMinSbox->value() > 0) ||
                (paramMaxSbox->value() < MAX_WORD_LEN)) &&
               (paramMinSbox->value() <= paramMaxSbox->value());

        case SearchCondition::NumAnagrams:
        case SearchCondition::ProbabilityOrder:
        case SearchCondition::LimitByProbabilityOrder:
        return ((paramMinSbox->value() > 0) ||
                (paramMaxSbox->value() < MAX_MAX_INT_VALUE)) &&
               (paramMinSbox->value() <= paramMaxSbox->value());

        case SearchCondition::ConsistOf:
        return (paramConsistMinSbox->value() <= paramConsistMaxSbox->value())
            && !paramConsistLine->text().isEmpty();

        case SearchCondition::InWordList:
        return !paramWordListString.isEmpty();

        case SearchCondition::UnknownSearchType:
        return false;

        default: return true;
    }
}

//---------------------------------------------------------------------------
//  typeChanged
//
//! Called when an option in the type dropdown box is activated.
//
//! @param string the activated option
//---------------------------------------------------------------------------
void
SearchConditionForm::typeChanged (const QString& string)
{
    SearchCondition::SearchType type = Auxil::stringToSearchType (string);

    switch (type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        negationCbox->setCheckState (Qt::Unchecked);
        negationCbox->setEnabled (false);
        paramLine->setValidator (patternValidator);
        paramStack->setCurrentWidget (paramLineWidget);
        break;

        case SearchCondition::Prefix:
        case SearchCondition::Suffix:
        case SearchCondition::IncludeLetters:
        negationCbox->setEnabled (true);
        paramLine->setValidator (letterValidator);
        paramStack->setCurrentWidget (paramLineWidget);
        break;

        case SearchCondition::Length:
        negationCbox->setCheckState (Qt::Unchecked);
        negationCbox->setEnabled (false);
        paramMinSbox->setMaximum (MAX_WORD_LEN);
        paramMinSbox->setValue (0);
        paramMaxSbox->setMaximum (MAX_WORD_LEN);
        paramMaxSbox->setValue (MAX_WORD_LEN);
        paramStack->setCurrentWidget (paramSboxWidget);
        break;

        case SearchCondition::NumAnagrams:
        case SearchCondition::ProbabilityOrder:
        case SearchCondition::LimitByProbabilityOrder:
        negationCbox->setCheckState (Qt::Unchecked);
        negationCbox->setEnabled (false);
        paramMinSbox->setMaximum (MAX_MAX_INT_VALUE);
        paramMinSbox->setValue (0);
        paramMaxSbox->setMaximum (MAX_MAX_INT_VALUE);
        paramMaxSbox->setValue (MAX_MAX_INT_VALUE);
        paramProbCbox->setCheckState (Qt::Checked);
        if (type == SearchCondition::NumAnagrams)
            paramProbCbox->hide();
        else
            paramProbCbox->show();
        paramStack->setCurrentWidget (paramSboxWidget);
        break;

        case SearchCondition::Probability:
        negationCbox->setCheckState (Qt::Unchecked);
        negationCbox->setEnabled (false);
        paramMinSbox->setMaximum (100);
        paramMinSbox->setValue (0);
        paramMaxSbox->setMaximum (100);
        paramMaxSbox->setValue (100);
        paramStack->setCurrentWidget (paramSboxWidget);
        break;

        case SearchCondition::BelongToGroup:
        negationCbox->setEnabled (true);
        paramCbox->clear();
        paramCbox->addItem (Auxil::searchSetToString (SetHookWords));
        paramCbox->addItem (Auxil::searchSetToString (SetFrontHooks));
        paramCbox->addItem (Auxil::searchSetToString (SetBackHooks));
        paramCbox->addItem (Auxil::searchSetToString (SetTypeOneSevens));
        paramCbox->addItem (Auxil::searchSetToString (SetTypeTwoSevens));
        paramCbox->addItem (Auxil::searchSetToString (SetTypeThreeSevens));
        paramCbox->addItem (Auxil::searchSetToString (SetTypeOneEights));
        paramCbox->addItem (Auxil::searchSetToString (SetTypeTwoEights));
        paramCbox->addItem (Auxil::searchSetToString (SetTypeThreeEights));
        paramCbox->addItem (Auxil::searchSetToString
                            (SetEightsFromSevenLetterStems));
        paramCbox->addItem (Auxil::searchSetToString (SetNewInOwl2));
        paramStack->setCurrentWidget (paramCboxWidget);
        break;

        case SearchCondition::ConsistOf:
        negationCbox->setCheckState (Qt::Unchecked);
        negationCbox->setEnabled (false);
        paramConsistMinSbox->setValue (0);
        paramConsistMaxSbox->setValue (100);
        paramStack->setCurrentWidget (paramConsistWidget);
        break;

        case SearchCondition::InWordList:
        negationCbox->setEnabled (true);
        setWordListString ("");
        paramStack->setCurrentWidget (paramWordListWidget);
        break;

        default:
        //qWarning ("Unrecognized search condition: " + string);
        break;
    }

    emit contentsChanged();
}

//---------------------------------------------------------------------------
//  editListClicked
//
//! Called when the Edit List button is clicked.  Display a dialog that allows
//! the user to enter words or add words from a file.
//---------------------------------------------------------------------------
void
SearchConditionForm::editListClicked()
{
    WordListDialog* dialog = new WordListDialog (this);
    Q_CHECK_PTR (dialog);

    dialog->setWords (paramWordListString);

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        setWordListString (dialog->getWords());
    }
    delete dialog;
}

//---------------------------------------------------------------------------
//  reset
//
//! Reset the form to its original, pristine state.
//---------------------------------------------------------------------------
void
SearchConditionForm::reset()
{
    paramLine->setText ("");
    paramMinSbox->setValue (0);
    paramMaxSbox->setValue (MAX_MAX_INT_VALUE);
    paramCbox->setCurrentIndex (0);
    paramConsistMinSbox->setValue (0);
    paramConsistMaxSbox->setValue (100);
    paramConsistLine->setText ("");
    typeCbox->setCurrentIndex (0);
    typeChanged (typeCbox->currentText());
}

//---------------------------------------------------------------------------
//  selectInputArea
//
//! Give focus to the currently showing input area.
//---------------------------------------------------------------------------
void
SearchConditionForm::selectInputArea()
{
    QWidget* currentWidget = paramStack->currentWidget();
    if (currentWidget == paramLineWidget) {
        paramLine->setFocus();
        paramLine->setSelection (0, paramLine->text().length());
    }
    else if (currentWidget == paramSboxWidget) {
        paramMinSbox->setFocus();
    }
    else if (currentWidget == paramCboxWidget) {
        paramCbox->setFocus();
    }
    else if (currentWidget == paramConsistWidget) {
        paramConsistLine->setFocus();
        paramConsistLine->setSelection (0, paramConsistLine->text().length());
    }
}

//---------------------------------------------------------------------------
//  setWordListString
//
//! Set the value of the word list string, and update the word list line to
//! reflect the number of words in the list.
//---------------------------------------------------------------------------
void
SearchConditionForm::setWordListString (const QString& string)
{
    paramWordListString = string;
    int numWords = 0;
    if (!string.isEmpty()) {
        QStringList wordList = string.split (QChar (' '));
        numWords = wordList.size();
    }
    paramWordListLine->setText (QString::number (numWords) + " word" +
                                (numWords == 1 ? QString() : QString ("s")));
    paramWordListLine->home (false);
}

//---------------------------------------------------------------------------
//  matchStringIsValid
//
//! Determine whether a string is a valid match string.
//
//! @return true if valid, false otherwise
//---------------------------------------------------------------------------
bool
SearchConditionForm::matchStringIsValid (const QString& string) const
{
    if (string.isEmpty())
        return false;

    bool inGroup = false;
    bool groupLetters = false;
    bool negatedGroup = false;
    int len = string.length();
    for (int i = 0; i < len; ++i) {
        QChar c = string.at (i);

        if (inGroup) {
            if (c == ']') {
                if (!groupLetters)
                    return false;
                inGroup = false;
                groupLetters = false;
            }
            else if (c == '^') {
                if (groupLetters)
                    return false;
                negatedGroup = true;
            }
            else if ((c == '[') || (c == '*') || (c == '?'))
                return false;
            else
                groupLetters = true;
        }

        else {
            if (c == '[')
                inGroup = true;
            else if ((c == ']') || (c == '^'))
                return false;
        }
    }

    return !inGroup;
}
