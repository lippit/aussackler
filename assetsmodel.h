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

#ifndef ASSETSMODEL_H
#define ASSETSMODEL_H

#include <QAbstractTableModel>
#include <QDate>
#include "tsort.h"
#include "transaction.h"
#include "invest.h"

class AssetsModel : public QAbstractTableModel,
    public ASTransactionSorter<ASInvestEntry*>
{
    Q_OBJECT

public:
    AssetsModel(ASTransactionList * transactions, QObject * parent = 0);

    virtual int rowCount(const QModelIndex& parent) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;

    void setPeriod(const QDate& fromDate, const QDate& toDate)
    {m_fromDate = fromDate; m_toDate = toDate;}

public slots:
    void transactionsChanged();

private:
    QDate m_fromDate;
    QDate m_toDate;
};

#endif
