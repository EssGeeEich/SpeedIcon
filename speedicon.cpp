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

#include "speedicon.h"
#include "settingsdialog.h"
#include <QPainter>
#include <QApplication>

recv_at_tp::recv_at_tp(std::uint64_t expSeconds, std::uint64_t in, std::uint64_t out, std::uint64_t rawIn, std::uint64_t rawOut)
	: m_expiration(ck::now() + std::chrono::seconds(expSeconds)),
	  m_in_speed(in), m_out_speed(out),
	  m_in_byte(rawIn), m_out_byte(rawOut)
{}

QTextOption options()
{
	QTextOption o(Qt::AlignCenter);
	o.setWrapMode(QTextOption::NoWrap);
	return o;
}

SpeedIcon::SpeedIcon(QObject *parent) :
	QSystemTrayIcon(parent),
	m_settingsDialog(nullptr),
	m_cachedPen(qRgba(255,255,255,255)),
	//m_cachedFontA("Arial"),
	//m_cachedFontB("Arial"),
	//m_size(24),
	//m_splitPercent(0.55),
	m_lastTime(ck::now())
{
	m_infoMenu.setTitle(tr("Speed Icon: Statistics"));
	m_infoMenu.setWindowTitle(tr("Speed Icon: Statistics"));
	m_infoMenu.setTearOffEnabled(true);
	setContextMenu(&m_infoMenu);
	m_genericInfo = m_infoMenu.addAction(tr("Loading..."));
	m_downloadSpeed = m_infoMenu.addAction(QString());
	m_uploadSpeed = m_infoMenu.addAction(QString());
	m_infoMenu.addSeparator();
	m_avgDownloadSpeed = m_infoMenu.addAction(QString());
	m_avgUploadSpeed = m_infoMenu.addAction(QString());
	m_infoMenu.addSeparator();
	m_downloadedSize = m_infoMenu.addAction(QString());
	m_uploadedSize = m_infoMenu.addAction(QString());
	
	connect(m_genericInfo,&QAction::triggered,[&]()->void{
		m_recv_data.clear();
		Update();
		//UpdateTooltip();
	});
	m_downloadSpeed->setEnabled(false);
	m_uploadSpeed->setEnabled(false);
	m_avgDownloadSpeed->setEnabled(false);
	m_avgUploadSpeed->setEnabled(false);
	m_downloadedSize->setEnabled(false);
	m_uploadedSize->setEnabled(false);
	
	QSettings s;
	
	connect(&m_updateTimer,&QTimer::timeout,this,&SpeedIcon::Update);
	m_updateTimer.setSingleShot(false);
	m_updateTimer.setInterval(1000);
	m_updateTimer.start();
	
	connect(this, &QSystemTrayIcon::activated, this, &SpeedIcon::onActivation);
	
	Reload(s);
	setIcon(QIcon(m_iconPixmap));
}

struct DataInfo {
	QString m_text;
	std::uint64_t m_trigger;
	std::uint64_t m_factor;
};

static DataInfo const g_units[] = {
	{ "B",	0,				1},
	{"KB",	1000,			1024},
	{"MB",	1000000,		1048576},
	{"GB",	1000000000,		1073741824},
	{"TB",	1000000000000,	1099511627776}
};

static std::size_t const g_unitCount = (sizeof(g_units)/sizeof(*g_units));

static void SplitSpeed(std::uint64_t bps, QString* value, QString* vunit, bool isSpeed = true)
{
	if(!value && !vunit)
		return;
	DataInfo const* unit = g_units;
	for(std::size_t i=0; i <g_unitCount; ++i)
	{
		if(bps >= g_units[i].m_trigger)
			unit = g_units+i;
		else
			break;
	}
	QString num;
	if(unit->m_factor == 1)
		num = QString::number(bps);
	else if(value != vunit)
		num = QString::number(static_cast<std::uint64_t>(static_cast<long double>(bps) / static_cast<long double>(unit->m_factor)));
	else
		num = QString::number(static_cast<long double>(bps) / static_cast<long double>(unit->m_factor),'f',1);
	
	QString const& unitStr = unit->m_text;
	
	if(value)
		value->clear();
	if(vunit && vunit != value)
		vunit->clear();
	
	if(value)
		*value += num;
	if(value == vunit)
		*value += QStringLiteral(" ");
	if(vunit)
		*vunit += unitStr;
	if(isSpeed && value == vunit)
		*vunit += QStringLiteral("/s");
}

