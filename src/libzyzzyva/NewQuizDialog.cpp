//---------------------------------------------------------------------------
// NewQuizDialog.cpp
//
// A dialog for prompting the user for a quiz.
//
// Copyright 2004-2012 Boshvark Software, LLC.
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
#include "LexiconSelectWidget.h"
#include "MainSettings.h"
#include "QuizDatabase.h"
#include "QuizSpec.h"
#include "SearchSpec.h"
#include "SearchSpecForm.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
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
//! @param e a word engine
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
NewQuizDialog::NewQuizDialog(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);

    lexiconWidget = new LexiconSelectWidget;
    mainVlay->addWidget(lexiconWidget);

    QGridLayout* quizGlay = new QGridLayout;
    mainVlay->addLayout(quizGlay);

    int row = 0;
    QLabel* typeLabel = new QLabel("Quiz Type:");
    quizGlay->addWidget(typeLabel, row, 0);

    typeCombo = new QComboBox;
    typeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizAnagrams));
    typeCombo->addItem(
        Auxil::quizTypeToString(QuizSpec::QuizAnagramsWithHooks));
    // Commented out until it can be fully implemented
    //typeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizBuild));
    typeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizWordListRecall));
    typeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizHooks));
    typeCombo->setCurrentIndex(typeCombo->findText(
        Auxil::quizTypeToString(QuizSpec::QuizAnagrams)));
    connect(typeCombo, SIGNAL(activated(const QString&)),
            SLOT(typeActivated(const QString&)));
    quizGlay->addWidget(typeCombo, row, 1);

    ++row;
    QLabel* methodLabel = new QLabel("Quiz Method:");
    quizGlay->addWidget(methodLabel, row, 0);

    methodCombo = new QComboBox;
    methodCombo->addItem(
        Auxil::quizMethodToString(QuizSpec::QuizMethodStandard));
    methodCombo->addItem(
        Auxil::quizMethodToString(QuizSpec::QuizMethodCardbox));
    methodCombo->setCurrentIndex(methodCombo->findText(
        Auxil::quizMethodToString(QuizSpec::QuizMethodStandard)));
    connect(methodCombo, SIGNAL(activated(const QString&)),
            SLOT(methodActivated(const QString&)));
    quizGlay->addWidget(methodCombo, row, 1);

    ++row;
    QLabel* quizOrderLabel = new QLabel("Question Order:");
    quizGlay->addWidget(quizOrderLabel, row, 0);

    QHBoxLayout* quizOrderHlay = new QHBoxLayout;
    quizOrderHlay->setMargin(0);
    quizGlay->addLayout(quizOrderHlay, row, 1);

    quizOrderCombo = new QComboBox;
    fillQuizOrderCombo(methodCombo->currentText());
    quizOrderCombo->setCurrentIndex(quizOrderCombo->findText(
        Auxil::quizQuizOrderToString(QuizSpec::QuizOrderRandom)));
    connect(quizOrderCombo, SIGNAL(activated(const QString&)),
            SLOT(quizOrderActivated(const QString&)));
    quizOrderHlay->addWidget(quizOrderCombo);

    probNumBlanksLabel = new QLabel("Blanks:");;
    probNumBlanksLabel->hide();
    quizOrderHlay->addWidget(probNumBlanksLabel);

    probNumBlanksSbox = new QSpinBox;
    probNumBlanksSbox->setMinimum(0);
    probNumBlanksSbox->setMaximum(Defs::MAX_BLANKS);
    probNumBlanksSbox->setValue(MainSettings::getProbabilityNumBlanks());
    connect(probNumBlanksSbox, SIGNAL(valueChanged(int)),
            SLOT(probNumBlanksValueChanged(int)));
    probNumBlanksSbox->hide();
    quizOrderHlay->addWidget(probNumBlanksSbox);

    sourceStack = new QStackedWidget;
    mainVlay->addWidget(sourceStack);

    buildWidget = new QWidget;
    sourceStack->addWidget(buildWidget);

    QHBoxLayout* responseHlay = new QHBoxLayout(buildWidget);
    responseHlay->setMargin(0);

    QLabel* responseLabel = new QLabel("Response length:");
    responseHlay->addWidget(responseLabel);

    QLabel* responseMinLabel = new QLabel("Min:");
    responseHlay->addWidget(responseMinLabel);

    responseMinSbox = new QSpinBox;
    responseMinSbox->setMinimum(0);
    responseMinSbox->setMaximum(MAX_WORD_LEN);
    responseHlay->addWidget(responseMinSbox);

    QLabel* responseMaxLabel = new QLabel("Max:");
    responseHlay->addWidget(responseMaxLabel);

    responseMaxSbox = new QSpinBox;
    responseMaxSbox->setMinimum(0);
    responseMaxSbox->setMaximum(MAX_WORD_LEN);
    responseHlay->addWidget(responseMaxSbox);

    searchWidget = new QWidget;
    sourceStack->addWidget(searchWidget);

    QVBoxLayout* searchVlay = new QVBoxLayout(searchWidget);
    searchVlay->setMargin(0);

    allCardboxButton = new QRadioButton;
    allCardboxButton->setText("Use all available words");
    allCardboxButton->setChecked(true);
    searchVlay->addWidget(allCardboxButton);

    useSearchButton = new QRadioButton;
    useSearchButton->setText("Use only words matching search specification");
    connect(useSearchButton, SIGNAL(toggled(bool)),
            SLOT(useSearchButtonToggled(bool)));
    searchVlay->addWidget(useSearchButton);

    searchSpecGbox = new QGroupBox("Search Specification");
    searchVlay->addWidget(searchSpecGbox);

    QHBoxLayout* specHlay = new QHBoxLayout(searchSpecGbox);

    searchSpecForm = new SearchSpecForm;
    connect(searchSpecForm, SIGNAL(contentsChanged()),
            SLOT(searchContentsChanged()));
    connect(searchSpecForm, SIGNAL(returnPressed()), SLOT(accept()));
    specHlay->addWidget(searchSpecForm);

    QHBoxLayout* progressHlay = new QHBoxLayout;
    mainVlay->addLayout(progressHlay);

    progressCbox = new QCheckBox("Restore &progress");
    progressCbox->setEnabled(false);
    progressHlay->addWidget(progressCbox);

    progressLabel = new QLabel;
    progressLabel->setEnabled(false);
    progressHlay->addWidget(progressLabel);

    progressHlay->addStretch(1);

    QHBoxLayout* timerHlay = new QHBoxLayout;
    mainVlay->addLayout(timerHlay);

    timerCbox = new QCheckBox("&Timer:");
    connect(timerCbox, SIGNAL(toggled(bool)), SLOT(timerToggled(bool)));
    timerHlay->addWidget(timerCbox);

    timerWidget = new QWidget;
    timerWidget->setEnabled(false);
    timerHlay->addWidget(timerWidget);

    QHBoxLayout* timerWidgetHlay = new QHBoxLayout(timerWidget);
    timerWidgetHlay->setMargin(0);

    timerSbox = new QSpinBox;
    timerSbox->setMinimum(1);
    timerSbox->setMaximum(9999);
    timerSbox->setValue(10);
    timerWidgetHlay->addWidget(timerSbox);

    QLabel* timerLabel = new QLabel("seconds");
    timerWidgetHlay->addWidget(timerLabel);

    timerCombo = new QComboBox;
    timerCombo->addItem(TIMER_PER_QUESTION);
    timerCombo->addItem(TIMER_PER_RESPONSE);
    timerCombo->setCurrentIndex(timerCombo->findText(TIMER_PER_RESPONSE));
    timerWidgetHlay->addWidget(timerCombo);

    // OK/Cancel buttons
    QHBoxLayout* buttonHlay = new QHBoxLayout;
    mainVlay->addLayout(buttonHlay);

    ZPushButton* loadQuizButton = new ZPushButton("&Load Quiz...");
    connect(loadQuizButton, SIGNAL(clicked()), SLOT(loadQuiz()));
    buttonHlay->addWidget(loadQuizButton);

    saveQuizButton = new ZPushButton("&Save Quiz...");
    connect(saveQuizButton, SIGNAL(clicked()), SLOT(saveQuiz()));
    buttonHlay->addWidget(saveQuizButton);

    buttonHlay->addStretch(1);

    okButton = new ZPushButton("OK");
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    buttonHlay->addWidget(okButton);

    ZPushButton* cancelButton = new ZPushButton("Cancel");
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    buttonHlay->addWidget(cancelButton);

    updateForm();

    setWindowTitle(DIALOG_CAPTION);
    searchSpecForm->selectInputArea();
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
        Auxil::stringToQuizMethod(methodCombo->currentText());
    QuizSpec::QuizType quizType =
        Auxil::stringToQuizType(typeCombo->currentText());

    quizSpec.setLexicon(lexiconWidget->getCurrentLexicon());
    quizSpec.setType(quizType);
    quizSpec.setMethod(quizMethod);
    quizSpec.setQuizOrder(
        Auxil::stringToQuizQuizOrder(quizOrderCombo->currentText()));
    quizSpec.setProbabilityNumBlanks(probNumBlanksSbox->value());

    if (quizType == QuizSpec::QuizBuild) {
        quizSpec.setQuizSource(QuizSpec::QuizSourceRandomLetters);
    }
    else if ((quizMethod == QuizSpec::QuizMethodCardbox) &&
              allCardboxButton->isChecked())
    {
        quizSpec.setQuizSource(QuizSpec::QuizSourceCardboxReady);
    }
    else {
        quizSpec.setQuizSource(QuizSpec::QuizSourceSearch);
        quizSpec.setSearchSpec(searchSpecForm->getSearchSpec());
    }

    QWidget* widget = sourceStack->currentWidget();
    if (widget == buildWidget) {
        quizSpec.setResponseMinLength(responseMinSbox->value());
        quizSpec.setResponseMaxLength(responseMaxSbox->value());
    }

    QuizTimerSpec timerSpec;
    if (timerCbox->isChecked()) {
        timerSpec.setDuration(timerSbox->value());
        QString timerType = timerCombo->currentText();
        if (timerType == TIMER_PER_QUESTION)
            timerSpec.setType(PerQuestion);
        else if (timerType == TIMER_PER_RESPONSE)
            timerSpec.setType(PerResponse);
    }
    quizSpec.setTimerSpec(timerSpec);

    if (!progressCbox->isChecked() &&
        (quizMethod != QuizSpec::QuizMethodCardbox))
    {
        quizSpec.setProgress(QuizProgress());
        quizSpec.setFilename(QString());
        quizSpec.setRandomSeed(0);
        quizSpec.setRandomSeed2(0);
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
NewQuizDialog::setQuizSpec(const QuizSpec& spec)
{
    bool lexiconOk = lexiconWidget->setCurrentLexicon(spec.getLexicon());

    // Set method before type, because type may end up changing the method
    methodCombo->setCurrentIndex(methodCombo->findText(
        Auxil::quizMethodToString(spec.getMethod())));
    methodActivated(methodCombo->currentText());
    typeCombo->setCurrentIndex(typeCombo->findText(
        Auxil::quizTypeToString(spec.getType())));
    typeActivated(typeCombo->currentText());
    quizOrderCombo->setCurrentIndex(quizOrderCombo->findText(
        Auxil::quizQuizOrderToString(spec.getQuizOrder())));
    quizOrderActivated(quizOrderCombo->currentText());
    probNumBlanksSbox->setValue(spec.getProbabilityNumBlanks());

    QuizSpec::QuizSource source = spec.getQuizSource();
    if (source == QuizSpec::QuizSourceSearch) {
        useSearchButton->setChecked(true);
        searchSpecForm->setSearchSpec(spec.getSearchSpec());
    }
    else if (source == QuizSpec::QuizSourceCardboxReady) {
        allCardboxButton->setChecked(true);
        searchSpecForm->setSearchSpec(SearchSpec());
    }

    int responseMin = spec.getResponseMinLength();
    if (responseMin)
        responseMinSbox->setValue(responseMin);

    int responseMax = spec.getResponseMaxLength();
    if (responseMax)
        responseMaxSbox->setValue(responseMax);

    timerCbox->setChecked(false);
    timerSbox->setValue(0);
    timerCombo->setCurrentIndex(timerCombo->findText(TIMER_PER_RESPONSE));
    QuizTimerSpec timerSpec = spec.getTimerSpec();
    if (timerSpec.getType() != NoTimer) {
        timerCbox->setChecked(true);
        timerSbox->setValue(timerSpec.getDuration());
        switch (timerSpec.getType()) {
            case PerQuestion:
            timerCombo->setCurrentIndex(
                timerCombo->findText(TIMER_PER_QUESTION));
            break;
            case PerResponse:
            timerCombo->setCurrentIndex(
                timerCombo->findText(TIMER_PER_RESPONSE));
            break;
            default: break;
        }
    }
    if (spec.getMethod() == QuizSpec::QuizMethodStandard) {
        progressCbox->setEnabled(true);
        progressCbox->setChecked(true);
        QuizProgress progress = spec.getProgress();
        int questionNum = progress.isEmpty() ? 1 : progress.getQuestion() + 1;
        progressLabel->setEnabled(true);
        progressLabel->setText(QString("(Question %1)").arg(questionNum));
    }
    quizSpec = spec;

    // Display warning of lexicon could not be selected
    if (!lexiconOk) {
        QString caption = "Error Setting Lexicon";
        QString message = "Cannot open lexicon '" + spec.getLexicon() + "'";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
    }
}

//---------------------------------------------------------------------------
//  typeActivated
//
//! Called when the contents of the Quiz Type combo box are changed.  Disable
//! the Random checkbox if the Word List Recall type is selected.  Also change
//! the method to Standard if the Build or Word List Recall type is selected.
//
//! @param text the text in the combo box
//---------------------------------------------------------------------------
void
NewQuizDialog::typeActivated(const QString& text)
{
    QuizSpec::QuizType type = Auxil::stringToQuizType(text);
    if ((type == QuizSpec::QuizWordListRecall) || (type == QuizSpec::QuizBuild))
    {
        methodCombo->setEnabled(false);
        methodCombo->setCurrentIndex(methodCombo->findText(
            Auxil::quizMethodToString(QuizSpec::QuizMethodStandard)));
    }
    else {
        methodCombo->setEnabled(true);
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
NewQuizDialog::methodActivated(const QString& text)
{
    fillQuizOrderCombo(text);
    updateForm();
    disableProgress();
    clearFilename();
}

//---------------------------------------------------------------------------
//  useSearchButtonToggled
//
//! Called when the Use Search Specification button is toggled.  Enable or
//! disable the search specification area.
//
//! @param on whether the search button is active
//---------------------------------------------------------------------------
void
NewQuizDialog::useSearchButtonToggled(bool)
{
    updateForm();
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
NewQuizDialog::timerToggled(bool on)
{
    timerWidget->setEnabled(on);
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
//  quizOrderActivated
//
//! Called when the contents of the Question Order combo box are changed.
//
//! @param text the new contents of the combo box
//---------------------------------------------------------------------------
void
NewQuizDialog::quizOrderActivated(const QString& str)
{
    disableProgress();
    clearFilename();

    QuizSpec::QuizOrder order = Auxil::stringToQuizQuizOrder(str);
    switch (order) {
        case QuizSpec::QuizOrderProbability:
        probNumBlanksLabel->show();
        probNumBlanksSbox->show();
        break;
        default:
        probNumBlanksLabel->hide();
        probNumBlanksSbox->hide();
        break;
    }
}

//---------------------------------------------------------------------------
//  probNumBlanksValueChanged
//
//! Called when the value of the Blanks spin box is changed.
//
//! @param text the new value of the spin box
//---------------------------------------------------------------------------
void
NewQuizDialog::probNumBlanksValueChanged(int)
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
    QString filename = QFileDialog::getOpenFileName(this, "Load Quiz",
        Auxil::getQuizDir() + "/saved", "Zyzzyva Quiz Files (*.zzq;*.db)");

    if (filename.isEmpty())
        return;

    bool isXml = filename.endsWith(".zzq");

    // ### Convert XML file to database here?
    if (isXml) {
        QFile file (filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString caption = "Error Opening Quiz File";
            QString message = "Cannot open file '" + filename + "': " +
                file.errorString();
            message = Auxil::dialogWordWrap(message);
            QMessageBox::warning(this, caption, message);
            return;
        }

        QString errorMsg;
        QuizSpec spec;
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        bool ok = spec.fromXmlFile(file, &errorMsg);
        QApplication::restoreOverrideCursor();

        if (!ok) {
            QString caption = "Error in Quiz File";
            QString message = "Error in quiz file.\n" + errorMsg;
            message = Auxil::dialogWordWrap(message);
            QMessageBox::warning(this, caption, message);
            return;
        }

        setQuizSpec(spec);
    }

    else {
        QuizDatabase quizDb (filename);
        QuizSpec spec = quizDb.quizSpec();

        qDebug("Loaded quiz spec: [%s]", spec.asXml().toUtf8().constData());

        setQuizSpec(spec);
    }
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
        currentFilename.remove("/[^/]+$");
        startDir = currentFilename;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Save Quiz",
        startDir, "Zyzzyva Quiz Files (*.zzq)");

    if (filename.isEmpty())
        return;

    bool filenameEdited = false;
    if (!filename.endsWith(".zzq", Qt::CaseInsensitive)) {
        filename += ".zzq";
        filenameEdited = true;
    }

    QFile file (filename);
    if (filenameEdited && file.exists()) {
        QString caption = "Overwrite Existing File?";
        QString message = "The file already exists.  Overwrite it?";
        message = Auxil::dialogWordWrap(message);
        int code = QMessageBox::warning(0, caption, message,
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::No);
        if (code != QMessageBox::Yes)
            return;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString caption = "Error Saving Quiz";
        QString message = "Cannot save quiz:\n" + file.errorString() + ".";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
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
    progressCbox->setChecked(false);
    progressCbox->setEnabled(false);
    progressLabel->setEnabled(false);
    progressLabel->setText(QString());
}

//---------------------------------------------------------------------------
//  clearFilename
//
//! Remove the filename associated with the quiz specification.
//---------------------------------------------------------------------------
void
NewQuizDialog::clearFilename()
{
    quizSpec.setFilename(QString());
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
        Auxil::stringToQuizType(typeCombo->currentText());
    QuizSpec::QuizMethod method =
        Auxil::stringToQuizMethod(methodCombo->currentText());

    if (method == QuizSpec::QuizMethodCardbox) {
        disableProgress();
        quizOrderCombo->setEnabled(true);
        quizOrderCombo->setCurrentIndex(quizOrderCombo->findText(
            Auxil::quizQuizOrderToString(QuizSpec::QuizOrderSchedule)));
        quizOrderActivated(quizOrderCombo->currentText());

        sourceStack->setCurrentWidget(searchWidget);
        allCardboxButton->show();
        useSearchButton->show();
        searchSpecGbox->setEnabled(useSearchButton->isChecked());
    }

    else {
        if ((type == QuizSpec::QuizWordListRecall) ||
            (type == QuizSpec::QuizBuild))
        {
            quizOrderCombo->setEnabled(false);
            quizOrderCombo->setCurrentIndex(quizOrderCombo->findText(
                Auxil::quizQuizOrderToString(QuizSpec::QuizOrderRandom)));
            quizOrderActivated(quizOrderCombo->currentText());
        }

        else {
            progressCbox->setEnabled(true);
            quizOrderCombo->setEnabled(true);
        }

        allCardboxButton->hide();
        useSearchButton->hide();

        if (type == QuizSpec::QuizBuild) {
            sourceStack->setCurrentWidget(buildWidget);
        }
        else {
            sourceStack->setCurrentWidget(searchWidget);
            searchSpecGbox->setEnabled(true);
        }
    }
}

//---------------------------------------------------------------------------
//  fillQuizOrderCombo
//
//! Fill the Question Order combo box with allowed values associated with a
//! quiz method.  For example, the Schedule question order is only valid with
//! in association with the Cardbox quiz method.
//
//! @param method the quiz method string
//---------------------------------------------------------------------------
void
NewQuizDialog::fillQuizOrderCombo(const QString& method)
{
    QString prevText = quizOrderCombo->currentText();
    QuizSpec::QuizMethod meth = Auxil::stringToQuizMethod(method);

    switch (meth) {
        case QuizSpec::QuizMethodStandard:
        quizOrderCombo->clear();
        quizOrderCombo->addItem(
            Auxil::quizQuizOrderToString(QuizSpec::QuizOrderRandom));
        quizOrderCombo->addItem(
            Auxil::quizQuizOrderToString(QuizSpec::QuizOrderAlphabetical));
        quizOrderCombo->addItem(
            Auxil::quizQuizOrderToString(QuizSpec::QuizOrderProbability));
        quizOrderCombo->addItem(
            Auxil::quizQuizOrderToString(QuizSpec::QuizOrderPlayability));
        break;

        case QuizSpec::QuizMethodCardbox:
        quizOrderCombo->clear();
        quizOrderCombo->addItem(
            Auxil::quizQuizOrderToString(QuizSpec::QuizOrderSchedule));
        quizOrderCombo->addItem(
            Auxil::quizQuizOrderToString(QuizSpec::QuizOrderScheduleZeroFirst));
        break;

        default: break;
    }

    int index = quizOrderCombo->findText(prevText);
    quizOrderCombo->setCurrentIndex(index >= 0 ? index : 0);
}
