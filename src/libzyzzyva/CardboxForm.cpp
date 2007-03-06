//---------------------------------------------------------------------------
// CardboxForm.cpp
//
// A form for querying and editing the contents of the cardbox system.
//
// Copyright 2007 Michael W Thelen <mthelen@gmail.com>.
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

#include "CardboxForm.h"
#include "QuizDatabase.h"
#include "WordEngine.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>

using namespace Defs;

const QString TITLE_PREFIX = "Cardbox";

//---------------------------------------------------------------------------
//  CardboxForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CardboxForm::CardboxForm (WordEngine* e, QWidget* parent, Qt::WFlags f)
    : ActionForm (CardboxFormType, parent, f), wordEngine (e),
    quizDatabase (0), cardboxCountModel (0), cardboxDaysModel (0)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this);
    mainVlay->setMargin (MARGIN);
    mainVlay->setSpacing (SPACING);
    Q_CHECK_PTR (mainVlay);

    QLabel* label = new QLabel
        ("Shift cardbox questions so the backlog contains a certain number "
         "of questions.  Currently this is hard-coded to reschedule all "
         "questions in the Anagrams cardbox.");
    Q_CHECK_PTR (label);
    label->setWordWrap (true);
    mainVlay->addWidget (label);

    // Shift area
    QHBoxLayout* shiftHlay = new QHBoxLayout;
    Q_CHECK_PTR (shiftHlay);
    shiftHlay->setSpacing (SPACING);
    mainVlay->addLayout (shiftHlay);

    QLabel* shiftLabel = new QLabel ("Desired backlog size:");
    Q_CHECK_PTR (shiftLabel);
    shiftHlay->addWidget (shiftLabel);

    backlogSbox = new QSpinBox;
    Q_CHECK_PTR (backlogSbox);
    backlogSbox->setMinimum (1);
    backlogSbox->setMaximum (999999);
    shiftHlay->addWidget (backlogSbox);

    ZPushButton* shiftButton = new ZPushButton;
    Q_CHECK_PTR (shiftButton);
    shiftButton->setText ("&Shift");
    shiftButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (shiftButton, SIGNAL (clicked()), SLOT (shiftClicked()));
    shiftHlay->addWidget (shiftButton);

    QHBoxLayout* cardboxHlay = new QHBoxLayout;
    Q_CHECK_PTR (cardboxHlay);
    cardboxHlay->setSpacing (SPACING);
    mainVlay->addLayout (cardboxHlay);

    cardboxCountView = new QTreeView;
    Q_CHECK_PTR (cardboxCountView);
    cardboxHlay->addWidget (cardboxCountView);

    cardboxDaysView = new QTreeView;
    Q_CHECK_PTR (cardboxDaysView);
    cardboxHlay->addWidget (cardboxDaysView);

    ZPushButton* refreshButton = new ZPushButton;
    Q_CHECK_PTR (refreshButton);
    refreshButton->setText ("&Refresh");
    refreshButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (refreshButton, SIGNAL (clicked()), SLOT (refreshClicked()));
    mainVlay->addWidget (refreshButton);

    mainVlay->addStretch (0);

    quizSpecChanged();
}

//---------------------------------------------------------------------------
//  getIcon
//
//! Returns the current icon.
//
//! @return the current icon
//---------------------------------------------------------------------------
QIcon
CardboxForm::getIcon() const
{
    return QIcon (":/cardbox-icon");
}

//---------------------------------------------------------------------------
//  getTitle
//
//! Returns the current title string.
//
//! @return the current title string
//---------------------------------------------------------------------------
QString
CardboxForm::getTitle() const
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
CardboxForm::getStatusString() const
{
    return QString::null;
}

//---------------------------------------------------------------------------
//  shiftClicked
//
//! Called when the Delay button is clicked.  Delay the cardbox contents.
//---------------------------------------------------------------------------
void
CardboxForm::shiftClicked()
{
    QApplication::setOverrideCursor (QCursor (Qt::WaitCursor));

    int desiredBacklog = backlogSbox->value();
    QString lexicon = wordEngine->getLexiconName();
    QString quizType = "Anagrams";
    // do some searching to get a limiting list of questions
    QStringList questions;

    QuizDatabase db (lexicon, quizType);
    db.shiftCardbox (questions, desiredBacklog);

    refreshClicked();

    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  refreshClicked
//
//! Called when the Refresh button is clicked.
//---------------------------------------------------------------------------
void
CardboxForm::refreshClicked()
{
    if (!quizDatabase)
        return;

    QApplication::setOverrideCursor (QCursor (Qt::WaitCursor));
    const QSqlDatabase* sqlDb = quizDatabase->getDatabase();

    // Refresh cardbox count list
    if (cardboxCountModel) {
        QString queryStr = "SELECT cardbox, count(*) FROM questions "
            "WHERE cardbox NOT NULL GROUP BY cardbox"; 
        cardboxCountModel->setQuery (queryStr, *sqlDb);
        cardboxCountModel->setHeaderData (0, Qt::Horizontal, "Cardbox");
        cardboxCountModel->setHeaderData (1, Qt::Horizontal, "Count");
    }

    // Refresh upcoming word counts
    if (cardboxDaysModel) {
        unsigned int now = std::time (0);
        QString queryStr =
            "SELECT round((next_scheduled - 43200.0 - %1) / 86400) AS days, "
            "count(*) FROM questions WHERE cardbox NOT NULL GROUP BY days";
        cardboxDaysModel->setQuery (queryStr.arg (now), *sqlDb);
        cardboxDaysModel->setHeaderData (0, Qt::Horizontal, "Days");
        cardboxDaysModel->setHeaderData (1, Qt::Horizontal, "Count");
    }

    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  quizSpecChanged
//
//! Called when the Lexicon or Quiz Type is changed by the user.
//---------------------------------------------------------------------------
void
CardboxForm::quizSpecChanged()
{
    QString lexicon = wordEngine->getLexiconName();
    QString quizType = "Anagrams";

    delete quizDatabase;
    quizDatabase = new QuizDatabase (lexicon, quizType);
    Q_CHECK_PTR (quizDatabase);

    delete cardboxCountModel;
    cardboxCountModel = new QSqlQueryModel;
    Q_CHECK_PTR (cardboxCountModel);
    cardboxCountView->setModel (cardboxCountModel);

    delete cardboxDaysModel;
    cardboxDaysModel = new QSqlQueryModel;
    Q_CHECK_PTR (cardboxDaysModel);
    cardboxDaysView->setModel (cardboxDaysModel);

    refreshClicked();
}
