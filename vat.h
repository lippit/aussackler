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

#ifndef VAT_H
#define VAT_H

#include "tsort.h"
#include "transaction.h"
#include "entry.h"

class ASVat : public QObject
{
    Q_OBJECT

public:
    ASVat(ASTransactionList * transactions, QObject * parent = NULL);

    void updateCalculation(int year, ASCategory * calcCategory);

protected:
    ASTransactionList * m_transactions;
};

#endif
