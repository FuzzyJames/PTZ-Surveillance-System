#pragma once

#include <QWidget>
#include <QCalendarWidget>
#include <QListWidget>
#include <QDate>

/// Calendar panel for selecting dates and browsing recorded video files.
class CalendarPanel : public QWidget {
    Q_OBJECT

public:
    explicit CalendarPanel(QWidget* parent = nullptr);
    ~CalendarPanel() override;

signals:
    void dateSelected(const QDate& date);
    void fileSelected(const QString& filepath);

private slots:
    void onDateClicked(const QDate& date);
    void onFileClicked(QListWidgetItem* item);

private:
    void loadRecordingsForDate(const QDate& date);

    QCalendarWidget* m_calendar = nullptr;
    QListWidget*     m_fileList = nullptr;
};
