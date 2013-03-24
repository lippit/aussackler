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

#include <QApplication>
#include <QFont>
#include "docmodel.h"

DocModel::DocModel(ASTransactionList * transactions, QObject * parent) :
    QAbstractTableModel(parent),
    ASTransactionSorter(transactions)
{
}

void DocModel::transactionsChanged()
{
    int rowsBefore = rows();
    int n = sortoutTransactions();
    if (n)
    {
        QModelIndex index;
        beginInsertRows(index, rowsBefore + 1, rowsBefore + n);
        endInsertRows();
    }
}

int DocModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return rows() + 1;
}

int DocModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return 4;
}

QVariant DocModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
         return QVariant();

    if (role == Qt::FontRole && index.row() == 0)
    {
        QFont f = QApplication::font();
        f.setBold(true);
        return f;
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.row() == 0)
    {
        switch(index.column())
        {
        case 0:
            return tr("Beschreibung");
        case 1:
            return tr("Belegdatum");
        case 2:
            return tr("Belegnummer");
        case 3:
            return tr("Interne ID");
        default:
            return QVariant();
        }
    }

    ASDocument * e = getSubsetList()[index.row()-1];

    switch(index.column())
    {
    case 0:
        return e->getDescription();
    case 1:
        return e->getDocumentDate();
    case 2:
        return e->getNumber();
    case 3:
        return e->getId();
    }

    return QVariant();
}

ASDocument * DocModel::getDocumentByRow(int row)
{
    QList<ASDocument*> l = getSubsetList();
    int r = row - 1;
    if (r < 0 || l.size() <= r)
        return NULL;
    return l.at(r);
}
