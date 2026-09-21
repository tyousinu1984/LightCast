# LightCast 輕量直播推流軟體
## 系統設計書

**Document Version:** 0.1  
**Target Product Version:** v0.1 MVP  
**Target Platform:** Windows 10 / 11 x64  
**Primary Language:** Go  
**Native Layer:** C / C++  
**UI Framework:** Wails + TypeScript Frontend  
**Media Stack:** Windows Native APIs + FFmpeg  
**Graphics:** Direct3D 11  
**Streaming:** RTMP / RTMPS  

---

# 1. 項目概要

## 1.1 項目名稱

暫定：

**LightCast**

定位：

> 一款面向普通直播使用者的輕量直播畫面直播推流軟體。

LightCast 不以取代 OBS Studio 的全部能力為目標，而是聚焦於普通使用者最高頻使用的核心功能：

- 螢幕直播
- 指定視窗直播
- 攝影機直播
- 麥克風
- 系統聲音
- 簡單版面配置
- RTMP / RTMPS 推流
- 本地錄影
- 基礎鏡頭美顏優化

產品核心理念：

> 不需要學習 OBS，也能完成一場直播。

---

# 2. 設計目標

## 2.1 核心目標

LightCast 必須優先滿足以下要求：

1. 啟動速度快
2. 記憶體低佔用
3. 操作流程簡
4. UI 認知成本低
5. 1080p60 穩定推流
6. 優先使用 GPU
7. 避免不必要的 CPU/GPU 資料搬遷
8. 對直播平台保持通用
9. 核心架構可擴展
10. 不因未來功能預留第三方架構重寫

---

# 3. 非目標

LightCast v0.1 不涉及：

- OBS Plugin 相容
- OBS Scene Collection 相容
- Browser Source
- Studio Mode
- Replay Buffer
- Lua / Python Script
- 多場景場景編輯器
- WebSocket Remote Control
- 腳本攝影機
- Donation / Alert
- Twitch Chat
- YouTube Chat
- Marketplace
- AR 特效
- AI 美肌
- AI 背景生成
- 多場景嵌套
- 專業廣播級專業軟混音

原則：

> 只有功能直接服務於「Capture → Enhance → Broadcast」主流程時，才考慮加入核心產品。

---

# 4. 產品功能範圍

## 4.1 v0.1 免費功能

### Video

- Display Capture
- Window Capture
- Camera Capture
- Camera Picture-in-Picture
- Source Position
- Source Scale
- Source Crop
- Image Overlay
- Text Overlay

### Audio

- Microphone
- Desktop Audio
- Volume Control
- Mute
- Audio Level Meter

### Output

- RTMP
- RTMPS
- Local Recording
- Custom RTMP Server

### Encoding

- H.264
- NVIDIA NVENC
- Intel Quick Sync
- AMD AMF
- Software Fallback

### Preset

例如：

- 720p30
- 1080p30
- 1080p60

---

# 5. 未來 Pro 功能

LightCast Pro 第一階段主打複雜 AI 美顏。

定位為：

**Camera Enhance**

包含：

- Skin Smoothing
- Brightness
- Exposure
- Contrast
- Saturation
- Temperature
- Tint
- Sharpen
- Noise Reduction
- Preset

不涉及：

- 濾鏡
- 大眼
- 鼻樑調整
- 3D Face Mesh
- Makeup
- AR Face Effect

這可以最大程度保持 GPU pipeline 輕量。

---

# 6. 總體架構

LightCast 架構：

