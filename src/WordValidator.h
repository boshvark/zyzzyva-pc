//---------------------------------------------------------------------------
// WordValidator.h
//
// A validator for ensuring words are well-formed.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef WORD_VALIDATOR_H
#define WORD_VALIDATOR_H

#include <qregexp.h>
#include <qvalidator.h>

class WordValidator : public QValidator
{
  Q_OBJECT
  public:
    WordValidator (QObject* parent = 0, const char* name = 0)
      : QValidator (parent, name), options (None) { }

    QValidator::State validate (QString& input, int& pos) const;

    int  getOptions() const { return options; }
    void setOptions (int o) { options = o; }

    enum Options {
        None = 0,
        AllowQuestionMarks = 1,
        AllowAsterisks = 2
    };

  private:
    void replaceRegExp (const QRegExp& re, const QString& str,
                        QString& input, int& pos) const;

    int options;
};

#endif // WORD_VALIDATOR_H
