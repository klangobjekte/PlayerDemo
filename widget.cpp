#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <map>
#include "psndevents.h"


QString
time2str(double t, int e)
{
  char const * sign = "";
  if (t < 0) {
    sign = "-";
    t = -t;
  }
  double shift = 1.0;
  double roundup = 0.5;
  for (int i=0; i<e; ++i) {
    shift *= 10.0;
    roundup /= 10.0;
  }
  t += roundup;
  int sec = (int)t;
  int msec = (int)((t-(double)sec)*shift);
  int min = sec / 60;
  int hour = min / 60;
  min %= 60;
  sec %= 60;
  char s[128];
  if (hour != 0)
    sprintf(s,"%s%d:%02d:%02d.",sign,hour,min,sec);
  else if (min != 0)
    sprintf(s,"%s%d:%02d.",sign,min,sec);
  else
    sprintf(s,"%s%d.",sign,sec);
  char f[16];
  sprintf(f,"%%0%dd",e);
  sprintf(&s[strlen(s)],f,msec);
  return QString(s);
}
#ifndef WIN32
//! make a normalized string for hfs formated Media
psnd_char *makeCharFromQString(QString qstring)
{
    QString utfstring;
    #ifdef WIN32
            setlocale(LC_ALL, "");
            //FileNameHelper *fileNameHelper;
            //fileNameHelper = new FileNameHelper;
            if(fileSystem(qstring)== FILESYSTEM_HFS){
                utfstring = qstring.normalized(QString::NormalizationForm_KD);
            }
            else{
                utfstring = qstring;
            }
            //qDebug() << "makeCharFromString: " <<utfstring;
            wstring w_string = utfstring.toStdWString();
            psf_char *filenamep = NULL;
            //! Kopiere in std::vector inklusive Nullterminierung
            vector<psf_char> vec(w_string.begin(), w_string.end());
            vec.push_back(L'\0');
            //! Extrahiere wchar_t*
            filenamep = &vec[0];

            //! Speicherplatz fuer _fileName
            //! damit dieser ueber die Funktion hinaus
            //! gueltig ist!
            int len = psf_strlen(filenamep);
            psf_char *charstring = 0;
            charstring = new psf_char[len+1];
            psf_csncpy(charstring,filenamep,len);
            charstring[len] = 0;
            //! Windows alternative normalisierung ueber Qt:
            //! Der normalisierte (combined UTF - gewandelte) path
            //! wird gewandelt in staticpath array kopiert:
            // FoldString( MAP_COMPOSITE ,filenamep,MAXPATHNAMELENGTH,
                    // _fileName,MAXPATHNAMELENGTH);
        #else
            psnd_char *charstring = 0;
            utfstring = qstring.toUtf8();
            string strAscii = utfstring.toStdString();
            int len = strlen(strAscii.c_str());
            charstring  = new  psnd_char [len+1];
            psnd_csncpy(charstring,strAscii.c_str(),len);
            charstring[len] = 0;
#endif
            return charstring;
}
#endif

//using namespace psndplayer;