```text
┌─────────────────────────────────────────┐
│              Frontend UI                │
│         Wails + TypeScript UI           │
└───────────────────┬─────────────────────┘
                    │
                    │ Wails Binding
                    ▼
┌─────────────────────────────────────────┐
│            Application Layer            │
│                   Go                    │
│                                         │
│ Session                                 │
│ Settings                                │
│ Source Management                       │
│ Stream Controller                       │
│ Recording Controller                    │
│ Licensing                               │
│ Update                                  │
└───────────────────┬─────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────┐
│              Media Core                 │
│                   Go                    │
│                                         │
│ Pipeline Management                     │
│ Clock                                   │
│ Stream State                            │
│ Audio / Video Synchronisation           │
│ Encoder Selection                       │
│ Output Management                       │
└───────────────────┬─────────────────────┘
                    │
                 Native API
                    │
                    ▼
┌─────────────────────────────────────────┐
│         LightCast Native Layer          │
│                 C / C++                 │
│                                         │
│ Windows.Graphics.Capture                │
│ Media Foundation                        │
│ WASAPI                                  │
│ Direct3D 11                             │
│ NVENC / QSV / AMF                       │
│ FFmpeg                                  │
└─────────────────────────────────────────┘
```

---

# 7. 技術選型

## 7.1 Go

主要負責：

- Application State
- Stream State
- Config
- Preset
- Device Metadata
- Pipeline Coordination
- Error Handling
- Logging
- Licensing
- Update
- Platform Configuration
- UI Backend

原因：

Go 適合處理：

- 生命週期管理
- concurrent task
- network
- configuration
- service orchestration
- 跨模組控制

不讓 Go 直接處理高頻 raw video pixel。

---

# 8. UI Framework

採用：

```text
Wails
+
TypeScript
+
React / Svelte
```

其中更推薦：

```text
Wails
+
Svelte
+
TypeScript
```

原因是 LightCast UI 本身並不複雜，Svelte runtime 和組件模型都比較輕。

Wails 使用 Go 作為後端主體，前端方法可以直接調用 Go binding。Windows 上使用 WebView2 作為 UI rendering engine。

---

# 9. Native Layer

Native Layer 必須非常薄。

目標：

> Native 只做 Go 不適合或效能不足承擔的事。

包含：

```text
native/
├── capture/
├── graphics/
├── camera/
├── audio/
├── encoder/
├── filter/
```

---

# 10. Screen Capture

Windows 螢幕/視窗擷取使用：

```text
Windows.Graphics.Capture
```

該 API 可以直接擷取 display 或 application window，並產生 frame。

資料路徑：

```text
Display / Window
      │
Windows.Graphics.Capture
      │
Direct3D11CaptureFrame
      │
ID3D11Texture2D
```

目標：

擷取完成後直接得到 GPU Texture。

避免：

```text
GPU
→
CPU RAM
→
GPU
```

---

# 11. Camera Capture

Camera 使用：

```text
Microsoft Media Foundation
```

主要負責：

- Camera enumeration
- Format enumeration
- Resolution
- FPS
- Device open
- Device lost handling

Media Foundation 官方最支持攝影機硬體。格式設定和 frame rate 設定。

預期 pipeline：

```text
Camera
 │
Media Foundation
 │
Video Frame
 │
D3D11 Texture
```

---

# 12. Audio Capture

## 12.1 Microphone

使用：

```text
WASAPI Capture
```

## 12.2 Desktop Audio

使用：

```text
WASAPI Loopback
```

WASAPI Loopback 可以擷取系統渲染 endpoint 的混合音訊。

資料路徑：

```text
Microphone
      │
WASAPI
      │
      ├──────
             ▼
         Audio Mixer
             ▲
      ──────┤
      │
Desktop Audio
      │
WASAPI Loopback
```

未來可以增加：

```text
Application Audio Capture
```

Windows 已支持 process 擷取指定應用程式音訊。

例如：

```text
Only capture:

Chrome.exe
Game.exe
Zoom.exe
```

這可以作為 v1.x 功能。

---

# 13. Video Pipeline

核心 video pipeline：

```text
VideoSource
      │
SourceFilter
      │
Transform
      │
Compositor
      │
Canvas
      ├──────────── Preview
      │
      │
Encoder
      │
Muxer
      │
Output
```

---

# 14. VideoSource

統一接口：

```go
type VideoSource interface {
    ID() string
    Start() error
    Stop() error
    Status() SourceStatus
}
```

Native video frame 不能直接曝露成 Go byte array。

改使用：

```text
NativeHandle
```

