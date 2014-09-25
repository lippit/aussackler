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

#ifndef TSORT_H
#define TSORT_H

#include "transaction.h"
#include "entry.h"

template <class T> class ASTransactionSorter
{
public:
    ASTransactionSorter(ASTransactionList * transactions)
    {
        m_transactions = transactions;
        m_lastTransactionSize = 0;
    }

    int sortoutTransactions()
    {
        if (m_lastTransactionSize == m_transactions->size())
            return 0;

        int sizeBefore = m_subsetList.size();

        for (int i = m_lastTransactionSize; i < m_transactions->size(); ++i)
        {
            T e = dynamic_cast<T>
                (m_transactions->at(i));
            if (e != NULL && !e->getHidden())
            {
                m_subsetList.append(e);
            }
        }

        m_lastTransactionSize = m_transactions->size();

        return m_subsetList.size() - sizeBefore;
    }

    QList<T> getSubsetList() const {return m_subsetList;}

    T getTransactionByRow(int row)
    {
        QList<T> l = getSubsetList();
        int r = row - 1;
        if (r < 0 || l.size() <= r)
            return NULL;
        return l.at(r);
    }

    int rows() const {return m_subsetList.size();}

protected:
    ASTransactionList * m_transactions;
    QList<T> m_subsetList;
    int m_lastTransactionSize;
};

#endif