PlayerWidget::PlayerWidget(QWidget *parent)
    :QWidget(parent),
    ui(new Ui::PlayerWidget),
    player(new psndplayer::PSndPlayer(this)),
    paused(false),
    speed(1.0),
    _repeat(false),
    _audioVolume(100),
    _manualPlayRequest(false)
{
    ui->setupUi(this);
    outputDevices = player->audioOutPut()->getOutputDevices();

    //! Set the Combobox for available AudioDevices
    for(auto iter: outputDevices){
        QString dev = QString::psnd_fromStdString((iter.second).name);
        ui->devicesComboBox->addItem(dev.toUtf8());
    }

    //! We clear the Combobox, cause
    //! on_devicesComboBox_currentIndexChanged
    //! could already have set Values
    ui->sampleratesComboBox->clear();
    OutDevice *currentDevice = player->audioOutPut()->getCurrentOutputDevice();
    for(auto iter: currentDevice->availableSamplerates){
        QString sr = QString::fromStdString(iter.first);
        ui->sampleratesComboBox->addItem(sr.toUtf8());
    }
    ui->sampleratesComboBox->setCurrentText(QString::fromStdString(currentDevice->currentSamplerate));


    //! Set the Combobox for available Samplerates of active AudioDevice


    //player->getPlayerTicker()->registerReceiver(this);
    //player->getPlayerTicker()->registerReceiver(waveformCursorProxy);
    ui->label_pos->setText("0.0000");
    //ui->label_pos->setBackgroundRole(QPalette::Dark);
    //ui->label_pos->setForegroundRole(QPalette::HighlightedText);


    ui->repeatPushButton->setCheckable(true);
    ui->revPlayPushButton->setCheckable(true);
    ui->playPushButton->setCheckable(true);
    ui->stopPushButton->setCheckable(true);
    ui->pausePushButton->setCheckable(true);
    ui->fastPlayPushButton->setCheckable(true);
    ui->fastRevPushButton->setCheckable(true);

    ui->seekSlider->setRange(0, 10*1000);

    //ui->rulerLayout_->addWidget(ruler);
    //ui->scrollBarLayout->addWidget(waveformScrolBar);

    ui->cursorArrowButton->setCheckable(true);
    ui->cursorArrowButton->setVisible(false);
    ui->cursorIbeamButton->setCheckable(true);
    ui->cursorOpenHandButton->setCheckable(true);

    ui->cursorGroupBox->addButton(ui->cursorArrowButton);
    ui->cursorGroupBox->addButton(ui->cursorIbeamButton);
    ui->cursorGroupBox->addButton(ui->cursorOpenHandButton);


    ui->cursorGroupBox->setId(ui->cursorArrowButton,0);
    ui->cursorGroupBox->setId(ui->cursorIbeamButton,1);
    ui->cursorGroupBox->setId(ui->cursorOpenHandButton,2);


    ui->hZoomSlider->setRange(1,299);
    ui->vZoomSlider->setValue(50);
    ui->vZoomSlider->setRange(1,100);


    ui->volPopup->setWindowFlags(Qt::Popup);
    ui->volPopup->setObjectName("volPopup");
    ui->volumeSlider->setParent(ui->volPopup);
    ui->volumeSlider->setValue(_audioVolume);
    ui->volumeSlider->setObjectName("volumeSlider");



    connect(player, SIGNAL(tick(double)), this, SLOT(on_tick_received(double))); //Timelabel
    connect(player,SIGNAL(duration(double)),this,SLOT(on_duration_received(double)));
    connect(player, SIGNAL(sourceChanged()),this, SLOT (on_Source_Changed()));
    connect(player,SIGNAL(mediaSourceStatusChanged(PSNDMEDIA::MEDIASTATUS)),
            this,SLOT(on_MediaSourceStatus_Changed(PSNDMEDIA::MEDIASTATUS)));
    connect(player,SIGNAL(playerStateChange(PSNDPLAYER::PLAYERSTATE)),
            this, SLOT(on_PlayerState_Changed(PSNDPLAYER::PLAYERSTATE)));

    waveformController = NULL;
    waveformController = new psndplayer::WaveformController(player,this);
    ui->gridLayout->addWidget(waveformController->getWaveformWidget(),0,0,1,0);
    connect(waveformController, SIGNAL(waveformSelectionChanged(double,double,PSNDWAVE::CHANGESELECTION)),
        this, SLOT(on_WaveformSelectionChanged(double,double,PSNDWAVE::CHANGESELECTION)));

    setInitialFile("Z:/Users/admin/music/wav/Test_Time_24_48_Mono.wav");
}

PlayerWidget::~PlayerWidget()
{
    qDebug() << "PlayerWidget destroyed";
    if(waveformController){delete waveformController;}
    waveformController = NULL;
    if(player){delete player;}
    player = NULL;
    delete ui;
}

void PlayerWidget::setInitialFile(QString text){
    ui->lineEdit->setText(text);
}

//! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! +++ OUTPUT DEVICES
//! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void PlayerWidget::on_devicesComboBox_currentIndexChanged(QString item){
    int deviceid =-1;
    ui->sampleratesComboBox->clear();

    for(auto iter: outputDevices){
        if(iter.second.name == item.psnd_toStdString().c_str()){
            deviceid =  iter.second.deviceIndex;
        }
    }
    if(deviceid >=0)
    player->audioOutPut()->setOutputDevice(deviceid);

    OutDevice *currentDevice = player->audioOutPut()->getCurrentOutputDevice();
    for(auto iter: currentDevice->availableSamplerates){
        QString sr = QString::fromStdString(iter.first);
        ui->sampleratesComboBox->addItem(sr.toUtf8());
    }
    ui->sampleratesComboBox->setCurrentText(QString::fromStdString(currentDevice->currentSamplerate));
}

