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

#ifndef ENTRYWINDOW_H
#define ENTRYWINDOW_H

#include "ui_entry.h"
#include "ui_doclist.h"
#include "transaction.h"
#include "document.h"

class ASEntryWindow : public QDialog
{
    Q_OBJECT

public:
    ASEntryWindow(ASTransactionList * transactions,
                  Ui::DocumentsWindow * docUi,
                  QDialog * docDialog,
                  QWidget * parent = 0);

    void populateLists();
    void setOverride(ASTransaction * override);

private slots:
    void on_expense_toggled(bool checked);
    void on_income_toggled(bool checked);
    void on_invest_toggled(bool checked);
    void on_amount_textChanged();
    void on_vatPercentage_currentIndexChanged(int index);
    void on_buttonBox_accepted();

    void chooseDocument(bool buttonOn);
    void documentSelected();
    void documentSetup();
    void documentReset();

private:
    Ui::EntryWindow ui;
    ASTransactionList * m_transactions;
    Ui::DocumentsWindow * m_docUi;
    QDialog * m_docDialog;
    ASDocument * m_selectedDocument;
    ASTransaction * m_override;

    void calculateVat();
};

#endif
