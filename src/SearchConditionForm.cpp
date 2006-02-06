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

    QStringList types;
    types << Auxil::searchTypeToString (SearchCondition::PatternMatch)
          << Auxil::searchTypeToString (SearchCondition::AnagramMatch)
          << Auxil::searchTypeToString (SearchCondition::SubanagramMatch)
          << Auxil::searchTypeToString (SearchCondition::ExactLength)
          << Auxil::searchTypeToString (SearchCondition::MinLength)
          << Auxil::searchTypeToString (SearchCondition::MaxLength)
          << Auxil::searchTypeToString (SearchCondition::TakesPrefix)
          << Auxil::searchTypeToString (SearchCondition::DoesNotTakePrefix)
          << Auxil::searchTypeToString (SearchCondition::TakesSuffix)
          << Auxil::searchTypeToString (SearchCondition::DoesNotTakeSuffix)
          << Auxil::searchTypeToString (SearchCondition::MustInclude)
          << Auxil::searchTypeToString (SearchCondition::MustExclude)
          << Auxil::searchTypeToString (SearchCondition::MustConsist)
          << Auxil::searchTypeToString (SearchCondition::MustBelong)
          << Auxil::searchTypeToString (SearchCondition::InWordList)
          << Auxil::searchTypeToString (SearchCondition::NotInWordList)
          << Auxil::searchTypeToString (SearchCondition::ExactAnagrams)
          << Auxil::searchTypeToString (SearchCondition::MinAnagrams)
          << Auxil::searchTypeToString (SearchCondition::MaxAnagrams);
          //<< Auxil::searchTypeToString (SearchCondition::MinProbability)
          //<< Auxil::searchTypeToString (SearchCondition::MaxProbability);

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

    // Frame containing just a spin box
    paramSboxWidget = new QWidget;
    Q_CHECK_PTR (paramSboxWidget);
    QHBoxLayout* paramSboxHlay = new QHBoxLayout (paramSboxWidget);
    Q_CHECK_PTR (paramSboxHlay);
    paramSboxHlay->setMargin (0);
    paramSboxHlay->setSpacing (SPACING);
    paramSbox = new QSpinBox;
    Q_CHECK_PTR (paramSbox);
    paramSbox->setMinimum (0);
    connect (paramSbox, SIGNAL (valueChanged (int)),
             SIGNAL (contentsChanged()));
    paramSboxHlay->addWidget (paramSbox);
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
    paramConsistSbox = new QSpinBox;
    Q_CHECK_PTR (paramConsistSbox);
    paramConsistSbox->setMinimum (0);
    paramConsistSbox->setMaximum (100);
    connect (paramConsistSbox, SIGNAL (valueChanged (int)),
             SIGNAL (contentsChanged()));
    paramConsistHlay->addWidget (paramConsistSbox);
    QLabel* pctLabel = new QLabel ("%");
    Q_CHECK_PTR (pctLabel);
    paramConsistHlay->addWidget (pctLabel);
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

    switch (condition.type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::TakesPrefix:
        case SearchCondition::DoesNotTakePrefix:
        case SearchCondition::TakesSuffix:
        case SearchCondition::DoesNotTakeSuffix:
        case SearchCondition::MustInclude:
        case SearchCondition::MustExclude:
        condition.stringValue = paramLine->text();
        break;

        case SearchCondition::ExactLength:
        case SearchCondition::MinLength:
        case SearchCondition::MaxLength:
        case SearchCondition::ExactAnagrams:
        case SearchCondition::MinAnagrams:
        case SearchCondition::MaxAnagrams:
        case SearchCondition::MinProbability:
        case SearchCondition::MaxProbability:
        condition.intValue = paramSbox->value();
        break;

        case SearchCondition::MustConsist:
        condition.stringValue = paramConsistLine->text();
        condition.intValue = paramConsistSbox->value();
        break;

        case SearchCondition::MustBelong:
        condition.stringValue = paramCbox->currentText();
        break;

        case SearchCondition::InWordList:
        case SearchCondition::NotInWordList:
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

    switch (condition.type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::TakesPrefix:
        case SearchCondition::DoesNotTakePrefix:
        case SearchCondition::TakesSuffix:
        case SearchCondition::DoesNotTakeSuffix:
        case SearchCondition::MustInclude:
        case SearchCondition::MustExclude:
        paramLine->setText (condition.stringValue);
        break;

        case SearchCondition::ExactLength:
        case SearchCondition::MinLength:
        case SearchCondition::MaxLength:
        case SearchCondition::ExactAnagrams:
        case SearchCondition::MinAnagrams:
        case SearchCondition::MaxAnagrams:
        paramSbox->setValue (condition.intValue);
        break;

        case SearchCondition::MustConsist:
        paramConsistSbox->setValue (condition.intValue);
        paramConsistLine->setText (condition.stringValue);
        break;

        case SearchCondition::MustBelong:
        paramCbox->setCurrentIndex (paramCbox->findText
                                    (condition.stringValue));
        break;

        case SearchCondition::InWordList:
        case SearchCondition::NotInWordList:
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
        case SearchCondition::MustInclude:
        case SearchCondition::MustExclude:
        return matchStringIsValid (paramLine->text());

        case SearchCondition::TakesPrefix:
        case SearchCondition::DoesNotTakePrefix:
        case SearchCondition::TakesSuffix:
        case SearchCondition::DoesNotTakeSuffix:
        return !paramLine->text().isEmpty();

        case SearchCondition::ExactLength:
        case SearchCondition::MinLength:
        case SearchCondition::MaxLength:
        case SearchCondition::ExactAnagrams:
        case SearchCondition::MinAnagrams:
        case SearchCondition::MaxAnagrams:
        return paramSbox->value() > 0;

        case SearchCondition::MustConsist:
        return (paramConsistSbox->value() > 0) &&
               !paramConsistLine->text().isEmpty();

        case SearchCondition::InWordList:
        case SearchCondition::NotInWordList:
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
        paramStack->setCurrentWidget (paramLineWidget);
        paramLine->setValidator (patternValidator);
        break;

        case SearchCondition::TakesPrefix:
        case SearchCondition::DoesNotTakePrefix:
        case SearchCondition::TakesSuffix:
        case SearchCondition::DoesNotTakeSuffix:
        case SearchCondition::MustInclude:
        case SearchCondition::MustExclude:
        paramStack->setCurrentWidget (paramLineWidget);
        paramLine->setValidator (letterValidator);
        break;

        case SearchCondition::ExactLength:
        case SearchCondition::MinLength:
        case SearchCondition::MaxLength:
        paramSbox->setMaximum (MAX_WORD_LEN);
        paramStack->setCurrentWidget (paramSboxWidget);
        break;

        case SearchCondition::ExactAnagrams:
        case SearchCondition::MinAnagrams:
        case SearchCondition::MaxAnagrams:
        paramSbox->setMaximum (100);
        paramStack->setCurrentWidget (paramSboxWidget);
        break;

        case SearchCondition::MinProbability:
        case SearchCondition::MaxProbability:
        paramSbox->setMaximum (100);
        paramStack->setCurrentWidget (paramSboxWidget);
        break;

        case SearchCondition::MustBelong:
        paramCbox->clear();
        paramCbox->addItem (Auxil::searchSetToString (SetHookWords));
        paramCbox->addItem (Auxil::searchSetToString (SetFrontHooks));
        paramCbox->addItem (Auxil::searchSetToString (SetBackHooks));
        paramCbox->addItem (Auxil::searchSetToString (SetTypeOneSevens));
        //paramCbox->addItem (Auxil::searchSetToString (SetTypeTwoSevens));
        //paramCbox->addItem (Auxil::searchSetToString (SetTypeThreeSevens));
        paramCbox->addItem (Auxil::searchSetToString (SetTypeOneEights));
        paramCbox->addItem (Auxil::searchSetToString
                               (SetEightsFromSevenLetterStems));
        paramCbox->addItem (Auxil::searchSetToString (SetNewInOwl2));
        paramStack->setCurrentWidget (paramCboxWidget);
        break;

        case SearchCondition::MustConsist:
        paramStack->setCurrentWidget (paramConsistWidget);
        break;

        case SearchCondition::InWordList:
        case SearchCondition::NotInWordList:
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
    paramSbox->setValue (0);
    paramCbox->setCurrentIndex (0);
    paramConsistSbox->setValue (0);
    paramConsistLine->setText ("");
    typeCbox->setCurrentIndex (0);
    typeChanged (typeCbox->currentText());
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
    QStringList wordList = string.split (QChar (' '));
    paramWordListLine->setText (QString::number (wordList.size()) + " words");
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
