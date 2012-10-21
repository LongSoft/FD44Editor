#include <QApplication>
#include "fd44editor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FD44Editor w;

    if(a.arguments().length() > 1)
        w.openImageFile(a.arguments().at(1));
    w.show();
    
    return a.exec();
}
