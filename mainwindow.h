#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "trivia.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_exitBtn_clicked();

    void on_optOneBtn_clicked();

    void on_optTwoBtn_clicked();

    void on_optThreeBtn_clicked();

    void on_resetBtn_clicked();

    void on_saveBtn_clicked();

protected:
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

private:
    Ui::MainWindow *ui;
    QVector<Trivia> triviaSet;
    QPoint oldPos;
    void updateScore(bool);
    void showQuestion();
    void loadQuestions();
    void loadHighscores();
    void saveHighscore();
    void errorClose(QString);
    void errorWriteOnly(QString);

};
#endif // MAINWINDOW_H
