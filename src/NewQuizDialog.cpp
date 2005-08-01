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

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>

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
//! @param name the name of this widget
//! @param modal whether the dialog is modal
//! @param f widget flags
//---------------------------------------------------------------------------
NewQuizDialog::NewQuizDialog (QWidget* parent, const char* name,
                              bool modal, WFlags f)
    : QDialog (parent, name, modal, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             "mainVlay");
    Q_CHECK_PTR (mainVlay);

    specForm = new SearchSpecForm (this, "specForm");
    Q_CHECK_PTR (specForm);
    mainVlay->addWidget (specForm);

    useListCbox = new QCheckBox ("&Use result list as a single question",
                                 this, "useListCbox");
    Q_CHECK_PTR (useListCbox);
    connect (useListCbox, SIGNAL (toggled (bool)),
             SLOT (useListToggled (bool)));
    mainVlay->addWidget (useListCbox);

    randomCbox = new QCheckBox ("&Randomize order", this, "randomCbox");
    Q_CHECK_PTR (randomCbox);
    randomCbox->setChecked (true);
    mainVlay->addWidget (randomCbox);

    QHBoxLayout* timerHlay = new QHBoxLayout (SPACING, "timerHlay");
    Q_CHECK_PTR (timerHlay);
    mainVlay->addLayout (timerHlay);

    timerCbox = new QCheckBox ("&Timer:", this, "timerCbox");
    Q_CHECK_PTR (timerCbox);
    connect (timerCbox, SIGNAL (toggled (bool)), SLOT (timerToggled (bool)));
    timerHlay->addWidget (timerCbox);

    timerWidget = new QWidget (this, "timerWidget");
    Q_CHECK_PTR (timerWidget);
    timerWidget->setEnabled (false);
    timerHlay->addWidget (timerWidget);

    QHBoxLayout* timerWidgetHlay = new QHBoxLayout (timerWidget, 0, SPACING,
                                                    "timerWidgetHlay");
    Q_CHECK_PTR (timerWidgetHlay);

    timerSbox = new QSpinBox (1, 9999, 1, timerWidget, "timerSbox");
    Q_CHECK_PTR (timerSbox);
    timerSbox->setValue (10);
    timerWidgetHlay->addWidget (timerSbox);

    QLabel* timerLabel = new QLabel ("seconds", timerWidget, "timerLabel");
    Q_CHECK_PTR (timerLabel);
    timerWidgetHlay->addWidget (timerLabel);

    timerCombo = new QComboBox (timerWidget, "timerCombo");
    timerCombo->insertItem (TIMER_PER_QUESTION);
    timerCombo->insertItem (TIMER_PER_RESPONSE);
    timerCombo->setCurrentText (TIMER_PER_RESPONSE);
    timerWidgetHlay->addWidget (timerCombo);

    // OK/Cancel buttons
    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    QPushButton* loadQuizButton = new QPushButton ("&Load...", this,
                                                   "loadQuizButton");
    Q_CHECK_PTR (loadQuizButton);
    connect (loadQuizButton, SIGNAL (clicked()), SLOT (loadQuiz()));
    buttonHlay->addWidget (loadQuizButton);

    saveQuizButton = new QPushButton ("&Save...", this, "saveQuizButton");
    Q_CHECK_PTR (saveQuizButton);
    connect (saveQuizButton, SIGNAL (clicked()), SLOT (saveQuiz()));
    buttonHlay->addWidget (saveQuizButton);

    buttonHlay->addStretch (1);

    okButton = new QPushButton ("OK", this, "okButton");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    QPushButton* cancelButton = new QPushButton ("Cancel", this,
                                                 "cancelButton");
    Q_CHECK_PTR (cancelButton);
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    resize (minimumSizeHint());
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
NewQuizDialog::getQuizSpec() const
{
    QuizSpec spec;
    spec.type = QuizAnagrams;
    spec.searchSpec = specForm->getSearchSpec();
    spec.useList = useListCbox->isChecked();
    spec.randomOrder = randomCbox->isChecked();

    QuizTimerSpec timerSpec;
    if (timerCbox->isChecked()) {
        timerSpec.duration = timerSbox->value();
        QString timerType = timerCombo->currentText();
        if (timerType == TIMER_PER_QUESTION)
            timerSpec.type = PerQuestion;
        else if (timerType == TIMER_PER_RESPONSE)
            timerSpec.type = PerResponse;
    }
    spec.timerSpec = timerSpec;

    return spec;
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
    specForm->setSearchSpec (spec.searchSpec);
    useListCbox->setChecked (spec.useList);
    randomCbox->setChecked (spec.randomOrder);
    timerCbox->setChecked (false);
    timerSbox->setValue (0);
    timerCombo->setCurrentText (TIMER_PER_RESPONSE);
    if (spec.timerSpec.type != NoTimer) {
        timerCbox->setChecked (true);
        timerSbox->setValue (spec.timerSpec.duration);
        switch (spec.timerSpec.type) {
            case PerQuestion:
            timerCombo->setCurrentText (TIMER_PER_QUESTION);
            break;
            case PerResponse:
            timerCombo->setCurrentText (TIMER_PER_RESPONSE);
            break;
            default: break;
        }
    }
}

