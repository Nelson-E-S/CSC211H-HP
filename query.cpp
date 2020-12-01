#include "query.h"

Query::Query(QString q){
    question = q;
}

QString Query::getQuestion(){
    return question;
}
