//
// Created by dmonnie5 on 22/03/23.
//
#include <QDate>
#include "mainwindow.h"
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChartView>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

using namespace QtCharts;


PriceTableWidgetItem::PriceTableWidgetItem(double price)
    : QTableWidgetItem(QString::number(price, 'f', 2))
{
}

bool PriceTableWidgetItem::operator <(const QTableWidgetItem &other) const
{
    return this->text().toDouble() < other.text().toDouble();
}

DateTableWidgetItem::DateTableWidgetItem(const QDate &date)
    : QTableWidgetItem(date.toString("dd/MM/yyyy"))
{
}

bool DateTableWidgetItem::operator <(const QTableWidgetItem &other) const
{
    QDate date1 = QDate::fromString(this->text(), "dd/MM/yyyy");
    QDate date2 = QDate::fromString(other.text(), "dd/MM/yyyy");
    return date1 < date2;
}

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    tabWidget = new QTabWidget(this);

    setupExpensesTab();
    tabWidget->addTab(tabExpenses, "Dépenses");

    setupStatisticsTab();
    tabWidget->addTab(tabStatistics, "Statistiques");

    setCentralWidget(tabWidget);
    setWindowTitle("Gestion de budget");
    setMinimumSize(800, 600);

    connect(tabWidget, &QTabWidget::currentChanged, [this](int index) {
        if (index == 1) {
            updatePieChart();
            if (tblStatistics) {
                updateStatisticsTab();
            }
        }
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupExpensesTab()
{
    // Create the widgets for the Expenses tab
    tabExpenses = new QWidget();
    lblName = new QLabel("Nom:");
    lblPrice = new QLabel("Prix:");
    lblDate = new QLabel("Date:");
    lblCategory = new QLabel("Categorie:");
    txtName = new QLineEdit();
    spnPrice = new QDoubleSpinBox();
    spnPrice->setRange(0, 1000000);
    spnPrice->setSingleStep(0.01);
    spnPrice->setDecimals(2);
    cboCategory = new QComboBox();
    tblExpenses = new QTableWidget();
    btnAddExpense = new QPushButton("Ajouter");
    hbxExpenses = new QHBoxLayout();
    vbxExpenses = new QVBoxLayout();
    QPushButton *btnDeleteExpense = new QPushButton("Supprimer");
    hbxExpenses->addWidget(btnDeleteExpense);
    btnExport = new QPushButton("Exporter");
    hbxExpenses->addWidget(btnExport);

    cboCategory->addItem("Nouveau...");
    cboCategory->addItem("Nourriture");
    cboCategory->addItem("Maison");
    cboCategory->addItem("Transport");
    cboCategory->addItem("Loisir");
    cboCategory->addItem("Autre");

    cboSortBy = new QComboBox();
    btnSort = new QPushButton("Trier");
    chkReverseOrder = new QCheckBox("Ordre inverse");

    cboSortBy->addItem("Nom");
    cboSortBy->addItem("Prix");
    cboSortBy->addItem("Date");
    cboSortBy->addItem("Categorie");

    QHBoxLayout *hbxSorting = new QHBoxLayout();
    hbxSorting->addWidget(new QLabel("Trier par:"));
    hbxSorting->addWidget(cboSortBy);
    hbxSorting->addWidget(chkReverseOrder);
    hbxSorting->addWidget(btnSort);
    vbxExpenses->addLayout(hbxSorting);

    tblExpenses->setColumnCount(4);
    QStringList headers;
    headers << "Nom" << "Prix" << "Date" << "Categorie";
    tblExpenses->setHorizontalHeaderLabels(headers);

    dateEdit = new QDateEdit();
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setDisplayFormat("dd/MM/yyyy");

    hbxExpenses->addWidget(lblName);
    hbxExpenses->addWidget(txtName);
    hbxExpenses->addWidget(lblPrice);
    hbxExpenses->addWidget(spnPrice);
    hbxExpenses->addWidget(lblDate);
    hbxExpenses->addWidget(dateEdit);
    hbxExpenses->addWidget(lblCategory);
    hbxExpenses->addWidget(cboCategory);
    hbxExpenses->addWidget(btnAddExpense);
    vbxExpenses->addLayout(hbxExpenses);
    vbxExpenses->addWidget(tblExpenses);
    tabExpenses->setLayout(vbxExpenses);

    connect(btnExport, &QPushButton::clicked, this, &MainWindow::onExportClicked);
    connect(btnSort, &QPushButton::clicked, this, &MainWindow::sortExpenses);
    connect(btnDeleteExpense, &QPushButton::clicked, this, &MainWindow::onDeleteExpenseClicked);

    // Connect the add button to a slot that will check if the selected category is "Nouveau..."
    // and show a dialog to add a new category
    connect(btnAddExpense, &QPushButton::clicked, [this]() {
        if (cboCategory->currentText() == "Nouveau...") {
            bool ok;
            QString newCategory = QInputDialog::getText(this, tr("Ajouter une catégorie"),
                                                        tr("Nom de la nouvelle catégorie à ajouter:"), QLineEdit::Normal,
                                                        "", &ok);
            if (ok && !newCategory.isEmpty()) {
                cboCategory->addItem(newCategory);
                cboCategory->setCurrentText(newCategory);
            }
        } else {
            onAddExpenseClicked();
        }
    });

}


void MainWindow::setupStatisticsTab()
{
    tabStatistics = new QWidget();
    vbxStatistics = new QVBoxLayout();

    // Add controls to select start and end dates
    QHBoxLayout *hbxDateSelection = new QHBoxLayout();
    QLabel *lblDateStart = new QLabel("Date de début:");
    QLabel *lblDateEnd = new QLabel("Date de fin:");
    dateEditStart = new QDateEdit();
    dateEditEnd = new QDateEdit();
    btnApplyDates = new QPushButton("Appliquer");

    dateEditStart->setDate(QDate(2000, 1, 1));
    dateEditStart->setDisplayFormat("dd/MM/yyyy");
    dateEditEnd->setDate(QDate(2100, 1, 1));
    dateEditEnd->setDisplayFormat("dd/MM/yyyy");

    hbxDateSelection->addWidget(lblDateStart);
    hbxDateSelection->addWidget(dateEditStart);
    hbxDateSelection->addWidget(lblDateEnd);
    hbxDateSelection->addWidget(dateEditEnd);
    hbxDateSelection->addWidget(btnApplyDates);
    vbxStatistics->addLayout(hbxDateSelection);

    // Create pie chart
    QPieSeries *series = new QPieSeries();
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Répartition des dépenses par catégorie"));
    chart->legend()->setVisible(true);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    vbxStatistics->addWidget(chartView);

    tblStatistics = new QTableWidget();
    tblStatistics->setColumnCount(2);
    QStringList headers;
    headers << "Categorie" << "Total";
    tblStatistics->setHorizontalHeaderLabels(headers);
    vbxStatistics->addWidget(tblStatistics);

    tabStatistics->setLayout(vbxStatistics);

    // Connect the Apply button to update the statistics
    connect(btnApplyDates, &QPushButton::clicked, [this]() {
            updatePieChart();
            updateStatisticsTab();
        });
}


void MainWindow::updatePieChart()
{
    QChart *chart = chartView->chart();
    QPieSeries *series = qobject_cast<QPieSeries*>(chart->series().at(0));
    series->clear();

    QMap<QString, double> totals;

    QDate startDate = dateEditStart->date();
    QDate endDate = dateEditEnd->date();

    // Calculate the totals for each category according to the selected period
    for (int i = 0; i < tblExpenses->rowCount(); i++) {
        QString category = tblExpenses->item(i, 3)->text();
        double price = tblExpenses->item(i, 1)->text().toDouble();
        QDate expenseDate = QDate::fromString(tblExpenses->item(i, 2)->text(), "dd/MM/yyyy");

        if (expenseDate >= startDate && expenseDate <= endDate) {
            totals[category] += price;
        }
    }

    // Add the totals to the pie chart
    for (auto it = totals.begin(); it != totals.end(); ++it) {
        if (it.value() > 0) {
            QPieSlice *slice = new QPieSlice(it.key(), it.value());
            series->append(slice);
        }
    }
}



void MainWindow::updateStatisticsTab() {
    QMap<QString, double> totals;

    QDate startDate = dateEditStart->date();
    QDate endDate = dateEditEnd->date();

    // Calculate the totals for each category according to the selected period
    for (int i = 0; i < tblExpenses->rowCount(); i++) {
        QString category = tblExpenses->item(i, 3)->text();
        double price = tblExpenses->item(i, 1)->text().toDouble();
        QDate expenseDate = QDate::fromString(tblExpenses->item(i, 2)->text(), "dd/MM/yyyy");

        if (expenseDate >= startDate && expenseDate <= endDate) {
            totals[category] += price;
        }
    }

    // Clear the statistics table
    tblStatistics->setRowCount(0);

    // Add each category total to the statistics table
    int row = 0;
    for (auto it = totals.begin(); it != totals.end(); ++it) {
        if (it.value() > 0) {
            this->tblStatistics->insertRow(row);
            this->tblStatistics->setItem(row, 0, new QTableWidgetItem(it.key()));
            this->tblStatistics->setItem(row, 1, new QTableWidgetItem(QString::number(it.value(), 'f', 2)));
            row++;
        }
    }
}


void MainWindow::onAddExpenseClicked()
{
    // Get the input values
    QString name = txtName->text();
    double price = spnPrice->value();
    QString category = cboCategory->currentText();
    QDate date = dateEdit->date();

    // Create a new row and add it to the table
    int row = tblExpenses->rowCount();
    tblExpenses->insertRow(row);
    tblExpenses->setItem(row, 0, new QTableWidgetItem(name));
    tblExpenses->setItem(row, 1, new PriceTableWidgetItem(price));
    tblExpenses->setItem(row, 2, new DateTableWidgetItem(date));
    tblExpenses->setItem(row, 3, new QTableWidgetItem(category));

    updateStatisticsTab();
    updatePieChart();
}

void MainWindow::sortExpenses() {
    int columnIndex = cboSortBy->currentIndex();
    Qt::SortOrder order = chkReverseOrder->isChecked() ? Qt::DescendingOrder : Qt::AscendingOrder;
    tblExpenses->sortByColumn(columnIndex, order);
}

void MainWindow::onDeleteExpenseClicked()
{
    QModelIndexList selectedRows = tblExpenses->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez sélectionner une dépense à supprimer. Cliquez sur le numéro de ligne."));
        return;
    }

    for (const QModelIndex &index : selectedRows)
    {
        tblExpenses->removeRow(index.row());
    }

    updateStatisticsTab();
    updatePieChart();
    
    // Clear the input fields
    txtName->clear();
    spnPrice->setValue(0);
}

void MainWindow::onExportClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Exporter les dépenses au format CSV"), "", tr("Fichiers CSV (*.csv);;Tous les fichiers (*)"));

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'ouvrir le fichier pour l'écriture."));
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    // Écrire les en-têtes de colonne
    QStringList headers;
    headers << "Nom" << "Prix" << "Date" << "Categorie";
    out << headers.join(",") << "\n";

    // Écrire les données du tableau
    for (int i = 0; i < tblExpenses->rowCount(); ++i) {
        QStringList rowData;
        for (int j = 0; j < tblExpenses->columnCount(); ++j) {
            rowData << tblExpenses->item(i, j)->text();
        }
        out << rowData.join(",") << "\n";
    }

    file.close();

    QMessageBox::information(this, tr("Succès"), tr("Les données ont été exportées avec succès."));
}

