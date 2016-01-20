#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QObject>

#ifdef Q_OS_WIN
#include <psndplayer.h>
#include <waveformcontroler.h>
//#include <psndcommon.h>
//#include <psndplayerdefenitions.h>
#else
#include <PSndPlayer/psndplayer.h>
#include <PSndPlayer/waveformcontroler.h>
#endif



namespace Ui {
class PlayerWidget;
}

class PlayerWidget : public QWidget
{
    Q_OBJECT

    
public:
    explicit PlayerWidget(QWidget *parent = 0);
    ~PlayerWidget();
    void setInitialFile(QString text);
    void setAudioVolume(int newVolume);
    PSNDWAVE::WAVE_CURSOR getWaveformCursorShape();
    void setWaveformCursorButton(PSNDWAVE::WAVE_CURSOR waveformCursorShape);



public slots:

    void on_browsePushButton_clicked();
    void on_addAndPlayPushButton_clicked();
    void on_addPushButton_clicked();
    void on_removePushButton_clicked();
    void on_playPushButton_clicked();
    void on_revPlayPushButton_clicked();
    void on_fastPlayPushButton_clicked();
    void on_fastRevPushButton_clicked();
    void on_repeatPushButton_clicked(bool repeat);
    void on_stopPushButton_clicked();
    void on_pausePushButton_clicked();
    void on_devicesComboBox_currentIndexChanged(QString item);
    int audioVolume();
    void on_loadWavePushButton_clicked();
    void on_cursorGroupBox_buttonClicked(int id);
    void on_WaveformSelectionChanged(double beg, double dur, PSNDWAVE::CHANGESELECTION type);
    //void unregisterWaveform();
    void on_hZoomSlider_sliderMoved(int value);
    void on_seekSlider_sliderMoved(int value);

    void on_volumeSlider_valueChanged(int value);
    void on_vZoomSlider_sliderMoved(int value);
    void on_testPushButton_clicked();
    void on_volPushButton_clicked();

signals:
    //void publishWaveformCursorChanged(int);
    void waveformCursorChanged(PSNDWAVE::WAVE_CURSOR);
    
private slots:
    void on_tableWidget_cellPressed(int row, int column);
    //void loadWaveform();
    void on_seekSlider_sliderPressed();
    void on_getInfoPushButton_pressed();
    void on_sampleratesComboBox_currentTextChanged(const QString &arg1);
    void on_tick_received(double t);
    void on_duration_received(double t);
    void on_Source_Changed();
    void on_MediaSourceStatus_Changed(PSNDMEDIA::MEDIASTATUS status);
    void on_PlayerState_Changed(PSNDPLAYER::PLAYERSTATE state);

private:

    void sendWaveformCursorShape(PSNDWAVE::WAVE_CURSOR waveformCursorShape);

    Ui::PlayerWidget *ui;
    psndplayer::PSndPlayer *player;
    psndplayer::WaveformController *waveformController;
    PSNDMEDIA::MEDIASTATUS _mediastatus;

    map<int,OutDevice> outputDevices;


    PSNDWAVE::WAVE_CURSOR _waveformCursorShape;
    bool _validFile;



    bool paused;

    int _audioVolume;
    double _startTime;
    double _selectionduration;
    double _endTime;
    double fileduration;
    psnd_string currentFile;
    double speed;
    double vratio;
    bool _repeat;
    bool _manualPlayRequest;

};

#endif // WIDGET_H
