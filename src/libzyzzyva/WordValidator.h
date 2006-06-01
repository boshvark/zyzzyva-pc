//---------------------------------------------------------------------------
// WordValidator.h
//
// A validator for ensuring words are well-formed.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mthelen@gmail.com>.
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

#ifndef ZYZZYVA_WORD_VALIDATOR_H
#define ZYZZYVA_WORD_VALIDATOR_H

#include <QRegExp>
#include <QValidator>

class WordValidator : public QValidator
{
    Q_OBJECT
    public:
    WordValidator (QObject* parent = 0)
      : QValidator (parent), options (None) { }

    QValidator::State validate (QString& input, int& pos) const;

    int  getOptions() const { return options; }
    void setOptions (int o) { options = o; }

    enum Options {
        None = 0,
        AllowQuestionMarks = 1,
        AllowAsterisks = 2,
        AllowCharacterClasses = 4,
        AllowHooks = 8
    };

    private:
    void replaceRegExp (const QRegExp& re, const QString& str,
                        QString& input, int& pos) const;

    int options;
};

#endif // ZYZZYVA_WORD_VALIDATOR_H
