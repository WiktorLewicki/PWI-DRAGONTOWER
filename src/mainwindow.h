#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_new_game_clicked();
    void update_stats();
    void update_balance();
    void handleBoardClick();
    void show_row(int k, int p);
    void show_all(int p);
    void Cashout();
    void lock_row(int p);
    void unlock_row(int p);
    void on_cash_out_clicked();
    void to_select(int p);
    void generateChart(const std::vector<long double>& accountHistory);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
