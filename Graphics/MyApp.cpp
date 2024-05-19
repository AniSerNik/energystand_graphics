#include "MyApp.h"

MyApp::MyApp(QWidget *parent)
    : QMainWindow(parent) {
	ui.setupUi(this);
	qtgrafic = new QTGrafics(ui.gfplot);
	grafthread = new QThread();
	qtgrafic->moveToThread(grafthread);
	grafthread->start();
	serialPort = new QSerialPort(this);
	dataAnaliz = new DataAnalitycs();
	updateGraphtmr = new QTimer();
	updateGraphtmr->setInterval(50);
	updateAnalitycstmr = new QTimer();
	updateAnalitycstmr->setInterval(5000);
	_setPortStatus("");
	_setButtonComVisualMode();
	connect(updateGraphtmr, SIGNAL(timeout()), this, SLOT(_updateGrafics()));
	connect(updateAnalitycstmr, SIGNAL(timeout()), this, SLOT(_updateAnalitycs()));
	connect(ui.actionSelectCom, &QToolButton::clicked, [=]() {openSettingsMenu(); });
	connect(ui.changeActivityCom, &QToolButton::clicked, [=]() {_changeActivityCom(); });
	connect(ui.actionResetCom, &QToolButton::clicked, [=]() {_resetComPort(); });
	connect(ui.CheckBox_Current, &QCheckBox::stateChanged, [=]() {_changeVisibleData(); });
	connect(ui.CheckBox_Voltage, &QCheckBox::stateChanged, [=]() {_changeVisibleData(); });
	connect(ui.CheckBox_Power, &QCheckBox::stateChanged, [=]() {_changeVisibleData(); });
	connect(ui.AutoUpdateGraphCB, &QCheckBox::stateChanged, [=]() {_setAutoUpdates(); });
	connect(ui.disableAutoRange, &QCheckBox::stateChanged, [=]() {_disableAutoRange(); });
	connect(ui.updateComGraphButt, &QPushButton::clicked, [=]() {_updateGrafics();});
	connect(ui.openAnalitycsButton, &QPushButton::clicked, [=]() {_openAnalitycs(); });
	connect(ui.lastDataCheckBox, &QCheckBox::stateChanged, [=]() {_setLastData(); });
	connect(ui.lastDataInput, &QSpinBox::editingFinished, [=]() {_setLastDataCount(); });
	connect(ui.clearPortDataPlain, &QPushButton::clicked, [=]() {_clearPortDataPlain(); });
	connect(ui.clearPortDataPlain, &QPushButton::clicked, [=]() {_clearPortDataPlain(); });
	connect(ui.enablePortDataPlain, &QCheckBox::stateChanged, [=]() {_setEnablePortPlain(); });
	serialPort->setDataBits(QSerialPort::Data8);
	serialPort->setParity(QSerialPort::NoParity);
	serialPort->setStopBits(QSerialPort::OneStop);
	serialPort->setFlowControl(QSerialPort::NoFlowControl);
	qtgrafic->initGraph();
}
MyApp::~MyApp() {}

void MyApp::closeEvent(QCloseEvent* event) {
	end();
}

void MyApp::start() {
	if (!serialPort->portName().isEmpty()) {
		end();
		_setPortStatus("Запуск");
		serialPort->open(QIODevice::ReadOnly);
		if (serialPort->isOpen()) {
			_setPortStatus("Открыт");
			_changeVisibleData();
			if(ui.AutoUpdateGraphCB->isChecked())
				updateGraphtmr->start();
			connect(serialPort, &QSerialPort::readyRead, this, &MyApp::readData);
			connect(serialPort, &QSerialPort::errorOccurred, this, &MyApp::_errorPort);
			_isActiveCom = true;
		}
		else
			_errorPort();
	}
	else
		openSettingsMenu();
}

void MyApp::readData() {
	if (_isActiveCom) {
		QByteArray ba = serialPort->readLine();
		charBufferRead.append(ba);
		if (ba.contains("\n")) {
			QString readString = charBufferRead.simplified();
			dataMeasure dm = _parseReadString(readString);
			if (_isValidDataDM(&dm)) {
				if (_enableportplain)
					ui.COMPortDataPlain->appendPlainText(readString);
				_addValueToGraph(dm);
			}
			charBufferRead.clear();
		}
	}
}

void MyApp::end() {
	if (serialPort->isOpen()) {
		serialPort->close();
		disconnect(serialPort, &QSerialPort::readyRead, 0, 0);
		disconnect(serialPort, &QSerialPort::errorOccurred, 0, 0);
	}
	updateGraphtmr->stop();
	_setPortStatus("Закрыт");
	_isActiveCom = false;
}

void MyApp::_errorPort() {
	end();
	_setPortStatus("Ошибка");
	_resetActivity();
}

void MyApp::_updateAnalitycs() {
	dataAnaliz->updateInfo(*qtgrafic->getDataAxisDate(), *qtgrafic->getDataAxisValue());
}

void MyApp::_setPortStatus(QString status) {
	if (!serialPort->portName().isEmpty()) {
		QString portName = serialPort->portName();
		ui.COMPortStatus->setText(portName + ": " + status);
	}
	else {
		ui.COMPortStatus->setText("COM Порт не выбран");
	}
}

void MyApp::_changeVisibleData() {
	QVector<bool> state = { ui.CheckBox_Voltage->isChecked(), \
		ui.CheckBox_Current->isChecked(), \
		ui.CheckBox_Power->isChecked() };
	qtgrafic->setEnabledGraphs(state);
	qtgrafic->updateGraph();
}

