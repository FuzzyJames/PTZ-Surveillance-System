#pragma once

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

/// Video playback controls: play, pause, seek, speed, and clip export.
class PlaybackController : public QWidget {
    Q_OBJECT

public:
    explicit PlaybackController(QWidget* parent = nullptr);
    ~PlaybackController() override;

signals:
    void playRequested();
    void pauseRequested();
    void seekRequested(int positionMs);
    void saveClipRequested();

private slots:
    void onPlayPause();
    void onSeek(int value);
    void onSaveClip();

private:
    QPushButton* m_playPauseBtn = nullptr;
    QPushButton* m_skipBackBtn = nullptr;
    QPushButton* m_skipFwdBtn = nullptr;
    QPushButton* m_saveClipBtn = nullptr;
    QSlider*     m_seekBar = nullptr;
    QLabel*      m_timeLabel = nullptr;
    bool         m_isPlaying = false;
};
