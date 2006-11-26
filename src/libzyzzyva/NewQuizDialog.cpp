//---------------------------------------------------------------------------
// NewQuizDialog.cpp
//
// A dialog for prompting the user for a quiz.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mthelen@gmail.com>.
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

#include "NewQuizDialog.h"
#include "QuizSpec.h"
#include "SearchSpec.h"
#include "SearchSpecForm.h"
#include "WordEngine.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>

#include <QtDebug>

const QString DIALOG_CAPTION = "New Quiz";
const QString TIMER_PER_QUESTION = "per question";
const QString TIMER_PER_RESPONSE = "per response";

using namespace Defs;

//---------------------------------------------------------------------------
//  NewQuizDialog
//
//! Constructor.
//
//! @param e a word engine
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
NewQuizDialog::NewQuizDialog (WordEngine* e, QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f), wordEngine (e)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this);
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* typeHlay = new QHBoxLayout;
    Q_CHECK_PTR (typeHlay);
    mainVlay->addLayout (typeHlay);

    QLabel* typeLabel = new QLabel ("Quiz Type:");
    Q_CHECK_PTR (typeLabel);
    typeHlay->addWidget (typeLabel);

    typeCombo = new QComboBox;
    typeCombo->addItem (Auxil::quizTypeToString (QuizSpec::QuizAnagrams));
    typeCombo->addItem (Auxil::quizTypeToString
                        (QuizSpec::QuizAnagramsWithHooks));
    typeCombo->addItem (Auxil::quizTypeToString
                        (QuizSpec::QuizWordListRecall));
    typeCombo->addItem (Auxil::quizTypeToString (QuizSpec::QuizHooks));
    typeCombo->setCurrentIndex (typeCombo->findText
        (Auxil::quizTypeToString (QuizSpec::QuizAnagrams)));
    connect (typeCombo, SIGNAL (activated (const QString&)),
             SLOT (typeActivated (const QString&)));
    typeHlay->addWidget (typeCombo);

    QHBoxLayout* methodHlay = new QHBoxLayout;
    Q_CHECK_PTR (methodHlay);
    mainVlay->addLayout (methodHlay);

    QLabel* methodLabel = new QLabel ("Quiz Method:");
    Q_CHECK_PTR (methodLabel);
    methodHlay->addWidget (methodLabel);

    methodCombo = new QComboBox;
    methodCombo->addItem (Auxil::quizMethodToString
                          (QuizSpec::StandardQuizMethod));
    methodCombo->addItem (Auxil::quizMethodToString
                          (QuizSpec::CardboxQuizMethod));
    methodCombo->setCurrentIndex (methodCombo->findText
        (Auxil::quizMethodToString (QuizSpec::StandardQuizMethod)));
    connect (methodCombo, SIGNAL (activated (const QString&)),
             SLOT (methodActivated (const QString&)));
    methodHlay->addWidget (methodCombo);

    QHBoxLayout* questionOrderHlay = new QHBoxLayout;
    Q_CHECK_PTR (questionOrderHlay);
    mainVlay->addLayout (questionOrderHlay);

    QLabel* questionOrderLabel = new QLabel ("Question Order:");
    Q_CHECK_PTR (questionOrderLabel);
    questionOrderHlay->addWidget (questionOrderLabel);

    questionOrderCombo = new QComboBox;
    fillQuestionOrderCombo (methodCombo->currentText());
    questionOrderCombo->setCurrentIndex (questionOrderCombo->findText
        (Auxil::quizQuestionOrderToString (QuizSpec::RandomOrder)));
    connect (questionOrderCombo, SIGNAL (activated (const QString&)),
             SLOT (questionOrderActivated (const QString&)));
    questionOrderHlay->addWidget (questionOrderCombo);

    QGroupBox* specGbox = new QGroupBox ("Search Specification");
    Q_CHECK_PTR (specGbox);
    mainVlay->addWidget (specGbox);

    QHBoxLayout* specHlay = new QHBoxLayout (specGbox);
    Q_CHECK_PTR (specHlay);

    specForm = new SearchSpecForm;
    Q_CHECK_PTR (specForm);
    connect (specForm, SIGNAL (contentsChanged()),
             SLOT (searchContentsChanged()));
    connect (specForm, SIGNAL (returnPressed()), SLOT (accept()));
    specHlay->addWidget (specForm);

    progressCbox = new QCheckBox ("Restore &progress");
    Q_CHECK_PTR (progressCbox);
    progressCbox->setEnabled (false);
    mainVlay->addWidget (progressCbox);

    QHBoxLayout* timerHlay = new QHBoxLayout;
    Q_CHECK_PTR (timerHlay);
    mainVlay->addLayout (timerHlay);

    timerCbox = new QCheckBox ("&Timer:");
    Q_CHECK_PTR (timerCbox);
    connect (timerCbox, SIGNAL (toggled (bool)), SLOT (timerToggled (bool)));
    timerHlay->addWidget (timerCbox);

    timerWidget = new QWidget;
    Q_CHECK_PTR (timerWidget);
    timerWidget->setEnabled (false);
    timerHlay->addWidget (timerWidget);

    QHBoxLayout* timerWidgetHlay = new QHBoxLayout (timerWidget);
    Q_CHECK_PTR (timerWidgetHlay);
    timerWidgetHlay->setMargin (0);

    timerSbox = new QSpinBox;
    Q_CHECK_PTR (timerSbox);
    timerSbox->setMinimum (1);
    timerSbox->setMaximum (9999);
    timerSbox->setValue (10);
    timerWidgetHlay->addWidget (timerSbox);

    QLabel* timerLabel = new QLabel ("seconds");
    Q_CHECK_PTR (timerLabel);
    timerWidgetHlay->addWidget (timerLabel);

    timerCombo = new QComboBox;
    timerCombo->addItem (TIMER_PER_QUESTION);
    timerCombo->addItem (TIMER_PER_RESPONSE);
    timerCombo->setCurrentIndex (timerCombo->findText (TIMER_PER_RESPONSE));
    timerWidgetHlay->addWidget (timerCombo);

    // OK/Cancel buttons
    QHBoxLayout* buttonHlay = new QHBoxLayout;
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    ZPushButton* loadQuizButton = new ZPushButton ("&Load Quiz...");
    Q_CHECK_PTR (loadQuizButton);
    connect (loadQuizButton, SIGNAL (clicked()), SLOT (loadQuiz()));
    buttonHlay->addWidget (loadQuizButton);

    saveQuizButton = new ZPushButton ("&Save Quiz...");
    Q_CHECK_PTR (saveQuizButton);
    connect (saveQuizButton, SIGNAL (clicked()), SLOT (saveQuiz()));
    buttonHlay->addWidget (saveQuizButton);

    buttonHlay->addStretch (1);

    okButton = new ZPushButton ("OK");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    ZPushButton* cancelButton = new ZPushButton ("Cancel");
    Q_CHECK_PTR (cancelButton);
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    setWindowTitle (DIALOG_CAPTION);
    specForm->selectInputArea();
}