例如：

```go
type NativeTextureHandle uintptr
```

Go 只保存 handle。

---

# 15. VideoFrame

邏輯模型：

```go
type VideoFrame struct {
    Texture NativeTextureHandle

    Width  int
    Height int

    Timestamp int64

    Format PixelFormat
}
```

注意：

`Texture` 只是 GPU resource handle。

不是：

```text
[]byte
```

否則每秒數 GB 的 memcpy 會直接破壞性能。

---

# 16. Filter Pipeline

過濾接口：

```go
type VideoFilter interface {
    Init() error
    Process(input NativeTextureHandle) NativeTextureHandle
    Close()
}
```

典型流程：

```text
Camera
   │
ColorFilter
   │
SkinSmoothFilter
   │
Transform
   │
Compositor
```

---

# 17. Camera Enhance

Camera Enhance 全在 GPU 做。

## Color Filter

一次 shader 完成：

- brightness
- exposure
- contrast
- saturation
- temperature
- tint

例如：

```text
Camera Texture
       │
ColorShader
       │
Processed Texture
```

---

# 18. Skin Smoothing

第一版不使用真正 AI。

可以採用：

```text
Edge Preserving Filter
+
Original Texture Blend
```

例如：

```text
Original
   │
Bilateral / Guided-like filter
   │
Smooth Texture
   │
Original + Smooth
   │
Final
```

可選：

```text
Skin Color Mask
```

避免：

- eyes
- hair
- background

全部一起被模糊。

---

# 19. Graphics Engine

使用：

```text
Direct3D 11
```

原因：

- Windows Graphics Capture 本身可直接與 D3D11 互通
- NVENC 支持 D3D11 texture
- Camera pipeline 可以轉成 D3D11 texture
- GPU shader 效果
- Windows 10 / 11 支持穩定

整個 video pipeline 維持保持：

```text
D3D11 Texture
```

---

# 20. Compositor

Compositor 負責：

```text
Canvas
├ Screen
├ Camera
├ Image
└ Text
```

例如：

```text
1920 × 1080 Canvas

Background
    Screen
      │
Camera PiP
      │
Logo
      │
Text
```

---

# 21. Transform

每個 source 保存：

```go
type Transform struct {
    X float32
    Y float32

    Width  float32
    Height float32

    Rotation float32

    CropLeft   float32
    CropRight  float32
    CropTop    float32
    CropBottom float32
}
```

座標使用 normalized coordinate：

```text
0.0 → 1.0
```

例如：

```text
X = 0.75
Y = 0.70
W = 0.22
H = 0.22
```

這樣和 canvas resolution 解耦。

---

# 22. Preview

Preview 是一個比較特殊的部分。

不建議：

```text
GPU Texture
→
CPU
→
WebView
→
HTML Video
```

正式方案：

```text
D3D11 Texture
      │
Native Preview Surface
```

Wails UI 在 preview 區域嵌入 native child window。

概念：

```text
┌─────────────────────────────────────┐
│ Wails                               │
│                                     │
│    ┌───────────────────────────┐    │
│    │ Native Preview HWND       │    │
│    │                           │    │
│    │ Direct3D11               │    │
│    └───────────────────────────┘    │
│                                     │
│ Sources       Audio       Live      │
└─────────────────────────────────────┘
```

v0.0 Prototype 階段可以先使用較簡易 preview。

---

# 23. Audio Pipeline

```text
Mic
 │
 ▼
Resampler
 │
 ├─────────────
               ▼
           Audio Mixer
               ▲
 ─────────────┤
 │
Desktop
 │
 ▼
Resampler
```

所有輸入最終統一為共同格式：

```text
48 kHz
Stereo
Float32
```

Mixer 後：

```text
Audio PCM
   │
AAC Encoder
```

---

# 24. AV Clock

必須建立：

```text
Master Clock
```

推薦：

```text
Audio Clock = Master
```

Video 根據 master clock 決定：

- render
- drop
- duplicate

例如：

```text
audio timestamp
      │
 master clock
      │
video timestamp comparison
```

