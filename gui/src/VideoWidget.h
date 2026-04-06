#pragma once

#include <QWidget>
#include <QImage>
#include <QMutex>
#include <vector>

/// Displays live camera feed and recorded video playback.
/// Supports bounding box overlay for AI detections.
class VideoWidget : public QWidget {
    Q_OBJECT

public:
    explicit VideoWidget(QWidget* parent = nullptr);
    ~VideoWidget() override;

    /// Update displayed frame (thread-safe, called from network thread)
    void updateFrame(const QImage& frame);

    /// Set bounding boxes to overlay on the current frame
    struct BoundingBox {
        float x, y, width, height;  // normalized 0-1
        QString label;
        bool isPerson;              // true = red box, false = green box
    };
    void setBoundingBoxes(const std::vector<BoundingBox>& boxes);

    /// Clear the display
    void clear();

signals:
    void frameReceived();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QImage m_currentFrame;
    std::vector<BoundingBox> m_boxes;
    QMutex m_mutex;
};
