#ifndef TRIVIA_H
#define TRIVIA_H

#include "query.h"
#include "answer.h"

/**
    A Trivia class that has three Answer objects and is a protected Query type
*/
class Trivia : protected Query
{
private:
    Answer optOne;
    Answer optTwo;
    Answer optThree;
public:
    Trivia(QString q,QString a1,bool r1,QString a2,bool r2,QString a3,bool r3) :
            Query(q),optOne(a1,r1),optTwo(a2,r2),optThree(a3,r3){};
    QString getQuestionText();
    QString getOptText(int);
    bool getOptRule(int);
};

#endif // TRIVIA_H