void PlayerWidget::on_sampleratesComboBox_currentTextChanged(const QString &arg1)
{
    if(!arg1.isEmpty())
        player->audioOutPut()->setOutpuDeviceSamplerate(arg1.toStdString().c_str());
}

//! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! +++ POSITION DURATION AND TIMELABELS
//! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void PlayerWidget::on_tick_received(double t){
   ui->label_pos->setText(time2str(t,3));
   ui->seekSlider->setSliderPosition(t*1000);
}

void PlayerWidget::on_duration_received(double t){
    ui->label_dur->setText(time2str(t,4));
    ui->seekSlider->setRange(0,t*1000);
}

void PlayerWidget::on_volPushButton_clicked()
{
    qDebug()<< "SHOW VOL POPUP";
    ui->volPopup->move(mapToGlobal(ui->volPushButton->geometry().center()));
    ui->volPopup->show();
}

void PlayerWidget::on_WaveformSelectionChanged(double beg, double dur, PSNDWAVE::CHANGESELECTION type)
{
    qDebug() << "on_WaveformSelectionChanged beg : " << beg;
    _startTime = beg;
    _selectionduration = dur;
    _endTime = beg+dur;
    ui->label_pos->setText(time2str((double)beg,4));
    ui->label_beg->setText(time2str(beg,4));
    ui->label_end->setText(time2str(beg+dur,4));
    ui->label_sel->setText(time2str(dur,4));
    qDebug() << "mediaPlayer->playOnClick(): " << player->playOnClick();
    qDebug() << "_waveformCursorShape: " << _waveformCursorShape;

    if(player->state() == PSNDPLAYER::PLAYING){
        if(player->playOnChange()){
            player->setPosition(beg,dur,type);
            //player->play();
        }
    }
    else{
        if(player->playOnClick() &&
                _waveformCursorShape == PSNDWAVE::WAVEFORMCURSOR_IBEAM){
            //if(mediaPlayer->playOnChange()){
                player->setPosition(beg,dur,type);
                player->play();
            //}
        }
    }
}

//! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! +++ VOLUME
//! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void PlayerWidget::on_volumeSlider_valueChanged(int value){
    qDebug() << "on_volumeSlider_valueChanged" << value;
    setAudioVolume(value);
}

int PlayerWidget::audioVolume()
{
    return _audioVolume;
}

void PlayerWidget::setAudioVolume(int newVolume)
{
    qDebug() << "setAudioVolume: " << newVolume;
    _audioVolume = newVolume;
    if (ui->volumeSlider)
        ui->volumeSlider->setValue(newVolume);
    if(player)
        player->setVolume(newVolume);
}


//! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! +++ WAVEFORMCURSOR
//! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!
void PlayerWidget::on_cursorGroupBox_buttonClicked(int id){
    qDebug() << "on_cursorGroupBox_buttonClicked: " << id;
    _waveformCursorShape = (PSNDWAVE::WAVE_CURSOR)id;
    //emit publishWaveformCursorChanged(id);
    setWaveformCursorButton(_waveformCursorShape);
}

PSNDWAVE::WAVE_CURSOR PlayerWidget::getWaveformCursorShape()
{
    return _waveformCursorShape;
}

void PlayerWidget::setWaveformCursorButton(PSNDWAVE::WAVE_CURSOR waveformCursorShape)
{
    if(waveformCursorShape == PSNDWAVE::WAVEFORMCURSOR_IBEAM ||
            waveformCursorShape == PSNDWAVE::WAVEFORMCURSOR_OPENHAND){
        _waveformCursorShape = waveformCursorShape;
        ui->cursorGroupBox->button(waveformCursorShape)->setChecked(true);
    }
}

void PlayerWidget::sendWaveformCursorShape(PSNDWAVE::WAVE_CURSOR waveformCursorShape)
{
    emit waveformCursorChanged(waveformCursorShape);
}

//! +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! +++ PLAYER CONTROL
//! +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void PlayerWidget::on_repeatPushButton_clicked(bool repeat){

    qDebug() << "+++++ play repeat " <<  repeat;
    ui->pausePushButton->setText("||");
    _repeat = repeat;
    player->setRepeat(repeat);
}

void PlayerWidget::on_revPlayPushButton_clicked(){
    qDebug() << "on_revPlayPushButton_clicked: ";
    player->setReverse(true);
    _manualPlayRequest = true;
}

void PlayerWidget::on_playPushButton_clicked(){
    qDebug() << "on_playPushButton_clicked: ";
    speed = 1.0;
    ui->pausePushButton->setText("||");
    _manualPlayRequest = true;
    player->play();
}

