//---------------------------------------------------------------------------
// IntroForm.cpp
//
// A form for displaying introductory help material.
//
// Copyright 2006-2012 Boshvark Software, LLC.
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

#include "IntroForm.h"
#include "Auxil.h"
#include <QTextBrowser>
#include <QVBoxLayout>

const QString TITLE_PREFIX = "Welcome";

//---------------------------------------------------------------------------
//  IntroForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
IntroForm::IntroForm(QWidget* parent, Qt::WFlags f)
    : ActionForm(IntroFormType, parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);

    QTextBrowser* browser = new QTextBrowser(this);
    Q_CHECK_PTR(browser);
    mainVlay->addWidget(browser);

    QString mainPage = Auxil::getHelpDir() + "/index.html";
    browser->setSource(QUrl::fromLocalFile(mainPage));
}

//---------------------------------------------------------------------------
//  getIcon
//
//! Returns the current icon.
//
//! @return the current icon
//---------------------------------------------------------------------------
QIcon
IntroForm::getIcon() const
{
    return QIcon(":/help-icon");
}

//---------------------------------------------------------------------------
//  getTitle
//
//! Returns the current title string.
//
//! @return the current title string
//---------------------------------------------------------------------------
QString
IntroForm::getTitle() const
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
IntroForm::getStatusString() const
{
    return QString();
}
