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

PlayerWidget::PlayerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerWidget),
    player(new PSndPlayer(this)),
    waveformCursorProxy(new WaveformCursorProxy(this)),
    waveformSelectionProxy(new WaveformSelectionProxy(this)),
    ruler(new WaveformRuler(true,this)),
    waveformScrolBar(new WaveformScrollBar(this)),
    paused(false),
    hasRuler(false),
    speed(1)
{
    ui->setupUi(this);
    outputDevices = player->audioOutPut()->getOutputDevices();

    //! Set the Combobox for available AudioDevices
    for(auto iter: outputDevices){
#ifdef _WIN32
         QString dev = QString::fromStdWString((iter.second).name);
#else
        QString dev = QString::fromStdString((iter.second).name);
#endif
        ui->devicesComboBox->addItem(dev.toUtf8());
    }

    //! set the Combobox CurrentoutputDevic
    for(auto iter: outputDevices){
        if(player->audioOutPut()->getCurrentOutputDevice() ==
                iter.second.deviceIndex){
            ui->devicesComboBox->setCurrentText(QString::psnd_fromStdString(iter.second.name));
        }
    }

    //! Set the Combobox for available Samplerates of active AudioDevice

    for(auto iter: outputDevices){
        if(iter.second.name == ui->devicesComboBox->currentText().psnd_toStdString().c_str()){
            OutDevice currentDevice = iter.second;
            //qDebug() << "current Device in widget: " << QString::psnd_fromStdString(currentDevice.name) << endl;
            //qDebug() << "current Device in widget samplerate: " << QString::fromStdString(currentDevice.currentSamplerate) << endl;

            //! set Combobox abailable Samplerates
            for(auto iter: currentDevice.availableSamplerates){
                QString sr = QString::fromStdString(iter.first);
                ui->sampleratesComboBox->addItem(sr.toUtf8());
            }

            //! set Combobox current Item
            auto search =
                    currentDevice.availableSamplerates.find(currentDevice.currentSamplerate);
            if(search != currentDevice.availableSamplerates.end()){
                QString current = QString::fromStdString(search->first);
                int id = search->second;
                ui->sampleratesComboBox->setCurrentIndex(search->second);
                qDebug() << "current Samplerate: " << current << "id: " <<id;
            }

        }
    }


    player->getPlayerTicker()->registerReceiver(this);
    player->getPlayerTicker()->registerReceiver(waveformCursorProxy);
    ui->label_position->setText("0.0000");
    //ui->label->setBackgroundRole(QPalette::Dark);
    //ui->label->setForegroundRole(QPalette::HighlightedText);


    ui->seekSlider->setRange(0, 10*1000);

    ui->rulerLayout_->addWidget(ruler);
    ui->scrollBarLayout->addWidget(waveformScrolBar);
    ui->cursorGroupBox->setId(ui->cursorArrowButton,0);
    ui->cursorGroupBox->setId(ui->cursorIbeamButton,1);
    ui->cursorGroupBox->setId(ui->cursorOpenHandButton,2);


    ui->hZoomSlider->setRange(1,299);
    connect(waveformSelectionProxy, SIGNAL(waveformSelectionChanged(double,double,Waveform*)),
        this, SLOT(changeSelection(double,double,Waveform*)));
    ui->vZoomSlider->setValue(50);
    ui->vZoomSlider->setRange(1,100);
}

PlayerWidget::~PlayerWidget()
{
    player->getPlayerTicker()->unregisterReceiver(this);
    player->getPlayerTicker()->unregisterReceiver(waveformCursorProxy);
    delete player;
    delete ui;
}

void PlayerWidget::setInitialFile(QString text){
    ui->lineEdit->setText(text);
}

void PlayerWidget::customEvent(QEvent *e){
    if (e->type() == (QEvent::Type)PlayerPosition){
      ui->label_position->setText(time2str(((PlayerPositionEvent*)e)->time(),4));
      ui->seekSlider->setSliderPosition(((PlayerPositionEvent*)e)->time()*1000);
    }

    if (e->type() == (QEvent::Type)PlayerDuration){
      ui->label_duration->setText(time2str(((PlayerDurationEvent*)e)->duration(),4));
    }
}

int PlayerWidget::getVolume(){
    return 0;
}

void PlayerWidget::changeSelection(double beg, double dur, Waveform*)
{
    qDebug() << "changeSelection beg : " << beg;
    startTime = beg;
    selectionduration = dur;
    endTime = beg+dur;
    //player->setPosition();
    player->setPosition(beg*1000);
    ui->label_position->setText(time2str((double)beg,4));
    //PLAYER_DEBUG << "double beg: " << beg;
    //selectionBeginTimeLabel->setTime(beg);
    //selectionEndTimeLabel->setTime(beg+dur);
    //selectionDurationTimeLabel->setTime(dur);
    //createSndFile->setTime(beg,dur);
}

