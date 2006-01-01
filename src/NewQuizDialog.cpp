//---------------------------------------------------------------------------
// NewQuizDialog.cpp
//
// A dialog for prompting the user for a quiz.
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

const QString DIALOG_CAPTION = "New Quiz";
const QString TIMER_PER_QUESTION = "per question";
const QString TIMER_PER_RESPONSE = "per response";

using namespace Defs;

//---------------------------------------------------------------------------
//  NewQuizDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
NewQuizDialog::NewQuizDialog (QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f)
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
    typeCombo->insertItem (Auxil::quizTypeToString (QuizSpec::QuizAnagrams));
    typeCombo->insertItem (Auxil::quizTypeToString
                           (QuizSpec::QuizWordListRecall));
    typeCombo->insertItem (Auxil::quizTypeToString (QuizSpec::QuizHooks));
    typeCombo->setCurrentText (Auxil::quizTypeToString
                               (QuizSpec::QuizAnagrams));
    connect (typeCombo, SIGNAL (activated (const QString&)),
             SLOT (typeActivated (const QString&)));
    typeHlay->addWidget (typeCombo);

    QGroupBox* specGbox = new QGroupBox ("Search Specification");
    Q_CHECK_PTR (specGbox);
    mainVlay->addWidget (specGbox);

    QHBoxLayout* specHlay = new QHBoxLayout (specGbox);
    Q_CHECK_PTR (specHlay);

    specForm = new SearchSpecForm;
    Q_CHECK_PTR (specForm);
    specHlay->addWidget (specForm);

    progressCbox = new QCheckBox ("Restore &progress");
    Q_CHECK_PTR (progressCbox);
    progressCbox->setEnabled (false);
    mainVlay->addWidget (progressCbox);

    randomCbox = new QCheckBox ("&Randomize order");
    Q_CHECK_PTR (randomCbox);
    randomCbox->setChecked (true);
    mainVlay->addWidget (randomCbox);

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
    timerCombo->insertItem (TIMER_PER_QUESTION);
    timerCombo->insertItem (TIMER_PER_RESPONSE);
    timerCombo->setCurrentText (TIMER_PER_RESPONSE);
    timerWidgetHlay->addWidget (timerCombo);

    // OK/Cancel buttons
    QHBoxLayout* buttonHlay = new QHBoxLayout;
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    QPushButton* loadQuizButton = new QPushButton ("&Load Quiz...");
    Q_CHECK_PTR (loadQuizButton);
    connect (loadQuizButton, SIGNAL (clicked()), SLOT (loadQuiz()));
    buttonHlay->addWidget (loadQuizButton);

    saveQuizButton = new QPushButton ("&Save Quiz...");
    Q_CHECK_PTR (saveQuizButton);
    connect (saveQuizButton, SIGNAL (clicked()), SLOT (saveQuiz()));
    buttonHlay->addWidget (saveQuizButton);

    buttonHlay->addStretch (1);

    okButton = new QPushButton ("OK");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    QPushButton* cancelButton = new QPushButton ("Cancel");
    Q_CHECK_PTR (cancelButton);
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    setCaption (DIALOG_CAPTION);
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
    quizSpec.setType (Auxil::stringToQuizType (typeCombo->currentText()));
    quizSpec.setSearchSpec (specForm->getSearchSpec());
    quizSpec.setRandomOrder (randomCbox->isChecked());

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

    if (!progressCbox->isChecked())
        quizSpec.setProgress (QuizProgress());

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
    quizSpec = spec;

    typeCombo->setCurrentText (Auxil::quizTypeToString (spec.getType()));
    typeActivated (typeCombo->currentText());
    specForm->setSearchSpec (spec.getSearchSpec());
    randomCbox->setChecked (spec.getRandomOrder());
    timerCbox->setChecked (false);
    timerSbox->setValue (0);
    timerCombo->setCurrentText (TIMER_PER_RESPONSE);
    QuizTimerSpec timerSpec = spec.getTimerSpec();
    if (timerSpec.getType() != NoTimer) {
        timerCbox->setChecked (true);
        timerSbox->setValue (timerSpec.getDuration());
        switch (timerSpec.getType()) {
            case PerQuestion:
            timerCombo->setCurrentText (TIMER_PER_QUESTION);
            break;
            case PerResponse:
            timerCombo->setCurrentText (TIMER_PER_RESPONSE);
            break;
            default: break;
        }
    }
    progressCbox->setEnabled (true);
    progressCbox->setChecked (true);
}