//---------------------------------------------------------------------------
//  useListToggled
//
//! Called when the Use List checkbox is toggled.  Disable the Random
//! checkbox unless the Use List checkbox is checked.
//
//! @param on whether the checkbox is checked
//---------------------------------------------------------------------------
void
NewQuizDialog::useListToggled (bool on)
{
    randomCbox->setEnabled (!on);
    if (on)
        randomCbox->setChecked (false);
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
    qDebug ("loadQuiz");

    // Copied from SearchSpecForm::loadSearch()

    QString filename = QFileDialog::getOpenFileName
        (Auxil::getQuizDir(), "Zyzzyva Quiz Files (*.zzq)", this,
         "loadQuizDialog", "Load Quiz");
    if (filename.isEmpty())
        return;

    QFile file (filename);
    if (!file.open (IO_ReadOnly)) {
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
    // XXX: Does something analogous to this still need to be called?
    //contentsChanged();
}

//---------------------------------------------------------------------------
//  saveQuiz
//
//! Save the current quiz spec to a file.
//---------------------------------------------------------------------------
void
NewQuizDialog::saveQuiz()
{
    qDebug ("saveQuiz");

    // Copied from SearchSpecForm::saveSearch

    //QString filename = QFileDialog::getSaveFileName
    //    (Auxil::getSearchDir() + "/saved", "Zyzzyva Search Files (*.zzs)",
    //     this, "saveDialog", "Save Search");

    //if (filename.isEmpty())
    //    return;

    //QFile file (filename);
    //if (file.exists()) {
    //    int code = QMessageBox::warning (0, "Overwrite Existing File?",
    //                                     "The file already exists.  "
    //                                     "Overwrite it?", QMessageBox::Yes,
    //                                     QMessageBox::No);
    //    if (code != QMessageBox::Yes)
    //        return;
    //}

    //if (!file.open (IO_WriteOnly)) {
    //    QMessageBox::warning (this, "Error Saving Search",
    //                          "Cannot save search:\n" + file.errorString() +
    //                          ".");
    //    return;
    //}

    //QDomImplementation implementation;
    //QDomDocument document (implementation.createDocumentType
    //                       ("zyzzyva-search", QString::null,
    //                        "http://pietdepsi.com/dtd/zyzzyva-search.dtd"));

    //document.appendChild (getSearchSpec().asDomElement());

    //// XXX: There should be a programmatic way to write the <?xml?> header
    //// based on the QDomImplementation, shouldn't there?
    //QTextStream stream (&file);
    //stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    //    << document.toString();
}
