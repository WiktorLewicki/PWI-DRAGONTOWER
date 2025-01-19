#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gameplay.h"
Gameplay game;
vector<long double> balance_history;
int showMoneyDialog() {
    QDialog dialog;
    dialog.setWindowTitle("Choose an amount of money");

    QLabel *label = new QLabel("Enter the amount of money you want to deposit:", &dialog);

    QLineEdit *lineEdit = new QLineEdit(&dialog);
    QIntValidator *validator = new QIntValidator(1, 100000, lineEdit);
    lineEdit->setValidator(validator);

    QLabel *errorLabel = new QLabel(&dialog);
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setVisible(false);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, [&dialog, &lineEdit, &errorLabel]() {
        if (lineEdit->text().isEmpty()) {
            errorLabel->setText("You must enter an amount!");
            errorLabel->setVisible(true);
        } else {
            dialog.accept();
        }
    });

    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(errorLabel);
    layout->addWidget(buttonBox);

    int result = dialog.exec();
    if (result == QDialog::Accepted) {
        return lineEdit->text().toInt();
    } else {
        return -1;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int money = showMoneyDialog();
    while (money == -1) {
        money = showMoneyDialog();
    }
    game.start_money(money);
    balance_history.push_back(money);
    generateChart(balance_history);
    QRegularExpression betRegExp("^[1-9][0-9]{0,5}$");
    QRegularExpressionValidator *betValidator = new QRegularExpressionValidator(betRegExp, this);
    ui->bet_amount->setValidator(betValidator);

    QRegularExpression bombRegExp("^[1-3]$");
    QRegularExpressionValidator *bombValidator = new QRegularExpressionValidator(bombRegExp, this);
    ui->egg_amount->setValidator(bombValidator);
    ui->cash_out->setVisible(false);
    ui->cash_out->setEnabled(false);
    update_balance();
    for (int i = 0; i <= 5; ++i) {
        for (int j = 0; j <= 3; ++j) {
            QString buttonName = QString("pole%1%2").arg(i).arg(j);
            QPushButton *button = findChild<QPushButton*>(buttonName);
            if (button) {
                button->setIconSize(QSize(85, 36));
                connect(button, &QPushButton::clicked, this, &MainWindow::handleBoardClick);
            }
        }
    }
}
void MainWindow::show_row(int k, int p){
    for (int j = 0; j <= 3; ++j) {
        if(j==p) continue;
        QString buttonName = QString("pole%1%2").arg(k).arg(j);
        QPushButton *button = findChild<QPushButton*>(buttonName);
        if(game.tab[k][j]) button->setIcon(QIcon(":/img/nietrafione_pole.png"));
        else button->setIcon(QIcon(":/img/puste_pole.png"));
    }
}
void MainWindow::show_all(int p){
    for (int i = p; i <= 5; ++i) {
        show_row(i, -1);
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::update_balance(){
    long double money = game.get_balance();
    QString qpm = QString::number(money, 'f', 2);
    QString result = "Balance: " + qpm + "EUR";
    ui->balance->setText(result);
}
void MainWindow::update_stats() {
    long double multiplier = game.get_multiplier();
    QString qpm = QString::number(multiplier, 'f', 2);
    QString result = "Multiplier: x" + qpm;
    ui->multiplier->setText(result);
    long double potential = game.get_potential();
    qpm = QString::number(potential, 'f', 2);
    result = "Potential Win: " + qpm;
    ui->potential->setText(result);
}
void MainWindow::to_select(int p){
    for (int j = 0; j <= 3; ++j) {
        QString buttonName = QString("pole%1%2").arg(p).arg(j);
        QPushButton *button = findChild<QPushButton*>(buttonName);
        button->setIcon(QIcon(":/img/wybierane_pole.png"));
    }
}
void MainWindow::on_new_game_clicked()
{
    QString betText = ui->bet_amount->text();
    QString eggText = ui->egg_amount->text();
    if (betText.isEmpty() || eggText.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("You must provide a value for both the bet and the number of bombs.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    int money = betText.toInt();
    int eggs = eggText.toInt();
    if((long double)money>game.get_balance()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("You don't have enough money.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    for(int i = 0; i <= 5; i++){
        for (int j = 0; j <= 3; ++j) {
            QString buttonName = QString("pole%1%2").arg(i).arg(j);
            QPushButton *button = findChild<QPushButton*>(buttonName);
            button->setIcon(QIcon());
        }
    }
    this->setStyleSheet("background-image: url(:/img/smok1.png)");
    ui->new_game->setVisible(false);
    ui->new_game->setEnabled(false);
    game.change_settings(money, eggs);
    ui->bet_amount->setEnabled(false);
    ui->egg_amount->setEnabled(false);
    ui->cash_out->setVisible(true);
    ui->cash_out->setEnabled(true);
    game.new_game();
    update_balance();
    update_stats();
    unlock_row(0);
}


void MainWindow::lock_row(int p){
    for(int j = 0; j <= 3; ++j) {
        QString buttonName = QString("pole%1%2").arg(p).arg(j);
        QPushButton *button = findChild<QPushButton*>(buttonName);
        button->setCursor(Qt::ArrowCursor);
    }

}
void MainWindow::unlock_row(int p){
    to_select(p);
    for(int j = 0; j <= 3; ++j) {
        QString buttonName = QString("pole%1%2").arg(p).arg(j);
        QPushButton *button = findChild<QPushButton*>(buttonName);
        button->setCursor(Qt::PointingHandCursor);
    }
}
void MainWindow::Cashout(){
    game.ok = false;
    if(game.who <= 5) lock_row(game.who);
    update_balance();
    balance_history.push_back(game.get_balance());
    generateChart(balance_history);
    ui->new_game->setVisible(true);
    ui->new_game->setEnabled(true);
    ui->bet_amount->setEnabled(true);
    ui->egg_amount->setEnabled(true);
    ui->cash_out->setVisible(false);
    ui->cash_out->setEnabled(false);
}
void MainWindow::handleBoardClick(){
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;
    QString buttonName = clickedButton->objectName();
    if (buttonName.startsWith("pole")) {
        if(!game.ok) return;
        int x = buttonName.mid(4, 1).toInt();
        int y = buttonName.mid(5, 1).toInt();
        if(x!=game.who) return;
        lock_row(game.who);
        game.who++;
        int pm = game.select(y);
        if(pm==0){
            if(game.who==3) this->setStyleSheet("background-image: url(:/img/smok2.png)");
            QString buttonName = QString("pole%1%2").arg(x).arg(y);
            QPushButton *button = findChild<QPushButton*>(buttonName);
            button->setIcon(QIcon(":/img/trafione_pole.png"));
            show_row(x, y);
            unlock_row(game.who);
            update_stats();
        }
        else if(pm==1){
            QString buttonName = QString("pole%1%2").arg(x).arg(y);
            QPushButton *button = findChild<QPushButton*>(buttonName);
            button->setIcon(QIcon(":/img/trafione_pole.png"));
            show_row(x, y);
            this->setStyleSheet("background-image: url(:/img/smok3.png)");
            update_stats();
            Cashout();
        }
        else{
            QString buttonName = QString("pole%1%2").arg(x).arg(y);
            QPushButton *button = findChild<QPushButton*>(buttonName);
            button->setIcon(QIcon(":/img/przegrane_pole.png"));
            show_row(x, y);
            show_all(game.who);
            update_stats();
            Cashout();
        }
    }
    return;
}


void MainWindow::on_cash_out_clicked()
{
    update_stats();
    game.cashout();
    show_all(game.who);
    Cashout();
}


void MainWindow::generateChart(const std::vector<long double>& accountHistory) {
    QLineSeries *series = new QLineSeries();
    for (size_t i = 0; i < accountHistory.size(); ++i) {
        series->append(static_cast<qreal>(i), static_cast<qreal>(accountHistory[i]));
    }
    series->setPointsVisible(false);
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Balance History");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Balance");
    axisY->setLabelFormat("%.2f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    if (ui->chartarea->layout() != nullptr) {
        QLayout *oldLayout = ui->chartarea->layout();
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chartView);
    ui->chartarea->setLayout(layout);
}

