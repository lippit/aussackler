/*
 * Copyright (c) 2013 Georg Lippitsch
 *
 * This file is part of Aussackler.
 *
 * Aussackler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Aussackler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Aussackler.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CALC_H
#define CALC_H

#include "tsort.h"
#include "transaction.h"
#include "entry.h"

class ASCalc : public QObject
{
    Q_OBJECT

public:
    ASCalc(ASTransactionList * transactions, QObject * parent = NULL);

    QString getCalculation(int year, ASCategory * depreciationCategory);

protected:
    ASTransactionList * m_transactions;
};

#endif
