#ifndef RELEASE_OBJECTS_HELPERS
#define RELEASE_OBJECTS_HELPERS

#include <node_api.h>
#include "definitions.h"
#include <cstdint>

class UplinkObjectReleaseHelper {
public:
  static void Init(napi_env env, napi_value &exports);
  static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);
  static bool IsUplinkObjectReleaseHelper(napi_env env, napi_value value);
  virtual size_t GetHandle() = 0;
  virtual UplinkError* Close() = 0;

protected:
  UplinkObjectReleaseHelper();
  virtual ~UplinkObjectReleaseHelper();
  static napi_value New(napi_env env, napi_callback_info info);
  static napi_ref constructor;

  bool resultExist{false};
  napi_env objEnv;
  napi_ref objWrapper;
  enum class ObjectType: std::uint32_t {
    DownloadObject,
    UploadObject
  };
};

class DownloadObjectReleaseHelper: public UplinkObjectReleaseHelper{
public:
  UplinkError* Close() override;
  static napi_value CreateInstanceAndSetDownloadResult(napi_env env, const UplinkDownloadResult &result);
  size_t GetHandle() override;
private:
  UplinkDownloadResult downloadResult;
};

class UploadObjectReleaseHelper: public UplinkObjectReleaseHelper{
public:
  UplinkError* Close() override;
  static napi_value CreateInstanceAndSetUploadResult(napi_env env, const UplinkUploadResult &result);
  size_t GetHandle() override;
private:
  UplinkUploadResult uploadResult;
};

#endif
