//---------------------------------------------------------------------------
// SearchForm.h
//
// A form for searching for words, patterns, anagrams, etc.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef SEARCH_FORM_H
#define SEARCH_FORM_H

#include <qlineedit.h>
#include <qlistbox.h>
#include <qradiobutton.h>

class WordEngine;

class SearchForm : public QFrame
{
  Q_OBJECT
  public:
    SearchForm (WordEngine* e, QWidget* parent = 0, const char* name = 0,
                WFlags f = 0);

  public slots:
    void search();

  private:
    WordEngine*   engine;
    QRadioButton* patternButton;
    QRadioButton* anagramButton;
    QRadioButton* subanagramButton;
    QLineEdit*    wordLine;
    QListBox*     resultList;
};

#endif // SEARCH_FORM_H
