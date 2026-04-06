#include "VideoWidget.h"
#include <QPainter>
#include <QPen>
#include <QFont>

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(640, 480);
    setStyleSheet("background-color: black;");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(this, &VideoWidget::frameReceived, this, [this]() {
        update();  // Schedule repaint on GUI thread
    });
}

VideoWidget::~VideoWidget() = default;

void VideoWidget::updateFrame(const QImage& frame) {
    QMutexLocker lock(&m_mutex);
    m_currentFrame = frame;
    emit frameReceived();
}

void VideoWidget::setBoundingBoxes(const std::vector<BoundingBox>& boxes) {
    QMutexLocker lock(&m_mutex);
    m_boxes = boxes;
}

void VideoWidget::clear() {
    QMutexLocker lock(&m_mutex);
    m_currentFrame = QImage();
    m_boxes.clear();
    update();
}

void VideoWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QMutexLocker lock(&m_mutex);

    if (m_currentFrame.isNull()) {
        // Draw placeholder
        painter.fillRect(rect(), Qt::black);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 16));
        painter.drawText(rect(), Qt::AlignCenter, "No Video Signal");
        return;
    }

    // Scale frame to fit widget while maintaining aspect ratio
    QImage scaled = m_currentFrame.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    int xOffset = (width() - scaled.width()) / 2;
    int yOffset = (height() - scaled.height()) / 2;
    painter.drawImage(xOffset, yOffset, scaled);

    // Draw bounding boxes
    for (const auto& box : m_boxes) {
        int bx = xOffset + static_cast<int>(box.x * scaled.width());
        int by = yOffset + static_cast<int>(box.y * scaled.height());
        int bw = static_cast<int>(box.width * scaled.width());
        int bh = static_cast<int>(box.height * scaled.height());

        QPen pen(box.isPerson ? Qt::red : Qt::green, 2);
        painter.setPen(pen);
        painter.drawRect(bx, by, bw, bh);

        // Draw label
        if (!box.label.isEmpty()) {
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            QRect labelRect(bx, by - 18, bw, 18);
            painter.fillRect(labelRect, QColor(0, 0, 0, 150));
            painter.setPen(Qt::white);
            painter.drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, " " + box.label);
        }
    }
}
