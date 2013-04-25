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
#include "assets.h"
#include "entry.h"
#include "invest.h"

ASAssets::ASAssets(ASTransactionList * transactions, QObject * parent) :
    QObject(parent),
    m_transactions(transactions)
{
}

void ASAssets::updateAssetsList()
{
    double total = 0.0;

    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        const ASInvestEntry * ie = dynamic_cast<const ASInvestEntry*>(*it);

        if (!ie)
            continue;

        if (ie->getOverwrittenBy())
            continue;

        total += ie->getDepreciation(2011);

        qDebug("%s %f", ie->getDescription().toUtf8().constData(), ie->getDepreciation(2011));
    }

    qDebug("Total: %f", total);
}
