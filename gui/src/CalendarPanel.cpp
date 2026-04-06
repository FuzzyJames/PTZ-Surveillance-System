#include "CalendarPanel.h"
#include "surveillance/logging.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

CalendarPanel::CalendarPanel(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    // Calendar widget
    auto* calGroup = new QGroupBox(tr("Select Date"));
    auto* calLayout = new QVBoxLayout(calGroup);
    m_calendar = new QCalendarWidget();
    m_calendar->setGridVisible(true);
    m_calendar->setMaximumDate(QDate::currentDate());
    calLayout->addWidget(m_calendar);
    layout->addWidget(calGroup);

    // File list
    auto* filesGroup = new QGroupBox(tr("Recordings"));
    auto* filesLayout = new QVBoxLayout(filesGroup);
    m_fileList = new QListWidget();
    filesLayout->addWidget(m_fileList);
    layout->addWidget(filesGroup);

    connect(m_calendar, &QCalendarWidget::clicked, this, &CalendarPanel::onDateClicked);
    connect(m_fileList, &QListWidget::itemClicked, this, &CalendarPanel::onFileClicked);
}

CalendarPanel::~CalendarPanel() = default;

void CalendarPanel::onDateClicked(const QDate& date) {
    LOG_INFO("Date selected: %s", date.toString("yyyy-MM-dd").toStdString().c_str());
    emit dateSelected(date);
    loadRecordingsForDate(date);
}

void CalendarPanel::onFileClicked(QListWidgetItem* item) {
    if (item) {
        QString filepath = item->data(Qt::UserRole).toString();
        LOG_INFO("File selected: %s", filepath.toStdString().c_str());
        emit fileSelected(filepath);
    }
}

void CalendarPanel::loadRecordingsForDate(const QDate& date) {
    m_fileList->clear();

    // TODO: Query StorageService via gRPC for recordings on this date
    // surveillance::PlaybackRequest request;
    // request.set_date(date.toString("yyyy-MM-dd").toStdString());
    // auto response = m_storageStub->ListRecordings(context, request);
    //
    // For now, show placeholder
    m_fileList->addItem(tr("(Connect to cluster to load recordings)"));
}