//---------------------------------------------------------------------------
//  getQuizSpec
//
//! Get a QuizSpec corresponding to the state of the form.
//
//! @return the quiz spec
//---------------------------------------------------------------------------
QuizSpec
NewQuizDialog::getQuizSpec()
{
    QuizSpec::QuizMethod quizMethod =
        Auxil::stringToQuizMethod (methodCombo->currentText());

    quizSpec.setLexicon (wordEngine->getLexiconName());
    quizSpec.setType (Auxil::stringToQuizType (typeCombo->currentText()));
    quizSpec.setMethod (quizMethod);
    quizSpec.setSearchSpec (specForm->getSearchSpec());
    quizSpec.setQuestionOrder (Auxil::stringToQuizQuestionOrder
                               (questionOrderCombo->currentText()));

    QuizTimerSpec timerSpec;
    if (timerCbox->isChecked()) {
        timerSpec.setDuration (timerSbox->value());
        QString timerType = timerCombo->currentText();
        if (timerType == TIMER_PER_QUESTION)
            timerSpec.setType (PerQuestion);
        else if (timerType == TIMER_PER_RESPONSE)
            timerSpec.setType (PerResponse);
    }
    quizSpec.setTimerSpec (timerSpec);

    if (!progressCbox->isChecked() &&
        (quizMethod != QuizSpec::CardboxQuizMethod))
    {
        quizSpec.setProgress (QuizProgress());
        quizSpec.setFilename (QString::null);
    }

    return quizSpec;
}

