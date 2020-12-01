#ifndef QUERY_H
#define QUERY_H

#include <QString>
/**
    A simple query class that holds and returns a question
*/
class Query{
    private:
        QString question;
    public:
        Query(QString);
        QString getQuestion();
};

#endif // QUERY_H