---

# 25. Encoder

Encoder abstraction：

```go
type VideoEncoder interface {
    Open(config VideoEncoderConfig) error

    Encode(frame NativeTextureHandle) error

    Close() error
}
```

實現：

```text
NVENCEncoder
QSVEncoder
AMFEncoder
SoftwareEncoder
```

Encoder 選擇優先：

```text
NVIDIA
→
Intel
→
AMD
→
Software
```

但實際上根據 GPU detection。

---

# 26. Encoding Defaults

1080p60：

```text
Resolution:
1920×1080

FPS:
60

Codec:
H.264

Bitrate:
8000 kbps

Keyframe:
2 sec

Audio:
AAC
48 kHz
160 kbps
```

1080p30：

```text
6000 kbps
```

720p30：

```text
3000–4000 kbps
```

最終可根據平台 preset 調整。

---

# 27. Muxer

Streaming：

```text
H.264
+
AAC
→
FLV
→
RTMP / RTMPS
```

Recording：

第一版推薦：

```text
MKV
```

原因：

直播軟體如果直接寫 MP4，異常退出可能導致檔案無法正常 finalize。

可以錄製：

```text
MKV
```

直播完成後提供：

```text
Remux MKV → MP4
```

---

# 28. Output abstraction

```go
type Output interface {
    Open(config OutputConfig) error

    WriteVideo(packet EncodedPacket) error

    WriteAudio(packet EncodedPacket) error

    Close() error
}
```

實現：

```text
RTMPOutput
RecordingOutput
```

未來：

```text
SRTOutput
WHIPOutput
```

---

# 29. Streaming State Machine

直播必須有嚴格狀態機：

```text
Idle
 │
Preparing
 │
Connecting
 │
Streaming
 │
Stopping
 │
Idle
```

錯誤：

```text
Connecting
 │
Failed
```

直播期間：

```text
Streaming
 │
Reconnecting
 │
Streaming
```

---

# 30. Go State Model

```go
type StreamState int

const (
    StreamIdle StreamState = iota
    StreamPreparing
    StreamConnecting
    StreamLive
    StreamReconnecting
    StreamStopping
    StreamError
)
```

UI 只能根據 state 顯示。

不允許 UI 自己推測直播狀態。

---

# 31. Reconnect

推流網路錯誤：

```text
Disconnect
   │
Wait 1s
   │
Reconnect
   │
Wait 2s
   │
Reconnect
   │
Wait 5s
```

例如：

```text
1
2
5
10
10
10
```

最大 retry time 可配置。

---

# 32. Stream Health

核心指標：

```text
Network bitrate
Dropped frames
Render lag
Encoder lag
CPU usage
GPU usage
Audio status
Connection status
```

但 UI 不直接全部顯示。

普通模式：

```text
✔ 良好
⚠ 網路不穩
⚠ GPU 負載
⚠ Encoder 負載
```

詳細模式才顯示完整資訊。

---

# 33. User Interface

介面仿 OBS 的結構，但拿掉 Scene。

```text
┌─────────────────────────────────────────────────┐
│ LightCast                               Ready   │
├─────────────────────────────────────────────────┤
│                                                 │
│                  PREVIEW                        │
│                                                 │
│                                                 │
│                                                 │
├──────────────┬──────────────┬───────────────────┤
│ Sources      │ Audio         │ Broadcast        │
│              │               │                  │
│ □ Screen     │ Mic ─────     │ YouTube          │
│ □ Camera     │ PC  ────      │ 1080p60          │
│ □ Logo       │               │ 8000 kbps        │
│              │               │                  │
│ + Add        │               │ ● GO LIVE        │
└──────────────┴──────────────┴───────────────────┘
```

---

# 34. Source Panel

Source：

```text
Screen
Camera
Image
Text
```

操作：

```text
Enable
Disable
Move
Delete
```

不提供：

```text
nested source
scene collection
source group
```

第一版控制複雜度。

---

# 35. Camera UI

