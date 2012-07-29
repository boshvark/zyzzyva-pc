//---------------------------------------------------------------------------
// SearchConditionForm.cpp
//
// A form for specifying a search condition.
//
// Copyright 2005-2012 Boshvark Software, LLC.
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
#include "MainSettings.h"
#include "SearchSet.h"
#include "WordLineEdit.h"
#include "WordListDialog.h"
#include "WordValidator.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QHBoxLayout>

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
SearchConditionForm::SearchConditionForm(QWidget* parent, Qt::WFlags f)
    : QWidget(parent, f),
    letterValidator(new WordValidator(this)),
    patternValidator(new WordValidator(this)), legacy(false)
{
    patternValidator->setOptions(WordValidator::AllowQuestionMarks |
                                 WordValidator::AllowAsterisks |
                                 WordValidator::AllowCharacterClasses);

    QHBoxLayout* mainHlay = new QHBoxLayout(this);
    Q_CHECK_PTR(mainHlay);
    mainHlay->setMargin(0);
    mainHlay->setSpacing(SPACING);

    addButton = new QPushButton;
    Q_CHECK_PTR(addButton);
    addButton->setIcon(QIcon(":/plus-icon"));
    addButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(addButton, SIGNAL(clicked()), SIGNAL(addClicked()));
    mainHlay->addWidget(addButton);

    deleteButton = new QPushButton;
    Q_CHECK_PTR(deleteButton);
    deleteButton->setIcon(QIcon(":/minus-icon"));
    deleteButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(deleteButton, SIGNAL(clicked()), SIGNAL(deleteClicked()));
    mainHlay->addWidget(deleteButton);

    negationCbox = new QCheckBox("Not");
    Q_CHECK_PTR(negationCbox);
    negationCbox->setEnabled(false);
    mainHlay->addWidget(negationCbox);

    QStringList types;
    types << Auxil::searchTypeToString(SearchCondition::AnagramMatch)
          << Auxil::searchTypeToString(SearchCondition::PatternMatch)
          << Auxil::searchTypeToString(SearchCondition::SubanagramMatch)
          << Auxil::searchTypeToString(SearchCondition::Length)
          << Auxil::searchTypeToString(SearchCondition::InLexicon)
          << Auxil::searchTypeToString(SearchCondition::InWordList)
          << Auxil::searchTypeToString(SearchCondition::BelongToGroup)
          << Auxil::searchTypeToString(SearchCondition::NumVowels)
          << Auxil::searchTypeToString(SearchCondition::IncludeLetters)
          << Auxil::searchTypeToString(SearchCondition::ProbabilityOrder)
          << Auxil::searchTypeToString(SearchCondition::LimitByProbabilityOrder)
          << Auxil::searchTypeToString(SearchCondition::PlayabilityOrder)
          << Auxil::searchTypeToString(SearchCondition::LimitByPlayabilityOrder)
          << Auxil::searchTypeToString(SearchCondition::NumUniqueLetters)
          << Auxil::searchTypeToString(SearchCondition::PointValue)
          << Auxil::searchTypeToString(SearchCondition::Prefix)
          << Auxil::searchTypeToString(SearchCondition::Suffix)
          << Auxil::searchTypeToString(SearchCondition::PartOfSpeech)
          << Auxil::searchTypeToString(SearchCondition::Definition)
          << Auxil::searchTypeToString(SearchCondition::ConsistOf)
          << Auxil::searchTypeToString(SearchCondition::NumAnagrams);

    typeCbox = new QComboBox;
    Q_CHECK_PTR(typeCbox);
    typeCbox->addItems(types);
    connect(typeCbox, SIGNAL(activated(const QString&)),
            SLOT(typeChanged(const QString&)));
    mainHlay->addWidget(typeCbox);

    paramStack = new QStackedWidget;
    Q_CHECK_PTR(paramStack);
    mainHlay->addWidget(paramStack);

    // Frame containing just an input line
    paramLineWidget = new QWidget;
    Q_CHECK_PTR(paramLineWidget);
    QHBoxLayout* paramLineHlay = new QHBoxLayout(paramLineWidget);
    Q_CHECK_PTR(paramLineHlay);
    paramLineHlay->setMargin(0);
    paramLineHlay->setSpacing(SPACING);
    paramLine = new WordLineEdit;
    Q_CHECK_PTR(paramLine);
    paramLine->setValidator(patternValidator);
    connect(paramLine, SIGNAL(returnPressed()), SIGNAL(returnPressed()));
    connect(paramLine, SIGNAL(textChanged(const QString&)),
            SIGNAL(contentsChanged()));
    paramLineHlay->addWidget(paramLine);
    paramStack->addWidget(paramLineWidget);

    // Frame containing just spin boxes
    paramSboxWidget = new QWidget;
    Q_CHECK_PTR(paramSboxWidget);

    QHBoxLayout* paramSboxHlay = new QHBoxLayout(paramSboxWidget);
    Q_CHECK_PTR(paramSboxHlay);
    paramSboxHlay->setMargin(0);
    paramSboxHlay->setSpacing(SPACING);

    QLabel* paramMinLabel = new QLabel("Min:");
    Q_CHECK_PTR(paramMinLabel);
    paramSboxHlay->addWidget(paramMinLabel);

    paramMinSbox = new QSpinBox;
    Q_CHECK_PTR(paramMinSbox);
    paramMinSbox->setMinimum(0);
    connect(paramMinSbox, SIGNAL(valueChanged(int)), SIGNAL(contentsChanged()));
    paramSboxHlay->addWidget(paramMinSbox, 1);

    QLabel* paramMaxLabel = new QLabel("Max:");
    Q_CHECK_PTR(paramMaxLabel);
    paramSboxHlay->addWidget(paramMaxLabel);

    paramMaxSbox = new QSpinBox;
    Q_CHECK_PTR(paramMaxSbox);
    paramMaxSbox->setMinimum(0);
    connect(paramMaxSbox, SIGNAL(valueChanged(int)), SIGNAL(contentsChanged()));
    paramSboxHlay->addWidget(paramMaxSbox, 1);

    paramBlanksLabel = new QLabel("Blanks:");
    Q_CHECK_PTR(paramBlanksLabel);
    paramSboxHlay->addWidget(paramBlanksLabel);

    paramBlanksSbox = new QSpinBox;
    Q_CHECK_PTR(paramBlanksSbox);
    paramBlanksSbox->setMinimum(0);
    paramBlanksSbox->setMaximum(Defs::MAX_BLANKS);
    connect(paramBlanksSbox, SIGNAL(valueChanged(int)),
            SIGNAL(contentsChanged()));
    paramSboxHlay->addWidget(paramBlanksSbox);

    paramProbCbox = new QCheckBox("Lax");
    Q_CHECK_PTR(paramProbCbox);
    paramProbCbox->setCheckState(Qt::Checked);
    paramSboxHlay->addWidget(paramProbCbox);

    // FIXME: with Qt 4.6.1 (but not Qt 4.3.2), this causes a crash when
    // loading the following search. Try to reduce to a minimal example. Also
    // try with other versions of Qt.
    //<!DOCTYPE zyzzyva-search SYSTEM 'http://pietdepsi.com/dtd/zyzzyva-search.dtd'>
    //<zyzzyva-search version="1" >
    // <conditions>
    //  <and>
    //   <condition type="Length" min="7" max="7" />
    //   <condition int="0" bool="false" type="Probability Order" min="1" max="100" />
    //   <condition int="2" bool="false" type="Probability Order" min="1" max="100" />
    //  </and>
    // </conditions>
    //</zyzzyva-search>

    paramStack->addWidget(paramSboxWidget);

    // Frame containing just a combo box
    paramCboxWidget = new QWidget;
    Q_CHECK_PTR(paramCboxWidget);
    QHBoxLayout* paramCboxHlay = new QHBoxLayout(paramCboxWidget);
    Q_CHECK_PTR(paramCboxHlay);
    paramCboxHlay->setMargin(0);
    paramCboxHlay->setSpacing(SPACING);
    paramCbox = new QComboBox;
    Q_CHECK_PTR(paramCbox);
    connect(paramCbox, SIGNAL(activated(const QString&)),
            SIGNAL(contentsChanged()));
    paramCboxHlay->addWidget(paramCbox);
    paramStack->addWidget(paramCboxWidget);

    // Frame containing spin box and input line
    paramConsistWidget = new QWidget;
    Q_CHECK_PTR(paramConsistWidget);
    QHBoxLayout* paramConsistHlay = new QHBoxLayout(paramConsistWidget);
    Q_CHECK_PTR(paramConsistHlay);
    paramConsistHlay->setMargin(0);
    paramConsistHlay->setSpacing(SPACING);

    QLabel* paramConsistMinLabel = new QLabel("Min:");
    Q_CHECK_PTR(paramConsistMinLabel);
    paramConsistHlay->addWidget(paramConsistMinLabel);

    paramConsistMinSbox = new QSpinBox;
    Q_CHECK_PTR(paramConsistMinSbox);
    paramConsistMinSbox->setMinimum(0);
    paramConsistMinSbox->setMaximum(100);
    connect(paramConsistMinSbox, SIGNAL(valueChanged(int)),
            SIGNAL(contentsChanged()));
    paramConsistHlay->addWidget(paramConsistMinSbox);

    QLabel* minPctLabel = new QLabel("%");
    Q_CHECK_PTR(minPctLabel);
    paramConsistHlay->addWidget(minPctLabel);

    QLabel* paramConsistMaxLabel = new QLabel("Max:");
    Q_CHECK_PTR(paramConsistMaxLabel);
    paramConsistHlay->addWidget(paramConsistMaxLabel);

    paramConsistMaxSbox = new QSpinBox;
    Q_CHECK_PTR(paramConsistMaxSbox);
    paramConsistMaxSbox->setMinimum(0);
    paramConsistMaxSbox->setMaximum(100);
    connect(paramConsistMaxSbox, SIGNAL(valueChanged(int)),
            SIGNAL(contentsChanged()));
    paramConsistHlay->addWidget(paramConsistMaxSbox);

    QLabel* maxPctLabel = new QLabel("%");
    Q_CHECK_PTR(maxPctLabel);
    paramConsistHlay->addWidget(maxPctLabel);

    paramConsistLine = new WordLineEdit;
    Q_CHECK_PTR(paramConsistLine);
    paramConsistLine->setValidator(letterValidator);
    connect(paramConsistLine, SIGNAL(returnPressed()), SIGNAL(returnPressed()));
    connect(paramConsistLine, SIGNAL(textChanged(const QString&)),
            SIGNAL(contentsChanged()));
    paramConsistHlay->addWidget(paramConsistLine);

    paramStack->addWidget(paramConsistWidget);

    // Frame containing disabled input line and push button for getting word
    // lists
    paramWordListWidget = new QWidget;
    Q_CHECK_PTR(paramWordListWidget);
    QHBoxLayout* paramWordListHlay = new QHBoxLayout(paramWordListWidget);
    Q_CHECK_PTR(paramWordListHlay);
    paramWordListHlay->setMargin(0);
    paramWordListHlay->setSpacing(SPACING);

    paramWordListLine = new QLineEdit;
    Q_CHECK_PTR(paramWordListLine);
    paramWordListLine->setText("0 words");
    paramWordListLine->setReadOnly(true);
    connect(paramWordListLine, SIGNAL(textChanged(const QString&)),
            SIGNAL(contentsChanged()));
    paramWordListHlay->addWidget(paramWordListLine);

    ZPushButton* paramWordListButton = new ZPushButton("Edit List...");
    Q_CHECK_PTR(paramWordListButton);
    connect(paramWordListButton, SIGNAL(clicked()), SLOT(editListClicked()));
    paramWordListHlay->addWidget(paramWordListButton);
    paramStack->addWidget(paramWordListWidget);

    typeChanged(typeCbox->currentText());
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
    condition.type = Auxil::stringToSearchType(typeStr);
    condition.negated = (negationCbox->checkState() == Qt::Checked);
    condition.legacy = legacy;

    switch (condition.type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::Prefix:
        case SearchCondition::Suffix:
        case SearchCondition::IncludeLetters:
        case SearchCondition::PartOfSpeech:
        case SearchCondition::Definition:
        condition.stringValue = paramLine->text();
        break;

        case SearchCondition::Length:
        case SearchCondition::NumAnagrams:
        case SearchCondition::NumVowels:
        case SearchCondition::NumUniqueLetters:
        case SearchCondition::PointValue:
        condition.minValue = paramMinSbox->value();
        condition.maxValue = paramMaxSbox->value();
        break;

        case SearchCondition::Probability:
        condition.minValue = paramMinSbox->value();
        condition.maxValue = paramMaxSbox->value();
        condition.intValue = paramBlanksSbox->value();;
        break;

        case SearchCondition::ProbabilityOrder:
        case SearchCondition::LimitByProbabilityOrder:
        condition.intValue = paramBlanksSbox->value();;

        // fall through

        case SearchCondition::PlayabilityOrder:
        case SearchCondition::LimitByPlayabilityOrder:
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
        case SearchCondition::InLexicon:
        condition.stringValue = paramCbox->currentText();
        break;

        case SearchCondition::InWordList:
        condition.stringValue = paramWordListString;
        break;

        default:
        //qWarning("Unrecognized search condition: " + typeStr);
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
SearchConditionForm::setSearchCondition(const SearchCondition& condition)
{
    reset();
    typeCbox->setCurrentIndex(typeCbox->findText(
        Auxil::searchTypeToString(condition.type)));
    typeChanged(typeCbox->currentText());
    negationCbox->setCheckState(condition.negated ? Qt::Checked
                                                  : Qt::Unchecked);
    legacy = condition.legacy;

    switch (condition.type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::Prefix:
        case SearchCondition::Suffix:
        case SearchCondition::IncludeLetters:
        case SearchCondition::PartOfSpeech:
        case SearchCondition::Definition:
        paramLine->setText(condition.stringValue);
        break;

        case SearchCondition::Length:
        case SearchCondition::NumAnagrams:
        case SearchCondition::NumVowels:
        case SearchCondition::NumUniqueLetters:
        case SearchCondition::PointValue:
        paramMinSbox->setValue(condition.minValue);
        paramMaxSbox->setValue(condition.maxValue);
        break;

        case SearchCondition::Probability:
        paramMinSbox->setValue(condition.minValue);
        paramMaxSbox->setValue(condition.maxValue);
        paramBlanksSbox->setValue(condition.intValue);
        break;

        case SearchCondition::ProbabilityOrder:
        case SearchCondition::LimitByProbabilityOrder:
        paramBlanksSbox->setValue(condition.intValue);

        // fall through

        case SearchCondition::PlayabilityOrder:
        case SearchCondition::LimitByPlayabilityOrder:
        paramMinSbox->setValue(condition.minValue);
        paramMaxSbox->setValue(condition.maxValue);
        paramProbCbox->setCheckState(condition.boolValue ? Qt::Checked
                                                         : Qt::Unchecked);
        break;

        case SearchCondition::ConsistOf:
        paramConsistMinSbox->setValue(condition.minValue);
        paramConsistMaxSbox->setValue(condition.maxValue);
        paramConsistLine->setText(condition.stringValue);
        break;

        case SearchCondition::BelongToGroup:
        case SearchCondition::InLexicon:
        paramCbox->setCurrentIndex(paramCbox->findText(condition.stringValue));
        break;

        case SearchCondition::InWordList:
        setWordListString(condition.stringValue);
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
    SearchCondition::SearchType type =
        Auxil::stringToSearchType(typeCbox->currentText());

    switch (type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::IncludeLetters:
        return matchStringIsValid(paramLine->text());

        case SearchCondition::Prefix:
        case SearchCondition::Suffix:
        case SearchCondition::PartOfSpeech:
        case SearchCondition::Definition:
        return !paramLine->text().isEmpty();

        case SearchCondition::Length:
        case SearchCondition::NumVowels:
        case SearchCondition::NumUniqueLetters:
        return ((paramMinSbox->value() > 0) ||
                (paramMaxSbox->value() < MAX_WORD_LEN)) &&
               (paramMinSbox->value() <= paramMaxSbox->value());

        case SearchCondition::PointValue:
        return ((paramMinSbox->value() > 0) ||
                (paramMaxSbox->value() < (10 * MAX_WORD_LEN))) &&
               (paramMinSbox->value() <= paramMaxSbox->value());

        case SearchCondition::NumAnagrams:
        case SearchCondition::PlayabilityOrder:
        case SearchCondition::LimitByPlayabilityOrder:
        return ((paramMinSbox->value() > 0) ||
                (paramMaxSbox->value() < MAX_MAX_INT_VALUE)) &&
               (paramMinSbox->value() <= paramMaxSbox->value());

        case SearchCondition::ProbabilityOrder:
        case SearchCondition::LimitByProbabilityOrder:
        return ((paramMinSbox->value() > 0) ||
                (paramMaxSbox->value() < MAX_MAX_INT_VALUE)) &&
               (paramMinSbox->value() <= paramMaxSbox->value()) &&
               (paramBlanksSbox->value() >= 0) &&
               (paramBlanksSbox->value() <= Defs::MAX_BLANKS);

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
SearchConditionForm::typeChanged(const QString& string)
{
    SearchCondition::SearchType type = Auxil::stringToSearchType(string);

    switch (type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::Prefix:
        case SearchCondition::Suffix:
        case SearchCondition::IncludeLetters:
        case SearchCondition::PartOfSpeech:
        case SearchCondition::Definition:
        {
            QValidator* validator = 0;
            if ((type == SearchCondition::Prefix) ||
                (type == SearchCondition::Suffix) ||
                (type == SearchCondition::IncludeLetters))
            {
                validator = letterValidator;
            }
            else if ((type == SearchCondition::PatternMatch) ||
                (type == SearchCondition::AnagramMatch) ||
                (type == SearchCondition::SubanagramMatch))
            {
                validator = patternValidator;
            }
            paramLine->setValidator(validator);
            if (validator) {
                QString text = paramLine->text();
                int pos = 0;
                validator->validate(text, pos);
                paramLine->setText(text);
            }
            negationCbox->setEnabled(true);
            paramStack->setCurrentWidget(paramLineWidget);
        }
        break;

        case SearchCondition::Length:
        case SearchCondition::NumVowels:
        case SearchCondition::NumUniqueLetters:
        negationCbox->setCheckState(Qt::Unchecked);
        negationCbox->setEnabled(false);
        paramMinSbox->setMaximum(MAX_WORD_LEN);
        paramMinSbox->setValue(0);
        paramMaxSbox->setMaximum(MAX_WORD_LEN);
        paramMaxSbox->setValue(MAX_WORD_LEN);
        paramBlanksLabel->hide();
        paramBlanksSbox->hide();
        paramProbCbox->hide();
        paramStack->setCurrentWidget(paramSboxWidget);
        break;

        case SearchCondition::PointValue:
        negationCbox->setCheckState(Qt::Unchecked);
        negationCbox->setEnabled(false);
        paramMinSbox->setMaximum(10 * MAX_WORD_LEN);
        paramMinSbox->setValue(0);
        paramMaxSbox->setMaximum(10 * MAX_WORD_LEN);
        paramMaxSbox->setValue(10 * MAX_WORD_LEN);
        paramBlanksLabel->hide();
        paramBlanksSbox->hide();
        paramProbCbox->hide();
        paramStack->setCurrentWidget(paramSboxWidget);
        break;

        case SearchCondition::NumAnagrams:
        case SearchCondition::ProbabilityOrder:
        case SearchCondition::LimitByProbabilityOrder:
        case SearchCondition::PlayabilityOrder:
        case SearchCondition::LimitByPlayabilityOrder:
        negationCbox->setCheckState(Qt::Unchecked);
        negationCbox->setEnabled(false);
        paramMinSbox->setMaximum(MAX_MAX_INT_VALUE);
        paramMinSbox->setValue(0);
        paramMaxSbox->setMaximum(MAX_MAX_INT_VALUE);
        paramMaxSbox->setValue(MAX_MAX_INT_VALUE);
        paramProbCbox->setCheckState(Qt::Checked);
        paramBlanksSbox->setValue(MainSettings::getProbabilityNumBlanks());

        if ((type == SearchCondition::ProbabilityOrder) ||
            (type == SearchCondition::LimitByProbabilityOrder))
        {
            paramBlanksLabel->show();
            paramBlanksSbox->show();
        }
        else
        {
            paramBlanksLabel->hide();
            paramBlanksSbox->hide();
        }

        if ((type == SearchCondition::ProbabilityOrder) ||
            (type == SearchCondition::LimitByProbabilityOrder) ||
            (type == SearchCondition::PlayabilityOrder) ||
            (type == SearchCondition::LimitByPlayabilityOrder))
        {
            paramProbCbox->show();
        }
        else {
            paramProbCbox->hide();
        }

        paramStack->setCurrentWidget(paramSboxWidget);
        break;

        case SearchCondition::Probability:
        negationCbox->setCheckState(Qt::Unchecked);
        negationCbox->setEnabled(false);
        paramMinSbox->setMaximum(100);
        paramMinSbox->setValue(0);
        paramMaxSbox->setMaximum(100);
        paramMaxSbox->setValue(100);
        paramStack->setCurrentWidget(paramSboxWidget);
        break;

        case SearchCondition::BelongToGroup:
        negationCbox->setEnabled(true);
        paramCbox->clear();
        paramCbox->addItem(Auxil::searchSetToString(SetHookWords));
        paramCbox->addItem(Auxil::searchSetToString(SetFrontHooks));
        paramCbox->addItem(Auxil::searchSetToString(SetBackHooks));
        paramCbox->addItem(Auxil::searchSetToString(SetHighFives));
        paramCbox->addItem(Auxil::searchSetToString(SetTypeOneSevens));
        paramCbox->addItem(Auxil::searchSetToString(SetTypeTwoSevens));
        paramCbox->addItem(Auxil::searchSetToString(SetTypeThreeSevens));
        paramCbox->addItem(Auxil::searchSetToString(SetTypeOneEights));
        paramCbox->addItem(Auxil::searchSetToString(SetTypeTwoEights));
        paramCbox->addItem(Auxil::searchSetToString(SetTypeThreeEights));
        paramCbox->addItem(
            Auxil::searchSetToString(SetEightsFromSevenLetterStems));
        paramStack->setCurrentWidget(paramCboxWidget);
        break;

        case SearchCondition::InLexicon:
        negationCbox->setEnabled(true);
        paramCbox->clear();
        paramCbox->addItems(MainSettings::getAutoImportLexicons());
        paramStack->setCurrentWidget(paramCboxWidget);
        break;

        case SearchCondition::ConsistOf:
        negationCbox->setCheckState(Qt::Unchecked);
        negationCbox->setEnabled(false);
        paramConsistMinSbox->setValue(0);
        paramConsistMaxSbox->setValue(100);
        paramStack->setCurrentWidget(paramConsistWidget);
        break;

        case SearchCondition::InWordList:
        negationCbox->setEnabled(true);
        setWordListString(QString());
        paramStack->setCurrentWidget(paramWordListWidget);
        break;

        default:
        //qWarning("Unrecognized search condition: " + string);
        break;
    }

    legacy = false;
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
    WordListDialog* dialog = new WordListDialog(this);
    Q_CHECK_PTR(dialog);

    dialog->setWords(paramWordListString);

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        setWordListString(dialog->getWords());
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
    paramLine->setText(QString());
    paramMinSbox->setValue(0);
    paramMaxSbox->setValue(MAX_MAX_INT_VALUE);
    paramCbox->setCurrentIndex(0);
    paramConsistMinSbox->setValue(0);
    paramConsistMaxSbox->setValue(100);
    paramConsistLine->setText(QString());
    typeCbox->setCurrentIndex(0);
    typeChanged(typeCbox->currentText());
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
        if (MainSettings::getSearchSelectInput()) {
            paramLine->setSelection(0, paramLine->text().length());
        }
    }
    else if (currentWidget == paramSboxWidget) {
        paramMinSbox->setFocus();
    }
    else if (currentWidget == paramCboxWidget) {
        paramCbox->setFocus();
    }
    else if (currentWidget == paramConsistWidget) {
        paramConsistLine->setFocus();
        if (MainSettings::getSearchSelectInput()) {
            paramConsistLine->setSelection(0,
                paramConsistLine->text().length());
        }
    }
}

//---------------------------------------------------------------------------
//  setAddEnabled
//
//! Enable or disable the add button.
//
//! @param enable true to enable the button, false to disable it
//---------------------------------------------------------------------------
void
SearchConditionForm::setAddEnabled(bool enable)
{
    addButton->setEnabled(enable);
}

//---------------------------------------------------------------------------
//  setDeleteEnabled
//
//! Enable or disable the delete button.
//
//! @param enable true to enable the button, false to disable it
//---------------------------------------------------------------------------
void
SearchConditionForm::setDeleteEnabled(bool enable)
{
    deleteButton->setEnabled(enable);
}

//---------------------------------------------------------------------------
//  setWordListString
//
//! Set the value of the word list string, and update the word list line to
//! reflect the number of words in the list.
//---------------------------------------------------------------------------
void
SearchConditionForm::setWordListString(const QString& string)
{
    paramWordListString = string;
    int numWords = 0;
    if (!string.isEmpty()) {
        QStringList wordList = string.split(QChar(' '));
        numWords = wordList.size();
    }
    paramWordListLine->setText(QString::number(numWords) + " word" +
                               (numWords == 1 ? QString() : QString("s")));
    paramWordListLine->home(false);
}

//---------------------------------------------------------------------------
//  matchStringIsValid
//
//! Determine whether a string is a valid match string.
//
//! @return true if valid, false otherwise
//---------------------------------------------------------------------------
bool
SearchConditionForm::matchStringIsValid(const QString& string) const
{
    if (string.isEmpty())
        return false;

    bool inGroup = false;
    bool groupLetters = false;
    bool negatedGroup = false;
    int len = string.length();
    for (int i = 0; i < len; ++i) {
        QChar c = string.at(i);

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
            else if ((c == '[') || (c == '*') || (c == '?') ||
                (c == '.') || (c == '@'))
            {
                return false;
            }
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
