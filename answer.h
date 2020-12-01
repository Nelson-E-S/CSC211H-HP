#ifndef ANSWER_H
#define ANSWER_H

#include <QString>
/**
    A simple Answer class that holds an anwser and an indication if it's correct
*/
class Answer
{
    private:
        QString answer;
        bool rule;
    public:
        Answer(QString,bool);
        QString getAnswer();
        bool getRule();
};

#endif // ANSWER_H
