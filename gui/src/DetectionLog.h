#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

/// Scrollable detection log panel. Displays vehicle and person detection
/// events with timestamps, auto-scrolls, and supports text search.
class DetectionLog : public QWidget {
    Q_OBJECT

public:
    explicit DetectionLog(QWidget* parent = nullptr);
    ~DetectionLog() override;

    /// Append a new log entry (thread-safe via signal)
    void appendEntry(const QString& entry);

    /// Clear all entries
    void clearLog();

signals:
    void entryAdded(const QString& entry);

private slots:
    void onSearch();
    void onEntryAdded(const QString& entry);

private:
    QTextEdit*   m_logView = nullptr;
    QLineEdit*   m_searchBox = nullptr;
    QPushButton* m_searchBtn = nullptr;
    QPushButton* m_clearBtn = nullptr;
};
