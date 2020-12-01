#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "trivia.h"
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QGroupBox>
#include <QLabel>
#include <QDebug>
#include <QDateTime>
#include <QMouseEvent>

//window functions
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    //set up window
    ui->setupUi(this);

    //load questions
    loadQuestions();

    //load highscores
    loadHighscores();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//private functions
/**
Loads the question from a questions.txt file
*/
void MainWindow::loadQuestions(){
    //QCoreApplication::applicationDirPath()
    //Lets attempt to load the question.txt file
    QString path = "../HonorsProject/questions.txt";
    QFile questions(path);
    QString errMsg;
    //QFileDevice::FileError err = QFileDevice::NoError;
    try{
        if (!questions.open(QIODevice::ReadOnly | QIODevice::Text)){
            errMsg = questions.errorString();
            //err = questions.error();
            throw errMsg;
        }
    }catch(QString e){
        //if we can't open the file then lets save the error to a log.txt file and close the program
        errorClose(e);
    }

    //these variables we'll use to help us read the questions.txt file
    QString q = "";
    QString a[3] = {"","",""};
    bool r[3] = {false,false,false};
    int indexCounter = 0;

    QTextStream in(&questions);
    try{
        while (!in.atEnd()) {
            QString line = in.readLine();
            //lets ignore and line that starts with a # symbol
            if(!line.startsWith('#')){
                //we're not supposed to proccess an answer before a question has been processed or
                //we're not supposed to process more than 3 answers per question
                if((line.startsWith('-') || line.startsWith('+')) && (indexCounter >= 3 || q == ""))
                    throw (QString) "Extra answer found before next question; three answers max per question";

                //lets save the question from the line that start with a ? symbol
                if(line.startsWith('?')){
                    //We should only attempt process a question line if three answers were processed already for the previous question
                    if(q != "" && indexCounter < 3)
                        throw (QString) "Two questions found before three answers processed for previous question";

                    q = line.split('?')[1];
                }
                //lets save the answer text and note that it's incorrect from line starting with a - symbol
                else if(line.startsWith('-')){
                    a[indexCounter] = line.split('-')[1];
                    r[indexCounter] = false;
                    indexCounter++;
                //lets save the answer text and note that it's correct from line starting with a + symbol
                }else if(line.startsWith('+')){
                    a[indexCounter] = line.split('+')[1];
                    r[indexCounter] = true;
                    indexCounter++;
                }
            }
            //each question will only have three answers to choose from so lets make the new Trivia using the details gathered
            //then lets reset the variables so we can use them again for a new question
            if (indexCounter == 3){
                triviaSet.append(Trivia(q,a[0],r[0],a[1],r[1],a[2],r[2]));
                q = "";
                a[0] = a[1] = a[2] = "";
                r[0] = r[1] = r[2] = false;
                indexCounter = 0;
            }
        }
       }catch(QString e){
        //if we run into any issues noted above then lets save the error to a log.txt file and close the program
        errorClose(e);
    }

    //randomize the questions
    srand(time(NULL));
    Trivia * temp;
    int index;
    for(int i = 0; i < triviaSet.size(); i++){
        temp = new Trivia(triviaSet[i].getQuestionText(),
                          triviaSet[i].getOptText(1),triviaSet[i].getOptRule(1),
                          triviaSet[i].getOptText(2),triviaSet[i].getOptRule(2),
                          triviaSet[i].getOptText(3),triviaSet[i].getOptRule(3));
        index = rand() % triviaSet.size();
        triviaSet[i] = triviaSet[index];
        triviaSet[index] = *temp;
        //reset temp
        delete temp;
        temp = nullptr;
    }

    //now that we've loaded the questions into a triviaSet lets load the first question
    showQuestion();
    //we don't need the questions file anymore so lets close it
    questions.close();
};

/**
    Loads a highscore text file
 */