//---------------------------------------------------------------------------
//  setQuizSpec
//
//! Set the contents of the dialog form based on the contents of a quiz spec.
//
//! @param spec the quiz spec
//---------------------------------------------------------------------------
void
NewQuizDialog::setQuizSpec (const QuizSpec& spec)
{
    // Set method before type, because type may end up changing the method
    methodCombo->setCurrentIndex
        (methodCombo->findText (Auxil::quizMethodToString (spec.getMethod())));
    methodActivated (methodCombo->currentText());
    typeCombo->setCurrentIndex
        (typeCombo->findText (Auxil::quizTypeToString (spec.getType())));
    typeActivated (typeCombo->currentText());
    specForm->setSearchSpec (spec.getSearchSpec());
    questionOrderCombo->setCurrentIndex
        (questionOrderCombo->findText (Auxil::quizQuestionOrderToString
                                       (spec.getQuestionOrder())));
    timerCbox->setChecked (false);
    timerSbox->setValue (0);
    timerCombo->setCurrentIndex (timerCombo->findText (TIMER_PER_RESPONSE));
    QuizTimerSpec timerSpec = spec.getTimerSpec();
    if (timerSpec.getType() != NoTimer) {
        timerCbox->setChecked (true);
        timerSbox->setValue (timerSpec.getDuration());
        switch (timerSpec.getType()) {
            case PerQuestion:
            timerCombo->setCurrentIndex (timerCombo->findText
                                         (TIMER_PER_QUESTION));
            break;
            case PerResponse:
            timerCombo->setCurrentIndex (timerCombo->findText
                                         (TIMER_PER_RESPONSE));
            break;
            default: break;
        }
    }
    if ((spec.getMethod() == QuizSpec::StandardQuizMethod) &&
        !spec.getProgress().isEmpty())
    {
        progressCbox->setEnabled (true);
        progressCbox->setChecked (true);
    }
    quizSpec = spec;
}

//---------------------------------------------------------------------------
//  typeActivated
//
//! Called when the contents of the Quiz Type combo box are changed.  Disable
//! the Random checkbox if the Word List Recall type is selected.  Also change
//! the method to Standard if the Word List Recall type is selected.
//
//! @param text the text in the combo box
//---------------------------------------------------------------------------
void
NewQuizDialog::typeActivated (const QString& text)
{
    QuizSpec::QuizType type = Auxil::stringToQuizType (text);
    if (type == QuizSpec::QuizWordListRecall) {
        methodCombo->setEnabled (false);
        methodCombo->setCurrentIndex (methodCombo->findText
            (Auxil::quizMethodToString (QuizSpec::StandardQuizMethod)));
    }
    else {
        methodCombo->setEnabled (true);
    }
    updateForm();
    disableProgress();
    clearFilename();
}

//---------------------------------------------------------------------------
//  methodActivated
//
//! Called when the contents of the Quiz Method combo box are changed.
//! Disable the Restore Progress and Random checkboxes if the Cardbox method
//! is selected.
//
//! @param text the text in the combo box
//---------------------------------------------------------------------------
void
NewQuizDialog::methodActivated (const QString& text)
{
    fillQuestionOrderCombo (text);
    updateForm();
    disableProgress();
    clearFilename();
}

