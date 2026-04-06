#include "MainWindow.h"
#include "VideoWidget.h"
#include "PtzController.h"
#include "CalendarPanel.h"
#include "DetectionLog.h"
#include "PlaybackController.h"
#include "ClipExporter.h"
#include "surveillance/logging.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QKeyEvent>
#include <QAction>
#include <QMenu>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    createMenuBar();
    createCentralLayout();
    createStatusBar();

    // Initialize controllers
    m_ptzController = new PtzController(this);
    m_clipExporter = new ClipExporter(this);

    LOG_INFO("MainWindow initialized.");
}

MainWindow::~MainWindow() = default;

void MainWindow::createMenuBar() {
    auto* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Connect to Cluster"), this, &MainWindow::onConnectToCluster);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), this, &QMainWindow::close, QKeySequence::Quit);

    auto* cameraMenu = menuBar()->addMenu(tr("&Camera"));
    auto* aiAction = cameraMenu->addAction(tr("Toggle &AI Mode"));
    aiAction->setCheckable(true);
    connect(aiAction, &QAction::triggered, this, &MainWindow::onToggleAiMode);

    auto* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("&Live View"));
    viewMenu->addAction(tr("&Playback Mode"));

    menuBar()->addMenu(tr("&Help"));
}

void MainWindow::createStatusBar() {
    m_latencyLabel = new QLabel(tr("Latency: -- ms"));
    m_fpsLabel = new QLabel(tr("FPS: --"));
    m_aiStatusLabel = new QLabel(tr("AI: OFF"));

    statusBar()->addPermanentWidget(m_latencyLabel);
    statusBar()->addPermanentWidget(m_fpsLabel);
    statusBar()->addPermanentWidget(m_aiStatusLabel);
    statusBar()->showMessage(tr("Disconnected from cluster"));
}

void MainWindow::createCentralLayout() {
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(centralWidget);

    // Top section: Video + Calendar side by side
    auto* topSplitter = new QSplitter(Qt::Horizontal);

    m_videoWidget = new VideoWidget(this);
    topSplitter->addWidget(m_videoWidget);

    // Right panel: Calendar + Playback controls
    auto* rightPanel = new QWidget();
    auto* rightLayout = new QVBoxLayout(rightPanel);
    m_calendarPanel = new CalendarPanel(this);
    m_playbackController = new PlaybackController(this);
    rightLayout->addWidget(m_calendarPanel);
    rightLayout->addWidget(m_playbackController);
    topSplitter->addWidget(rightPanel);

    topSplitter->setStretchFactor(0, 3);  // Video gets 3/4
    topSplitter->setStretchFactor(1, 1);  // Calendar gets 1/4

    // Bottom section: Detection log
    auto* mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->addWidget(topSplitter);

    m_detectionLog = new DetectionLog(this);
    mainSplitter->addWidget(m_detectionLog);

    mainSplitter->setStretchFactor(0, 3);  // Top gets 3/4
    mainSplitter->setStretchFactor(1, 1);  // Log gets 1/4

    mainLayout->addWidget(mainSplitter);
    setCentralWidget(centralWidget);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat() && m_ptzController) {
        switch (event->key()) {
            case Qt::Key_W: m_ptzController->startMove(0.0f, 0.5f, 0.0f); return;
            case Qt::Key_S: m_ptzController->startMove(0.0f, -0.5f, 0.0f); return;
            case Qt::Key_A: m_ptzController->startMove(-0.5f, 0.0f, 0.0f); return;
            case Qt::Key_D: m_ptzController->startMove(0.5f, 0.0f, 0.0f); return;
            case Qt::Key_Q: m_ptzController->startMove(0.0f, 0.0f, -0.3f); return;
            case Qt::Key_E: m_ptzController->startMove(0.0f, 0.0f, 0.3f); return;
            default: break;
        }
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat() && m_ptzController) {
        switch (event->key()) {
            case Qt::Key_W: case Qt::Key_S:
            case Qt::Key_A: case Qt::Key_D:
            case Qt::Key_Q: case Qt::Key_E:
                m_ptzController->stopMove();
                return;
            default: break;
        }
    }
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::onToggleAiMode() {
    m_aiModeEnabled = !m_aiModeEnabled;
    m_aiStatusLabel->setText(m_aiModeEnabled ? tr("AI: ON") : tr("AI: OFF"));
    LOG_INFO("AI mode %s", m_aiModeEnabled ? "enabled" : "disabled");

    // TODO: Send gRPC command to AI Detection Service
}

void MainWindow::onConnectToCluster() {
    LOG_INFO("Connecting to cluster...");
    statusBar()->showMessage(tr("Connecting to cluster..."));

    // TODO: Establish gRPC connections to all cluster services
    // TODO: Start receiving live video stream via WebRTC/RTP
}

void MainWindow::onUpdateLatency(double latencyMs) {
    m_latencyLabel->setText(QString("Latency: %1 ms").arg(latencyMs, 0, 'f', 1));
}
