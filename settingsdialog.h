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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include "speedicon.h"

namespace Ui {
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit SettingsDialog(QWidget *parent = 0);
	void Associate(SpeedIcon*);
	~SettingsDialog();
	
private slots:
	void on_SettingsDialog_accepted();
	
	void on_buttonBox_clicked(QAbstractButton *button);
	
private:
	Ui::SettingsDialog *ui;
	SpeedIcon* m_icon;
};

#endif // SETTINGSDIALOG_H
