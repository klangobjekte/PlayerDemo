#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
//#define __DONT_USE_PLAYER
#define __DONT_USE_WAVEFORM
#ifndef __DONT_USE_PLAYER
#include <psndplayer.h>
#endif


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void on_reverseButton_clicked(bool checked);
    void on_browsePushButton_clicked();
    void on_addAndPlayPushButton_clicked();
    void on_addPushButton_clicked();
    void on_removePushButton_clicked();
    void on_playPushButton_clicked();
    void on_repeatPushButton_clicked();
    void on_stopPushButton_clicked();
    void on_pausePushButton_clicked();
    void on_devicesComboBox_IndexChanged(QString);
    void customEvent(QEvent *e);
    
private:
    Ui::Widget *ui;
#ifndef __DONT_USE_PLAYER
    PSndPlayer *player;
    map<int, string> outDevices;
#endif
    bool paused;
};

#endif // WIDGET_H
