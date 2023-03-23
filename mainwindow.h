//
// Created by dmonnie5 on 22/03/23.
//

#ifndef PROJET_BUDGET_MAINWINDOW_H
#define PROJET_BUDGET_MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QTabWidget *tabWidget;
    QWidget *tabExpenses;
    QWidget *tabStatistics;
    QLabel *lblName;
    QLabel *lblPrice;
    QLabel *lblDate;
    QLabel *lblCategory;
    QLineEdit *txtName;
    QSpinBox *spnPrice;
    QComboBox *cboCategory;
    QTableWidget *tblExpenses;
    QPushButton *btnAddExpense;
    QHBoxLayout *hbxExpenses;
    QVBoxLayout *vbxExpenses;
    QVBoxLayout *vbxStatistics;

    void setupExpensesTab();
    void setupStatisticsTab();
    void onAddExpenseClicked();
};


#endif //PROJET_BUDGET_MAINWINDOW_H
