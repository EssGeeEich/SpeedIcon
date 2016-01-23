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

#ifndef SPEEDICON_H
#define SPEEDICON_H

#include <QSystemTrayIcon>
#include <QTimer>
#include <QPixmap>
#include <QPen>
#include <QFont>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <chrono>
#include "sysnetinfo.h"
class SettingsDialog;

using ck = std::chrono::high_resolution_clock;

struct recv_at_tp {
	recv_at_tp(std::uint64_t expSeconds, std::uint64_t in, std::uint64_t out, std::uint64_t rawIn, std::uint64_t rawOut);
	ck::time_point m_expiration;
	std::uint64_t m_in_speed;
	std::uint64_t m_out_speed;
	std::uint64_t m_in_byte;
	std::uint64_t m_out_byte;
};

class SpeedIcon : public QSystemTrayIcon
{
	Q_OBJECT
public:
	explicit SpeedIcon(QObject *parent = 0);
	void Reload(QSettings const& = QSettings());
	void Update();
	void UpdateTooltip();
	QIcon GenerateIcon(QString const& top, QString const& bot);
private:
	void onActivation(QSystemTrayIcon::ActivationReason reason);
	
	QTimer m_updateTimer;
	SettingsDialog* m_settingsDialog;
	std::uint64_t m_statDataInterval;
	
	QMenu m_infoMenu;
	QAction* m_genericInfo;
	QAction* m_downloadSpeed;
	QAction* m_uploadSpeed;
	QAction* m_avgDownloadSpeed;
	QAction* m_avgUploadSpeed;
	QAction* m_downloadedSize;
	QAction* m_uploadedSize;
	
	QPixmap m_iconPixmap;
	QPen m_cachedPen;
	QFont m_cachedFontA;
	QFont m_cachedFontB;
	int m_size;
	double m_splitPercent;
	
	std::wstring m_monitoredInterface;
	ck::time_point m_lastTime;
	NetworkedData m_lastInfo;
	std::list<recv_at_tp> m_recv_data;
};

#endif // SPEEDICON_H
