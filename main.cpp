#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PlayerWidget w;
    w.show();
    //w.setInitialFile("/Users/admin/Music/iTunes/iTunes Media/Music/Yes/Yessongs (CD 1)/05 And You And I.mp3");
    //w.on_addAndPlayPushButton_clicked();
    
    return a.exec();
}
