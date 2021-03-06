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

#ifndef DOCMODEL_H
#define DOCMODEL_H

#include <QAbstractTableModel>
#include "tsort.h"
#include "transaction.h"
#include "entry.h"
#include "document.h"

class DocModel : public QAbstractTableModel,
    public ASTransactionSorter<ASDocument*>
{
    Q_OBJECT

public:
    DocModel(ASTransactionList * transactions, QObject * parent = 0);

    virtual int rowCount(const QModelIndex& parent) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;

public slots:
    void transactionsChanged();
};

#endif
