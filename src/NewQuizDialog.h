//---------------------------------------------------------------------------
// NewQuizDialog.h
//
// A dialog for prompting the user for a quiz.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef NEW_QUIZ_DIALOG_H
#define NEW_QUIZ_DIALOG_H

#include <qcheckbox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include "MatchType.h"

class NewQuizDialog : public QDialog
{
  Q_OBJECT
  public:
    NewQuizDialog (QWidget* parent = 0, const char* name = 0,
                   bool modal = false, WFlags f = 0);

    ~NewQuizDialog() { }

    MatchType getQuizType() const;
    bool      getQuizAlphagrams() const;
    bool      getQuizRandomOrder() const;
    QString   getQuizString() const;

  public slots:
    void alphagramsToggled (bool on);
    void inputChanged (const QString& text);

  private:
    QRadioButton* patternButton;
    QRadioButton* anagramButton;
    QRadioButton* subanagramButton;
    QCheckBox*    alphagramCbox;
    QCheckBox*    randomCbox;
    QLineEdit*    inputLine;
    QPushButton*  okButton;
};

#endif // NEW_QUIZ_DIALOG_H

