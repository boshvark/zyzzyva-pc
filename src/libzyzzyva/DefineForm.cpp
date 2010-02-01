//---------------------------------------------------------------------------
// DefineForm.cpp
//
// A form for looking up and displaying word definitions.
//
// Copyright 2004-2010 Michael W Thelen <mthelen@gmail.com>.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "DefineForm.h"
#include "DefinitionBox.h"
#include "LexiconSelectWidget.h"
#include "MainSettings.h"
#include "WordEngine.h"
#include "WordLineEdit.h"
#include "WordValidator.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

using namespace Defs;

const QString TITLE_PREFIX = "Definition";
const QString NONE_STR = "(none)";

//---------------------------------------------------------------------------
//  DefineForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
DefineForm::DefineForm(WordEngine* e, QWidget* parent, Qt::WFlags f)
    : ActionForm(DefineFormType, parent, f), engine(e)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);
    Q_CHECK_PTR(mainVlay);

    QHBoxLayout* lexiconHlay = new QHBoxLayout;
    Q_CHECK_PTR(lexiconHlay);
    lexiconHlay->setSpacing(SPACING);
    mainVlay->addLayout(lexiconHlay);

    lexiconHlay->addStretch(1);

    lexiconWidget = new LexiconSelectWidget;
    Q_CHECK_PTR(lexiconWidget);
    lexiconHlay->addWidget(lexiconWidget);

    lexiconHlay->addStretch(1);

    QHBoxLayout* lookupHlay = new QHBoxLayout;
    lookupHlay->setSpacing(SPACING);
    Q_CHECK_PTR(lookupHlay);
    mainVlay->addLayout(lookupHlay);

    QLabel* label = new QLabel("Word : ");
    Q_CHECK_PTR(label);
    lookupHlay->addWidget(label);

    wordLine = new WordLineEdit;
    Q_CHECK_PTR(wordLine);
    wordLine->setValidator(new WordValidator(wordLine));
    connect(wordLine, SIGNAL(textChanged(const QString&)),
            SLOT(wordChanged(const QString&)));
    connect(wordLine, SIGNAL(returnPressed()), SLOT(displayDefinition()));
    lookupHlay->addWidget(wordLine);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing(SPACING);
    Q_CHECK_PTR(buttonHlay);
    mainVlay->addLayout(buttonHlay);

    buttonHlay->addStretch(1);

    defineButton = new ZPushButton("&Define");
    Q_CHECK_PTR(defineButton);
    defineButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(defineButton, SIGNAL(clicked()), SLOT(displayDefinition()));
    buttonHlay->addWidget(defineButton);

    infoButton = new ZPushButton("Get All &Info");
    Q_CHECK_PTR(infoButton);
    infoButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(infoButton, SIGNAL(clicked()), SLOT(displayAllInfo()));
    buttonHlay->addWidget(infoButton);

    buttonHlay->addStretch(1);

    resultBox = new DefinitionBox;
    Q_CHECK_PTR(resultBox);
    mainVlay->addWidget(resultBox, 1);

    mainVlay->addStretch(0);

    defineButton->setEnabled(false);
    infoButton->setEnabled(false);
    resultBox->hide();
}

//---------------------------------------------------------------------------
//  getIcon
//
//! Returns the current icon.
//
//! @return the current icon
//---------------------------------------------------------------------------
QIcon
DefineForm::getIcon() const
{
    return QIcon(":/define-icon");
}

//---------------------------------------------------------------------------
//  getTitle
//
//! Returns the current title string.
//
//! @return the current title string
//---------------------------------------------------------------------------
QString
DefineForm::getTitle() const
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
DefineForm::getStatusString() const
{
    return QString();
}

//---------------------------------------------------------------------------
//  selectInputArea
//
//! Give focus to the input area and select its contents.
//---------------------------------------------------------------------------
void
DefineForm::selectInputArea()
{
    wordLine->setFocus();
    wordLine->setSelection(0, wordLine->text().length());
}

//---------------------------------------------------------------------------
//  wordChanged
//
//! Called when the contents of the input line change.  Enables or disables
//! the buttons appropriately.
//---------------------------------------------------------------------------
void
DefineForm::wordChanged(const QString& word)
{
    defineButton->setEnabled(!word.isEmpty());
    infoButton->setEnabled(!word.isEmpty());
}

//---------------------------------------------------------------------------
//  displayDefinition
//
//! Display only the word definition, without any other information.
//---------------------------------------------------------------------------
void
DefineForm::displayDefinition()
{
    displayInfo(false);
}

//---------------------------------------------------------------------------
//  displayAllInfo
//
//! Display all information about a word.
//---------------------------------------------------------------------------
void
DefineForm::displayAllInfo()
{
    displayInfo(true);
}

