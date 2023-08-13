#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "iostream"
#include "stdio.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  calculate_button = ui->calculateButton;
  connect(calculate_button, &QPushButton::pressed, this,
          &MainWindow::handleCalculateButton);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::handleCalculateButton() {
  try {
    QString string = ui->expressionTextEdit->toPlainText();
    QString Xstring = ui->xTextEdit->displayText();
    if (string.size() >= 256 || Xstring.size() >= 256) {
      ui->resultTextEdit->setPlainText(QString("Input too big"));
      return;
    }

    char expression[1024];
    for (int i = 0; i < 1024; i++) expression[i] = '\0';
    std::string stdString = string.toStdString();
    for (int i = 0; i < stdString.size(); i++) {
      expression[i] = stdString.at(i);
    }
    expression[stdString.size()] = '\0';

    bool error = false;
    double XValue = Xstring.toDouble(&error);
    List infix = parseString(expression, &error);
    List polish = toPolish(infix, &error);
    if (error == false) {
      char *polishString = getAsString(polish, ' ');
      ui->polishNotationTextEdit->setPlainText(QString(polishString));
      free(polishString);
    }
    double result = calculateForX(polish, XValue, &error);

    if (error == true)
      ui->resultTextEdit->setPlainText(QString("ERROR"));
    else {
      QString resQString = QString::number(result);

      ui->resultTextEdit->setPlainText(resQString);
    }

    bool er;
    double Xmin = ui->Xmin->displayText().toDouble(&er);
    double Xmax = ui->Xmax->displayText().toDouble(&er);
    double num = 1000000;
    QVector<double> x(num), y(num);
    for (int i = 0; i < num; i++) {
      x[i] = Xmin + (Xmax - Xmin) / num * i;
      y[i] = calculateForX(polish, x[i], &error);
    }

    QCustomPlot *plot = ui->Plot;
    plot->addGraph();
    plot->graph(0)->setData(x, y);
    plot->xAxis->setRange(Xmin, Xmax);
    if (ui->Yauto->isChecked()) {
      double Ymin = y[0];
      double Ymax = y[0];
      for (auto i : y) {
        if (i < Ymin) Ymin = i;
        if (i > Ymax) Ymax = i;
      }
      plot->yAxis->setRange(Ymin, Ymax);
      ui->Ymin->setText(QString::number(Ymin));
      ui->Ymax->setText(QString::number(Ymax));
    } else {
      plot->yAxis->setRange(ui->Ymin->displayText().toDouble(&er),
                            ui->Ymax->displayText().toDouble(&er));
    }
    plot->replot();

    listFree(&infix);
    listFree(&polish);
  } catch (std::exception &e) {
    ui->resultTextEdit->setPlainText(QString("ERROR"));
  }
}
