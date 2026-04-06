#include "PtzController.h"
#include "surveillance/logging.h"

PtzController::PtzController(QObject* parent)
    : QObject(parent)
{
}

PtzController::~PtzController() = default;

void PtzController::startMove(float pan, float tilt, float zoom) {
    LOG_DEBUG("PTZ move: pan=%.2f tilt=%.2f zoom=%.2f", pan, tilt, zoom);

    if (!m_connected) {
        LOG_WARN("PTZ not connected, ignoring move command");
        return;
    }

    // TODO: Send gRPC PtzMoveRequest to PtzControlService
    // surveillance::PtzMoveRequest request;
    // request.set_pan_speed(pan);
    // request.set_tilt_speed(tilt);
    // request.set_zoom_speed(zoom);
    // grpc::ClientContext context;
    // google::protobuf::Empty response;
    // m_stub->Move(&context, request, &response);
}

void PtzController::stopMove() {
    LOG_DEBUG("PTZ stop");

    if (!m_connected) return;

    // TODO: Send gRPC Stop to PtzControlService
}

void PtzController::connectToService(const QString& address) {
    LOG_INFO("Connecting to PTZ service at %s", address.toStdString().c_str());

    // TODO: Create gRPC channel and stub
    // auto channel = grpc::CreateChannel(address.toStdString(), grpc::InsecureChannelCredentials());
    // m_stub = surveillance::PtzControlService::NewStub(channel);

    m_connected = true;  // TODO: verify connection
    emit connectionChanged(m_connected);
}

bool PtzController::isConnected() const {
    return m_connected;
}
