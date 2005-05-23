//---------------------------------------------------------------------------
// SearchSpecForm.h
//
// A form for specifying a search specification.
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

#ifndef SEARCH_SPEC_FORM_H
#define SEARCH_SPEC_FORM_H

#include "SearchSpec.h"
#include "MatchType.h"
#include <qcombobox.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <set>

class SearchSpecForm : public QFrame
{
  Q_OBJECT
  public:
    SearchSpecForm (QWidget* parent = 0, const char* name = 0, WFlags f = 0);

    void reset();
    SearchSpec getSearchSpec() const;
    MatchType getMatchType() const;
    QString getPattern() const { return patternLine->text(); }
    QString getIncludeLetters() const { return includeLine->text(); }
    QString getExcludeLetters() const { return excludeLine->text(); }
    int getConsistPercent() const { return consistPctSbox->value(); }
    QString getConsistLetters() const { return consistLine->text(); }
    int getMinLength() const { return minLengthSbox->value(); }
    int getMaxLength() const { return maxLengthSbox->value(); }
    std::set<SearchSet> getSetMemberships() const;

  signals:
    void returnPressed();
    void patternChanged (const QString&);

  public slots:
    void minLengthChanged (int);
    void maxLengthChanged (int);

  private:
    QComboBox* matchCbox;
    QLineEdit* patternLine;
    QLineEdit* includeLine;
    QLineEdit* excludeLine;
    QSpinBox*  consistPctSbox;
    QLineEdit* consistLine;
    QSpinBox*  minLengthSbox;
    QSpinBox*  maxLengthSbox;
    QListView* setMembershipList;
};

#endif // SEARCH_SPEC_FORM_H
