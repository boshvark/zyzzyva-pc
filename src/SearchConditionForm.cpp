//---------------------------------------------------------------------------
// SearchConditionForm.cpp
//
// A form for specifying a search condition.
//
// Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
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
#include "WordValidator.h"
#include "Auxil.h"
#include "Defs.h"
#include <qlabel.h>
#include <qlayout.h>

using namespace Defs;

const QString PATTERN_MATCH_TYPE = "Pattern Match";
const QString ANAGRAM_MATCH_TYPE = "Anagram Match";
const QString SUBANAGRAM_MATCH_TYPE = "Subanagram Match";
const QString MIN_LENGTH_TYPE = "Minimum Length";
const QString MAX_LENGTH_TYPE = "Maximum Length";
const QString MUST_INCLUDE_TYPE = "Must Include";
const QString MUST_EXCLUDE_TYPE = "Must Exclude";
const QString MUST_CONSIST_TYPE = "Must Consist of";
const QString MUST_BELONG_TYPE = "Must Belong to";
const QString MIN_ANAGRAMS_TYPE = "Minimum Anagrams";
const QString MAX_ANAGRAMS_TYPE = "Maximum Anagrams";
const QString MIN_PROBABILITY_TYPE = "Minimum Probability";
const QString MAX_PROBABILITY_TYPE = "Maximum Probability";

//---------------------------------------------------------------------------
//  SearchConditionForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
SearchConditionForm::SearchConditionForm (QWidget* parent, const char* name,
                                          WFlags f)
    : QFrame (parent, name, f)
{
    WordValidator* letterValidator = new WordValidator (this,
                                                        "letterValidator");
    Q_CHECK_PTR (letterValidator);

    WordValidator* patternValidator = new WordValidator (this,
                                                         "patternValidator");
    Q_CHECK_PTR (patternValidator);
    patternValidator->setOptions (WordValidator::AllowQuestionMarks |
                                  WordValidator::AllowAsterisks);

    QHBoxLayout* mainHlay = new QHBoxLayout (this, 0, SPACING, "mainHlay");
    Q_CHECK_PTR (mainHlay);

    QStringList types;
    types << PATTERN_MATCH_TYPE << ANAGRAM_MATCH_TYPE << SUBANAGRAM_MATCH_TYPE
        << MIN_LENGTH_TYPE << MAX_LENGTH_TYPE << MUST_INCLUDE_TYPE <<
        MUST_EXCLUDE_TYPE << MUST_CONSIST_TYPE << MUST_BELONG_TYPE <<
        MIN_ANAGRAMS_TYPE << MAX_ANAGRAMS_TYPE << MIN_PROBABILITY_TYPE <<
        MAX_PROBABILITY_TYPE;
    typeCbox = new QComboBox (this, "typeCbox");
    Q_CHECK_PTR (typeCbox);
    typeCbox->insertStringList (types);
    connect (typeCbox, SIGNAL (activated (const QString&)),
             SLOT (typeChanged (const QString&)));
    mainHlay->addWidget (typeCbox);

    paramStack = new QWidgetStack (this, "paramStack");
    Q_CHECK_PTR (paramStack);
    mainHlay->addWidget (paramStack);

    // Frame containing just an input line
    paramLineFrame = new QFrame (paramStack, "paramLineFrame");
    Q_CHECK_PTR (paramLineFrame);
    QHBoxLayout* paramLineHlay = new QHBoxLayout (paramLineFrame, 0, SPACING,
                                                  "paramLineHlay");
    Q_CHECK_PTR (paramLineHlay);
    paramLine = new QLineEdit (paramLineFrame, "paramLine");
    Q_CHECK_PTR (paramLine);
    paramLine->setValidator (patternValidator);
    paramLineHlay->addWidget (paramLine);
    paramStack->addWidget (paramLineFrame);

    // Frame containing just a spin box
    paramSboxFrame = new QFrame (paramStack, "paramSboxFrame");
    Q_CHECK_PTR (paramSboxFrame);
    QHBoxLayout* paramSboxHlay = new QHBoxLayout (paramSboxFrame, 0, SPACING,
                                                  "paramSboxHlay");
    Q_CHECK_PTR (paramSboxHlay);
    paramSbox = new QSpinBox (paramSboxFrame, "paramSbox");
    Q_CHECK_PTR (paramSbox);
    paramSboxHlay->addWidget (paramSbox);
    paramStack->addWidget (paramSboxFrame);

    // Frame containing just a combo box
    paramCboxFrame = new QFrame (paramStack, "paramCboxFrame");
    Q_CHECK_PTR (paramCboxFrame);
    QHBoxLayout* paramCboxHlay = new QHBoxLayout (paramCboxFrame, 0, SPACING,
                                                  "paramCboxHlay");
    Q_CHECK_PTR (paramCboxHlay);
    paramCbox = new QComboBox (paramCboxFrame, "paramCbox");
    Q_CHECK_PTR (paramCbox);
    paramCboxHlay->addWidget (paramCbox);
    paramStack->addWidget (paramCboxFrame);

    // Frame containing spin box and input line
    paramConsistFrame = new QFrame (paramStack, "paramConsistFrame");
    Q_CHECK_PTR (paramConsistFrame);
    QHBoxLayout* paramConsistHlay = new QHBoxLayout (paramConsistFrame, 0,
                                                     SPACING,
                                                     "paramConsistHlay");
    Q_CHECK_PTR (paramConsistHlay);
    paramConsistSbox = new QSpinBox (paramConsistFrame, "paramConsist");
    Q_CHECK_PTR (paramConsistSbox);
    paramConsistHlay->addWidget (paramConsistSbox);
    paramConsistLine = new QLineEdit (paramConsistFrame, "paramConsistLine");
    Q_CHECK_PTR (paramConsistLine);
    paramConsistHlay->addWidget (paramConsistLine);
    paramStack->addWidget (paramConsistFrame);
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
    condition.type = stringToSearchType (typeStr);

    switch (condition.type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::MustInclude:
        case SearchCondition::MustExclude:
        condition.stringValue = paramLine->text();
        break;

        case SearchCondition::MinLength:
        case SearchCondition::MaxLength:
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

        default:
        qWarning ("Unrecognized search condition: " + typeStr);
        break;
    }

    return condition;
}


