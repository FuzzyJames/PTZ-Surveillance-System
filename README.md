# PTZ Camera Surveillance System

A distributed C++ surveillance system deployed across a K3s Kubernetes cluster with AI-powered object detection, real-time video streaming, and a Qt6 Windows GUI.

## Architecture

- **4x Raspberry Pi 4** nodes running K3s (managed by Rancher)
- **1x Jetson Orin Nano** for GPU-accelerated AI inference
- **1x WD Purple 2TB** drive for video/log storage (NFS shared)
- **Windows GUI** (Qt6) for live monitoring, PTZ control, and playback

### Cluster Services

| Service | Node | Port | Purpose |
|---------|------|------|---------|
| Camera Ingest | Pi1 (cluster1) | 50051 | RTSP pull, H.264 distribution via RTP multicast |
| AI Detection | Nano | 50052 | YOLOv8 + TensorRT vehicle/person detection, OCR |
| PTZ Control | Pi3 (cluster3) | 50053 | ONVIF PTZ commands, patrol, AI tracking |
| Storage | Pi2 (cluster2) | 50054 | MPEG recording, folder hierarchy, drive management |
| Stream Gateway | Pi4 (cluster4) | 50055 | WebRTC/RTP relay to GUI, playback server |
| Log Service | Pi3 (cluster3) | 50056 | Rolling 1GB detection log, real-time streaming |

### Key Features

- **< 100ms latency** camera-to-GUI live streaming
- **WASD/QE** manual PTZ control with AI auto-patrol and person tracking
- **AI Detection**: vehicles (type classification + license plate OCR), people (description + tracking)
- **Dual recording**: 10fps continuous + 60fps on person detection (with 30s pre-buffer)
- **Calendar-based playback** with clip export for law enforcement
- **Automatic storage management**: date-based folder hierarchy, oldest-day rotation when full

## Project Structure

```
CMakeLists.txt              # Top-level build
vcpkg.json                  # Windows dependency manifest
cmake/                      # Cross-compilation toolchains
proto/                      # gRPC/Protobuf service definitions
common/                     # Shared config and logging library
services/
  ingest/                   # Camera Ingest Service
  ai_detect/                # AI Detection Service (Jetson)
  ptz_control/              # PTZ Control Service
  storage/                  # Storage Service
  stream_gateway/           # Streaming Gateway
  log_service/              # Log Service
gui/                        # Windows Qt6 GUI application
k8s/                        # Kubernetes deployment manifests
```

## Build Requirements

### Windows GUI
- Visual Studio 2022
- CMake 3.20+
- vcpkg (Qt6, gRPC, FFmpeg, spdlog, nlohmann-json)

### Cluster Services
- Docker (ARM64 images built via multi-stage Dockerfiles)
- K3s cluster with Rancher

## Quick Start

### Windows GUI (from VS2022 Developer Command Prompt)
```powershell
cmake -S . -B build -DBUILD_GUI=ON -DBUILD_SERVICES=OFF
cmake --build build --config Release
```

### Deploy to Cluster
```bash
kubectl apply -f k8s/namespace.yaml
kubectl apply -f k8s/configmap.yaml
kubectl apply -f k8s/secrets.yaml
kubectl apply -f k8s/pv-wdpurple.yaml
kubectl apply -f k8s/deployments.yaml
kubectl apply -f k8s/services.yaml
```

## Documentation

Architecture documentation is in `../Architecture documentation/`:
- System Architecture (hardware, protocols, latency budget)
- Class Diagrams (GUI + cluster services)
- Sequence Diagrams (6 key workflows)
- GUI Storyboard (wireframes)
- Data Flow & Storage Specification

## License

Private project.
