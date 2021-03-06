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
#include "calc.h"
#include "entry.h"
#include "invest.h"

ASCalc::ASCalc(ASTransactionList * transactions, QObject * parent) :
    QObject(parent),
    m_transactions(transactions)
{
}

QString ASCalc::getCalculation(const QDate& fromDate, const QDate& toDate,
                               ASCategory * depreciationCategory)
{
    QString report;

    QHash<const ASCategory *, double> category;

    double total = 0.0;
    int indent = 0;

    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        const ASCategory * cat = dynamic_cast<const ASCategory*>(*it);
        if (cat)
        {
            category.insert(cat, 0.0);
            if (cat->getDescription().length() > indent)
            {
                indent = cat->getDescription().length();
            }
        }
    }
    indent += 3;

    it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        const ASAccountEntry * ae = dynamic_cast<const ASAccountEntry*>(*it);
        if (!ae)
            continue;

        if (ae->getOverwrittenBy() || ae->getHidden())
            continue;

        const ASInvestEntry * ie = dynamic_cast<const ASInvestEntry*>(ae);

        if (ie)
        {
            for (int y = fromDate.year(); y <= toDate.year(); y++)
            {
                double depreciation = ie->getDepreciation(y) * ie->getChargePercentage() / 100.0;
                if (depreciationCategory)
                {
                    category[depreciationCategory] += depreciation;
                }
                total += depreciation;
            }
        }
        else
        {
            if (ae->getDate() >= fromDate && ae->getDate() <= toDate)
            {
                double charge = ae->getAmount() * ae->getChargePercentage() / 100.0;
                category[ae->getCategory()] += (double)(qRound(charge * 100)) / 100.0;
                total += charge;
            }
        }
    }

    QHashIterator<const ASCategory *, double> hit(category);
    while (hit.hasNext()) {
        hit.next();
        QString line = QString("%1 %2\n")
            .arg(hit.key()->getDescription(), -indent)
            .arg(qAbs(hit.value()), 9, 'f', 2);
        report.append(line);
    }

    QString line = QString("\n%1 %2\n")
        .arg("Gesamt Gewinn/Verlust:", total >= 0.0 ? -indent : 1-indent)
        .arg(total, 9, 'f', 2);
    report.append(line);

    return report;
}
