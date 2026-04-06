#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>

/// Handles exporting video clips from recorded data for law enforcement use.
class ClipExporter : public QObject {
    Q_OBJECT

public:
    explicit ClipExporter(QObject* parent = nullptr);
    ~ClipExporter() override;

    /// Request a clip export from the cluster
    void exportClip(const QString& sourceFile,
                    const QDateTime& startTime,
                    const QDateTime& endTime,
                    const QString& outputPath);

signals:
    void exportStarted();
    void exportProgress(int percent);
    void exportComplete(const QString& outputPath);
    void exportError(const QString& error);
};