```text
Camera
──────────────

Device
[ Sony Camera ▼ ]

Resolution
[ 1920 × 1080 ▼ ]

FPS
[ 60 ▼ ]

────────────

Image

Brightness     ────────

Contrast       ────────

Saturation     ────────

Temperature    ────────

Smooth Skin    ──────── PRO
```

---

# 36. Settings

僅有：

```text
General
Video
Audio
Stream
Output
```

避免 OBS 那種過於深層設定。

---

# 37. Repository

建議：

```text
lightcast/
│
├── cmd/
│   └── lightcast/
│       └── main.go
│
├── internal/
│   ├── app/
│   │
│   ├── config/
│   │
│   ├── source/
│   │
│   ├── stream/
│   │
│   ├── recording/
│   │
│   ├── encoder/
│   │
│   ├── output/
│   │
│   ├── audio/
│   │
│   ├── video/
│   │
│   └── license/
│
├── native/
│   ├── include/
│   │
│   └── src/
│       ├── capture/
│       ├── camera/
│       ├── audio/
│       ├── graphics/
│       ├── encoder/
│       └── filters/
│
├── frontend/
│   ├── src/
│   └── package.json
│
├── shaders/
│   ├── color.hlsl
│   ├── smooth.hlsl
│   └── composite.hlsl
│
├── configs/
│
├── assets/
│
├── docs/
│   ├── architecture.md
│   ├── pipeline.md
│   └── native-api.md
│
├── CMakeLists.txt
├── go.mod
├── wails.json
└── README.md
```

---

# 38. Go / Native Boundary

Native API 一律採用 C ABI。

例如：

```c
typedef void* LCTexture;

LCResult lc_capture_create(
    const LCCaptureConfig* config,
    LCCaptureHandle* handle
);

LCResult lc_capture_start(
    LCCaptureHandle handle
);

LCResult lc_capture_stop(
    LCCaptureHandle handle
);
```

Go：

```go
type CaptureHandle uintptr
```

不將 C++ class 直接 expose 給 Go。

---

# 39. Native Error

統一：

```c
typedef enum {
    LC_OK = 0,

    LC_ERROR_UNKNOWN,
    LC_ERROR_DEVICE_NOT_FOUND,
    LC_ERROR_DEVICE_LOST,
    LC_ERROR_ENCODER_INIT,
    LC_ERROR_CAPTURE_INIT,
    LC_ERROR_GPU,
} LCResult;
```

Go 端：

```go
return errors.New(...)
```

---

# 40. Concurrency Model

Go 層：

```text
UI Thread
   │
App Controller
   │
   ├ Stream goroutine
   ├ Stats goroutine
   ├ Device monitor
   └ Logging
```

Native：

```text
Capture Thread
Render Thread
Audio Thread
Encoder Thread
Output Thread
```

不要讓 Go goroutine 處理每一個 frame。

---

# 41. Video Thread

核心：

```text
Capture
 │
Frame Queue
 │
Render
 │
Filter
 │
Composite
 │
Encoder
```

Render thread 建議獨立。

目標：

```text
16.67 ms
```

for 60 FPS。

---

# 42. Queue

Frame Queue：

```text
Capture Queue
```

只保存：

```text
2–3 frames
```

如果 queue full：

```text
drop oldest
```

直播需要：

> 最新鮮 frame

而不是：

> 所有 frame

不能累積 latency。

---

# 43. Audio Queue

Audio 不可以隨意 drop。

使用 ring buffer。

例如：

```text
100–200 ms
```

buffer。

---

# 44. Performance Target

1080p60：

```text
CPU
< 15%

GPU
< 30%

RAM
< 500 MB
```

實際數值依設備參照。

但設計目標：

```text
Idle RAM:
< 200 MB

Streaming RAM:
< 500 MB
```

---

# 45. Startup Target

目標：

```text
Cold Start
< 3 seconds
```

UI 提前初始化。

設備 enumeration 可以 asynchronous。

---

# 46. FFmpeg Strategy

FFmpeg 有兩種使用方式。

## Prototype