void PlayerWidget::on_stopPushButton_clicked(){
    qDebug() << "onstopPushButtonClicked";
    speed = 1.0;
    player->stop();
}

void PlayerWidget::on_pausePushButton_clicked(){
    player->pause();
}

void PlayerWidget::on_fastPlayPushButton_clicked(){
    speed+=0.1;
    qDebug() << "on_fastPlayPushButton_clicked" << speed;
    //player->audioOutPut()->setPlaydirection(FORWARD);
    player->audioOutPut()->setPlaySpeed(speed);
}

void PlayerWidget::on_fastRevPushButton_clicked(){
    qDebug() << "on_fastRevPushButton_clicked";
    //player->audioOutPut()->setPlaydirection(REVERSE);
    speed-=0.1;
    player->audioOutPut()->setPlaySpeed(speed);
}

void PlayerWidget::on_seekSlider_sliderMoved(int value){
    qDebug() << "on_seekSlider_sliderMoved: " << value;
    //player->setPosition(value);
    ui->label_pos->setText(time2str((double)value/1000,4));
}

void PlayerWidget::on_seekSlider_sliderPressed()
{
    qDebug() << "on_seekSlider_sliderPressed: " ;
    //player->setPosition(ui->seekSlider->value());
    ui->label_pos->setText(time2str((double)ui->seekSlider->value()/1000,4));

}

void PlayerWidget::on_vZoomSlider_sliderMoved(int value){
    double vratio = value/50.0;
    qDebug() << "on_vZoomSlider_sliderMoved: " << vratio;
    //vratio *= 1.5;
    //vratio /= 1.5;
    //emit setVertZoom(vratio);
/*
    map<psnd_string,WaveformMap >::iterator witer;
    witer  = waveforms.find(currentFile);
    WaveformMap wMap = witer->second;
    WaveformMap::iterator channeliter;
    for(channeliter = wMap.begin();channeliter != wMap.end();channeliter++){
        Waveform *waveform = channeliter->second;
        //waveformCursorProxy->unregisterWaveform(waveform);
        //waveformSelectionProxy->unregisterWaveform(waveform);
        //ruler->disconnectWaveform();
        double len = waveform->getWaveFormBuffer()->getLengthSeconds();
        //waveform->display(0,len/value,true);
        waveform->setAmplitudeRatio(vratio);
    }
    */
}

void PlayerWidget::on_hZoomSlider_sliderMoved(int value){
    qDebug() << "on_hZoomSlider_sliderMoved: " << value;
    /*
    map<psnd_string,WaveformMap >::iterator witer;
    witer  = waveforms.find(currentFile);
    WaveformMap wMap = witer->second;
    WaveformMap::iterator channeliter;
    for(channeliter = wMap.begin();channeliter != wMap.end();channeliter++){
        Waveform *waveform = channeliter->second;
        //waveformCursorProxy->unregisterWaveform(waveform);
        //waveformSelectionProxy->unregisterWaveform(waveform);
        //ruler->disconnectWaveform();
        double len = waveform->getWaveFormBuffer()->getLengthSeconds();
        waveform->display(0,len/value,true);
    }
    */
}


//! +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! +++ PLAYLIST
//! +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void PlayerWidget::on_tableWidget_cellPressed(int row, int column)
{
    qDebug() << "on_tableWidget_cellPressed: " << row << column;
    ui->lineEdit->setText(ui->tableWidget->item(row,column)->text());
    player->addFile( ui->lineEdit->text().psnd_toStdString().c_str());
    //player->stop();
    //on_addAndPlayPushButton_clicked();
    //on_addPushButton_clicked();
    //on_loadWavePushButton_clicked();
}

void PlayerWidget::on_loadWavePushButton_clicked(){

    qDebug() << "on_loadWavePushButton_clicked";
    //loadWaveform();


}

void PlayerWidget::on_browsePushButton_clicked(){
    //QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

    QString home = QDir::homePath();
#ifdef WIN32
    home = "Z:/Users/admin/music";
#else
   //home =  tr("/Users/admin/music");
    home= home>"/music";
#endif
    qDebug() << "File Dialog home path:" << home;
    QFileDialog dialog(this);
    //dialog.setOption(QFileDialog::DontUseNativeDialog,true);

    //dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDirectory(home);
    QStringList files;
    files = dialog.getOpenFileNames(
        this,tr("Open Audio File"),
        home,
         "All (**)"
            );
    //if (sname.length() > 0)
    //  ui->lineEdit->setText(sname.toUtf8());

    foreach(QString name, files){
    QString utfstring = name.toUtf8();
    QTableWidgetItem *pathItem = new QTableWidgetItem(utfstring);

    int currentRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(currentRow);
    ui->tableWidget->setItem(currentRow,0,pathItem);
    ui->tableWidget->resizeColumnsToContents();
    //if (ui->tableWidget->columnWidth(0) > 300)
    //    ui->tableWidget->setColumnWidth(0, 300);
    }

}

