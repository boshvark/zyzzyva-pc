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
      : QValidator (parent, name) { }

    QValidator::State validate (QString& input, int& pos) const;

  private:
    void replaceRegExp (const QRegExp& re, const QString& str,
                        QString& input, int& pos) const;
};

#endif // WORD_VALIDATOR_H
