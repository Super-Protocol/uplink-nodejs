#include "release_objects_helpers.h"

napi_ref DowloadObjectReleaseHelper::constructor;

DowloadObjectReleaseHelper::DowloadObjectReleaseHelper()
    : objEnv(nullptr), objWrapper(nullptr) {}

DowloadObjectReleaseHelper::~DowloadObjectReleaseHelper()
{
  Close();
  napi_delete_reference(objEnv, objWrapper);
}

void DowloadObjectReleaseHelper::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  reinterpret_cast<DowloadObjectReleaseHelper*>(nativeObject)->~DowloadObjectReleaseHelper();
}

void DowloadObjectReleaseHelper::Init(napi_env env, napi_value &exports) {
  napi_status status;

  napi_value cons;
  status = napi_define_class(env, "DowloadObjectReleaseHelper", NAPI_AUTO_LENGTH, New, nullptr, 0, 0, &cons);
  assert(status == napi_ok);

  status = napi_create_reference(env, cons, 1, &constructor);
  assert(status == napi_ok);

  status = napi_set_named_property(env, exports, "DowloadObjectReleaseHelper", cons);
  assert(status == napi_ok);
}

napi_value DowloadObjectReleaseHelper::New(napi_env env, napi_callback_info info) {
  napi_status status;

  napi_value target;
  status = napi_get_new_target(env, info, &target);
  assert(status == napi_ok);
  bool is_constructor = target != nullptr;

  if (is_constructor) {
    napi_value jsthis;
    status = napi_get_cb_info(env, info, 0,0, &jsthis, nullptr);
    assert(status == napi_ok);

    DowloadObjectReleaseHelper* obj = new DowloadObjectReleaseHelper();

    obj->objEnv = env;
    status = napi_wrap(env,
                       jsthis,
                       reinterpret_cast<void*>(obj),
                       DowloadObjectReleaseHelper::Destructor,
                       nullptr,  // finalize_hint
                       &obj->objWrapper);
    assert(status == napi_ok);

    return jsthis;
  } else {

    status = napi_get_cb_info(env, info, 0, 0, nullptr, nullptr);
    assert(status == napi_ok);

    napi_value cons;
    status = napi_get_reference_value(env, constructor, &cons);
    assert(status == napi_ok);

    napi_value instance;
    status = napi_new_instance(env, cons, 0, 0, &instance);
    assert(status == napi_ok);

    return instance;
  }
}

bool DowloadObjectReleaseHelper::IsDowloadObjectReleaseHelper(napi_env env, napi_value value)
{
  napi_value cons;
  napi_status status;

  status = napi_get_reference_value(env, constructor, &cons);
  assert(status == napi_ok);

  bool isDowloadObjectReleaseHelper = false;
  status = napi_instanceof(env, value, cons, &isDowloadObjectReleaseHelper);
  assert(status == napi_ok);
  return isDowloadObjectReleaseHelper;
}

UplinkError* DowloadObjectReleaseHelper::Close()
{
  if (resultExist) {
    auto err = uplink_close_download(downloadResult.download);
    uplink_free_download_result(downloadResult);
    resultExist = false;
    return err;
  }
  return nullptr;
}

napi_value DowloadObjectReleaseHelper::CreateInstanceAndSetDownloadResult(napi_env env, const UplinkDownloadResult &result)
{
  napi_status status;
  napi_value cons;
  status = napi_get_reference_value(env, DowloadObjectReleaseHelper::constructor, &cons);
  assert(status == napi_ok);

  napi_value instance;
  status = napi_new_instance(env, cons, 0, 0, &instance);
  assert(status == napi_ok);

  DowloadObjectReleaseHelper* dowloadObjectReleaseHelper;
  status = napi_unwrap(env, instance, reinterpret_cast<void**>(&dowloadObjectReleaseHelper));
  assert(status == napi_ok);
  dowloadObjectReleaseHelper->downloadResult = result;
  dowloadObjectReleaseHelper->resultExist = true;
  return instance;
}