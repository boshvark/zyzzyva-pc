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
const int REFRESH_MSECS = 120000;

//---------------------------------------------------------------------------
//  CardboxForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CardboxForm::CardboxForm(WordEngine* e, QWidget* parent, Qt::WFlags f)
    : ActionForm(CardboxFormType, parent, f), wordEngine(e),
    quizDatabase(0), cardboxCountModel(0), cardboxDaysModel(0),
    cardboxContentsModel(0)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);
    Q_CHECK_PTR(mainVlay);

    QHBoxLayout* cardboxHlay = new QHBoxLayout;
    Q_CHECK_PTR(cardboxHlay);
    cardboxHlay->setSpacing(SPACING);
    mainVlay->addLayout(cardboxHlay);

    cardboxCountView = new QTreeView;
    Q_CHECK_PTR(cardboxCountView);
    cardboxHlay->addWidget(cardboxCountView);

    cardboxDaysView = new QTreeView;
    Q_CHECK_PTR(cardboxDaysView);
    cardboxHlay->addWidget(cardboxDaysView);

    cardboxContentsView = new QTreeView;
    Q_CHECK_PTR(cardboxContentsView);
    mainVlay->addWidget(cardboxContentsView);

    ZPushButton* refreshButton = new ZPushButton;
    Q_CHECK_PTR(refreshButton);
    refreshButton->setText("&Refresh");
    refreshButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(refreshButton, SIGNAL(clicked()), SLOT(refreshClicked()));
    mainVlay->addWidget(refreshButton);

    mainVlay->addStretch(0);

    quizSpecChanged();

    //connect(&refreshTimer, SIGNAL(timeout()), SLOT(refreshClicked()));
    //refreshTimer.start(REFRESH_MSECS);
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
    return QIcon(":/cardbox-icon");
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
    return QString();
}

//---------------------------------------------------------------------------
//  refreshClicked
//
//! Called when the Refresh button is clicked.
//---------------------------------------------------------------------------
void
CardboxForm::refreshClicked()
{
    qDebug("CardboxForm::refreshClicked");
    if (!quizDatabase)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    const QSqlDatabase* sqlDb = quizDatabase->getDatabase();

    // Refresh cardbox count list
    if (cardboxCountModel) {
        QString queryStr = "SELECT cardbox, count(*) FROM questions "
            "WHERE cardbox NOT NULL GROUP BY cardbox"; 
        cardboxCountModel->setQuery(queryStr, *sqlDb);
        cardboxCountModel->setHeaderData(0, Qt::Horizontal, "Cardbox");
        cardboxCountModel->setHeaderData(1, Qt::Horizontal, "Count");
    }

    // Refresh upcoming word counts
    if (cardboxDaysModel) {
        unsigned int now = QDateTime::currentDateTime().toTime_t();

        QString queryStr =
            "SELECT round((next_scheduled - 43200.0 - %1) / 86400) AS days, "
            "count(*) FROM questions WHERE cardbox NOT NULL GROUP BY days";
        cardboxDaysModel->setQuery(queryStr.arg(now), *sqlDb);
        cardboxDaysModel->setHeaderData(0, Qt::Horizontal, "Days");
        cardboxDaysModel->setHeaderData(1, Qt::Horizontal, "Count");
    }

    // Refresh cardbox contents
    if (cardboxContentsModel) {
        QString queryStr =
            "SELECT question, cardbox, correct, incorrect, streak, "
            "next_scheduled FROM questions WHERE cardbox NOT NULL "
            "ORDER BY next_scheduled";


        //cardboxContentsModel->select();

        cardboxContentsModel->setQuery(queryStr, *sqlDb);
        //while (cardboxContentsModel->canFetchMore()) {
        //    qDebug("Fetching more for cardbox contents model");
        //    cardboxContentsModel->fetchMore();
        //}

        //cardboxContentsModel->setSort(7, Qt::AscendingOrder);
        cardboxContentsModel->setHeaderData(0, Qt::Horizontal, "Question");
        cardboxContentsModel->setHeaderData(1, Qt::Horizontal, "Cardbox");
        cardboxContentsModel->setHeaderData(2, Qt::Horizontal, "Correct");
        cardboxContentsModel->setHeaderData(3, Qt::Horizontal, "Incorrect");
        cardboxContentsModel->setHeaderData(4, Qt::Horizontal, "Streak");
        cardboxContentsModel->setHeaderData(5, Qt::Horizontal, "Next Scheduled");
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
    quizDatabase = new QuizDatabase(lexicon, quizType);
    Q_CHECK_PTR(quizDatabase);

    delete cardboxCountModel;
    cardboxCountModel = new QSqlQueryModel;
    Q_CHECK_PTR(cardboxCountModel);
    cardboxCountView->setModel(cardboxCountModel);

    delete cardboxDaysModel;
    cardboxDaysModel = new QSqlQueryModel;
    Q_CHECK_PTR(cardboxDaysModel);
    cardboxDaysView->setModel(cardboxDaysModel);

    //const QSqlDatabase* sqlDb = quizDatabase->getDatabase();
    delete cardboxContentsModel;
    cardboxContentsModel = new QSqlQueryModel;
    //cardboxContentsModel = new QSqlTableModel(this, *sqlDb);
    Q_CHECK_PTR(cardboxContentsModel);

    //cardboxContentsModel->setTable("questions");
    //cardboxContentsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //cardboxContentsModel->setSort(7, Qt::AscendingOrder);
    //cardboxContentsModel->setHeaderData(0, Qt::Horizontal, "Question");
    //cardboxContentsModel->setHeaderData(1, Qt::Horizontal, "Correct");
    //cardboxContentsModel->setHeaderData(2, Qt::Horizontal, "Incorrect");
    //cardboxContentsModel->setHeaderData(3, Qt::Horizontal, "Streak");
    //cardboxContentsModel->setHeaderData(4, Qt::Horizontal, "Last Correct");
    //cardboxContentsModel->setHeaderData(5, Qt::Horizontal, "Difficulty");
    //cardboxContentsModel->setHeaderData(6, Qt::Horizontal, "Cardbox");
    //cardboxContentsModel->setHeaderData(7, Qt::Horizontal, "Next Scheduled");

    cardboxContentsView->setModel(cardboxContentsModel);

    refreshClicked();
}
