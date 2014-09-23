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
#include "assetsmodel.h"
#include "entry.h"
#include "invest.h"

AssetsModel::AssetsModel(ASTransactionList * transactions, QObject * parent) :
    QAbstractTableModel(parent),
    ASTransactionSorter(transactions)
{
}

void AssetsModel::transactionsChanged()
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

int AssetsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return rows() + 1;
}

int AssetsModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return 5;
}

QVariant AssetsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
         return QVariant();

    if (role == Qt::FontRole)
    {
        if (index.row() == 0)
        {
            QFont f = QApplication::font();
            f.setBold(true);
            return f;
        }
        else if (index.row() > 0)
        {
            ASAccountEntry * e = getSubsetList()[index.row()-1];
            if (e->getOverwrittenBy() != NULL)
            {
                QFont f = QApplication::font();
                f.setStrikeOut(true);
                return f;
            }
        }
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    if (role == Qt::DisplayRole && index.row() == 0)
    {
        switch(index.column())
        {
        case 0:
            return tr("Beschreibung");
        case 1:
            return tr("Anschaffungsdatum");
        case 2:
            return tr("Abschreibdauer");
        case 3:
            return tr("Anschaffungswert");
        case 4:
            return trUtf8("Veränderung");
        default:
            return QVariant();
        }
    }

    ASInvestEntry * e = getSubsetList()[index.row()-1];

    switch(index.column())
    {
    case 0:
        return e->getDescription();
    case 1:
        return e->getDate().toString(tr("dd.MM.yyyy"));
    case 2:
        return e->getDepreciationPeriod();
    case 3:
        return e->getAmount();
    case 4:
    {
        double d = 0.0;
        for (int y = m_fromDate.year(); y <= m_toDate.year(); y++)
            d += e->getDepreciation(y);
        return d;
    }
    }

    return QVariant();
}