//---------------------------------------------------------------------------
//  timerToggled
//
//! Called when the Timer checkbox is toggled.  Disable the timer
//! configuration unless the Timer checkbox is checked.
//
//! @param on whether the checkbox is checked
//---------------------------------------------------------------------------
void
NewQuizDialog::timerToggled (bool on)
{
    timerWidget->setEnabled (on);
}

//---------------------------------------------------------------------------
//  searchContentsChanged
//
//! Called when the contents of the search form change.
//---------------------------------------------------------------------------
void
NewQuizDialog::searchContentsChanged()
{
    disableProgress();
    clearFilename();
}

//---------------------------------------------------------------------------
//  questionOrderActivated
//
//! Called when the contents of the Question Order combo box are changed.
//
//! @param text the new contents of the combo box
//---------------------------------------------------------------------------
void
NewQuizDialog::questionOrderActivated (const QString&)
{
    disableProgress();
    clearFilename();
}

//---------------------------------------------------------------------------
//  loadQuiz
//
//! Load a quiz spec from a file.
//---------------------------------------------------------------------------
void
NewQuizDialog::loadQuiz()
{
    QString filename = QFileDialog::getOpenFileName (this, "Load Quiz",
        Auxil::getQuizDir() + "/saved", "Zyzzyva Quiz Files (*.zzq)");

    if (filename.isEmpty())
        return;

    QFile file (filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        QString caption = "Error Opening Quiz File";
        QString message = "Cannot open file '" + filename + "': " +
            file.errorString();
        message = Auxil::dialogWordWrap (message);
        QMessageBox::warning (this, caption, message);
        return;
    }

    QString errorMsg;
    QuizSpec spec;
    QApplication::setOverrideCursor (QCursor (Qt::WaitCursor));
    bool ok = spec.fromXmlFile (file, &errorMsg);
    QApplication::restoreOverrideCursor();

    if (!ok) {
        QString caption = "Error in Quiz File";
        QString message = "Error in quiz file.\n" + errorMsg;
        message = Auxil::dialogWordWrap (message);
        QMessageBox::warning (this, caption, message);
        return;
    }

    QString quizLexicon = spec.getLexicon();
    QString currentLexicon = wordEngine->getLexiconName();

    if (quizLexicon != currentLexicon) {
        if (quizLexicon.isEmpty())
            quizLexicon = "unspecified";

        QString caption = "Lexicon Mismatch";
        QString message = "The lexicon associated with the quiz is "
                          + quizLexicon + ", but the current lexicon is "
                          + currentLexicon + ".  "
                          "If a different lexicon is used, the quiz may "
                          "encounter problems.\n\n"
                          "Are you sure you want to proceed?";
        message = Auxil::dialogWordWrap (message);
        int code = QMessageBox::warning (this, caption, message,
                                         QMessageBox::Yes, QMessageBox::No);

        if (code != QMessageBox::Yes)
            return;

        spec.setLexicon (currentLexicon);
    }

    setQuizSpec (spec);
}

//---------------------------------------------------------------------------
//  saveQuiz
//
//! Save the current quiz spec to a file.
//---------------------------------------------------------------------------
void
NewQuizDialog::saveQuiz()
{
    // Try saving in the same location as the spec's current filename
    QString startDir = Auxil::getQuizDir() + "/saved";
    QString currentFilename = quizSpec.getFilename();
    if (!currentFilename.isEmpty()) {
        currentFilename.remove ("/[^/]+$");
        startDir = currentFilename;
    }

    QString filename = QFileDialog::getSaveFileName (this, "Save Quiz",
        startDir, "Zyzzyva Quiz Files (*.zzq)");

    if (filename.isEmpty())
        return;

    bool filenameEdited = false;
    if (!filename.endsWith (".zzq", Qt::CaseInsensitive)) {
        filename += ".zzq";
        filenameEdited = true;
    }

    QFile file (filename);
    if (filenameEdited && file.exists()) {
        QString caption = "Overwrite Existing File?";
        QString message = "The file already exists.  Overwrite it?";
        message = Auxil::dialogWordWrap (message);
        int code = QMessageBox::warning (0, caption, message,
                                         QMessageBox::Yes, QMessageBox::No);
        if (code != QMessageBox::Yes)
            return;
    }

    if (!file.open (QIODevice::WriteOnly | QIODevice::Text)) {
        QString caption = "Error Saving Quiz";
        QString message = "Cannot save quiz:\n" + file.errorString() + ".";
        message = Auxil::dialogWordWrap (message);
        QMessageBox::warning (this, caption, message);
        return;
    }

    QTextStream stream (&file);
    stream << getQuizSpec().asXml();
}

