#ifndef EXAMPLEMYFOCUS_H
#define EXAMPLEMYFOCUS_H

#include <QLineEdit>
#include "widgetKeyBoard.h"
#include <iostream>
#include <QTextStream>

//
// If you want to change existing QLineEdit in your ui files, since moc generation, the only way is to "Promote" your
// widget to an istance of this class name (in the auto generated ui files you will see a creation of this class).
// In focusInEvent() there is necessary to understand if this is coming from a promotion or a normal instance
class exampleMyFocus : public QLineEdit
{
    Q_OBJECT
    private:
        widgetKeyBoard *m_keyBoard;



    public:
        exampleMyFocus(QWidget *parent = NULL, widgetKeyBoard *keyBoard = NULL);

signals:
  void focussed(bool hasFocus);

    protected:
       void focusInEvent (QFocusEvent * event);
       void focusOutEvent(QFocusEvent *e);

};

#endif // EXAMPLEMYFOCUS_H