void SpeedIcon::Reload(const QSettings& s)
{
	m_monitoredInterface = s.value("monitored-interface").toString().toStdWString();
	m_lastTime = ck::now();
	m_lastInfo = SysNetInfo::GetNetworkedData(m_monitoredInterface);
	m_recv_data.clear();
	m_statDataInterval = s.value("sbStatInterval",15).toInt();
	
	m_size = s.value("sbIconSize",24).toInt();
	if(m_size < 8)
		m_size = 8;
	else if(m_size > 512)
		m_size = 512;
	m_splitPercent = static_cast<double>(s.value("vsSpacing",55).toInt()) * 0.01; // 0-100
	double const countPtSize = (m_splitPercent * static_cast<qreal>(m_size) * s.value("sbFont1Mul",1.0).toDouble()) + s.value("sbFont1Extra",0.0).toDouble();
	double const unitPtSize = ((1.0 - m_splitPercent) * static_cast<qreal>(m_size) * s.value("sbFont2Mul",1.0).toDouble()) + s.value("sbFont2Extra",0.0).toDouble();
	m_iconPixmap = QPixmap(m_size,m_size);
	m_iconPixmap.fill(Qt::transparent);
	
	m_cachedFontA = QFont(s.value("cbFont1","Arial").toString());
	m_cachedFontA.setStyleStrategy(QFont::NoAntialias);
	m_cachedFontA.setStretch(100 - (s.value("hsCondens",9).toInt() * 8)); // 0-10
	m_cachedFontA.setLetterSpacing(QFont::PercentageSpacing, static_cast<qreal>(s.value("hsSpac",9).toInt()) * 10.0); // 0-10
	m_cachedFontA.setPointSizeF(countPtSize);
	
	m_cachedFontB = QFont(s.value("cbFont2","Arial").toString());
	m_cachedFontB.setStyleStrategy(QFont::NoAntialias);
	m_cachedFontB.setPointSizeF(unitPtSize);
	
	int intvl = s.value("sbMonitorInterval",1000).toInt();
	if(intvl < 10)
		intvl = 10;
	else if(intvl > 10000)
		intvl = 10000;
	m_updateTimer.stop();
	Update();
	m_updateTimer.setInterval(intvl);
	m_updateTimer.start();
}

QIcon SpeedIcon::GenerateIcon(const QString &top, const QString &bot)
{
	m_iconPixmap.fill(Qt::transparent);
	QPainter m_painter(&m_iconPixmap);
	m_painter.setPen(m_cachedPen);
	int const split_loc = static_cast<int>(static_cast<qreal>(m_size) * m_splitPercent);
	m_painter.setFont(m_cachedFontA);
	m_painter.drawText(QRect(0,0,m_size,split_loc), top, options());
	m_painter.setFont(m_cachedFontB);
	m_painter.drawText(QRect(0,split_loc,m_size,m_size-split_loc), bot, options());
	return QIcon(m_iconPixmap);
}