//---------------------------------------------------------------------------
//  disableProgress
//
//! Disable the progress checkbox and set its state to unchecked.
//---------------------------------------------------------------------------
void
NewQuizDialog::disableProgress()
{
    progressCbox->setChecked (false);
    progressCbox->setEnabled (false);
}

//---------------------------------------------------------------------------
//  clearFilename
//
//! Remove the filename associated with the quiz specification.
//---------------------------------------------------------------------------
void
NewQuizDialog::clearFilename()
{
    quizSpec.setFilename (QString::null);
}

//---------------------------------------------------------------------------
//  updateForm
//
//! Update the state of the form based on the contents of the Quiz Type and
//! Quiz Method combo boxes.
//---------------------------------------------------------------------------
void
NewQuizDialog::updateForm()
{
    QuizSpec::QuizType type =
        Auxil::stringToQuizType (typeCombo->currentText());
    QuizSpec::QuizMethod method =
        Auxil::stringToQuizMethod (methodCombo->currentText());

    if (method == QuizSpec::CardboxQuizMethod) {
        progressCbox->setEnabled (false);
        progressCbox->setChecked (false);
        questionOrderCombo->setEnabled (false);
        questionOrderCombo->setCurrentIndex (questionOrderCombo->findText
            (Auxil::quizQuestionOrderToString (QuizSpec::ScheduleOrder)));
    }

    else if (type == QuizSpec::QuizWordListRecall) {
        questionOrderCombo->setEnabled (false);
        questionOrderCombo->setCurrentIndex (questionOrderCombo->findText
            (Auxil::quizQuestionOrderToString (QuizSpec::RandomOrder)));
    }

    else {
        progressCbox->setEnabled (true);
        questionOrderCombo->setEnabled (true);
    }
}

//---------------------------------------------------------------------------
//  fillQuestionOrderCombo
//
//! Fill the Question Order combo box with allowed values associated with a
//! quiz method.  For example, the Schedule question order is only valid with
//! in association with the Cardbox quiz method.
//
//! @param method the quiz method string
//---------------------------------------------------------------------------
void
NewQuizDialog::fillQuestionOrderCombo (const QString& method)
{
    QString prevText = questionOrderCombo->currentText();
    QuizSpec::QuizMethod meth = Auxil::stringToQuizMethod (method);

    switch (meth) {
        case QuizSpec::StandardQuizMethod:
        questionOrderCombo->clear();
        questionOrderCombo->addItem (Auxil::quizQuestionOrderToString
                                    (QuizSpec::RandomOrder));
        questionOrderCombo->addItem (Auxil::quizQuestionOrderToString
                                    (QuizSpec::AlphabeticalOrder));
        questionOrderCombo->addItem (Auxil::quizQuestionOrderToString
                                    (QuizSpec::ProbabilityOrder));
        break;

        case QuizSpec::CardboxQuizMethod:
        questionOrderCombo->clear();
        questionOrderCombo->addItem (Auxil::quizQuestionOrderToString
                                    (QuizSpec::ScheduleOrder));
        break;

        default: break;
    }

    int index = questionOrderCombo->findText (prevText);
    questionOrderCombo->setCurrentIndex (index >= 0 ? index : 0);
}
