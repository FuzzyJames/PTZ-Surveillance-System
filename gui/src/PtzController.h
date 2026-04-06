#pragma once

#include <QObject>

/// Manages PTZ camera control from the GUI.
/// Translates keyboard input (WASD/QE) into gRPC commands
/// sent to the PTZ Control Service on the cluster.
class PtzController : public QObject {
    Q_OBJECT

public:
    explicit PtzController(QObject* parent = nullptr);
    ~PtzController() override;

    /// Start continuous movement
    /// @param pan  -1.0 (left) to 1.0 (right)
    /// @param tilt -1.0 (down) to 1.0 (up)
    /// @param zoom -1.0 (out) to 1.0 (in)
    void startMove(float pan, float tilt, float zoom);

    /// Stop all movement
    void stopMove();

    /// Connect to the PTZ service on the cluster
    void connectToService(const QString& address);

    /// Check if connected
    bool isConnected() const;

signals:
    void connectionChanged(bool connected);

private:
    bool m_connected = false;
    // TODO: gRPC channel and stub
    // std::unique_ptr<surveillance::PtzControlService::Stub> m_stub;
};