//---------------------------------------------------------------------------
//  typeActivated
//
//! Called when the contents of the Quiz Type combo box are changed.  Disable
//! the Random checkbox if the Word List Recall type is selected.
//
//! @param text the text in the combo box
//---------------------------------------------------------------------------
void
NewQuizDialog::typeActivated (const QString& text)
{
    QuizSpec::QuizType type = Auxil::stringToQuizType (text);
    if (type == QuizSpec::QuizWordListRecall) {
        randomCbox->setEnabled (false);
        randomCbox->setChecked (false);
    }
    else {
        randomCbox->setEnabled (true);
    }
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
//  loadQuiz
//
//! Load a quiz spec from a file.
//---------------------------------------------------------------------------
void
NewQuizDialog::loadQuiz()
{
    QString filename = QFileDialog::getOpenFileName (this, "Load Quiz",
        Auxil::getQuizDir(), "Zyzzyva Quiz Files (*.zzq)");

    if (filename.isEmpty())
        return;

    QFile file (filename);
    // FIXME Qt4: QIODevice::Translate no longer exists!
    //if (!file.open (QIODevice::ReadOnly | QIODevice::Translate)) {
    if (!file.open (QIODevice::ReadOnly)) {
        QMessageBox::warning (this, "Error Opening Quiz File",
                              "Cannot open file '" + filename + "': " +
                              file.errorString());
        return;
    }

    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;

    QApplication::setOverrideCursor (Qt::waitCursor);
    QDomDocument document;
    bool success = document.setContent (&file, false, &errorMsg, &errorLine,
                                        &errorColumn);
    QApplication::restoreOverrideCursor();

    if (!success) {
        QMessageBox::warning (this, "Error in Quiz File",
                              "Error in quiz file, line " +
                              QString::number (errorLine) + ", column " +
                              QString::number (errorColumn) + ": " + 
                              errorMsg);
        return;
    }

    QuizSpec spec;
    if (!spec.fromDomElement (document.documentElement())) {
        QMessageBox::warning (this, "Error in Quiz File",
                              "Error in quiz file.");
        return;
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
    QString filename = QFileDialog::getSaveFileName (this, "Save Quiz",
        Auxil::getQuizDir() + "/saved", "Zyzzyva Quiz Files (*.zzq)");

    if (filename.isEmpty())
        return;

    bool filenameEdited = false;
    if (!filename.endsWith (".zzq", false)) {
        filename += ".zzq";
        filenameEdited = true;
    }

    QFile file (filename);
    if (filenameEdited && file.exists()) {
        int code = QMessageBox::warning (0, "Overwrite Existing File?",
                                         "The file already exists.  "
                                         "Overwrite it?", QMessageBox::Yes,
                                         QMessageBox::No);
        if (code != QMessageBox::Yes)
            return;
    }

    // FIXME Qt4: QIODevice::Translate no longer exists!
    //if (!file.open (QIODevice::WriteOnly | QIODevice::Translate)) {
    if (!file.open (QIODevice::WriteOnly)) {
        QMessageBox::warning (this, "Error Saving Quiz",
                              "Cannot save quiz:\n" + file.errorString() +
                              ".");
        return;
    }

    QDomImplementation implementation;
    QDomDocument document (implementation.createDocumentType
                           ("zyzzyva-quiz", QString::null,
                            "http://pietdepsi.com/dtd/zyzzyva-quiz.dtd"));

    document.appendChild (getQuizSpec().asDomElement());

    //// XXX: There should be a programmatic way to write the <?xml?> header
    //// based on the QDomImplementation, shouldn't there?
    QTextStream stream (&file);
    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        << document.toString();
}
