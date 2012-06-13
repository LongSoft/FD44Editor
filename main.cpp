#include <QApplication>
#include "fd44editor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FD44Editor w;
    w.show();
    
    return a.exec();
}
