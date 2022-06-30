#ifndef RELEASE_OBJECTS_HELPERS
#define RELEASE_OBJECTS_HELPERS

#include <node_api.h>
#include "definitions.h"

class DowloadObjectReleaseHelper {
public:
  static void Init(napi_env env, napi_value &exports);
  static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);
  static napi_value CreateInstanceAndSetDownloadResult(napi_env env, const UplinkDownloadResult &result);
  static bool IsDowloadObjectReleaseHelper(napi_env env, napi_value value);
  size_t GetDownloadHandle() { return downloadResult.download->_handle; }
  UplinkError* Close();

private:
  DowloadObjectReleaseHelper();
  virtual ~DowloadObjectReleaseHelper();
  static napi_value New(napi_env env, napi_callback_info info);
  static napi_ref constructor;

  UplinkDownloadResult downloadResult;
  bool resultExist{false};
  napi_env objEnv;
  napi_ref objWrapper;
};

#endif