//---------------------------------------------------------------------------
//  displayInfo
//
//! Look up and display the acceptability and definition of the word currently
//! in the word edit area.
//
//! @param allInfo whether to display all information in addition to the
//! definition
//---------------------------------------------------------------------------
void
DefineForm::displayInfo(bool allInfo)
{
    QString word = wordLine->text();
    if (word.isEmpty())
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString lexicon = lexiconWidget->getCurrentLexicon();
    bool showSymbols = MainSettings::getWordListUseLexiconStyles();

    bool acceptable = engine->isAcceptable(lexicon, word);
    QString resultStr = acceptable ?
                        QString("<font color=\"blue\">Acceptable</font>") :
                        QString("<font color=\"red\">Unacceptable</font>");

    // FIXME: if the word is acceptable, get as much information as possible
    // from WordEngine::getWordInfo, not getDefinition, etc. separately

    // Get definition
    QString definition;
    if (acceptable) {
        resultStr += "<br>";
        if (allInfo)
            resultStr += "<b>Definition:</b> ";
        definition = engine->getDefinition(lexicon, word);
        if (definition.isEmpty())
            definition = EMPTY_DEFINITION;
        resultStr += definition;
    }

    if (allInfo) {
        SearchSpec spec;
        SearchCondition condition;

        // Get anagrams
        condition.type = SearchCondition::AnagramMatch;
        condition.stringValue = word;
        spec.conditions.append(condition);
        QStringList anagrams = engine->search(lexicon, spec, true);
        anagrams.removeAll(word);
        if (showSymbols) {
            QMutableListIterator<QString> it (anagrams);
            while (it.hasNext()) {
                QString& anagram = it.next();
                anagram.append(engine->getLexiconSymbols(lexicon, anagram));
            }
        }
        QString anagramsStr = anagrams.isEmpty() ? NONE_STR :
            QString("(%1): ").arg(anagrams.count()) + anagrams.join(", ");
        resultStr += "<br><b>Anagrams:</b> " + anagramsStr;

        // Get front hooks
        QString fHooks = engine->getFrontHookLetters(lexicon, word).toUpper();
        if (!showSymbols)
            fHooks.replace(QRegExp("[\\W_\\d]+"), QString());
        QString fHookStr = fHooks.isEmpty() ? NONE_STR :
            QString("(%1): ").arg(fHooks.count(QRegExp("[^\\W_\\d]"))) + fHooks;
        resultStr += "<br><b>Front Hooks:</b> " + fHookStr;

        // Get back hooks
        QString bHooks = engine->getBackHookLetters(lexicon, word).toUpper();
        if (!showSymbols)
            bHooks.replace(QRegExp("[\\W_\\d]+"), QString());
        QString bHookStr = bHooks.isEmpty() ? NONE_STR :
            QString("(%1): ").arg(bHooks.count(QRegExp("[^\\W_\\d]"))) + bHooks;
        resultStr += "<br><b>Back Hooks:</b> " + bHookStr;

        // Get front extensions
        spec.conditions.clear();
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "*?" + word;
        spec.conditions.append(condition);
        QStringList fExts = engine->search(lexicon, spec, true);
        if (showSymbols) {
            QMutableListIterator<QString> it (fExts);
            while (it.hasNext()) {
                QString& fExt = it.next();
                fExt.append(engine->getLexiconSymbols(lexicon, fExt));
            }
        }
        QString fExtStr = fExts.isEmpty() ? NONE_STR :
            QString("(%1): ").arg(fExts.count()) +
            fExts.replaceInStrings(
                QRegExp(word + "([\\W_\\d]*)$"), "-\\1").join(", ");
        resultStr += "<br><b>Front Extensions:</b> " + fExtStr;

        // Get back extensions
        spec.conditions.clear();
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = word + "?*";
        spec.conditions.append(condition);
        QStringList bExts = engine->search(lexicon, spec, true);
        if (showSymbols) {
            QMutableListIterator<QString> it (bExts);
            while (it.hasNext()) {
                QString& bExt = it.next();
                bExt.append(engine->getLexiconSymbols(lexicon, bExt));
            }
        }
        QString bExtStr = bExts.isEmpty() ? NONE_STR :
            QString("(%1): ").arg(bExts.count()) +
            bExts.replaceInStrings(QRegExp("^" + word), "-").join(", ");
        resultStr += "<br><b>Back Extensions:</b> " + bExtStr;

        // Get double extensions
        spec.conditions.clear();
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "*?" + word + "?*";
        spec.conditions.append(condition);
        QStringList dExts = engine->search(lexicon, spec, true);
        if (showSymbols) {
            QMutableListIterator<QString> it (dExts);
            while (it.hasNext()) {
                QString& dExt = it.next();
                dExt.append(engine->getLexiconSymbols(lexicon, dExt));
            }
        }
        QString dExtStr = dExts.isEmpty() ? NONE_STR :
            QString("(%1): ").arg(dExts.count()) +
            dExts.replaceInStrings(QRegExp(word), "-").join(", ");
        resultStr += "<br><b>Double Extensions:</b> " + dExtStr;
    }

    resultBox->setText(resultStr);

    if (showSymbols)
        word += engine->getLexiconSymbols(lexicon, word);

    resultBox->setTitle(word);
    resultBox->show();
    selectInputArea();

    QApplication::restoreOverrideCursor();
}
