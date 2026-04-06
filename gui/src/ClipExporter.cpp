#include "ClipExporter.h"
#include "surveillance/logging.h"

ClipExporter::ClipExporter(QObject* parent)
    : QObject(parent)
{
}

ClipExporter::~ClipExporter() = default;

void ClipExporter::exportClip(const QString& sourceFile,
                              const QDateTime& startTime,
                              const QDateTime& endTime,
                              const QString& outputPath) {
    LOG_INFO("Exporting clip: %s [%s - %s] -> %s",
             sourceFile.toStdString().c_str(),
             startTime.toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str(),
             endTime.toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str(),
             outputPath.toStdString().c_str());

    emit exportStarted();

    // TODO: Phase 6 implementation
    // 1. Send gRPC ClipExportRequest to StorageService
    // 2. Receive VideoChunk stream
    // 3. Write chunks to outputPath file
    // 4. Emit progress updates
    // 5. Emit exportComplete or exportError

    emit exportError("Clip export not yet implemented");
}
