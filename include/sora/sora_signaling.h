#ifndef SORA_SORA_SIGNALING_H_INCLUDED
#define SORA_SORA_SIGNALING_H_INCLUDED

#include <functional>
#include <memory>
#include <string>
#include <vector>

// Boost
#include <boost/json.hpp>
#include <boost/optional.hpp>

// WebRTC
#include <api/media_stream_interface.h>
#include <api/peer_connection_interface.h>
#include <api/scoped_refptr.h>

#include "data_channel.h"
#include "websocket.h"

namespace sora {

enum class SoraSignalingErrorCode {
  CLOSE_SUCCEEDED,
  CLOSE_FAILED,
  INTERNAL_ERROR,
  INVALID_PARAMETER,
  WEBSOCKET_HANDSHAKE_FAILED,
  WEBSOCKET_ONCLOSE,
  WEBSOCKET_ONERROR,
  PEER_CONNECTION_STATE_FAILED,
  ICE_FAILED,
};

class SoraSignalingObserver {
 public:
  virtual void OnSetOffer() = 0;
  virtual void OnDisconnect(SoraSignalingErrorCode ec, std::string message) = 0;
  virtual void OnNotify(std::string text) = 0;
  virtual void OnPush(std::string text) = 0;
  virtual void OnMessage(std::string label, std::string data) = 0;
};

struct SoraSignalingConfig {
  boost::asio::io_context* io_context;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory;
  std::weak_ptr<SoraSignalingObserver> observer;

  std::vector<std::string> signaling_urls;
  std::string channel_id;

  std::string sora_client;

  bool insecure = false;
  bool video = true;
  bool audio = true;
  std::string video_codec_type = "";
  std::string audio_codec_type = "";
  int video_bit_rate = 0;
  int audio_bit_rate = 0;
  boost::json::value metadata;
  std::string role = "sendonly";
  bool multistream = false;
  bool spotlight = false;
  int spotlight_number = 0;
  bool simulcast = false;
  boost::optional<bool> data_channel_signaling;
  int data_channel_signaling_timeout = 180;
  boost::optional<bool> ignore_disconnect_websocket;
  int disconnect_wait_timeout = 5;
  struct DataChannel {
    std::string label;
    std::string direction;
    boost::optional<bool> ordered;
    boost::optional<int32_t> max_packet_life_time;
    boost::optional<int32_t> max_retransmits;
    boost::optional<std::string> protocol;
    boost::optional<bool> compress;
  };
  std::vector<DataChannel> data_channels;

  std::string client_cert;
  std::string client_key;

  int websocket_close_timeout = 3;
  int websocket_connection_timeout = 30;
};

class SoraSignaling : public std::enable_shared_from_this<SoraSignaling>,
                      public webrtc::PeerConnectionObserver,
                      public DataChannelObserver {
  SoraSignaling(const SoraSignalingConfig& config);

 public:
  ~SoraSignaling();
  static std::shared_ptr<SoraSignaling> Create(
      const SoraSignalingConfig& config);
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> GetPeerConnection() const;

  void Connect();
  void Disconnect();
  void SendDataChannel(const std::string& label, const std::string& data);

 private:
  static bool ParseURL(const std::string& url, URLParts& parts, bool& ssl);

  void Redirect(std::string url);
  void OnRedirect(boost::system::error_code ec,
                  std::string url,
                  std::shared_ptr<Websocket> ws);

  void DoRead();
  void DoSendConnect(bool redirect);
  void DoSendPong();
  void DoSendPong(
      const rtc::scoped_refptr<const webrtc::RTCStatsReport>& report);
  void DoSendUpdate(const std::string& sdp, std::string type);

  rtc::scoped_refptr<webrtc::PeerConnectionInterface> CreatePeerConnection(
      boost::json::value jconfig);

 private:
  // 出来るだけサーバに type: disconnect を送ってから閉じる
  // force_error_code が設定されていない場合、NO-ERROR でサーバに送信し、ユーザにはデフォルトのエラーコードとメッセージでコールバックする。reason や message は無視される。
  // force_error_code が設定されている場合、reason でサーバに送信し、指定されたエラーコードと、message にデフォルトのメッセージを追加したメッセージでコールバックする。
  void DoInternalDisconnect(
      boost::optional<SoraSignalingErrorCode> force_error_code,
      std::string reason,
      std::string message);

  std::function<void(webrtc::RTCError)> CreateIceError(std::string message);

  void OnConnect(boost::system::error_code ec,
                 std::string url,
                 std::shared_ptr<Websocket> ws);
  void OnRead(boost::system::error_code ec,
              std::size_t bytes_transferred,
              std::string text);
  void DoConnect();

 private:
  void SetEncodingParameters(
      std::string mid,
      std::vector<webrtc::RtpEncodingParameters> encodings);
  void ResetEncodingParameters();

  void SendOnDisconnect(SoraSignalingErrorCode ec, std::string message);

  webrtc::DataBuffer ConvertToDataBuffer(const std::string& label,
                                         const std::string& input);

  void Clear();

  // webrtc::PeerConnectionObserver の実装
 private:
  void OnSignalingChange(
      webrtc::PeerConnectionInterface::SignalingState new_state) override {}
  void OnDataChannel(
      rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
  void OnStandardizedIceConnectionChange(
      webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
  void OnConnectionChange(
      webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
  void OnIceGatheringChange(
      webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
  void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
  void OnIceCandidateError(const std::string& address,
                           int port,
                           const std::string& url,
                           int error_code,
                           const std::string& error_text) override;

  // DataChannelObserver の実装
 private:
  void OnStateChange(
      rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
  void OnMessage(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel,
                 const webrtc::DataBuffer& buffer) override;

 private:
  SoraSignalingConfig config_;

  std::vector<std::shared_ptr<Websocket>> connecting_wss_;
  std::string connected_signaling_url_;
  std::shared_ptr<Websocket> ws_;
  std::shared_ptr<DataChannel> dc_;
  bool using_datachannel_ = false;
  bool ws_connected_ = false;
  std::set<std::string> compressed_labels_;

  rtc::scoped_refptr<webrtc::PeerConnectionInterface> pc_;
  std::vector<webrtc::RtpEncodingParameters> encodings_;
  std::string mid_;

  boost::asio::deadline_timer connection_timeout_timer_;
  boost::asio::deadline_timer closing_timeout_timer_;
  std::function<void(boost::system::error_code ec)> on_ws_close_;
  webrtc::PeerConnectionInterface::IceConnectionState ice_state_ =
      webrtc::PeerConnectionInterface::kIceConnectionNew;
  webrtc::PeerConnectionInterface::PeerConnectionState connection_state_ =
      webrtc::PeerConnectionInterface::PeerConnectionState::kNew;

  enum State {
    Init,
    Connecting,
    Redirecting,
    Connected,
    Closing,
    Closed,
    Destructing,
  };

  State state_ = State::Init;
};

}  // namespace sora

#endif