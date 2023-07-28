#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qcustomplot.h>
#include <qpushbutton.h>

#include <QMainWindow>

extern "C" {
#include "../../backend/banks.h"
#include "../../backend/calc.h"
}

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  // void on_actioncalculateButtonPressed_triggered();
  void handleCalculateButton();
  void handleDifLoanButton();
  void handleAnnLoanButton();
  void handleDepositButton();

 private:
  Ui::MainWindow *ui;
  QPushButton *calculate_button;
  QPushButton *dif_loan_button;
  QPushButton *ann_loan_button;
  QPushButton *deposit_button;
};
#endif  // MAINWINDOW_H