void MyApp::_addValueToGraph(dataMeasure newData) {
	qtgrafic->addData(newData);
}

void MyApp::_updateGrafics() {
	qtgrafic->updateGraph();
}

dataMeasure MyApp::_parseReadString(QString str) {
	QStringList strlist = str.split(" ");
	dataMeasure gfData = {};
	if (strlist.size() < 3)
		return dataMeasure();
	for (int i = 0; i < strlist.size(); i++) {
		QStringList strlisti = strlist[i].split(":");
		if (strlisti.size() < 2)
			break;
		double value = strlisti[1].toDouble();
		if (strlisti[0].contains("voltage"))
			gfData.voltage = value;
		else if (strlisti[0].contains("current"))
			gfData.current = value;
		else if (strlisti[0].contains("power"))
			gfData.power = value;
	}
	gfData.unix = double(QDateTime::currentMSecsSinceEpoch()) / 1000;
	return gfData;
}

void MyApp::_changeActivityCom() {
	if (_isActiveCom) {
		_resetActivity();
		_setPortStatus("Пауза");
	}
	else {
		ui.changeActivityCom->setIcon(QWidget::style()->standardIcon(QStyle::SP_MediaPause));
		start();
	}
}

void MyApp::_setButtonComVisualMode(int i) {
	if (i == 0) {
		ui.changeActivityCom->setVisible(false);
		ui.actionResetCom->setVisible(false);
		ui.actionSelectCom->setVisible(true);
	}
	else if (i == 1) {
		ui.changeActivityCom->setVisible(true);
		ui.actionResetCom->setVisible(true);
		ui.actionSelectCom->setVisible(false);
		_resetActivity();
	}
}

void MyApp::_resetActivity() {
	ui.changeActivityCom->setIcon(QWidget::style()->standardIcon(QStyle::SP_MediaPlay));
	_isActiveCom = false;
}

void MyApp::_resetComPort() {
	end();
	serialPort->setPortName("");
	_setButtonComVisualMode();
	_setPortStatus("");
}

bool MyApp::_isValidDataDM(dataMeasure* dm) {
	if (dm->voltage == -1 || dm->current == -1 || dm->power == -1)
		return false;
	return true;
}

void MyApp::_openAnalitycs() {
	dataAnaliz->openWindow();
	_updateAnalitycs();
	updateAnalitycstmr->start();
}

void MyApp::_disableAutoRange() {
	qtgrafic->disableAutoRange(ui.disableAutoRange->isChecked());
}

void MyApp::_setAutoUpdates() {
	if (ui.AutoUpdateGraphCB->isChecked())
		updateGraphtmr->start();
	else
		updateGraphtmr->stop();
	ui.updateComGraphButt->setEnabled(!ui.AutoUpdateGraphCB->isChecked());
}

void MyApp::_setLastData() {
	qtgrafic->setOnlyLastData(ui.lastDataCheckBox->isChecked());
	ui.lastDataInput->setEnabled(ui.lastDataCheckBox->isChecked());
}

void MyApp::_setLastDataCount() {
	qtgrafic->setCountLastData(ui.lastDataInput->value());
}

void MyApp::_clearPortDataPlain() {
	ui.COMPortDataPlain->clear();
}

void MyApp::_setEnablePortPlain() {
	_enableportplain = ui.enablePortDataPlain->isChecked();
}

//settigns
void MyApp::openSettingsMenu() {
	_mwindow = new QDialog(this);
	_mlayout = new QGridLayout();
	_selComLabel = new QLabel();
	_selBaudLabel = new QLabel();
	_listOfPort = new QComboBox();
	_inputBaudBox = new QSpinBox();
	_submitButt = new QPushButton();
	_selComLabel->setText("Выберите COM Порт");
	_selBaudLabel->setText("Выберите BaudRate");
	_inputBaudBox->setMaximum(115200);
	_inputBaudBox->setValue(9600);
	_submitButt->setText("Сохранить");
	_listOfPort->addItems(_getAllNamesPort());

	_mwindow->setLayout(_mlayout);
	_mlayout->addWidget(_selComLabel);
	_mlayout->addWidget(_listOfPort);
	_mlayout->addWidget(_selBaudLabel);
	_mlayout->addWidget(_inputBaudBox);
	_mlayout->addWidget(_submitButt);

	connect(_submitButt, &QPushButton::clicked, [=]() {_setSettings(); });

	_mwindow->show();
}


void MyApp::_setSettings() {
	if (_listOfPort->currentIndex() == -1 || serialInfo->availablePorts().size() <= _listOfPort->currentIndex())
		return;
	if (serialPort->isOpen())
		end();
	serialPort->setPort(serialInfo->availablePorts()[_listOfPort->currentIndex()]); //user
	serialPort->setBaudRate(_inputBaudBox->value()); //user

	_setPortStatus("Выбран");
	_setButtonComVisualMode(1);
	_mwindow->close();
}

QStringList MyApp::_getAllNamesPort() {
	QStringList allNames;
	serialInfo = new QSerialPortInfo();
	QList allPorts = serialInfo->availablePorts();
	for (int i = 0; i < allPorts.size(); i++) {
		QString namePort = allPorts[i].portName() + " (" + allPorts[i].description();
		if (!allPorts[i].serialNumber().isNull())
			namePort += " [" + allPorts[i].serialNumber() + "]";
		namePort += ")";
		allNames.append(namePort);
	}
	return allNames;
}