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
**	File created on: 13/12/2015
*/

#include <QApplication>
#include "settingsdialog.h"
#include "speedicon.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("SGH");
	QCoreApplication::setOrganizationDomain("http://example.org");
	QCoreApplication::setApplicationName("SpeedIcon");
	
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(false);
	SpeedIcon icon;
	icon.show();
	
	return a.exec();
}
