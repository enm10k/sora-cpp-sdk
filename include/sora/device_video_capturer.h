/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef SORA_DEVICE_VIDEO_CAPTURER_H_
#define SORA_DEVICE_VIDEO_CAPTURER_H_

#include <memory>
#include <vector>

// WebRTC
#include <api/scoped_refptr.h>
#include <modules/video_capture/video_capture.h>
#include <rtc_base/ref_counted_object.h>
#if defined(SORA_CPP_SDK_HOLOLENS2)
#include <modules/video_capture/winuwp/mrc_video_effect_definition.h>
#endif

#include "scalable_track_source.h"

namespace sora {

// webrtc::VideoCaptureModule を使ったデバイスキャプチャラ。
// このキャプチャラでは動かない環境もあるため、このキャプチャラを直接利用する必要は無い。
// 様々な環境で動作するデバイスキャプチャラを利用したい場合、
// CreateCameraDeviceCapturer 関数を利用して生成するのが良い。
class DeviceVideoCapturer : public ScalableVideoTrackSource,
                            public rtc::VideoSinkInterface<webrtc::VideoFrame> {
 public:
  static rtc::scoped_refptr<DeviceVideoCapturer> Create(
      size_t width,
      size_t height,
      size_t target_fps
#if defined(SORA_CPP_SDK_HOLOLENS2)
      ,
      std::shared_ptr<webrtc::MrcVideoEffectDefinition> mrc
#endif
  );
  static rtc::scoped_refptr<DeviceVideoCapturer> Create(
      size_t width,
      size_t height,
      size_t target_fps,
      size_t capture_device_index
#if defined(SORA_CPP_SDK_HOLOLENS2)
      ,
      std::shared_ptr<webrtc::MrcVideoEffectDefinition> mrc
#endif
  );
  static rtc::scoped_refptr<DeviceVideoCapturer> Create(
      size_t width,
      size_t height,
      size_t target_fps,
      const std::string& capture_device
#if defined(SORA_CPP_SDK_HOLOLENS2)
      ,
      std::shared_ptr<webrtc::MrcVideoEffectDefinition> mrc
#endif
  );
  DeviceVideoCapturer();
  virtual ~DeviceVideoCapturer();

 private:
  bool Init(size_t width,
            size_t height,
            size_t target_fps,
            size_t capture_device_index
#if defined(SORA_CPP_SDK_HOLOLENS2)
            ,
            std::shared_ptr<webrtc::MrcVideoEffectDefinition> mrc
#endif
  );
  void Destroy();

  // rtc::VideoSinkInterface interface.
  void OnFrame(const webrtc::VideoFrame& frame) override;

  int LogDeviceInfo();
  int GetDeviceIndex(const std::string& device);

  rtc::scoped_refptr<webrtc::VideoCaptureModule> vcm_;
  webrtc::VideoCaptureCapability capability_;
};

}  // namespace sora

#endif
