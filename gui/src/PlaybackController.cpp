#include "PlaybackController.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

PlaybackController::PlaybackController(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* group = new QGroupBox(tr("Playback Controls"));
    auto* groupLayout = new QVBoxLayout(group);

    // Seek bar
    m_seekBar = new QSlider(Qt::Horizontal);
    m_seekBar->setRange(0, 100);
    groupLayout->addWidget(m_seekBar);

    // Time label
    m_timeLabel = new QLabel("00:00:00 / 00:00:00");
    m_timeLabel->setAlignment(Qt::AlignCenter);
    groupLayout->addWidget(m_timeLabel);

    // Buttons
    auto* btnLayout = new QHBoxLayout();
    m_skipBackBtn = new QPushButton("|<");
    m_playPauseBtn = new QPushButton("Play");
    m_skipFwdBtn = new QPushButton(">|");
    m_saveClipBtn = new QPushButton("Save Clip");

    m_skipBackBtn->setFixedWidth(40);
    m_skipFwdBtn->setFixedWidth(40);

    btnLayout->addWidget(m_skipBackBtn);
    btnLayout->addWidget(m_playPauseBtn);
    btnLayout->addWidget(m_skipFwdBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(m_saveClipBtn);
    groupLayout->addLayout(btnLayout);

    layout->addWidget(group);

    connect(m_playPauseBtn, &QPushButton::clicked, this, &PlaybackController::onPlayPause);
    connect(m_seekBar, &QSlider::sliderMoved, this, &PlaybackController::onSeek);
    connect(m_saveClipBtn, &QPushButton::clicked, this, &PlaybackController::onSaveClip);
}

PlaybackController::~PlaybackController() = default;

void PlaybackController::onPlayPause() {
    m_isPlaying = !m_isPlaying;
    m_playPauseBtn->setText(m_isPlaying ? "Pause" : "Play");

    if (m_isPlaying) {
        emit playRequested();
    } else {
        emit pauseRequested();
    }
}

void PlaybackController::onSeek(int value) {
    emit seekRequested(value);
}

void PlaybackController::onSaveClip() {
    emit saveClipRequested();
}