void MainWindow::loadHighscores(){
    //QCoreApplication::applicationDirPath()
    //Lets attempt to load the highscores.txt file
    QString path = "../HonorsProject/highscores.txt";
    QFile highscores(path);
    QString errMsg;
    //QFileDevice::FileError err = QFileDevice::NoError;
    try{
        if (!highscores.open(QIODevice::ReadOnly | QIODevice::Text)){
            errMsg = highscores.errorString();
            //err = questions.error();
            throw errMsg;
        }
    }catch(QString e){
        //if we can't open the file then lets save the error to a log.txt file and close the program
        errorClose(e);
    }

    //variable to process scores
    QVector<QString> * scores = new QVector<QString>;
    QTextStream in(&highscores);
    //lets do quick read, reading only valid lines for highscore
    while(!in.atEnd()){
        QString line = in.readLine();
        if(!line.startsWith('#')){
            try{
                if(line.contains(" "))
                    if(line.split(" ")[0].length() == 3)
                        scores->append(line);
                    else
                        throw (QString) "Invalid highscore line";
                else
                    throw (QString) "Invalid highscore line";
            }catch (QString e){
                //lets just record the error but not close the program
                errorWriteOnly(e);
            }
        }
    };

    //sort it for no funny business
    if(scores->size() > 1)
        for(int i = 0; i < scores->size()-1; i++){
            if(scores->at(i+1).split(" ")[1].toInt() > scores->at(i).split(" ")[1].toInt()){

                QString * temp = new QString((*scores)[i]);
                (*scores)[i] = (*scores)[i+1];
                (*scores)[i+1] = *temp;
                delete temp;
                i = -1;
            }
        }
    //there's only ten hs labels so:
    for (int i = 1; i <= 10;  i++){
        findChild<QLabel*>("highscoreLbl_" + QString::number(i))->setText(QString::number(i) + ". " + scores->at(i - 1));

        //if there are less scores than space available lets get out of here
        if(i == scores->size())
            break;
    };

    delete scores;
    highscores.close();
};

/**
   Save scores to a highscores.txt file
*/
void MainWindow::saveHighscore(){
    //QCoreApplication::applicationDirPath()
    //Lets attempt to load the highscores.txt file
    QString path = "../HonorsProject/highscores.txt";
    QFile highscores(path);
    QString errMsg;
    //QFileDevice::FileError err = QFileDevice::NoError;
    try{
        if (!highscores.open(QIODevice::ReadOnly | QIODevice::Text)){
            errMsg = highscores.errorString();
            //err = questions.error();
            throw errMsg;
        }
    }catch(QString e){
        //if we can't open the file then lets save the error to a log.txt file and close the program
        errorClose(e);
    }

    //variable to process scores
    QVector<QString> * scores = new QVector<QString>;
    QTextStream in(&highscores);
    //lets do quick read, reading only valid lines for highscore
    while(!in.atEnd()){
        QString line = in.readLine();
        if(!line.startsWith('#')){
            try{
                if(line.contains(" "))
                    if(line.split(" ")[0].length() == 3)
                        scores->append(line);
                    else
                        throw (QString) "Invalid highscore line";
                else
                    throw (QString) "Invalid highscore line";
            }catch (QString e){
                //lets just record the error but not close the program
                errorWriteOnly(e);
            }
        }
    };

    //lets grab the score tally and initials
    try{
        //we'll only progress from here if the initials are set correctly
        if( findChild<QLineEdit*>("initialsLn")->text().length() != 3)
            throw (QString) "Need to make sure initials are exactly 3 in lenghth";

        //lets make a new score entry
        QString newScore = findChild<QLineEdit*>("initialsLn")->text() + " " + findChild<QLabel*>("tallyLbl")->text();
        //lets add the new score entry with the existing scores collected
        scores->append(newScore);

        //sort the scores
        if(scores->size() > 1)
            for(int i = 0; i < scores->size()-1; i++){
                if(scores->at(i+1).split(" ")[1].toInt() > scores->at(i).split(" ")[1].toInt()){

                    QString * temp = new QString((*scores)[i]);
                    (*scores)[i] = (*scores)[i+1];
                    (*scores)[i+1] = *temp;
                    delete temp;
                    i = -1;
                }
            }
        //lets close the read only highschores file
        highscores.close();

        //Lets attempt to write to the highscores.txt file
        if (!highscores.open(QIODevice::WriteOnly | QIODevice::Text)){
            errMsg = highscores.errorString();
            //err = questions.error();
        }

        //qDebug() << "reopened file";

        if (scores->size() > 0){
            QTextStream stream(&highscores);
            stream << "#New Scores:" << Qt::endl;
            for(int i = 0; i < scores->size(); i++)
                stream << (*scores)[i] << Qt::endl;
        }
    }catch (QString e){
        //lets just record the error but not close the program
        errorWriteOnly(e);
    }
    //clear scores pointer and close the highscores file
    delete scores;
    highscores.close();
}

/**
 * Shows the question at the top of the Trivia set
*/
void MainWindow::showQuestion(){
    //note: because trivia access query as protected we need to use getQuestionText to retrieve
    //the question as query public getQuestion become protected, so only trivia can use it
    //within itself
    findChild<QLabel*>("questionLbl")->setText(triviaSet[0].getQuestionText());
    findChild<QPushButton*>("optOneBtn")->setText(triviaSet[0].getOptText(1));
    findChild<QPushButton*>("optTwoBtn")->setText(triviaSet[0].getOptText(2));
    findChild<QPushButton*>("optThreeBtn")->setText(triviaSet[0].getOptText(3));
}

