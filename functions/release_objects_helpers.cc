#include "release_objects_helpers.h"

napi_ref UplinkObjectReleaseHelper::constructor;

UplinkObjectReleaseHelper::UplinkObjectReleaseHelper()
    : objEnv(nullptr), objWrapper(nullptr) {}

UplinkObjectReleaseHelper::~UplinkObjectReleaseHelper()
{
   napi_delete_reference(objEnv, objWrapper);
}

void UplinkObjectReleaseHelper::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  UplinkObjectReleaseHelper* obj =  reinterpret_cast<UplinkObjectReleaseHelper*>(nativeObject);
  obj->Close();
  delete obj;
}

void UplinkObjectReleaseHelper::Init(napi_env env, napi_value &exports) {
  napi_status status;

  napi_value cons;
  status = napi_define_class(env, "UplinkObjectReleaseHelper", NAPI_AUTO_LENGTH, New, nullptr, 0, 0, &cons);
  assert(status == napi_ok);

  status = napi_create_reference(env, cons, 1, &constructor);
  assert(status == napi_ok);

  status = napi_set_named_property(env, exports, "UplinkObjectReleaseHelper", cons);
  assert(status == napi_ok);
}

napi_value UplinkObjectReleaseHelper::New(napi_env env, napi_callback_info info) {
  napi_status status;

  napi_value target;
  status = napi_get_new_target(env, info, &target);
  assert(status == napi_ok);
  bool is_constructor = target != nullptr;

  if (is_constructor) {
    size_t argc = 1;
    napi_value args[1];
    napi_value jsthis;
    status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
    assert(status == napi_ok);

    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    assert(status == napi_ok);
    uint32_t value = 0;
    if (valuetype != napi_undefined) {
      status = napi_get_value_uint32(env, args[0], &value);
      assert(status == napi_ok);
    }

    UplinkObjectReleaseHelper* obj = nullptr;

    switch(static_cast<UplinkObjectReleaseHelper::ObjectType>(value)) {
      case UplinkObjectReleaseHelper::ObjectType::DownloadObject:
        obj =  new DownloadObjectReleaseHelper();
        break;
     case  UplinkObjectReleaseHelper::ObjectType::UploadObject:
        obj =  new UploadObjectReleaseHelper();
        break;
      default:
        napi_throw_type_error(env, nullptr, "Unsupported object type passed");
        return nullptr;
    }

    obj->objEnv = env;
    status = napi_wrap(env,
                       jsthis,
                       reinterpret_cast<void*>(obj),
                       UplinkObjectReleaseHelper::Destructor,
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

bool UplinkObjectReleaseHelper::IsUplinkObjectReleaseHelper(napi_env env, napi_value value)
{
  napi_value cons;
  napi_status status;

  status = napi_get_reference_value(env, constructor, &cons);
  assert(status == napi_ok);

  bool isUplinkObjectReleaseHelper = false;
  status = napi_instanceof(env, value, cons, &isUplinkObjectReleaseHelper);
  assert(status == napi_ok);
  return isUplinkObjectReleaseHelper;
}

napi_value DownloadObjectReleaseHelper::CreateInstanceAndSetDownloadResult(napi_env env, const UplinkDownloadResult &result)
{
  napi_status status;
  napi_value cons;
  status = napi_get_reference_value(env, UplinkObjectReleaseHelper::constructor, &cons);
  assert(status == napi_ok);

  napi_value args[1];
  status = napi_create_uint32(env, static_cast<uint32_t>(UplinkObjectReleaseHelper::ObjectType::DownloadObject), &args[0]);

  napi_value instance;
  status = napi_new_instance(env, cons, 1, args, &instance);
  assert(status == napi_ok);

  DownloadObjectReleaseHelper* downloadObjectReleaseHelper;
  status = napi_unwrap(env, instance, reinterpret_cast<void**>(&downloadObjectReleaseHelper));
  assert(status == napi_ok);
  downloadObjectReleaseHelper->downloadResult = result;
  downloadObjectReleaseHelper->resultExist = true;
  return instance;
}

size_t DownloadObjectReleaseHelper::GetHandle()
{
  assert(resultExist);
  return downloadResult.download->_handle;
}

UplinkError* DownloadObjectReleaseHelper::Close()
{
  if (resultExist) {
    auto err = uplink_close_download(downloadResult.download);
    uplink_free_download_result(downloadResult);
    resultExist = false;
    return err;
  }
  return nullptr;
}

napi_value UploadObjectReleaseHelper::CreateInstanceAndSetUploadResult(napi_env env, const UplinkUploadResult &result)
{
  napi_status status;
  napi_value cons;
  status = napi_get_reference_value(env, UplinkObjectReleaseHelper::constructor, &cons);
  assert(status == napi_ok);

  napi_value args[1];
  status = napi_create_uint32(env, static_cast<uint32_t>(UplinkObjectReleaseHelper::ObjectType::UploadObject), &args[0]);

  napi_value instance;
  status = napi_new_instance(env, cons, 1, args, &instance);
  assert(status == napi_ok);

  UploadObjectReleaseHelper* uploadObjectReleaseHelper;
  status = napi_unwrap(env, instance, reinterpret_cast<void**>(&uploadObjectReleaseHelper));
  assert(status == napi_ok);
  uploadObjectReleaseHelper->uploadResult = result;
  uploadObjectReleaseHelper->resultExist = true;
  return instance;
}

size_t UploadObjectReleaseHelper::GetHandle()
{
  assert(resultExist);
  return uploadResult.upload->_handle;
}

UplinkError* UploadObjectReleaseHelper::Close()
{
  if (resultExist) {
    uplink_free_upload_result(uploadResult);
    resultExist = false;
  }
  return nullptr;
}