void PlayerWidget::unregisterWaveform(){
    hasRuler = false;
    int count = ui->gridLayout->rowCount();
    for(int i=0;i<count;i++){
        if(!(ui->gridLayout->itemAtPosition(i,0)==0))
            ui->gridLayout->removeItem(ui->gridLayout->itemAtPosition(i,0));
    }
    map<psnd_string,WaveformMap >::iterator witer;
    witer  = waveforms.find(currentFile);
    if(witer!= waveforms.end()){
        WaveformMap wMap = witer->second;
        WaveformMap::iterator channeliter;
        for(channeliter = wMap.begin();channeliter != wMap.end();channeliter++){
            Waveform *waveform = channeliter->second;
            //waveformCursorProxy->unregisterWaveform(waveform);
            //waveformSelectionProxy->unregisterWaveform(waveform);
            //ruler->disconnectWaveform();
            waveform->hide();
        }
    }
}

void PlayerWidget::on_tableWidget_cellPressed(int row, int column)
{
    qDebug() << "on_tableWidget_cellPressed: " << row << column;
    ui->lineEdit->setText(ui->tableWidget->item(row,column)->text());
    player->stop();
    on_addAndPlayPushButton_clicked();
    //on_addPushButton_clicked();
    //on_loadWavePushButton_clicked();
}

void PlayerWidget::loadWaveform(){
    currentSources = player->getCurrentSources();

    map<psnd_string,MediaSource*>::iterator sourceIter;
    psnd_string filename;
    //map<psnd_string,WaveformMap >::iterator witer;
    int gridCurRow = 0;
    Waveform* waveform =0;
    MediaSource *mediaSource = 0;
    unregisterWaveform();
    filename = ui->lineEdit->text().psnd_toStdString();

    if(currentSources.find(filename) != currentSources.end()){
            mediaSource = currentSources[filename];
            currentFile = filename;
            double len = mediaSource->duration();
            qDebug() << "len: " << len;
            qDebug() << "ch: " << mediaSource->getMyOutData()->channels;
            ui->seekSlider->setRange(0, len*1000);
            if(waveforms.find(filename) == waveforms.end()){
                waveFormBuffer = new WaveFormBuffer(this,
                                                filename.c_str(),
                                                mediaSource->getMyOutData()->outSampleRate,
                                                mediaSource->getMyOutData()->channels,
                                                mediaSource->getMyOutData()->num_frames,
                                                len);
                //qDebug() << "calling loadBuffer 1 --------------------------------------";
                //waveFormBuffer->loadBuffer(mediaSource->getMyOutData()->data,
                //                       0,
                //                       len);
                for (int ch=0; ch<mediaSource->getMyOutData()->channels; ++ch)
                {
                    //WaveformVRuler *verticalRuler = new WaveformVRuler(this);
                    waveform = new Waveform  (waveFormBuffer, ch, 0.0, len, this); //4ter Parameter ist Duration
                    //waveform->setObjectName(tr("waveform%1").arg(_sndfiles.size()));
                    //waveform->setWaveformCursorShape(_waveformCursorShape);
                    //ui->gridLayout->addWidget(verticalRuler, gridCurRow, 0); // Vertical Ruler (WaveformRuler)

                    waveformSelectionProxy->registerWaveform(waveform);
                    waveformCursorProxy->registerWaveform(waveform);

                    //waveformScrolBar->registerWaveform(waveform);

                    connect(waveformSelectionProxy,SIGNAL(waveformSelectionChanged(double,double,Waveform*)),
                            waveform,SLOT(setSelectionParameter(double,double,Waveform*)));
                    connect(ui->cursorGroupBox,SIGNAL(buttonClicked(int)),
                            waveform,SLOT(setWaveformCursorShape(int)));
                    waveforms[filename][ch] = waveform;


                }// for channels

            }//! existing waveform?

            waveformMap = waveforms[filename];
            WaveformMap::iterator channeliter;
            bool runOnce = true;
            for(channeliter = waveformMap.begin();channeliter != waveformMap.end();channeliter++){
                waveform = channeliter->second;
                if(runOnce){
                    waveform->getWaveFormBuffer()->noCopyBuffer(mediaSource->getMyOutData()->data,0,len);
                    runOnce = false;
                }
                ui->gridLayout->addWidget(waveform, gridCurRow, 0); // Waveform
                //verticalRuler->connectToWaveform(waveform);
                //verticalRuler->show();

                waveform->show();
                if (hasRuler == false) {
                    ruler->connectToWaveform(waveform);

                    ruler->show();
                    //waveform->setHasRuler(true);
                    hasRuler = true;
                }
                //connect(waveform, SIGNAL(waveformMouseMoved(Waveform*,double)),
                //    this, SLOT(setTime(Waveform*,double)));
                //connect(waveform, SIGNAL(waveformMouseMoved(double)),
                //    this, SLOT(seek(int)));

                //! nach Click in Waveform wird ab Klick weitergespielt
                //connect(waveform, SIGNAL(waveformMouseReleased(Waveform*,double)),
                //    this, SLOT(onRelChangePlayPosition()));
                //connect(this, SIGNAL(setVertZoom(double)),
                //        waveform, SLOT(setAmplitudeRatio(double)));
                //connect(this,SIGNAL(waveformCursorChanged(int)),
                //        waveform,SLOT(setWaveformCursorShape(int)));

                //QColor waveformColor(QColor(42, 38, 38));
                //QColor backgroundColor(QColor(85, 85, 127));
                //soundfile->setColor(waveformColor,backgroundColor);
                waveform->display(0,len,true);
                ++gridCurRow;
            }//! iterchannels
        }// if filename
      //player->resume();
       //QSlider *hzoomSlider = new QSlider(this);
       //hzoomSlider->setOrientation(Qt::Horizontal);
       //hzoomSlider->setFixedWidth(300);
       //hzoomSlider->setMinimum(-10);
       //hzoomSlider->setMaximum(10);
       //grid->addWidget(hzoomSlider, 4,0);
    //}//! for sources
}

