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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "ui_account.h"
#include "ui_category.h"
#include "ui_doclist.h"
#include "entrywindow.h"
#include "transaction.h"
#include "entrymodel.h"
#include "docmodel.h"

class ASMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ASMainWindow(ASTransactionList * transactions,
                 QWidget * parent = 0);

private slots:
    void on_actionNew_activated();
    void on_actionLoad_activated();
    void on_actionSave_activated();
    void on_actionSaveas_activated();
    void on_actionQuit_activated();

    void on_actionAddEntries_activated();
    void on_actionNewEntry_activated();
    void on_actionNewAccount_activated();
    void on_actionNewCategory_activated();

    void createAccount();
    void createCategory();

private:
    QString m_currentFileName;
    Ui::MainWindow ui;
    Ui::AccountWindow accountUi;
    Ui::CategoryWindow categoryUi;
    Ui::DocumentsWindow docUi;
    ASEntryWindow * entryWindow;
    QDialog * accountDialog;
    QDialog * categoryDialog;
    QDialog * docDialog;
    ASTransactionList * m_transactions;
    EntryModel * m_entryModel;
    DocModel * m_docModel;
    QString m_fileFilter;
    bool m_populate;

    void createEntryWindow();

    void connectModels();
    void disconnectModels();
};

#endif