void PlayerWidget::on_addAndPlayPushButton_clicked(){
    qDebug() << "on_addAndPlayPushButton_clicked!";
    player->addFile(ui->lineEdit->text().psnd_toStdString().c_str());
    player->play();
}

void PlayerWidget::on_addPushButton_clicked(){
    qDebug() << "on_addPushButton_clicked!";
    //player->pause();
    player->addFile(ui->lineEdit->text().psnd_toStdString().c_str());
}

void PlayerWidget::on_removePushButton_clicked(){

}

void PlayerWidget::on_Source_Changed(){
    qDebug() << "PlayerWidget on_Source_Changed";
    if(waveformController){waveformController->startLoadWaveform();}
    _startTime = 0.0;
    _selectionduration = 0.0;
    _endTime = 0.0;
    vratio = 1;
}

void PlayerWidget::on_PlayerState_Changed(PSNDPLAYER::PLAYERSTATE state)
{
    switch (state)
    {
    case PSNDPLAYER::PLAYING:
        qDebug() << "PlayerWidget State:  PSNDPLAYER::PLAYING" << state;
        //playingindex = _currentindex;
        //playBtn->setChecked(true);
        //repeatBtn->setChecked(false);
        ui->pausePushButton->setChecked(false);
        ui->stopPushButton->setChecked(false);
        ui->playPushButton->setChecked(true);
        ui->revPlayPushButton->setChecked(false);
        ui->repeatPushButton->setChecked(false);

        //emit setPlayerActions(psf::PLAYING);
        //emit filePlayingStatus(_qfileNameCurrent,BLUE_MESSAGE);
        break;

    case PSNDPLAYER::REVERSE:
        qDebug() << "PlayerWidget State:  PSNDPLAYER::REVERSE" << state;
       // playingindex = _currentindex;
        //playRevBtn->setChecked(true);
        //repeatBtn->setChecked(false);
        ui->pausePushButton->setChecked(false);
        ui->stopPushButton->setChecked(false);
        ui->revPlayPushButton->setChecked(true);
        ui->playPushButton->setChecked(false);
        ui->repeatPushButton->setChecked(false);

        //emit setPlayerActions(psf::REVERSE);
        //emit filePlayingStatus(_qfileNameCurrent,BLUE_MESSAGE);
        break;
    case PSNDPLAYER::REPEATPLAY:
        qDebug() << "PlayerWidget State: PPSNDPLAYER::REPEATPLAY " << state;
        //repeatBtn->setChecked(true);
        //playBtn->setChecked(true);
        ui->revPlayPushButton->setChecked(false);
        ui->playPushButton->setChecked(true);
        ui->repeatPushButton->setChecked(true);
        ui->pausePushButton->setChecked(false);
        ui->stopPushButton->setChecked(false);

        //pauseBtn->setChecked(true);
        //emit setPlayerActions(psf::REPEATPLAY);
        //emit filePlayingStatus(QString ("<font color=#00FF66>%1</font>").arg(strippedName(_qfileNameCurrent)),GREEN_MESSAGE);
        break;
    case PSNDPLAYER::REPEATREVERSE:
        qDebug() << "PlayerWidget State: PPSNDPLAYER::REPEATREVERSE " << state;
        //repeatBtn->setChecked(true);
        //playRevBtn->setChecked(true);
        ui->pausePushButton->setChecked(false);
        ui->stopPushButton->setChecked(false);
        ui->revPlayPushButton->setChecked(true);
        ui->playPushButton->setChecked(false);
        ui->repeatPushButton->setChecked(true);

        //emit setPlayerActions(psf::REPEATREVERSE);
        //pauseBtn->setChecked(true);
        //emit setPlayerActions(playerPaused);
        //emit filePlayingStatus(QString ("<font color=#00FF66>%1</font>").arg(strippedName(_qfileNameCurrent)),GREEN_MESSAGE);
        break;

    case PSNDPLAYER::STOPPED:
        qDebug() << "PlayerWidget State:  PSNDPLAYER::STOPPED" << state;
        //playBtn->setChecked(false);
        //stopBtn->setChecked(true);
        ui->stopPushButton->setChecked(true);
        ui->pausePushButton->setChecked(false);
        ui->revPlayPushButton->setChecked(false);
        ui->playPushButton->setChecked(false);

        //emit setPlayerActions(psf::STOPPED);
        //emit filePlayingStatus(QString("<font color=#00FF66>%1</font>").arg(strippedName(_qfileNameCurrent)),GREEN_MESSAGE);
        break;

    case PSNDPLAYER::PAUSED:
        qDebug() << "PlayerWidget State: PSNDPLAYER::PAUSED " << state;
        //playBtn->setChecked(false);
        //pauseBtn->setChecked(true);
        ui->pausePushButton->setChecked(true);
        ui->stopPushButton->setChecked(false);
        ui->revPlayPushButton->setChecked(false);
        ui->playPushButton->setChecked(false);

        //emit setPlayerActions(psf::PAUSED);
        //emit filePlayingStatus(QString ("<font color=#00FF66>%1</font>").arg(strippedName(_qfileNameCurrent)),GREEN_MESSAGE);
        break;
    }
}

