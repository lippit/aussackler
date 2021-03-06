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

#ifndef ENTRYMODEL_H
#define ENTRYMODEL_H

#include <QAbstractTableModel>
#include "tsort.h"
#include "transaction.h"
#include "entry.h"

class EntryModel : public QAbstractTableModel,
    public ASTransactionSorter<ASAccountEntry*>
{
    Q_OBJECT

public:
    EntryModel(ASTransactionList * transactions, QObject * parent = 0);

    virtual int rowCount(const QModelIndex& parent) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    ASAccountEntry * getEntry(const QModelIndex& index);

public slots:
    void transactionsChanged();
};

#endif
