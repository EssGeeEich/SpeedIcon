/*	Copyright (c) 2015 SGH
**	
**	Permission is granted to use, modify and redistribute this software.
**	Modified versions of this software MUST be marked as such.
**	
**	This software is provided "AS IS". In no event shall
**	the authors or copyright holders be liable for any claim,
**	damages or other liability. The above copyright notice
**	and this permission notice shall be included in all copies
**	or substantial portions of the software.
**	
**	File created on: 14/12/2015
*/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "sysnetinfo.h"
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog),
	m_icon(nullptr)
{
	ui->setupUi(this);
	std::vector<std::wstring> intfc;
	if(!SysNetInfo::GetNetworkInterfaces(intfc))
		throw std::runtime_error("Could not retrieve network interfaces!");
	
	QSettings s;
	std::wstring match = s.value("monitored-interface").toString().toStdWString();
	ui->cbFont1->setCurrentFont(QFont(s.value("cbFont1").toString()));
	ui->cbFont2->setCurrentFont(QFont(s.value("cbFont2").toString()));
	ui->hsCondens->setValue(s.value("hsCondens",ui->hsCondens->minimum()).toInt());
	ui->hsSpac->setValue(s.value("hsSpac",ui->hsSpac->maximum()).toInt());
	ui->sbFont1Extra->setValue(s.value("sbFont1Extra",ui->sbFont1Extra->value()).toDouble());
	ui->sbFont1Mul->setValue(s.value("sbFont1Mul",ui->sbFont1Mul->value()).toDouble());
	ui->sbFont2Extra->setValue(s.value("sbFont2Extra",ui->sbFont2Extra->value()).toDouble());
	ui->sbFont2Mul->setValue(s.value("sbFont2Mul",ui->sbFont2Mul->value()).toDouble());
	ui->sbIconSize->setValue(s.value("sbIconSize",ui->sbIconSize->value()).toInt());
	ui->sbMonitorInterval->setValue(s.value("sbMonitorInterval",ui->sbMonitorInterval->value()).toInt());
	ui->sbStatInterval->setValue(s.value("sbStatInterval",ui->sbStatInterval->value()).toInt());
	ui->vsSpacing->setValue(s.value("vsSpacing",ui->vsSpacing->value()).toInt());
	
	std::size_t selection = std::size_t(-1);
	for(std::size_t i=0; i < intfc.size(); ++i)
	{
		std::wstring const& w = intfc[i];
		if(w == match)
			selection = i;
		ui->cbInterface->addItem(QString::fromStdWString(w));
	}
	if(selection != std::size_t(-1))
		ui->cbInterface->setCurrentIndex(selection);
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::Associate(SpeedIcon* i)
{
	m_icon = i;
}

void SettingsDialog::on_SettingsDialog_accepted()
{
	QSettings s;
	s.setValue("monitored-interface",ui->cbInterface->currentText());
	s.setValue("cbFont1",ui->cbFont1->currentFont().family());
	s.setValue("cbFont2",ui->cbFont2->currentFont().family());
	s.setValue("hsCondens",ui->hsCondens->value());
	s.setValue("hsSpac",ui->hsSpac->value());
	s.setValue("sbFont1Extra",ui->sbFont1Extra->value());
	s.setValue("sbFont1Mul",ui->sbFont1Mul->value());
	s.setValue("sbFont2Extra",ui->sbFont2Extra->value());
	s.setValue("sbFont2Mul",ui->sbFont2Mul->value());
	s.setValue("sbIconSize",ui->sbIconSize->value());
	s.setValue("sbMonitorInterval",ui->sbMonitorInterval->value());
	s.setValue("sbStatInterval",ui->sbStatInterval->value());
	s.setValue("vsSpacing",ui->vsSpacing->value());
	m_icon->Reload(s);
}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole)
		on_SettingsDialog_accepted();
}