void PlayerWidget::on_MediaSourceStatus_Changed(PSNDMEDIA::MEDIASTATUS status){
    _mediastatus = status;
    switch(status){
    case PSNDMEDIA::UnknownMediaStatus:
        qDebug() << "PlayerWidget MediaStatus Event +++++++++++++++++++++++++++ UnknownMediaStatus   " << status;
        _validFile = false;
        break;
    case PSNDMEDIA::NoMedia:
        qDebug() << "PlayerWidget MediaStatus Event +++++++++++++++++++++++++++ NoMedia              " << status;
        _validFile = false;
        break;
    case PSNDMEDIA::LoadingMedia:
        qDebug() << "PlayerWidget MediaStatus Event +++++++++++++++++++++++++++ LoadingMedia          " << status;
        //setFileDuration(mediaPlayer->getCurrentMediasource()->duration());
        //prepareonce = true;
        _validFile = false;
        break;
    case PSNDMEDIA::LoadedMedia:
        qDebug() << "PlayerWidget MediaStatus Event +++++++++++++++++++++++++++ LoadedMedia           " << status;
        //prepareonce = true;
        //phononloadError = false;
        _validFile = true;
        //setFileDuration(mediaPlayer->getCurrentMediasource()->getMyOutData()->duration);
        break;
    case PSNDMEDIA::StalledMedia:
        qDebug() << "PlayerWidget MediaStatus Event +++++++++++++++++++++++++++ StalledMedia          " << status;
        break;
    case PSNDMEDIA::BufferingMedia:
        qDebug() << "PlayerWidget MediaStatus Event +++++++++++++++++++++++++++ BufferingMedia        " << status;
        //setFileDuration(mediaPlayer->getCurrentMediasource()->getMyOutData()->duration);
        //stoppedState = true;
        //pausedState = false;
        _validFile = true;
        break;
    case PSNDMEDIA::BufferedMedia:
        qDebug() << "PlayerWidget MediaStatus Event +++++++++++++++++++++++++++ BufferedMedia         " << status;
        //playingindex = _currentindex;
        //setFileDuration(mediaPlayer->getCurrentMediasource()->getMyOutData()->duration);
        if(waveformController){waveformController->startLoadWaveform();}
        _validFile = true;
        break;
    case PSNDMEDIA::EndOfMedia:
        qDebug() << "PlayerWidget MediaStatus Event +++++++++++++++++++++++++++ EndOfMedia            " << status;
        //emit publishFinished();
        break;
    case PSNDMEDIA::InvalidMedia:
        qDebug() << "PlayerWidget MediaStatus Event +++++++++++++++++++++++++++ InvalidMedia          " << status;
        if(waveformController)
            waveformController->resetWaveform();
        //phononloadError = true;
        _validFile = false;
        //emit fileSpotted(mediaPlayer->errorString());
        break;
    }
}

//! +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! +++ VARIABLES AND SETTINGS
//! +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//! +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! +++ HELPER
//! +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void PlayerWidget::on_testPushButton_clicked(){
    player->testbutton();
}

void PlayerWidget::on_getInfoPushButton_pressed()
{
    if(player->getCurrentMediasource()){
        player->getCurrentMediasource()->getInfo();
    }
}
