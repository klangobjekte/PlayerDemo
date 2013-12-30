#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
//#define __DONT_USE_PLAYER
#define __DONT_USE_WAVEFORM
#ifndef __DONT_USE_PLAYER
#include <psndplayer.h>
#endif

#include <Waveform.h>
#include <WaveformScrollBar.h>
#include <WaveformRuler.h>
#include <WaveformCursorProxy.h>
#include <WaveformSelectionProxy.h>
#include <waveformtextproxy.h>
#include <waveformbuffer.h>
#include <waveformthread.h>
#include <TimeLabel.h>
#include <SndFile.h>
//#include <sndfile.h>

//using namespace QWave2;

namespace Ui {
class PlayerWidget;
}

class PlayerWidget : public QWidget
{
    Q_OBJECT

    
public:
    explicit PlayerWidget(QWidget *parent = 0);
    ~PlayerWidget();

public slots:
    void on_reverseButton_clicked(bool checked);
    void on_browsePushButton_clicked();
    void on_addAndPlayPushButton_clicked();
    void on_addPushButton_clicked();
    void on_removePushButton_clicked();
    void on_playPushButton_clicked();
    void on_revPlayPushButton_clicked();
    void on_fastPlayPushButton_clicked();
    void on_fastRevPushButton_clicked();
    void on_repeatPushButton_clicked();
    void on_stopPushButton_clicked();
    void on_pausePushButton_clicked();
    void on_devicesComboBox_currentIndexChanged(QString);
    void customEvent(QEvent *e);
    int getVolume();
    void on_loadWavePushButton_clicked();
    void on_cursorGroupBox_buttonClicked(int id);
    void changeSelection(double beg, double dur, Waveform*);
    void unregisterWaveform();
    void on_hZoomSlider_sliderMoved(int value);
    void on_seekSlider_sliderMoved(int value);
    void on_volumeSlider_valueChanged(int value);
    void on_vZoomSlider_sliderMoved(int value);

signals:
    void publishWaveformCursorChanged(int);

    
private slots:
    void on_tableWidget_cellPressed(int row, int column);

private:
    WaveFormBuffer *waveFormBuffer;
    SndFile* soundfile;
    WaveformRuler* ruler;
    WaveformScrollBar* sb;
    WaveformCursorProxy* waveformCursorProxy;
    WaveformSelectionProxy* waveformSelectionProxy;
    WaveformScrollBar* waveformScrolBar;
    WaveFormTextProxy *waveFormTextProxy;
    std::map<psnd_string,MediaSource*> currentSources;
    //vector<SndFile*> sndfiles;
    //map<int,map<int,Waveform*> > waveforms;
    typedef map<int,Waveform*> WaveformMap;
    map<psnd_string,WaveformMap > waveforms;
    WaveformMap waveformMap;
    bool hasRuler;

    Ui::PlayerWidget *ui;
#ifndef __DONT_USE_PLAYER
    PSndPlayer *player;
    map<int, string> outDevices;
#endif
    bool paused;

    double startTime;
    double selectionduration;
    double endTime;
    double fileduration;
    psnd_string currentFile;
    double speed;





};

#endif // WIDGET_H
