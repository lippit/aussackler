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
#include "vat.h"
#include "entry.h"
#include "invest.h"

ASVat::ASVat(ASTransactionList * transactions, QObject * parent) :
    QObject(parent),
    m_transactions(transactions)
{
}

void ASVat::updateCalculation(int year, ASCategory * calcCategory)
{
    qDebug("Umsatzsteuer\n");

    double taxableBase = 0.0;
    double vatPaid = 0.0;

    QHash<const ASCategory *, double> category;

    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        const ASAccountEntry * ae = dynamic_cast<const ASAccountEntry*>(*it);
        if (!ae)
            continue;

        if (ae->getOverwrittenBy() || ae->getDate().year() != year)
            continue;

        if (ae->getCategory() == calcCategory)
        {
            taxableBase += ae->getAmount();
        }
        else
        {
            vatPaid -= ae->getVatAmount();
        }

        if (ae->getVatAmount() == 0)
        {
            qDebug("%s - %f", ae->getDescription().toUtf8().constData(), ae->getAmount());
        }
    }

    qDebug("\nBemessungsgrundlage: %f", taxableBase);
    qDebug("Vorsteuer: %f\n", vatPaid);
}
