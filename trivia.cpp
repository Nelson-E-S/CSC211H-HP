#include "trivia.h"

QString Trivia::getQuestionText(){
    return getQuestion();
};

QString Trivia::getOptText(int c){
  QString ansTxt;
  switch(c){
    case 1:
      ansTxt = optOne.getAnswer();
      break;
     case 2:
      ansTxt = optTwo.getAnswer();
      break;
     case 3:
      ansTxt = optThree.getAnswer();
      break;
  };
  return ansTxt;
};

bool Trivia::getOptRule(int c){
  bool ansRule;
  switch(c){
    case 1:
      ansRule = optOne.getRule();
      break;
     case 2:
      ansRule = optTwo.getRule();
      break;
     case 3:
      ansRule = optThree.getRule();
      break;
  };
  return ansRule;
};
