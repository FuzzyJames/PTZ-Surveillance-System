#include "DetectionLog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QTextCursor>
#include <QGroupBox>

DetectionLog::DetectionLog(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    auto* group = new QGroupBox(tr("Detection Log"));
    auto* groupLayout = new QVBoxLayout(group);

    // Search bar
    auto* searchLayout = new QHBoxLayout();
    m_searchBox = new QLineEdit();
    m_searchBox->setPlaceholderText(tr("Search detections..."));
    m_searchBtn = new QPushButton(tr("Search"));
    m_clearBtn = new QPushButton(tr("Clear"));
    searchLayout->addWidget(m_searchBox, 1);
    searchLayout->addWidget(m_searchBtn);
    searchLayout->addWidget(m_clearBtn);
    groupLayout->addLayout(searchLayout);

    // Log view
    m_logView = new QTextEdit();
    m_logView->setReadOnly(true);
    m_logView->setFont(QFont("Consolas", 9));
    m_logView->setStyleSheet(
        "QTextEdit { background-color: #1e1e1e; color: #d4d4d4; }");
    groupLayout->addWidget(m_logView);

    layout->addWidget(group);

    connect(m_searchBtn, &QPushButton::clicked, this, &DetectionLog::onSearch);
    connect(m_clearBtn, &QPushButton::clicked, this, &DetectionLog::clearLog);
    connect(this, &DetectionLog::entryAdded, this, &DetectionLog::onEntryAdded);
}

DetectionLog::~DetectionLog() = default;

void DetectionLog::appendEntry(const QString& entry) {
    emit entryAdded(entry);  // Thread-safe: marshal to GUI thread
}

void DetectionLog::onEntryAdded(const QString& entry) {
    m_logView->append(entry);
    // Auto-scroll to bottom
    QScrollBar* sb = m_logView->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void DetectionLog::clearLog() {
    m_logView->clear();
}

void DetectionLog::onSearch() {
    QString term = m_searchBox->text();
    if (term.isEmpty()) return;

    // Highlight all matches
    QTextCursor cursor(m_logView->document());
    m_logView->moveCursor(QTextCursor::Start);

    QTextCharFormat highlightFmt;
    highlightFmt.setBackground(QColor(255, 255, 0, 100));

    // Reset formatting first
    cursor.select(QTextCursor::Document);
    QTextCharFormat defaultFmt;
    defaultFmt.setBackground(Qt::transparent);
    cursor.mergeCharFormat(defaultFmt);

    // Find and highlight
    while (m_logView->find(term)) {
        QTextCursor found = m_logView->textCursor();
        found.mergeCharFormat(highlightFmt);
    }

    // Move back to start
    m_logView->moveCursor(QTextCursor::Start);
    m_logView->find(term);  // Select first match
}
