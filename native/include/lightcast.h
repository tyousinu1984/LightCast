#ifndef LIGHTCAST_NATIVE_H
#define LIGHTCAST_NATIVE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LC_OK = 0,

    LC_ERROR_UNKNOWN,
    LC_ERROR_DEVICE_NOT_FOUND,
    LC_ERROR_DEVICE_LOST,
    LC_ERROR_ENCODER_INIT,
    LC_ERROR_CAPTURE_INIT,
    LC_ERROR_GPU,
} LCResult;

typedef void* LCCaptureHandle;

typedef struct {
    // Target display/window, pixel format, etc. — fill in during the
    // Windows.Graphics.Capture spike (design doc section 10 / 63).
    int placeholder;
} LCCaptureConfig;

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

#ifdef __cplusplus
}
#endif

#endif // LIGHTCAST_NATIVE_H