```text
Go
 │
FFmpeg subprocess
```

適合 v0.0。

## Product

```text
LightCast Native
 │
libavcodec
libavformat
libavutil
```

適合 v0.1+。

這樣才能精確控制：

- encoder
- packet
- timestamp
- mux
- reconnect

FFmpeg 會保留為 codec/mux 基礎，但 compositor 和 GPU graphics 不依賴 FFmpeg filter graph。

---

# 47. v0.0 Prototype

目標：

證明：

```text
LightCast UI
     │
Stream
     │
RTMP
```

功能：

```text
Screen
Camera
Mic
Desktop Audio
YouTube RTMP
```

可以採用：

```text
Go
+
Wails
+
FFmpeg subprocess
```

---

# 48. v0.1 MVP

正式 Media Pipeline：

```text
Windows.Graphics.Capture
+
Media Foundation
+
WASAPI
+
D3D11
+
Hardware Encoder
+
FFmpeg mux
```

功能：

```text
Screen
Window
Camera
Mic
Desktop
Image
Text

Preview

NVENC/QSV/AMF

RTMP
Recording
```

---

# 49. v0.2

增加：

```text
Camera Enhance

Brightness
Contrast
Exposure
Saturation
Temperature
Sharpen
Smooth Skin
```

同時加入：

```text
Pro license
```

---

# 50. v0.3

增加：

```text
Stream Health

Automatic bitrate suggestion
Network diagnostics
Reconnect
Better device recovery
```

---

# 51. v1.0

產品成熟版本：

```text
Multi-platform presets

YouTube
Twitch
TikTok compatible RTMP
Custom RTMP

Application Audio Capture

Better Camera Enhance

Update system

Crash Recovery

Preset management
```

---

# 52. License Architecture

不要把 Pro 判斷散落在 UI。

建立：

```go
type Feature string
```

例如：

```text
camera.smooth
camera.denoise
camera.preset.pro
```

License service：

```go
HasFeature(feature Feature) bool
```

UI：

```text
if feature unavailable:

show PRO
```

Native layer 不負責 business license。

---

# 53. Config

推薦：

```text
JSON
```

例如：

```json
{
  "video": {
    "width": 1920,
    "height": 1080,
    "fps": 60
  },

  "encoder": {
    "type": "nvenc",
    "bitrate": 8000
  },

  "audio": {
    "sampleRate": 48000
  }
}
```

---

# 54. Scene Model

雖然 UI 不顯示 Scene，但內部可以保留：

```go
type Composition struct {
    Sources []Source
}
```

以便未來增加：

```text
Layout Preset
```

例如：

```text
Screen
Camera

Camera Only

Screen + Camera
```

但不要第一版曝露複雜 Scene 管理。

---

# 55. Device Recovery

必須處理：

```text
Camera unplug
Microphone unplug
Display removed
GPU device lost
Audio endpoint changed
```

Source State：

```text
Active
Unavailable
Reconnecting
Error
```

---

# 56. Logging

推薦：

```text
logs/
lightcast.log
```

格式：

```text
timestamp
level
module
message
```

例如：

```text
2026-09-21 18:20:15
INFO
encoder
NVENC initialized
```

---

# 57. Crash Handling

Crash dump：

```text
LightCast.dmp
```

崩潰重啟後：

```text
Previous session did not close correctly.
```

如果 recording 是 MKV，

大部分情況仍然可以保留。

---

# 58. Security

Stream Key：

不能明文出現在 log。

例如：

禁止：

```text
rtmp://server/live/abcdef
```

log：

```text
rtmp://server/live/******
```

License Token 同理。

---

# 59. Testing

至少分：

```text
unit
integration
media
performance
```

Unit：

```text
Config
State machine
Preset
Transform
```

Integration：

```text
Capture → Encoder
Audio → AAC
Mux → File
```

---

# 60. Performance Test

固定 benchmark：

```text
1080p30
1080p60
720p60
```

測量：

```text
CPU
GPU
RAM
Dropped Frames
Encoder Lag
Render Lag
```

---