void SpeedIcon::UpdateTooltip()
{
	std::uint64_t m_max_in = 0;
	std::uint64_t m_max_out = 0;
	long double m_cuml_in = 0.0;
	long double m_cuml_out = 0.0;
	std::uint64_t m_downloaded = 0;
	std::uint64_t m_uploaded = 0;
	ck::time_point now = ck::now();
	while(!m_recv_data.empty() && (now.time_since_epoch() >= m_recv_data.front().m_expiration.time_since_epoch()))
		m_recv_data.pop_front();
	
	for(auto it = m_recv_data.begin(); it != m_recv_data.end(); ++it)
	{
		if(it->m_in_speed > m_max_in)
			m_max_in = it->m_in_speed;
		if(it->m_out_speed > m_max_out)
			m_max_out = it->m_out_speed;
		m_cuml_in += static_cast<long double>(it->m_in_speed);
		m_cuml_out += static_cast<long double>(it->m_out_speed);
		
		if((m_downloaded + it->m_in_byte) < m_downloaded)
			m_downloaded = std::numeric_limits<std::uint64_t>::max();
		else
			m_downloaded += it->m_in_byte;
		
		if((m_uploaded + it->m_out_byte) < m_uploaded)
			m_uploaded = std::numeric_limits<std::uint64_t>::max();
		else
			m_uploaded += it->m_out_byte;
	}
	
	if(m_cuml_in >= 0.001L)
		m_cuml_in /= static_cast<long double>(m_recv_data.size());
	if(m_cuml_out >= 0.001L)
		m_cuml_out /= static_cast<long double>(m_recv_data.size());
	QString tooltip;
	QString speed;
	
	m_genericInfo->setText(tr("In the last %1 seconds:").arg(m_statDataInterval));
	
	SplitSpeed(m_max_in,&speed,&speed);
	tooltip = tr("Top Download Speed: ");
	tooltip += speed;
	m_downloadSpeed->setText(tooltip);
	
	
	SplitSpeed(m_max_out,&speed,&speed);
	tooltip = tr("Top Upload Speed: ");
	tooltip += speed;
	m_uploadSpeed->setText(tooltip);
	
	SplitSpeed(static_cast<std::uint64_t>(m_cuml_in),&speed,&speed);
	tooltip = tr("Avg Download Speed: ");
	tooltip += speed;
	m_avgDownloadSpeed->setText(tooltip);
	
	SplitSpeed(static_cast<std::uint64_t>(m_cuml_out),&speed,&speed);
	tooltip = tr("Avg Upload Speed: ");
	tooltip += speed;
	m_avgUploadSpeed->setText(tooltip);
	
	SplitSpeed(m_downloaded,&speed,&speed,false);
	tooltip = tr("Downloaded: ");
	tooltip += speed;
	m_downloadedSize->setText(tooltip);
	
	SplitSpeed(m_uploaded,&speed,&speed,false);
	tooltip = tr("Uploaded: ");
	tooltip += speed;
	m_uploadedSize->setText(tooltip);
}

void SpeedIcon::Update()
{
	ck::time_point now = ck::now();
	if(now == m_lastTime)
		return; // Cannot compute time -> cannot compure speed. Ignore.
	
	ck::duration::rep c = ck::duration(now - m_lastTime).count();
	if(!c)
		return; // Cannot compute time -> cannot compure speed. Ignore.
	
	m_lastTime = now;
	if(c < 0)
		return;
	
	long double TimeFactor = 1.0L /
			(static_cast<long double>(c) * static_cast<long double>(ck::duration::period::num) / static_cast<long double>(ck::duration::period::den));
	NetworkedData d = SysNetInfo::GetNetworkedData(m_monitoredInterface);
	
	if(d.m_recv < m_lastInfo.m_recv)
	{
		// Interface changed; reset infos
		m_lastInfo = d;
		setIcon(GenerateIcon(QString(),QStringLiteral("...")));
		return;
	}
	
	std::uint64_t inBps = static_cast<std::uint64_t>(static_cast<long double>(d.m_recv - m_lastInfo.m_recv) * TimeFactor);
	std::uint64_t outBps = static_cast<std::uint64_t>(static_cast<long double>(d.m_send - m_lastInfo.m_send) * TimeFactor);
	std::uint64_t rawInBps = d.m_recv - m_lastInfo.m_recv;
	std::uint64_t rawOutBps = d.m_send - m_lastInfo.m_send;
	m_lastInfo = d;
	
	QString unitCount; QString unitStr;
	SplitSpeed(inBps,&unitCount,&unitStr);
	
	setIcon(GenerateIcon(unitCount,unitStr));
	
	m_recv_data.push_back(recv_at_tp(m_statDataInterval,inBps,outBps,rawInBps,rawOutBps));
	UpdateTooltip();
}

void SpeedIcon::onActivation(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::DoubleClick)
	{
		if(m_settingsDialog)
		{
			m_settingsDialog->setFocus();
			return;
		}
		
		SettingsDialog settings;
		m_settingsDialog = &settings;
		connect(&settings, &SettingsDialog::destroyed, [&]()->void{
			m_settingsDialog = nullptr;
		});
		settings.Associate(this);
		settings.setModal(true);
		settings.setWindowModality(Qt::ApplicationModal);
		settings.exec();
	}
	else if(reason == QSystemTrayIcon::MiddleClick)
	{
		qApp->quit();
	}
}
