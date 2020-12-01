#include "answer.h"

Answer::Answer(QString a, bool r){
    answer = a;
    rule = r;
}

QString Answer::getAnswer(){
    return answer;
}

bool Answer::getRule(){
    return rule;
}
