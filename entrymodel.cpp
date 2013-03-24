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
#include "entrymodel.h"
#include "entry.h"

EntryModel::EntryModel(ASTransactionList * transactions, QObject * parent) :
    QAbstractTableModel(parent),
    ASTransactionSorter(transactions)
{
}

void EntryModel::transactionsChanged()
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

int EntryModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return rows() + 1;
}

int EntryModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return 10;
}

QVariant EntryModel::data(const QModelIndex& index, int role) const
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
            return tr("Buchungsdatum");
        case 2:
            return tr("Betrag Netto");
        case 3:
            return tr("Ust. Betrag");
        case 4:
            return tr("Ust. Steuersatz");
        case 5:
            return tr("Betr. Anteil");
        case 6:
            return tr("Konto");
        case 7:
            return tr("Kategorie");
        case 8:
            return tr("Beleg");
        case 9:
            return tr("Belegdatum");
        default:
            return QVariant();
        }
    }

    ASAccountEntry * e = getSubsetList()[index.row()-1];

    switch(index.column())
    {
    case 0:
        return e->getDescription();
    case 1:
        return e->getDate().toString(tr("dd.MM.yyyy"));
    case 2:
        return e->getAmount();
    case 3:
        return e->getVatAmount();
    case 4:
        if (e->getVatPercentage() >= 0.0)
            return QString::number(e->getVatPercentage()) + " %";
        else
            return "Unbekannt";
    case 5:
        return QString::number(e->getChargePercentage()) + " %";
    case 6:
        if (e->getAccount())
            return e->getAccount()->getDescription();
        else
            return "";
    case 7:
        if (e->getCategory())
            return e->getCategory()->getDescription();
        else
            return "";
    case 8:
        if (e->getDocument())
            return e->getDocument()->getDescription();
        else
            return "";
    case 9:
        if (e->getDocument())
            return e->getDocument()->getDocumentDate().
                toString(tr("dd.MM.yyyy"));
        else
            return "";
    }

    return QVariant();
}