//---------------------------------------------------------------------------
//  typeChanged
//
//! Called when an option in the type dropdown box is activated.
//
//! @param type the activated option
//---------------------------------------------------------------------------
void
SearchConditionForm::typeChanged (const QString& type)
{
    if ((type == PATTERN_MATCH_TYPE) || (type == ANAGRAM_MATCH_TYPE) ||
        (type == SUBANAGRAM_MATCH_TYPE) || (type == MUST_INCLUDE_TYPE) ||
        (type == MUST_EXCLUDE_TYPE))
    {
        paramStack->raiseWidget (paramLineFrame);
    }

    else if ((type == MIN_LENGTH_TYPE) || (type == MAX_LENGTH_TYPE) ||
             (type == MIN_ANAGRAMS_TYPE) || (type == MAX_ANAGRAMS_TYPE) ||
             (type == MIN_PROBABILITY_TYPE) || (type == MAX_PROBABILITY_TYPE))
    {
        paramStack->raiseWidget (paramSboxFrame);
    }

    else if (type == MUST_BELONG_TYPE) {
        paramCbox->clear();
        paramCbox->insertItem (Auxil::searchSetToString (SetHookWords));
        paramCbox->insertItem (Auxil::searchSetToString (SetTypeOneSevens));
        paramCbox->insertItem (Auxil::searchSetToString (SetTypeTwoSevens));
        paramCbox->insertItem (Auxil::searchSetToString (SetTypeThreeSevens));
        paramCbox->insertItem (Auxil::searchSetToString (SetTypeOneEights));
        paramCbox->insertItem (Auxil::searchSetToString
                               (SetEightsFromSevenLetterStems));
        paramStack->raiseWidget (paramCboxFrame);
    }

    else if (type == MUST_CONSIST_TYPE) {
        paramStack->raiseWidget (paramConsistFrame);
    }

    else {
        qDebug ("Unrecognized type: |" + type + "|");
    }
}

//---------------------------------------------------------------------------
//  stringToSearchType
//
//! Return a search type enumeration value corresponding to a string.
//
//! @param string the string
//! @return the corresponding search type value
//---------------------------------------------------------------------------
SearchCondition::SearchType
SearchConditionForm::stringToSearchType (const QString& string) const
{
    if (string == PATTERN_MATCH_TYPE)
        return SearchCondition::PatternMatch;
    else if (string == ANAGRAM_MATCH_TYPE)
        return SearchCondition::AnagramMatch;
    else if (string == SUBANAGRAM_MATCH_TYPE)
        return SearchCondition::SubanagramMatch;
    else if (string == MIN_LENGTH_TYPE)
        return SearchCondition::MinLength;
    else if (string == MAX_LENGTH_TYPE)
        return SearchCondition::MaxLength;
    else if (string == MUST_INCLUDE_TYPE)
        return SearchCondition::MustInclude;
    else if (string == MUST_EXCLUDE_TYPE)
        return SearchCondition::MustExclude;
    else if (string == MUST_CONSIST_TYPE)
        return SearchCondition::MustConsist;
    else if (string == MUST_BELONG_TYPE)
        return SearchCondition::MustBelong;
    else if (string == MIN_ANAGRAMS_TYPE)
        return SearchCondition::MinAnagrams;
    else if (string == MAX_ANAGRAMS_TYPE)
        return SearchCondition::MaxAnagrams;
    else if (string == MIN_PROBABILITY_TYPE)
        return SearchCondition::MinProbability;
    else if (string == MAX_PROBABILITY_TYPE)
        return SearchCondition::MaxProbability;
    return SearchCondition::UnknownSearchType;
}