# 61. MVP Acceptance Criteria

LightCast v0.1 可以發布的最低標準：

### Capture

```text
Screen capture works
Window capture works
Camera works
```

### Audio

```text
Microphone works
Desktop audio works
```

### Render

```text
Screen + Camera works
```

### Encode

```text
NVENC
QSV
AMF
```

至少其中一種可用機型正常使用。

### Streaming

```text
1080p60
RTMPS
30 minutes
No crash
```

### Recording

```text
30 minutes recording
Playable
AV sync acceptable
```

---

# 62. 開發優先順序

不要先做完整 UI。

推薦順序：

```text
Phase 1

Screen Capture
      │
D3D11 Texture
      │
Preview
```

完成後：

```text
Phase 2

Texture
 │
H264
 │
File
```

完成後：

```text
Phase 3

H264 + AAC
 │
RTMP
```

之後：

```text
Phase 4

Camera
```

然後：

```text
Phase 5

Compositor
```

最後：

```text
Phase 6

Wails UI
```

---

# 63. 第一個 Prototype

真正第一個技術目標應該只是：

> 擷取畫面 → 顯示 Preview。

也就是：

```text
Windows.Graphics.Capture
        │
ID3D11Texture2D
        │
D3D11 Preview
```

完全先不碰：

```text
RTMP
Audio
Camera
FFmpeg
UI
```

如果這條路跑通，整個 graphics architecture 就基本成立。

---

# 64. 第二個 Prototype

```text
Screen
 │
D3D11
 │
NVENC
 │
H264
```

輸出：

```text
test.h264
```

確認：

- FPS
- GPU
- latency
- quality

---

# 65. 第三個 Prototype

```text
Mic
+
Desktop Audio
+
Video
 │
Mux
 │
RTMP
```

達成 LightCast 的核心閉環完成。

---

# 66. 架構決策總結

LightCast 必須長期遵守以下原則：

### 1

UI 和 Media Engine 完全分離。

### 2

Go 不直接處理 raw video pixel。

### 3

Video frame 傳遞一直存在 GPU。

### 4

Native layer 必須薄。

### 5

避免 CPU/GPU round trip。

### 6

不要選擇建立 plugin ecosystem。

### 7

不要選擇增加 AI。

### 8

不要複製 OBS 的功能複雜度。

### 9

Pipeline 優先於 UI。

### 10

每個新功能必須先問：

> 它是否明顯改善 Capture、Enhance 或 Broadcast？

如果不是：

暫緩不加入核心。

---

# 67. 最終架構

LightCast 長期形成：

```text
                  LightCast

                Wails UI
                    │
                    ▼
              Go Application
                    │
                    ▼
                Media Core
                    │
        ┌───────────┼───────────┐
        ▼           ▼           ▼
      Capture     Audio      Control
        │           │
        └─────┬─────┘
              ▼
          Native Layer
              │
              ▼
           D3D11 GPU
              │
      ┌───────┼───────┐
      ▼       ▼        ▼
   Filter  Composite  Preview
              │
              ▼
           Encoder
              │
          ┌───┴─────┐
          ▼        ▼
       Stream    Record
```

產品最終仍然保持極度簡單：

```text
Capture
   │
Enhance
   │
Broadcast
```

而不是逐步演變成第二個 OBS。

---

# 68. 技術結論

第一階段推薦技術棧：

```text
Application
Go

UI
Wails
Svelte
TypeScript

Native
C / C++

Graphics
Direct3D 11

Screen Capture
Windows.Graphics.Capture

Camera
Media Foundation

Audio
WASAPI

Codec
FFmpeg

Hardware Encoder
NVENC
QSV
AMF

Stream
RTMP / RTMPS

Build
Go
CMake
Wails
```

其中最重要的工程分界是：

> Go 管理系統，Native 管理高頻，GPU 管理界面。

這樣可以最大程度兼顧：

- 開發效率
- 可維護性
- 效能
- 輕量化
- 未來擴展能力

並且避免整個項目長陷大型 C++ codebase 的維護負擔。
