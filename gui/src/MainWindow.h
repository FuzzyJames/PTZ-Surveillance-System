#pragma once

#include <QMainWindow>
#include <QSplitter>
#include <QStatusBar>
#include <QLabel>
#include <QMenuBar>

class VideoWidget;
class PtzController;
class CalendarPanel;
class DetectionLog;
class PlaybackController;
class ClipExporter;

/// Main application window containing all GUI panels.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void onToggleAiMode();
    void onConnectToCluster();
    void onUpdateLatency(double latencyMs);

private:
    void createMenuBar();
    void createStatusBar();
    void createCentralLayout();

    // Main panels
    VideoWidget*        m_videoWidget = nullptr;
    CalendarPanel*      m_calendarPanel = nullptr;
    DetectionLog*       m_detectionLog = nullptr;
    PlaybackController* m_playbackController = nullptr;

    // Controllers
    PtzController* m_ptzController = nullptr;
    ClipExporter*  m_clipExporter = nullptr;

    // Status bar widgets
    QLabel* m_latencyLabel = nullptr;
    QLabel* m_fpsLabel = nullptr;
    QLabel* m_aiStatusLabel = nullptr;

    bool m_aiModeEnabled = false;
};