void PlayerWidget::on_loadWavePushButton_clicked(){

    qDebug() << "on_loadWavePushButton_clicked";
    loadWaveform();


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
    player->pause();

    if(player->addFile( ui->lineEdit->text().psnd_toStdString().c_str()))
    {
        //loadWaveform();
    }
}

void PlayerWidget::on_addPushButton_clicked(){
    qDebug() << "on_addPushButton_clicked!";
    player->pause();
    player->addFile(ui->lineEdit->text().psnd_toStdString().c_str());

}

void PlayerWidget::on_removePushButton_clicked(){

}

void PlayerWidget::on_reverseButton_clicked(bool checked){
    qDebug() << "on_reverseButton_clicked: " << checked;
    player->reverse(checked);
}

void PlayerWidget::on_playPushButton_clicked(){
    speed = 1;
    player->audioOutPut()->setPlaydirection(FORWARD);
    paused = false;
    ui->pausePushButton->setText("||");
    player->play(0,5);
}

void PlayerWidget::on_revPlayPushButton_clicked(){
    player->audioOutPut()->setPlaydirection(REVERSE);

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

void PlayerWidget::on_repeatPushButton_clicked(){
    paused = false;
    ui->pausePushButton->setText("||");
    //player->repeat(waveformSelection->getBeginSeconds(),
      //	 waveformSelection->getWidthSeconds());
}

void PlayerWidget::on_stopPushButton_clicked(){
    qDebug() << "onstopPushButtonClicked";
    speed = 1;
    player->stop();
}

void PlayerWidget::on_pausePushButton_clicked(){

}

void PlayerWidget::on_devicesComboBox_currentIndexChanged(QString item){
    int _id =0;
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    qDebug() << "QT Version 5";
    //! c++11
    for(auto iter: outputDevices){
        #ifdef _WIN32
        if(iter.second.name == item.toStdWString().c_str()){
        #else
        if(iter.second.name == item.toStdString().c_str()){
        #endif
            _id =  iter.second.deviceIndex;
        }
    }
#else
    qDebug() << "QT Version 4";
    map<int,string>::iterator iter;
    for(iter = outDevices.begin();iter != outDevices.end();++iter) {
        if(iter->second == id.toStdString().c_str()){
        _id =  iter->first;
        }
    }
#endif
    player->audioOutPut()->setOutputDevice(_id);
}

//! fuer Mainwindow
void PlayerWidget::on_cursorGroupBox_buttonClicked(int id){
    qDebug() << "on_cursorGroupBox_buttonClicked: " << id;
    emit publishWaveformCursorChanged(id);
}

void PlayerWidget::on_seekSlider_sliderMoved(int value){
    qDebug() << "on_seekSlider_sliderMoved: " << value;
    player->setPosition(value);
    ui->label_position->setText(time2str((double)value/1000,4));
}

void PlayerWidget::on_seekSlider_sliderPressed()
{
    qDebug() << "on_seekSlider_sliderPressed: " ;
    player->setPosition(ui->seekSlider->value());
    ui->label_position->setText(time2str((double)ui->seekSlider->value()/1000,4));

}




void PlayerWidget::on_volumeSlider_valueChanged(int value){
    qDebug() << "on_volumeSlider_valueChanged" << value;
    player->setVolume(value);
}

void PlayerWidget::on_vZoomSlider_sliderMoved(int value){
    double vratio = value/50.0;
    qDebug() << "on_vZoomSlider_sliderMoved: " << vratio;
    //vratio *= 1.5;
    //vratio /= 1.5;
    //emit setVertZoom(vratio);

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
}

void PlayerWidget::on_hZoomSlider_sliderMoved(int value){
    qDebug() << "on_hZoomSlider_sliderMoved: " << value;
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
}

void PlayerWidget::on_testPushButton_clicked(){
    player->testbutton();
}


void PlayerWidget::on_getInfoPushButton_pressed()
{
    if(player->getCurrentMediasource()){
        player->getCurrentMediasource()->getInfo();
    }
}



void PlayerWidget::on_sampleratesComboBox_currentTextChanged(const QString &arg1)
{
    player->audioOutPut()->setOutpuDeviceSamplerate(arg1.toStdString().c_str());
}