/**
 * Updates the score label;
    Requires a bool to note if the correct answer was given
*/
void MainWindow::updateScore(bool correct){
    if(correct){
        //first step: add 100 to current score
        QLabel * currentScoreLbl = findChild<QLabel*>("tallyLbl");
        QString currentScoreStr = currentScoreLbl->text();
        int currentScoreInt = currentScoreStr.toInt();
        currentScoreLbl->setText(QString::number(currentScoreInt + 100));
        //second step: pop triviaSet
        triviaSet.erase(triviaSet.begin());
        //third step check if there are any remaining questions
        //if there are show the first elements details
        //otherwise disable the question box and enable the reset button
        if(triviaSet.size()>0){
            showQuestion();
        }else{
            findChild<QGroupBox*>("questionsBox")->setDisabled(true);
            findChild<QPushButton*>("resetBtn")->setEnabled(true);
            findChild<QPushButton*>("saveBtn")->setEnabled(true);
        }
    }else{
        //first step: pop triviaSet
        triviaSet.erase(triviaSet.begin());
        //second step check if there are any remaining questions
        //if there are show the first elements details
        //otherwise disable the question box and enable the reset button
        if(triviaSet.size()>0){
            showQuestion();
        }else{
            findChild<QGroupBox*>("questionsBox")->setDisabled(true);
            findChild<QPushButton*>("resetBtn")->setEnabled(true);
            findChild<QPushButton*>("saveBtn")->setEnabled(true);
        }
    }
};

/**
    write and error message to a file then close the program
*/
void MainWindow::errorClose(QString e){
    QString logPath = "./log.txt";
    QFile log(logPath);
    log.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream out(&log);
    QDateTime dt = QDateTime::currentDateTime();
    out << dt.toString("yyyyMMddTHHmmss") << " Error: " << e << Qt::endl;
    log.close();
    exit(0);
}
/**
    write and error message to a file only
*/
void MainWindow::errorWriteOnly(QString e){
    QString logPath = "./log.txt";
    QFile log(logPath);
    log.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    QTextStream out(&log);
    QDateTime dt = QDateTime::currentDateTime();
    out << dt.toString("yyyyMMddTHHmmss") << " Error: " << e << Qt::endl;
    log.close();
}

//slots functions
/**
    Simple exit button
*/
void MainWindow::on_exitBtn_clicked()
{
    exit(0);
}

/**
    process the click of option 1 for the trivia
*/
void MainWindow::on_optOneBtn_clicked()
{
    if (triviaSet[0].getOptRule(1)){
        updateScore(true);
    }else{
        updateScore(false);
    }
}
/**
    process the click of option 2 for the trivia
*/
void MainWindow::on_optTwoBtn_clicked()
{
    if (triviaSet[0].getOptRule(2)){
        updateScore(true);
    }else{
        updateScore(false);
    }
}
/**
    process the click of option 3 for the trivia
*/
void MainWindow::on_optThreeBtn_clicked()
{
    if (triviaSet[0].getOptRule(3)){
        updateScore(true);
    }else{
        updateScore(false);
    }
}
/**
    resets the game state to initial state
*/
void MainWindow::on_resetBtn_clicked()
{
    loadQuestions();
    findChild<QLabel*>("tallyLbl")->setText("0");
    findChild<QPushButton*>("saveBtn")->setDisabled(true);
    findChild<QGroupBox*>("questionsBox")->setEnabled(true);
    findChild<QPushButton*>("resetBtn")->setDisabled(true);
    findChild<QLineEdit*>("initialsLn")->setStyleSheet("background: blue");
}
/**
  handles saving a highscore, main actions only occur if there's 3 initials in the lineedit
*/
void MainWindow::on_saveBtn_clicked()
{
    //we need to make sure there's enough characters in the QLineEdit before saving the score with initials
    if( findChild<QLineEdit*>("initialsLn")->text().length() == 3){
        findChild<QLineEdit*>("initialsLn")->setStyleSheet("background: blue");
        findChild<QPushButton*>("saveBtn")->setDisabled(true);
        saveHighscore();
        loadHighscores();
    }else{
        findChild<QLineEdit*>("initialsLn")->setStyleSheet("background: red");
    }
}

/**
   handles mousePressEvent for when user presses on the main window
   source: https://stackoverflow.com/questions/11314429/select-moving-qwidget-in-the-screen
*/
void MainWindow::mousePressEvent(QMouseEvent * evt){
    oldPos = evt->globalPos();
}

/**
   handles mouseMoveEvent for when user presses on the main window
   source: https://stackoverflow.com/questions/11314429/select-moving-qwidget-in-the-screen
*/
void MainWindow::mouseMoveEvent(QMouseEvent * evt){
    const QPoint delta = evt->globalPos() - oldPos;
    move(x()+delta.x(),y()+delta.y());
    oldPos = evt->globalPos();
}
