#ifndef SORA_SORA_VIDEO_DECODER_FACTORY_H_
#define SORA_SORA_VIDEO_DECODER_FACTORY_H_

#include <memory>
#include <vector>

// WebRTC
#include <api/video/video_codec_type.h>
#include <api/video_codecs/video_decoder_factory.h>

#include "sora/cuda_context.h"

namespace sora {

struct VideoDecoderConfig {
  VideoDecoderConfig() = default;
  VideoDecoderConfig(webrtc::VideoCodecType codec,
                     std::function<std::unique_ptr<webrtc::VideoDecoder>(
                         const webrtc::SdpVideoFormat&)> create_video_decoder)
      : codec(codec), create_video_decoder(std::move(create_video_decoder)) {}
  VideoDecoderConfig(std::function<std::vector<webrtc::SdpVideoFormat>()>
                         get_supported_formats,
                     std::function<std::unique_ptr<webrtc::VideoDecoder>(
                         const webrtc::SdpVideoFormat&)> create_video_decoder)
      : get_supported_formats(std::move(get_supported_formats)),
        create_video_decoder(std::move(create_video_decoder)) {}
  VideoDecoderConfig(std::unique_ptr<webrtc::VideoDecoderFactory> factory)
      : factory(std::move(factory)) {}

  webrtc::VideoCodecType codec = webrtc::VideoCodecType::kVideoCodecGeneric;
  std::function<std::vector<webrtc::SdpVideoFormat>()> get_supported_formats;
  std::function<std::unique_ptr<webrtc::VideoDecoder>(
      const webrtc::SdpVideoFormat&)>
      create_video_decoder;
  std::shared_ptr<webrtc::VideoDecoderFactory> factory;
};

struct SoraVideoDecoderFactoryConfig {
  std::vector<VideoDecoderConfig> decoders;
};

class SoraVideoDecoderFactory : public webrtc::VideoDecoderFactory {
 public:
  SoraVideoDecoderFactory(SoraVideoDecoderFactoryConfig config);
  virtual ~SoraVideoDecoderFactory() {}

  std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override;

  std::unique_ptr<webrtc::VideoDecoder> CreateVideoDecoder(
      const webrtc::SdpVideoFormat& format) override;

 private:
  SoraVideoDecoderFactoryConfig config_;
  mutable std::vector<std::vector<webrtc::SdpVideoFormat>> formats_;
};

// ハードウェアデコーダを出来るだけ使おうとして、見つからなければソフトウェアデコーダを使う設定を返す
SoraVideoDecoderFactoryConfig GetDefaultVideoDecoderFactoryConfig(
    std::shared_ptr<CudaContext> cuda_context);
// ソフトウェアデコーダのみを使う設定を返す
SoraVideoDecoderFactoryConfig GetSoftwareOnlyVideoDecoderFactoryConfig();

}  // namespace sora

#endif