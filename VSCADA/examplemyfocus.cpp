#include "examplemyfocus.h"
//#include "tester.h"


exampleMyFocus::exampleMyFocus(QWidget *parent, widgetKeyBoard *keyBoard) : QLineEdit(parent), m_keyBoard(keyBoard)
{
    //this->setEchoMode(QLineEdit::Password);
}


void exampleMyFocus::focusInEvent ( QFocusEvent * event )
{
//    if (this->m_keyBoard == NULL) // this is a promotion from Design
//        ((Tester*)this->parent())->getKeyboard()->focusThis(this);
//    else
//     this->m_keyBoard->show(this, NULL, false);
        this->m_keyBoard->focusThis(this); // modifies focus for keyboard
    QLineEdit::focusInEvent(event);
//    std::cout << "Kanaeeeeeee!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    emit focussed(true);
}

void exampleMyFocus::focusOutEvent(QFocusEvent *e)
{
  QLineEdit::focusOutEvent(e);
//  std::cout << "Manakaaaaaaaaaaaa!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  emit focussed(false);
}
