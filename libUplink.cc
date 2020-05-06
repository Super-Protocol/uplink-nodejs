#include <assert.h>
#include <node_api.h>
#include "uplink_definitions.h"
#include "libuplinkc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std; 

/* A utility function to reverse a string  */
void reverse(char str[], int length) 
{ 
    int start = 0; 
    int end = length -1; 
    while (start < end) 
    { 
        swap(*(str+start), *(str+end)); 
        start++; 
        end--; 
    } 
} 
  

char* itoa(int num, char* str, int base) 
{ 
    int i = 0; 
    bool isNegative = false; 
  
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0) 
    { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        return str; 
    } 
  
    // In standard itoa(), negative numbers are handled only with  
    // base 10. Otherwise numbers are considered unsigned. 
    if (num < 0 && base == 10) 
    { 
        isNegative = true; 
        num = -num; 
    } 
  
    // Process individual digits 
    while (num != 0) 
    { 
        int rem = num % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    } 
  
    // If number is negative, append '-' 
    if (isNegative) 
        str[i++] = '-'; 
  
    str[i] = '\0'; // Append string terminator 
  
    // Reverse the string 
    reverse(str, i); 
  
    return str; 
}

// createObjectResult function creates NAPI type ObjectResult for returning it to javascript
napi_value createObjectResult(napi_env env,Object* objectPtr,Error* error_result){
  napi_value returnObject,objectNAPI,errorObject,SystemMetadataNAPI,CustomMetadataNAPI,CustomMetadataEntryNAPI;
  napi_status status;

  status = napi_create_object(env,&returnObject);
  assert(status == napi_ok);

  if(objectPtr!=NULL){

    Object object_result = *(objectPtr);

    status = napi_create_object(env,&objectNAPI);
    assert(status == napi_ok);
  
    status = napi_create_object(env,&SystemMetadataNAPI);
    assert(status == napi_ok);

    status = napi_create_object(env,&CustomMetadataNAPI);
    assert(status == napi_ok);
  
    status = napi_create_object(env,&CustomMetadataEntryNAPI);
    assert(status == napi_ok);
    
    napi_value keyObjectNAPI;
    status = napi_create_string_utf8(env,objectPtr->key,NAPI_AUTO_LENGTH,&keyObjectNAPI);
    assert(status == napi_ok);
    
    napi_value is_prefixNAPI;
    if(objectPtr->is_prefix){
      status = napi_create_int64(env, 1, &is_prefixNAPI);
      assert(status == napi_ok);
    }else{
      status = napi_create_int64(env, 0, &is_prefixNAPI);
      assert(status == napi_ok);
    }

    //
    SystemMetadata systemMetadata;
    CustomMetadata customMetadata;
    CustomMetadataEntry customMetadataEntry;
    systemMetadata = object_result.system;
    customMetadata = object_result.custom;
    napi_value entriesArray;
    status = napi_create_object(env,&entriesArray);
    assert(status == napi_ok);
    char empty[]="";
    if(customMetadata.count>0){
      CustomMetadataEntry* CustomMetadataEntryListPointer = customMetadata.entries;
      for(int j=0;j<customMetadata.count;j++){
        customMetadataEntry = *(CustomMetadataEntryListPointer+j);
        napi_value key_lengthNAPI,value_lengthNAPI,keyNAPI,valueNAPI,customMetaNAPI;
        status = napi_create_object(env,&customMetaNAPI);
        assert(status == napi_ok);

        status = napi_create_int64(env, customMetadataEntry.key_length, &key_lengthNAPI);
        assert(status == napi_ok);
    
        status = napi_create_int64(env, customMetadataEntry.value_length, &value_lengthNAPI);
        assert(status == napi_ok);

        status = napi_create_string_utf8(env,customMetadataEntry.key,NAPI_AUTO_LENGTH,&keyNAPI);
        assert(status == napi_ok);
        
        status = napi_create_string_utf8(env,customMetadataEntry.value,NAPI_AUTO_LENGTH,&valueNAPI);
        assert(status == napi_ok);
        
        status = napi_set_named_property(env,customMetaNAPI,"key",keyNAPI);
        assert(status == napi_ok);

        status = napi_set_named_property(env,customMetaNAPI,"key_length",key_lengthNAPI);
        assert(status == napi_ok);

        status = napi_set_named_property(env,customMetaNAPI,"value",valueNAPI);
        assert(status == napi_ok);

        status = napi_set_named_property(env,customMetaNAPI,"value_length",value_lengthNAPI);
        assert(status == napi_ok);

        status = napi_set_named_property(env,customMetaNAPI,"value_length",value_lengthNAPI);
        assert(status == napi_ok);

        char str[100];
        itoa(j, str, 10);
        status = napi_set_named_property(env,entriesArray,str,customMetaNAPI);
        assert(status == napi_ok);
      }
    }else{
      napi_value emptyNAPI;
      status = napi_create_string_utf8(env,&empty[0],NAPI_AUTO_LENGTH,&emptyNAPI);
      assert(status == napi_ok);

      status = napi_set_named_property(env,entriesArray,"0",emptyNAPI);
      assert(status == napi_ok);
    }
    
    
    napi_value createdNAPI;
    status = napi_create_int64(env, objectPtr->system.created, &createdNAPI);
    assert(status == napi_ok);
    
    napi_value expiresNAPI;
    status = napi_create_int64(env, objectPtr->system.expires, &expiresNAPI);
    assert(status == napi_ok);

    napi_value countNAPI;
    status = napi_create_int64(env, customMetadata.count, &countNAPI);
    assert(status == napi_ok);

    napi_value content_lengthNAPI;
    status = napi_create_int64(env, systemMetadata.content_length, &content_lengthNAPI);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,CustomMetadataNAPI,"entries",entriesArray);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,CustomMetadataNAPI,"count",countNAPI);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,objectNAPI,"custom",CustomMetadataNAPI);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,SystemMetadataNAPI,"created",createdNAPI);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,SystemMetadataNAPI,"expires",expiresNAPI);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,SystemMetadataNAPI,"content_length",content_lengthNAPI);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,objectNAPI,"system",SystemMetadataNAPI);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,objectNAPI,"is_prefix",is_prefixNAPI);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,objectNAPI,"key",keyObjectNAPI);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,returnObject,"object",objectNAPI);
    assert(status == napi_ok);
    
    napi_value nullvalue;
    status = napi_create_string_utf8(env,&empty[0],NAPI_AUTO_LENGTH,&nullvalue);
    assert(status == napi_ok);
    
    status = napi_create_object(env,&errorObject);
    assert(status == napi_ok);

    napi_value errorCode;
    status = napi_create_int64(env, 0, &errorCode);
    assert(status == napi_ok);

    status = napi_set_named_property(env,errorObject,"code",errorCode);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,errorObject,"message",nullvalue);
    assert(status == napi_ok);

    status = napi_set_named_property(env,returnObject,"error",errorObject);
    assert(status == napi_ok);

  }else if(error_result!=NULL){
    
    Error errorResult = *(error_result);
    
    status = napi_create_object(env,&errorObject);
    assert(status == napi_ok);
    
    napi_value errorCode;
    status = napi_create_int64(env, errorResult.code, &errorCode);
    assert(status == napi_ok);
    
    napi_value errorMessage;
    status = napi_create_string_utf8(env,errorResult.message,NAPI_AUTO_LENGTH,&errorMessage);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,errorObject,"code",errorCode);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,errorObject,"message",errorMessage);
    assert(status == napi_ok);
    
    char empty[]="";
    napi_value nullvalue;
    status = napi_create_string_utf8(env,&empty[0],NAPI_AUTO_LENGTH,&nullvalue);
    assert(status == napi_ok);

    status = napi_set_named_property(env,returnObject,"object",nullvalue);
    assert(status == napi_ok);
    
    status = napi_set_named_property(env,returnObject,"error",errorObject);
    assert(status == napi_ok);
  }
  return returnObject;
}

//function create's NAPI type result for accessResult , ProjectResult
napi_value createResult(napi_env env,string resultType,long handlevalue,int64_t accessError,char* errorMessagePtr){
  napi_status status;
  napi_value ObjectRef,returnObject,errorObject; 

  //Creating NAPI Object's
  status = napi_create_object(env,&ObjectRef);
  assert(status == napi_ok);
  
  status = napi_create_object(env,&errorObject);
  assert(status == napi_ok);

  status = napi_create_object(env,&returnObject);
  assert(status == napi_ok);

  //Creating int64 NAPI type
  napi_value _handle,errorCode,errorMessage;
  
  status = napi_create_int64(env, handlevalue, &_handle);
  assert(status == napi_ok);
  
  status = napi_set_named_property(env,ObjectRef,"_handle",_handle);
  assert(status == napi_ok);
  
  status = napi_create_object(env,&errorObject);
  assert(status == napi_ok);
  
  status = napi_create_int64(env, accessError, &errorCode);
  assert(status == napi_ok);
  
  status = napi_create_string_utf8(env,errorMessagePtr,NAPI_AUTO_LENGTH,&errorMessage);
  assert(status == napi_ok);
  
  status = napi_set_named_property(env,errorObject,"code",errorCode);
  assert(status == napi_ok);
  
  status = napi_set_named_property(env,errorObject,"message",errorMessage);
  assert(status == napi_ok);
  
  status = napi_set_named_property(env,returnObject,"error",errorObject);
  assert(status == napi_ok);
  
  const char* resultTypeChar = resultType.c_str();
  
  status = napi_set_named_property(env,returnObject,&resultTypeChar[0],ObjectRef);
  assert(status == napi_ok);

  return returnObject;
}

//function creates BucketResults
napi_value createBucketResult(napi_env env,string resultType,int64_t bucketCreated,char* bucketNamePtr,int64_t bucketErrorCode,char* errorMessagePtr){
  napi_status status;
  napi_value ObjectRef,returnObject,errorObject; 
  
  //Creating NAPI Object
  status = napi_create_object(env,&ObjectRef);
  assert(status == napi_ok);
  
  status = napi_create_object(env,&errorObject);
  assert(status == napi_ok);

  status = napi_create_object(env,&returnObject);
  assert(status == napi_ok);
  
  napi_value created,errorCode,errorMessage,bucketName;
  status = napi_create_int64(env, bucketCreated, &created);
  assert(status == napi_ok);
  status = napi_set_named_property(env,ObjectRef,"created",created);
  assert(status == napi_ok);
  
  status = napi_create_string_utf8(env,bucketNamePtr,NAPI_AUTO_LENGTH,&bucketName);
  assert(status == napi_ok);
  
  status = napi_set_named_property(env,ObjectRef,"name",bucketName);
  assert(status == napi_ok);
  
  status = napi_create_object(env,&errorObject);
  assert(status == napi_ok);
  
  status = napi_create_int64(env, bucketErrorCode, &errorCode);
  assert(status == napi_ok);
  
  status = napi_create_string_utf8(env,errorMessagePtr,NAPI_AUTO_LENGTH,&errorMessage);
  assert(status == napi_ok);
  
  status = napi_set_named_property(env,errorObject,"code",errorCode);
  assert(status == napi_ok);
  
  status = napi_set_named_property(env,errorObject,"message",errorMessage);
  assert(status == napi_ok);
  
  status = napi_set_named_property(env,returnObject,"error",errorObject);
  assert(status == napi_ok);
  
  const char* resultTypeChar = resultType.c_str();
  status = napi_set_named_property(env,returnObject,&resultTypeChar[0],ObjectRef);
  assert(status == napi_ok);
  
  return returnObject;
}

//function creates NAPI type error object
napi_value createError(napi_env env,int64_t accessError,char* errorMessagePtr){
  napi_status status;
  napi_value returnObject,errorObject,errorCode,errorMessage;
  
  status = napi_create_object(env,&errorObject);
  assert(status == napi_ok);
  
  status = napi_create_object(env,&returnObject);
  assert(status == napi_ok);
  
  status = napi_create_int64(env, accessError, &errorCode);
  assert(status == napi_ok);

  status = napi_create_string_utf8(env,errorMessagePtr,NAPI_AUTO_LENGTH,&errorMessage);
  assert(status == napi_ok);
  
  status = napi_set_named_property(env,errorObject,"code",errorCode);
  assert(status == napi_ok);
  
  status = napi_set_named_property(env,errorObject,"message",errorMessage);
  assert(status == napi_ok);
  
  status = napi_set_named_property(env,returnObject,"error",errorObject);
  assert(status == napi_ok);
  return returnObject;
}

//
long getHandleValue(napi_env env,napi_value handleobj){
  
  napi_status status;
  
  napi_valuetype checktypeofinput;
  
  status = napi_typeof(env,handleobj,&checktypeofinput);
  assert(status == napi_ok);
  
  if(checktypeofinput!=napi_object){
    return 0;
  }
  napi_value propertyname;
  status = napi_get_property_names(env,handleobj,&propertyname);
  assert(status == napi_ok);

  napi_value handlevalue;
  status = napi_get_property(env,handleobj,propertyname,&handlevalue);
  assert(status == napi_ok);
  
  status = napi_typeof(env,handlevalue,&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_number){
    napi_throw_type_error(env,nullptr,"\nWrong Handle type\n");
    return 0;
  }
  int64_t handle = 0;
  status = napi_get_value_int64(env,handlevalue,&handle);
  assert(status == napi_ok);

  long handlelong = (long)handle;
  return handlelong;
}

//  * function parse_accessc to parses serialized access grant string
//  * pre-requisites: access_sharec
//  * inputs: accessString (String)
//  * output: AccessResult (Object)
napi_value parse_accessc(napi_env env, napi_callback_info info){
  napi_status status;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 1 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! argument excepted to be string type\n");
    return NULL;
  }

  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[0],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;

  char* accessString =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[0],accessString,convertedvalue,&bufsize);
  assert(status == napi_ok);

	AccessResult access_Result = parse_access(accessString);
	if(access_Result.access==NULL){
		if(access_Result.error!=NULL){
			Error error_result = *(access_Result.error);
			char* errorMessagePtr = error_result.message;
			return createResult(env,"access",0,error_result.code,&errorMessagePtr[0]);
		  }else{
			char blank[] = "";
			return createResult(env,"access",0,0,&blank[0]);
		  }
	}
	Access access = *(access_Result.access);
	size_t handlevalue = access._handle;
	char blank[] = "";
	return createResult(env,"access",handlevalue,0,&blank[0]);
}


  //  * function request_access_with_passphrasec requests satellite for a new access grant using a passhprase
  //  * pre-requisites: None
  //  * inputs: satellite address (String),API key (String) and passphrase (String)
  //  * output: AccessResult (Object)
napi_value request_access_with_passphrasec(napi_env env, napi_callback_info info){
  napi_status status;

  size_t argc = 3;
  napi_value args[3];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  
  if (argc < 3) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 3 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be string type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string type\n");
    return NULL;
  }

  status = napi_typeof(env,args[2],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Third argument excepted to be string type\n");
    return NULL;
  }

  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[0],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* satellite_address =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[0],satellite_address,convertedvalue,&bufsize);
  assert(status == napi_ok);
  
  bufsize = 0;
  convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[1],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* api_key =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[1],api_key,convertedvalue,&bufsize);
  assert(status == napi_ok);

  bufsize = 0;
  convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[2],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* passphrase =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[2],passphrase,convertedvalue,&bufsize);
  assert(status == napi_ok);

  AccessResult access_Result = request_access_with_passphrase(satellite_address,api_key,passphrase);
      
	  if(access_Result.access==NULL){
		
		if(access_Result.error!=NULL){
		  Error error_result = *(access_Result.error);
		  char* errorMessagePtr = error_result.message;
		  return createResult(env,"access",0,error_result.code,&errorMessagePtr[0]);
		}else{
		  char blank[] = "";
		  return createResult(env,"access",0,-1,&blank[0]);
		}
	  }
      Access access = *(access_Result.access);
      size_t handlevalue = access._handle;
      char blank[] = "";
      return createResult(env,"access",handlevalue,0,&blank[0]);

}


  //  * function config_request_access_with_passphrasec requests satellite for a new access grant using a passhprase  and config
  //  * pre-requisites: None
  //  * inputs: satellite address (String),API key (String) and passphrase (String)
  //  * output: AccessResult (Object)
napi_value config_request_access_with_passphrasec(napi_env env, napi_callback_info info){
  napi_status status;

  size_t argc = 4;
  napi_value args[4];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  
  if (argc < 4) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 3 arguments\n");
    return NULL;
  }

  Config config;

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string type\n");
    return NULL;
  }

  status = napi_typeof(env,args[2],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Third argument excepted to be string type\n");
    return NULL;
  }

  status = napi_typeof(env,args[3],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Fourth argument excepted to be string type\n");
    return NULL;
  }
  //
  bool configUserAgentExists,configDialTimeoutMilliSecondsExists,configTempDirectoryExists = false;
  napi_value configUserAgentNAPI,configDialTimeoutMilliSecondsNAPI,configTempDirectoryNAPI;
  char* user_agent_property = "user_agent";
  char* dial_timeout_milliseconds_property = "dial_timeout_milliseconds";
  char* temp_directory_property = "temp_directory";
  
  status = napi_create_string_utf8(env,user_agent_property,NAPI_AUTO_LENGTH ,&configUserAgentNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],configUserAgentNAPI,&configUserAgentExists);
  assert(status == napi_ok);
  //
  if(!configUserAgentExists){
    napi_throw_type_error(env,nullptr,"\nInvalid Config Object \n");
    return NULL;
  }
  //
  status = napi_create_string_utf8(env,dial_timeout_milliseconds_property,NAPI_AUTO_LENGTH ,&configDialTimeoutMilliSecondsNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],configDialTimeoutMilliSecondsNAPI,&configDialTimeoutMilliSecondsExists);
  assert(status == napi_ok);
  //
  if(!configDialTimeoutMilliSecondsExists){
    napi_throw_type_error(env,nullptr,"\nInvalid Config Object \n");
    return NULL;
  }
  //
  status = napi_create_string_utf8(env,temp_directory_property,NAPI_AUTO_LENGTH ,&configTempDirectoryNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],configTempDirectoryNAPI,&configTempDirectoryExists);
  assert(status == napi_ok);
  //
  if(!configTempDirectoryExists){
    napi_throw_type_error(env,nullptr,"\nInvalid Config Object \n");
    return NULL;
  }
  //
  napi_value user_agentNAPI;
  status = napi_get_named_property(env,args[0],"user_agent",&user_agentNAPI);
  assert(status == napi_ok);
  //
  napi_value dial_timeout_millisecondsNAPI;
  status = napi_get_named_property(env,args[0],"dial_timeout_milliseconds",&dial_timeout_millisecondsNAPI);
  assert(status == napi_ok);
  //
  napi_value temp_directoryNAPI;
  status = napi_get_named_property(env,args[0],"temp_directory",&temp_directoryNAPI);
  assert(status == napi_ok);
  //
  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, user_agentNAPI,NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;

  char* user_agent =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,user_agentNAPI,user_agent,convertedvalue,&bufsize);
  assert(status == napi_ok);
  //
  bufsize = 0;
  convertedvalue = 0;
  status = napi_get_value_string_utf8(env, temp_directoryNAPI,NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* temp_directory =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,temp_directoryNAPI,temp_directory,convertedvalue,&bufsize);
  assert(status == napi_ok);
  //
  int32_t dial_timeout_milliseconds;
  status = napi_get_value_int32(env,dial_timeout_millisecondsNAPI,&dial_timeout_milliseconds);
  assert(status == napi_ok);
  //
  config.temp_directory = temp_directory;
  config.dial_timeout_milliseconds = dial_timeout_milliseconds;
  config.user_agent = user_agent;
  //
  bufsize = 0;
  convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[1],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* satellite_address =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[1],satellite_address,convertedvalue,&bufsize);
  assert(status == napi_ok);
  
  bufsize = 0;
  convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[2],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* api_key =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[2],api_key,convertedvalue,&bufsize);
  assert(status == napi_ok);

  bufsize = 0;
  convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[3],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* passphrase =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[3],passphrase,convertedvalue,&bufsize);
  assert(status == napi_ok);

  AccessResult access_Result = config_request_access_with_passphrase(config,satellite_address,api_key,passphrase);
      
    if(access_Result.access==NULL){
    
    if(access_Result.error!=NULL){
      Error error_result = *(access_Result.error);
      char* errorMessagePtr = error_result.message;
      return createResult(env,"access",0,error_result.code,&errorMessagePtr[0]);
    }else{
      char blank[] = "";
      return createResult(env,"access",0,-1,&blank[0]);
    }
    }
      Access access = *(access_Result.access);
      size_t handlevalue = access._handle;
      char blank[] = "";
      return createResult(env,"access",handlevalue,0,&blank[0]);

}


  //  * function open_project opens project using access grant.
  //  * pre-requisites: request_access_with_passphrasec
  //  * inputs: Access (Object)
  //  * output: ProjectResult (Object)
napi_value open_projectc(napi_env env, napi_callback_info info){
  napi_status status;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 1 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }
  Access access;
  access._handle = getHandleValue(env,args[0]);
  if(access._handle==0){
    return NULL;
  }
      ProjectResult project_result = open_project(&access);
      if(project_result.project==NULL){
        
        if(project_result.error!=NULL){
          Error error_result = *(project_result.error);
          char* errorMessagePtr = error_result.message;
          return createResult(env,"project",0,error_result.code,errorMessagePtr);
        }else{
          char blank[] = "";
          return createResult(env,"project",0,-1,&blank[0]);
        }
      }
      Project project = *(project_result.project);
      size_t handlevalue = project._handle;
      char blank[] = "";
      return createResult(env,"project",handlevalue,0,&blank[0]);
}


  //  * function config_open_project opens project using access grant and config.
  //  * pre-requisites: request_access_with_passphrasec
  //  * inputs: Access (Object)
  //  * output: ProjectResult (Object)
napi_value config_open_projectc(napi_env env, napi_callback_info info){
  napi_status status;

  size_t argc = 2;
  napi_value args[2];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 2) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 1 arguments\n");
    return NULL;
  }
  Config config;
  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be object type\n");
    return NULL;
  }
  //
  bool configUserAgentExists,configDialTimeoutMilliSecondsExists,configTempDirectoryExists = false;
  napi_value configUserAgentNAPI,configDialTimeoutMilliSecondsNAPI,configTempDirectoryNAPI;
  char* user_agent_property = "user_agent";
  char* dial_timeout_milliseconds_property = "dial_timeout_milliseconds";
  char* temp_directory_property = "temp_directory";
  
  status = napi_create_string_utf8(env,user_agent_property,NAPI_AUTO_LENGTH ,&configUserAgentNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],configUserAgentNAPI,&configUserAgentExists);
  assert(status == napi_ok);
  //
  if(!configUserAgentExists){
    napi_throw_type_error(env,nullptr,"\nInvalid Config Object \n");
    return NULL;
  }
  //
  status = napi_create_string_utf8(env,dial_timeout_milliseconds_property,NAPI_AUTO_LENGTH ,&configDialTimeoutMilliSecondsNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],configDialTimeoutMilliSecondsNAPI,&configDialTimeoutMilliSecondsExists);
  assert(status == napi_ok);
  //
  if(!configDialTimeoutMilliSecondsExists){
    napi_throw_type_error(env,nullptr,"\nInvalid Config Object \n");
    return NULL;
  }
  //
  status = napi_create_string_utf8(env,temp_directory_property,NAPI_AUTO_LENGTH ,&configTempDirectoryNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],configTempDirectoryNAPI,&configTempDirectoryExists);
  assert(status == napi_ok);
  //
  if(!configTempDirectoryExists){
    napi_throw_type_error(env,nullptr,"\nInvalid Config Object \n");
    return NULL;
  }
  //
  napi_value user_agentNAPI;
  status = napi_get_named_property(env,args[0],"user_agent",&user_agentNAPI);
  assert(status == napi_ok);
  //
  napi_value dial_timeout_millisecondsNAPI;
  status = napi_get_named_property(env,args[0],"dial_timeout_milliseconds",&dial_timeout_millisecondsNAPI);
  assert(status == napi_ok);
  //
  napi_value temp_directoryNAPI;
  status = napi_get_named_property(env,args[0],"temp_directory",&temp_directoryNAPI);
  assert(status == napi_ok);
  //
  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, user_agentNAPI,NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;

  char* user_agent =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,user_agentNAPI,user_agent,convertedvalue,&bufsize);
  assert(status == napi_ok);
  //
  bufsize = 0;
  convertedvalue = 0;
  status = napi_get_value_string_utf8(env, temp_directoryNAPI,NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* temp_directory =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,temp_directoryNAPI,temp_directory,convertedvalue,&bufsize);
  assert(status == napi_ok);
  //
  int32_t dial_timeout_milliseconds;
  status = napi_get_value_int32(env,dial_timeout_millisecondsNAPI,&dial_timeout_milliseconds);
  assert(status == napi_ok);
  //
  config.temp_directory = temp_directory;
  config.dial_timeout_milliseconds = dial_timeout_milliseconds;
  config.user_agent = user_agent;
  //
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[1],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }
  Access access;
  access._handle = getHandleValue(env,args[1]);
  if(access._handle==0){
    return NULL;
  }
  

  ProjectResult project_result = config_open_project(config,&access);
  if(project_result.project==NULL){
    
    if(project_result.error!=NULL){
      Error error_result = *(project_result.error);
      char* errorMessagePtr = error_result.message;
      return createResult(env,"project",0,error_result.code,errorMessagePtr);
    }else{
      char blank[] = "";
      return createResult(env,"project",0,-1,&blank[0]);
    }
  }
    Project project = *(project_result.project);
    size_t handlevalue = project._handle;
    char blank[] = "";
    return createResult(env,"project",handlevalue,0,&blank[0]);
}



  //  * function close_projectc closes the project.
  //  * pre-requisites: open_projectc
  //  * inputs: Project (Object)
  //  * output: Error (Object)
napi_value close_projectc(napi_env env, napi_callback_info info){
  napi_status status;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 1 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Project project_result;
  project_result._handle = getHandleValue(env,args[0]);
  if(project_result._handle==0){
    return NULL;
  }
      Error* error_result = close_project(&project_result);
      if(error_result!=NULL){
        Error errorResult = *(error_result);
        char* errorMessagePtr = errorResult.message;
        return createError(env,errorResult.code,&errorMessagePtr[0]);
      }
      char blank[] = "";
      return createError(env,0,&blank[0]);
}
  //  * function stat_bucketc returns information about a bucket.
  //  * pre-requisites: open_projectc
  //  * inputs: Project (Object) ,Bucket Name (String)
  //  * output: BucketResult (Object)
napi_value stat_bucketc(napi_env env, napi_callback_info info){
  napi_status status;

  size_t argc = 2;
  napi_value args[2];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 2) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 2 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string type\n");
    return NULL;
  }

  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[1],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  char* bucketName =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[1],bucketName,convertedvalue,&bufsize);
  assert(status == napi_ok);

  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Project project_result;
  project_result._handle = getHandleValue(env,args[0]);
  if(project_result._handle==0){
    return NULL;
  }
  BucketResult bucket_Result = stat_bucket(&project_result, bucketName);

      if(bucket_Result.bucket==NULL){
        
        if(bucket_Result.error!=NULL){
          Error error_result = *(bucket_Result.error);
          char* errorMessagePtr = error_result.message;
          char blank[] = "";
          return createBucketResult(env,"bucket",0,&blank[0],error_result.code,&errorMessagePtr[0]);

        }else{
          char blank[] = "";
          return createBucketResult(env,"bucket",-1,&blank[0],-1,&blank[0]);
        }
      }
      Bucket bucket_result = *(bucket_Result.bucket);
      char* bucketNamePtr = bucket_result.name;
      int64_t bucketCreated = bucket_result.created;
      char blank[] = "";
      return createBucketResult(env,"bucket",bucketCreated,bucketNamePtr,0,&blank[0]);
}

  //  * function create_bucketc creates a new bucket.
  //  * pre-requisites: open_projectc
  //  * inputs: Project (Object) ,Bucket Name (String)
  //  * output: BucketResult (Object)
napi_value create_bucketc(napi_env env, napi_callback_info info){
  napi_status status;

  size_t argc = 2;
  napi_value args[2];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 2) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 2 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string type\n");
    return NULL;
  }

  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[1],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* bucketName =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[1],bucketName,convertedvalue,&bufsize);
  assert(status == napi_ok);
  
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Project project_result;
  project_result._handle = getHandleValue(env,args[0]);
  if(project_result._handle==0){
    return NULL;
  }
  //
  BucketResult bucket_Result = create_bucket(&project_result, bucketName);
    
    if(bucket_Result.bucket==NULL){
      if(bucket_Result.error!=NULL){
        Error error_result = *(bucket_Result.error);
        char* errorMessagePtr = error_result.message;
        char blank[] = "";
        return createBucketResult(env,"bucket",0,&blank[0],error_result.code,&errorMessagePtr[0]);

      }else{
        char blank[] = "";
        return createBucketResult(env,"bucket",-1,&blank[0],-1,&blank[0]);
      }
   }

    Bucket bucket_result = *(bucket_Result.bucket);
    char* bucketNamePtr = bucket_result.name;
    int64_t bucketCreated = bucket_result.created;
    char blank[] = "";
      return createBucketResult(env,"bucket",bucketCreated,bucketNamePtr,0,&blank[0]);
}

  //  * function ensure_bucketc creates a new bucket and ignores the error when it already exists
  //  * pre-requisites: open_projectc
  //  * inputs: Project (Object) ,Bucket Name (String)
  //  * output: BucketResult (Object)
napi_value ensure_bucketc(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 2;
  napi_value args[2];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 2) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 2 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string type\n");
    return NULL;
  }

  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[1],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  char* bucketName =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[1],&bucketName[0],convertedvalue,&bufsize);
  assert(status == napi_ok);

  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Project project_result;
  project_result._handle = getHandleValue(env,args[0]);
  if(project_result._handle==0){
    return NULL;
  }
  
	BucketResult bucket_Result = ensure_bucket(&project_result, bucketName);
      if(bucket_Result.bucket==NULL){
        
        if(bucket_Result.error!=NULL){
          Error error_result = *(bucket_Result.error);
          char* errorMessagePtr = error_result.message;
          char blank[] = "";
          return createBucketResult(env,"bucket",0,&blank[0],error_result.code,&errorMessagePtr[0]);
        }else{
          char blank[] = "";
          return createBucketResult(env,"bucket",-1,&blank[0],-1,&blank[0]);
        }
      }
      Bucket bucket_result = *(bucket_Result.bucket);
      char* bucketNamePtr = bucket_result.name;
      int64_t bucketCreated = bucket_result.created;
      char blank[] = "";
      return createBucketResult(env,"bucket",bucketCreated,bucketNamePtr,0,&blank[0]); 
}

  //  * function delete_bucketc deletes a bucket.
  //  * pre-requisites: open_projectc
  //  * inputs: Project (object) ,Bucket Name (string)
  //  * output: BucketResult (object)
napi_value delete_bucketc(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 2;
  napi_value args[2];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 2) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 2 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string type\n");
    return NULL;
  }

  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[1],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  char* bucketName =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[1],&bucketName[0],convertedvalue,&bufsize);
  assert(status == napi_ok);
  
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Project project_result;
  project_result._handle = getHandleValue(env,args[0]);
  if(project_result._handle==0){
    return NULL;
  }
  
	BucketResult bucket_Result = delete_bucket(&project_result, bucketName);
	  if(bucket_Result.bucket==NULL){
		
		if(bucket_Result.error!=NULL){
		  Error error_result = *(bucket_Result.error);
		  char* errorMessagePtr = error_result.message;
		  char blank[] = "";
		  return createBucketResult(env,"bucket",0,&blank[0],error_result.code,&errorMessagePtr[0]);

		}else{
		  char blank[] = "";
		  return createBucketResult(env,"bucket",-1,&blank[0],-1,&blank[0]);
		}
	  }
      Bucket bucket_result = *(bucket_Result.bucket);
      char* bucketNamePtr = bucket_result.name;
      int64_t bucketCreated = bucket_result.created;
      char blank[] = "";
      return createBucketResult(env,"bucket",bucketCreated,bucketNamePtr,0,&blank[0]); 
}

  //  * function list_bucketsc lists buckets
  //  * pre-requisites: open_projectc
  //  * inputs: Project (Object) ,ListBucketsOptions(Object)
  //  * output: BucketList (Object)
napi_value list_bucketsc(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 2;
  napi_value args[2],returnObject;

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 2) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 2 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if((checktypeofinput!=napi_object)&&(checktypeofinput!=napi_null)){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be object type or null\n");
    return NULL;
  }
  //
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }
  //
  Project project_result;
  project_result._handle = getHandleValue(env,args[0]);
  if(project_result._handle==0){
    return NULL;
  }
  ListBucketsOptions listBucketsOptions;
  //
  BucketIterator *bucket_resultIterator= nullptr;

	if(checktypeofinput==napi_null){
        bucket_resultIterator = list_buckets(&project_result,NULL);
	  }else if(checktypeofinput==napi_object){
		napi_value cursorNAPI;
		status = napi_get_named_property(env,args[1],"cursor",&cursorNAPI);
		assert(status == napi_ok);
		size_t bufsize = 0;
		size_t convertedvalue = 0;
		status = napi_get_value_string_utf8(env, cursorNAPI,NULL,bufsize,&convertedvalue);
		assert(status == napi_ok);
		convertedvalue=convertedvalue+1;

		char* cursor =  new char[convertedvalue];
		status = napi_get_value_string_utf8(env,cursorNAPI,cursor,convertedvalue,&bufsize);
		assert(status == napi_ok);
		listBucketsOptions.cursor = cursor;
		bucket_resultIterator = list_buckets(&project_result,&listBucketsOptions);

	  }

          
	napi_value BucketList,errorObject;
	
	  status = napi_create_object(env,&BucketList);
	  assert(status == napi_ok);

	  status = napi_create_object(env,&errorObject);
	  assert(status == napi_ok);

	  status = napi_create_object(env,&returnObject);
	  assert(status == napi_ok);

	  int count = 0;
	  while(bucket_iterator_next(bucket_resultIterator)){
		  Bucket *bucket_result = bucket_iterator_item(bucket_resultIterator);

		  napi_value BucketInfoObj;
		  status = napi_create_object(env,&BucketInfoObj);
		  assert(status == napi_ok);

		  char* bucketNamePtr = bucket_result->name;

		  napi_value bucketName;

		  status = napi_create_string_utf8(env,bucketNamePtr,NAPI_AUTO_LENGTH,&bucketName);
		  assert(status == napi_ok);

		  status = napi_set_named_property(env,BucketInfoObj,"name",bucketName);
		  assert(status == napi_ok);
		  
		  napi_value createdBucket;
		  int64_t bucketcreatedvalue = bucket_result->created;

		  status = napi_create_int64(env, bucketcreatedvalue, &createdBucket);
		  assert(status == napi_ok);

		  status = napi_set_named_property(env,BucketInfoObj,"created",createdBucket);
		  assert(status == napi_ok);

		  char str[100];
		  itoa(count, str, 10);

		  status = napi_set_named_property(env,BucketList,str,BucketInfoObj);
		  assert(status == napi_ok);

		  count++;
	  }

	  Error *err = bucket_iterator_err(bucket_resultIterator);

	  if(err==NULL){
		int32_t code = 0;
		char message[] = "";
		napi_value codenapi,errormessage;

		status = napi_create_int64(env, code, &codenapi);
		assert(status == napi_ok);

		status = napi_create_string_utf8(env,&message[0],NAPI_AUTO_LENGTH,&errormessage);
		assert(status == napi_ok);

		status = napi_set_named_property(env,errorObject,"error",codenapi);
		assert(status == napi_ok);

		status = napi_set_named_property(env,errorObject,"message",errormessage);
		assert(status == napi_ok);
	  }else{
		
		Error errorResult = *(err);
		char* messagePtr = errorResult.message;
		napi_value codenapi,errormessage;
		
		status = napi_create_int64(env, errorResult.code, &codenapi);
		assert(status == napi_ok);
	  
		status = napi_create_string_utf8(env,&messagePtr[0],NAPI_AUTO_LENGTH,&errormessage);
		assert(status == napi_ok);

		status = napi_set_named_property(env,errorObject,"error",codenapi);
		assert(status == napi_ok);

		status = napi_set_named_property(env,errorObject,"message",errormessage);
		assert(status == napi_ok);
	  }
	  status = napi_set_named_property(env,returnObject,"error",errorObject);
	  assert(status == napi_ok);
	  //
	  status = napi_set_named_property(env,returnObject,"bucketList",BucketList);
	  assert(status == napi_ok);
	  return returnObject;
}

  //  * function stat_objectc information about an object at the specific key.
  //  * pre-requisites: open_projectc
  //  * inputs: Project (Object) ,Bucket Name (String) , Object Key(String)
  //  * output: ObjectResult (Object)
napi_value stat_objectc(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 3;
  napi_value args[3];
  //
  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  //
  if (argc < 3) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 2 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string type\n");
    return NULL;
  }
  //
  status = napi_typeof(env,args[2],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Third argument excepted to be string type\n");
    return NULL;
  }

  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Project project_result;
  project_result._handle = getHandleValue(env,args[0]);
  if(project_result._handle==0){
    return NULL;
  }

  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[1],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* bucketName =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[1],bucketName,convertedvalue,&bufsize);
  assert(status == napi_ok);
  bufsize = 0;
  convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[2],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* objectKey =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[2],objectKey,convertedvalue,&bufsize);
  assert(status == napi_ok);
  

  ObjectResult object_result = stat_object(&project_result,bucketName,objectKey);
	if(object_result.error!=NULL){
	  return createObjectResult(env,NULL,object_result.error);
	}else{
	  return createObjectResult(env,object_result.object,NULL);
	}
}

  //  * function upload_objectc starts an upload to the specified key.
  //  * pre-requisites: open_projectc
  //  * inputs: Project (Object) ,Bucket Name (String) , Object Key(String) , Uplaod Options(Object)
  //  * output: UploadResult  (Object)
napi_value upload_objectc(napi_env env, napi_callback_info info) {
  
  napi_status status;
  size_t argc = 4;
  napi_value args[4],returnObject;
  //
  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  //
  if (argc < 4) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 4 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string type\n");
    return NULL;
  }

  status = napi_typeof(env,args[2],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Third argument excepted to be string type\n");
    return NULL;
  }
  
  status = napi_typeof(env,args[3],&checktypeofinput);
  assert(status == napi_ok);

  if((checktypeofinput!=napi_object)&&(checktypeofinput!=napi_null)){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Fourth argument excepted to be object type or null\n");
    return NULL;
  }
  
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Project project_result;
  project_result._handle = getHandleValue(env,args[0]);
  if(project_result._handle==0){
    return NULL;
  }
  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[1],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* bucketName =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[1],bucketName,convertedvalue,&bufsize);
  assert(status == napi_ok);
  bufsize = 0;
  convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[2],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* objectKey =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[2],objectKey,convertedvalue,&bufsize);
  assert(status == napi_ok);

  UploadResult upload_result;
      if(checktypeofinput==napi_null){
        upload_result = upload_object(&project_result,&bucketName[0],&objectKey[0],NULL);
      }else{
        
        UploadOptions uploadOptions;
        napi_value expiresNAPI;

        status = napi_get_named_property(env,args[3],"expires",&expiresNAPI);
        assert(status == napi_ok);

        int64_t expires;
        status = napi_get_value_int64(env,expiresNAPI,&expires);
        assert(status == napi_ok);
        uploadOptions.expires = expires;

        upload_result = upload_object(&project_result,bucketName,objectKey,&uploadOptions);
      }
      if(upload_result.upload==NULL){
        
        if(upload_result.error!=NULL){

          Error error_result = *(upload_result.error);
          char* errorMessagePtr = error_result.message;
          return createResult(env,"upload",0,error_result.code,&errorMessagePtr[0]);
        }else{
          char blank[] = "";
          return createResult(env,"upload",0,-1,&blank[0]);
        }
      }
      
      Upload upload = *(upload_result.upload);
      
      size_t handlevalue = upload._handle;
      
      char blank[] = "";
      return createResult(env,"upload",handlevalue,0,&blank[0]);
}


  //  * function upload_writec uploads len(p) bytes from p to the object's data stream.
  //  * pre-requisites: upload_objectc
  //  * inputs: Upload (Object) ,Pointer(Buffer) , Length (size_t)
  //  * output: WriteResult (Object)
napi_value upload_writec(napi_env env, napi_callback_info info) {
  napi_status status;
  
  size_t argc = 3;
  napi_value args[3];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  
  if (argc < 3) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 3 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;

  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);
  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! first argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);
  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! second argument excepted to be object/buffer type\n");
    return NULL;
  }

  status = napi_typeof(env,args[2],&checktypeofinput);
  assert(status == napi_ok);
  if(checktypeofinput!=napi_number){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! third argument excepted to be number type\n");
    return NULL;
  }
  
  char *_err = "";
  char **err = &_err;
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Upload upload_resultRef;
  upload_resultRef._handle = getHandleValue(env,args[0]);
  if(upload_resultRef._handle==0){
    return NULL;
  }
  bool buffertype1;
  status = napi_is_arraybuffer(env, args[1], &buffertype1);
  assert(status == napi_ok);
  size_t lengthOfBuffer;  

  void* bufferPtr = NULL;  

  status = napi_get_buffer_info(env, args[1], &bufferPtr,&lengthOfBuffer);
  assert(status == napi_ok);

  int64_t bytes64;
  status = napi_get_value_int64(env,args[2],&bytes64);
  assert(status == napi_ok);

  size_t bytesread = (size_t)bytes64;

  uint8_t *ptrToData ; 
  ptrToData = (uint8_t *)bufferPtr;

  WriteResult write_result = upload_write(&upload_resultRef,ptrToData,bytesread);
      napi_value returnObject,errorObject;
      status = napi_create_object(env,&returnObject);
      if(write_result.error!=NULL){
          Error error_result = *(write_result.error);
          char* errorMessagePtr = error_result.message;
          size_t datauploaded = 0;
          napi_value datauploadedObject;
          status = napi_create_object(env,&errorObject);
          assert(status == napi_ok);
          status = napi_create_int32(env,datauploaded,&datauploadedObject);
          assert(status == napi_ok);
          napi_value errorObject,errorCode,errorMessage;

          status = napi_set_named_property(env,returnObject,"bytes_written",datauploadedObject);
          assert(status == napi_ok);

          status = napi_create_int64(env, error_result.code, &errorCode);
          assert(status == napi_ok);

          status = napi_create_string_utf8(env,&errorMessagePtr[0],NAPI_AUTO_LENGTH,&errorMessage);
          assert(status == napi_ok);
          status = napi_set_named_property(env,errorObject,"code",errorCode);
          assert(status == napi_ok);
          
          status = napi_set_named_property(env,errorObject,"message",errorMessage);
          assert(status == napi_ok);

          status = napi_set_named_property(env,returnObject,"error",errorObject);
          assert(status == napi_ok);

          return returnObject;
      }else{
          char blank[] = "";
          napi_value errorObject,errorMessage;
          napi_value errorCode;
          status = napi_create_object(env,&errorObject);
          assert(status == napi_ok);

          size_t datauploaded = write_result.bytes_written;
          napi_value datauploadedObject;
          status = napi_create_int32(env,datauploaded,&datauploadedObject);
          assert(status == napi_ok);
          //
          status = napi_set_named_property(env,returnObject,"bytes_written",datauploadedObject);
          assert(status == napi_ok);

          status = napi_create_int64(env, 0, &errorCode);
          assert(status == napi_ok);

          status = napi_create_string_utf8(env,&blank[0],NAPI_AUTO_LENGTH,&errorMessage);
          assert(status == napi_ok);
          status = napi_set_named_property(env,errorObject,"code",errorCode);
          assert(status == napi_ok);
          
          status = napi_set_named_property(env,errorObject,"message",errorMessage);
          assert(status == napi_ok);

          status = napi_set_named_property(env,returnObject,"error",errorObject);
          assert(status == napi_ok);
          return returnObject;
      }
}

  //  * function download_objectc starts  download to the specified key.
  //  * pre-requisites: open_projectc
  //  * inputs: Project (Object), Buxcket Name(String), Object Key(String), Download Options(Object)
  //  * output: DownloadResult (Object)
napi_value download_objectc(napi_env env, napi_callback_info info) {
  
  napi_status status;
  size_t argc = 4;
  napi_value args[4];
  //
  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  //
  if (argc < 4) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 2 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string type\n");
    return NULL;
  }
  //
  status = napi_typeof(env,args[2],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Third argument excepted to be string type\n");
    return NULL;
  }
  //
  status = napi_typeof(env,args[3],&checktypeofinput);
  assert(status == napi_ok);

  if((checktypeofinput!=napi_object)&&(checktypeofinput!=napi_null)){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Fourth argument excepted to be object type or null\n");
    return NULL;
  }
  
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Project project_result;
  project_result._handle = getHandleValue(env,args[0]);
  if(project_result._handle==0){
    return NULL;
  }
  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[1],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* bucketName =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[1],bucketName,convertedvalue,&bufsize);
  assert(status == napi_ok);
  bufsize = 0;
  convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[2],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* objectKey =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[2],objectKey,convertedvalue,&bufsize);
  assert(status == napi_ok);
  DownloadResult download_result;
      if(checktypeofinput==napi_null){
        download_result = download_object(&project_result,bucketName,objectKey,NULL);
      }else{
        DownloadOptions downloadOption;
        napi_value offsetNAPI,lengthNAPI;

        status = napi_get_named_property(env,args[3],"offset",&offsetNAPI);
        assert(status == napi_ok);
        
        status = napi_get_named_property(env,args[3],"length",&lengthNAPI);
        assert(status == napi_ok);
        
        int64_t offset,length;
        status = napi_get_value_int64(env,offsetNAPI,&offset);
        assert(status == napi_ok);
        status = napi_get_value_int64(env,lengthNAPI,&length);
        assert(status == napi_ok);

        downloadOption.offset = offset;
        downloadOption.length = length;
        download_result = download_object(&project_result,&bucketName[0],&objectKey[0],&downloadOption);
      }
      
      napi_value returnObject;
      
      status = napi_create_object(env,&returnObject);
      
      if(download_result.download==NULL){
        if(download_result.error!=NULL){
          Error error_result = *(download_result.error);
          char* errorMessagePtr = error_result.message;
          return createResult(env,"upload",0,error_result.code,&errorMessagePtr[0]);
        }else{
          char blank[] = "";
          return createResult(env,"upload",0,-1,&blank[0]);
        }
      }
      Download download = *(download_result.download);
      size_t handlevalue = download._handle;
      char blank[] = "";
      return createResult(env,"download",handlevalue,0,&blank[0]);

}

  //  * function upload_commitc commits the uploaded data.
  //  * pre-requisites: upload_objectc
  //  * inputs: Upload (Object)
  //  * output: Error (Object)
napi_value upload_commitc(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 1 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Upload upload_result;
  upload_result._handle = getHandleValue(env,args[0]);
  if(upload_result._handle==0){
    return NULL;
  }
   Error* error_result = upload_commit(&upload_result);
      if(error_result!=NULL){
        Error errorResult = *(error_result);
        char* errorMessagePtr = errorResult.message;
        return createError(env,errorResult.code,&errorMessagePtr[0]);
      }
      char blank[] = "";
      return createError(env,0,&blank[0]);
}

//  * function close_downloadc closes the download.
//  * pre-requisites: download_objectc
//  * inputs: Download (Object)
//  * output: Error (Object)
napi_value close_downloadc(napi_env env, napi_callback_info info) {
  
  napi_status status;
  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 1 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }
  
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Download download_result;
  download_result._handle = getHandleValue(env,args[0]);
      Error* error_result = close_download(&download_result);
  //
      if(error_result!=NULL){
        
        Error errorResult = *(error_result);
        char* errorMessagePtr = errorResult.message;
        return createError(env,errorResult.code,&errorMessagePtr[0]);
      }

      char blank[] = "";
      return createError(env,0,&blank[0]);
}

// * function download_readc downloads from object's data stream into bytes up to length amount.
// * pre-requisites: download_objectc
// * inputs: Download (Object), Pointer (Buffer), Length(size_t) 
// * output: ReadResult  (Object)
napi_value download_readc(napi_env env, napi_callback_info info) {
  napi_status status;
  //
  size_t argc = 3;
  napi_value args[3];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  //
  if (argc < 3) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 3 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;

  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);
  //
  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! first argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);
  //
  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! second argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[2],&checktypeofinput);
  assert(status == napi_ok);
  //
  if(checktypeofinput!=napi_number){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! third argument excepted to be number type\n");
    return NULL;
  }

  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Download download_resulterRef;
  download_resulterRef._handle = getHandleValue(env,args[0]);
  
  void* bufferPtr = NULL;  
  size_t lengthOfBuffer;  
  status = napi_get_buffer_info(env, args[1], &bufferPtr,&lengthOfBuffer);
  assert(status == napi_ok);
  //
  uint8_t *ptrToData ; 
  ptrToData = (uint8_t *)bufferPtr;
  //
  ReadResult read_result = download_read(&download_resulterRef,ptrToData,lengthOfBuffer);
        //
        napi_value returnObject;
        //
        status = napi_create_object(env,&returnObject);
        //
        if(read_result.error!=NULL){
          
            napi_value errorObject,errorCode,errorMessage;
            Error error_result = *(read_result.error);
            char* errorMessagePtr = error_result.message;
            
            size_t datadownload = 0;
            napi_value dataDownloadObject;
            status = napi_create_object(env,&errorObject);
            assert(status == napi_ok);
            
            status = napi_create_int32(env,datadownload,&dataDownloadObject);
            assert(status == napi_ok);
            
            status = napi_set_named_property(env,returnObject,"bytes_read",dataDownloadObject);
            assert(status == napi_ok);
            
            error_result.code = 0;
            
            status = napi_create_int64(env, error_result.code, &errorCode);
            assert(status == napi_ok);
            
            if(errorMessagePtr!=NULL){
              status = napi_create_string_utf8(env,errorMessagePtr,NAPI_AUTO_LENGTH,&errorMessage);
              assert(status == napi_ok);
            
            }else{
              char blank[] = "";

              status = napi_create_string_utf8(env,&blank[0],NAPI_AUTO_LENGTH,&errorMessage);
              assert(status == napi_ok);
            }
            status = napi_set_named_property(env,errorObject,"code",errorCode);
            assert(status == napi_ok);
            
            status = napi_set_named_property(env,errorObject,"message",errorMessage);
            assert(status == napi_ok);
            
            status = napi_set_named_property(env,returnObject,"error",errorObject);
            assert(status == napi_ok);

            return returnObject;
        }else{
            
            char blank[] = "";
            napi_value errorObject,errorMessage;
            napi_value errorCode;
            status = napi_create_object(env,&errorObject);
            assert(status == napi_ok);
            
            size_t datauploaded = read_result.bytes_read;
            napi_value dataDownloadObject;
            status = napi_create_int32(env,datauploaded,&dataDownloadObject);
            assert(status == napi_ok);
            
            status = napi_set_named_property(env,returnObject,"bytes_read",dataDownloadObject);
            assert(status == napi_ok);

            status = napi_create_int64(env, 0, &errorCode);
            assert(status == napi_ok);
            
            status = napi_create_string_utf8(env,&blank[0],NAPI_AUTO_LENGTH,&errorMessage);
            assert(status == napi_ok);
            
            status = napi_set_named_property(env,errorObject,"code",errorCode);
            assert(status == napi_ok);
            
            status = napi_set_named_property(env,errorObject,"message",errorMessage);
            assert(status == napi_ok);

            status = napi_set_named_property(env,returnObject,"error",errorObject);
            assert(status == napi_ok);
            return returnObject;
        }
}

//  * function delete_objectc deletes an object.
//  * pre-requisites: open_projectc
//  * inputs: Project (Object), Bucket Name (String), Object Key (String) 
//  * output: ObjectResult  (Object)
napi_value delete_objectc(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 3;
  napi_value args[3];
  //
  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  //
  if (argc < 3) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 3 arguments\n");
    return NULL;
  }
  
  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }
  
  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string type\n");
    return NULL;
  }
  
  status = napi_typeof(env,args[2],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Third argument excepted to be string type\n");
    return NULL;
  }

  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Project project_result;
  project_result._handle = getHandleValue(env,args[0]);
  if(project_result._handle==0){
    return NULL;
  }
  
  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[1],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* bucketName =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[1],bucketName,convertedvalue,&bufsize);
  assert(status == napi_ok);
  bufsize = 0;
  convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[2],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* objectKey =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[2],objectKey,convertedvalue,&bufsize);
  assert(status == napi_ok);
  
  ObjectResult object_result = delete_object(&project_result,bucketName,objectKey);
  if(object_result.error!=NULL){
	  return createObjectResult(env,NULL,object_result.error);
  }else{
	 return createObjectResult(env,object_result.object,NULL);
  }
}

//  * function access_sharec creates new access grant with specific permission. Permission will be applied to prefixes when defined.
//  * pre-requisites: parse_accessc
//  * inputs: Access (Object), Permission (Object), Share Prefix (Object), prefix count (int)
//  * output: String Result (Object)
napi_value access_sharec(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 4;
  napi_value args[4];
  //
  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  //
  if (argc < 4) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 4 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput1,checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput1);
  assert(status == napi_ok);

  if((checktypeofinput1!=napi_object)&&(checktypeofinput1!=napi_null)){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }
  
  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string type\n");
    return NULL;
  }
  //
  status = napi_typeof(env,args[2],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Third argument excepted to be string type\n");
    return NULL;
  }
  
  status = napi_typeof(env,args[3],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_number){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Fourth argument excepted to be string type\n");
    return NULL;
  }

  Access access;
  if(checktypeofinput1!=napi_null){
    bool propertyexists = false;
    napi_value ObjectkeyNAPI;
    char* handle = "_handle";
    status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
    assert(status == napi_ok);

    status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
    assert(status == napi_ok);
    if(!propertyexists){
      napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
      return NULL;
    }
    access._handle = getHandleValue(env,args[0]);
    if(access._handle==0){
      napi_throw_type_error(env,nullptr,"\nInvalid Handle\n");
      return NULL;
    }
  }
  
  Permission permission;
  
  napi_value allow_downloadNAPI;
  status = napi_get_named_property(env,args[1],"allow_download",&allow_downloadNAPI);
  assert(status == napi_ok);
  
  bool allow_downloadc;
  status = napi_get_value_bool(env,allow_downloadNAPI,&allow_downloadc);
  assert(status == napi_ok);
  
  permission.allow_download = allow_downloadc;

  napi_value allow_uploadNAPI;
  status = napi_get_named_property(env,args[1],"allow_upload",&allow_uploadNAPI);
  assert(status == napi_ok);
  
  bool allow_uploadc;
  status = napi_get_value_bool(env,allow_uploadNAPI,&allow_uploadc);
  assert(status == napi_ok);
  
  permission.allow_upload = allow_uploadc;
  
  napi_value allow_listNAPI;
  status = napi_get_named_property(env,args[1],"allow_list",&allow_listNAPI);
  assert(status == napi_ok);
  //
  bool allow_listc;
  status = napi_get_value_bool(env,allow_listNAPI,&allow_listc);
  assert(status == napi_ok);
  
  permission.allow_list = allow_listc;
  
  napi_value allow_deleteNAPI;
  status = napi_get_named_property(env,args[1],"allow_delete",&allow_deleteNAPI);
  assert(status == napi_ok);
  //
  bool allow_deletec;
  status = napi_get_value_bool(env,allow_deleteNAPI,&allow_deletec);
  assert(status == napi_ok);
  //
  permission.allow_delete = allow_deletec;

  napi_value not_beforeNAPI;
  status = napi_get_named_property(env,args[1],"not_before",&not_beforeNAPI);
  assert(status == napi_ok);

  int64_t not_beforec;
  status = napi_get_value_int64(env,not_beforeNAPI,&not_beforec);
  assert(status == napi_ok);
  
  permission.not_before = not_beforec;
  //
  napi_value not_afterNAPI;
  status = napi_get_named_property(env,args[1],"not_after",&not_afterNAPI);
  assert(status == napi_ok);

  int64_t not_afterc;
  status = napi_get_value_int64(env,not_afterNAPI,&not_afterc);
  assert(status == napi_ok);
  
  permission.not_after = not_afterc;
  //
  int64_t sharePrefixSize;
  status = napi_get_value_int64(env,args[3],&sharePrefixSize);
  assert(status == napi_ok);

  //Checking whether is array of not
  bool isarray=false;
  status = napi_is_array(env,args[2],&isarray);
  assert(status == napi_ok);
  if(!isarray){
    napi_throw_type_error(env,nullptr,"\nWrong data type of 3 parameter \n");
    return NULL;
  }
  
  uint32_t sizeOfArray = 0;
  status = napi_get_array_length(env,args[2],&sizeOfArray);
  assert(status == napi_ok);
  
  int sizeOfArrayInt = (int)sizeOfArray;
  int size; 
  if(sizeOfArray==sharePrefixSize){
    size = (int)sharePrefixSize;
  }else{
    size = sizeOfArrayInt;
  }
  SharePrefix* SharePrefixListPointer = new SharePrefix[size];
  
  napi_value SharePrefixObject;
  
  for(uint32_t i=0;i<(uint32_t)sizeOfArrayInt;i++){
    
    SharePrefix sharePrefix;
    status = napi_get_element(env,args[2],i,&SharePrefixObject);
    assert(status == napi_ok);

    napi_value bucket;
    status = napi_get_named_property(env,SharePrefixObject,"bucket",&bucket);
    assert(status == napi_ok);
    //
    size_t bufsize = 0;
    size_t convertedvalue = 0;
    status = napi_get_value_string_utf8(env, bucket,NULL,bufsize,&convertedvalue);
    assert(status == napi_ok);
    convertedvalue = convertedvalue+1;

    char* bucketc =  new char[convertedvalue];
    status = napi_get_value_string_utf8(env,bucket,bucketc,convertedvalue,&bufsize);
    assert(status == napi_ok);
    napi_value prefix;
    status = napi_get_named_property(env,SharePrefixObject,"prefix",&prefix);
    assert(status == napi_ok);
    //
    bufsize = 0;
    convertedvalue = 0;
    status = napi_get_value_string_utf8(env, prefix,NULL,bufsize,&convertedvalue);
    assert(status == napi_ok);
    convertedvalue = convertedvalue+1;

    char* prefixc =  new char[convertedvalue];
    status = napi_get_value_string_utf8(env,prefix,prefixc,convertedvalue,&bufsize);
    assert(status == napi_ok);
    sharePrefix.bucket = bucketc;
    sharePrefix.prefix = prefixc;
    *(SharePrefixListPointer+i)=sharePrefix;
  }


      AccessResult access_Result = access_share(&access,permission,SharePrefixListPointer,sharePrefixSize);

      if(access_Result.access==NULL){
        
        if(access_Result.error!=NULL){
          Error error_result = *(access_Result.error);
          char* errorMessagePtr = error_result.message;
          return createResult(env,"access",0,error_result.code,&errorMessagePtr[0]);
        }else{
          char blank[] = "";
          return createResult(env,"access",0,-1,&blank[0]);
        }
      }
      access = *(access_Result.access);
      size_t handlevalue = access._handle;
      
      char blank[] = "";
      return createResult(env,"access",handlevalue,0,&blank[0]);
}




//  * function list_objectsc lists objects
//  * pre-requisites: open_projectc
//  * inputs: Project (Object), Bucket Name(String), ListObjectsOptions(Object) 
//  * output: ObjectList (Object)
napi_value list_objectsc(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 3;
  napi_value args[3],returnObject;

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 3) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 2 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_string){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be string\n");
    return NULL;
  }
  //
  status = napi_typeof(env,args[2],&checktypeofinput);
  assert(status == napi_ok);

  if((checktypeofinput!=napi_object)&&(checktypeofinput!=napi_null)){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! Second argument excepted to be object or NULL\n");
    return NULL;
  }
  //
  size_t bufsize = 0;
  size_t convertedvalue = 0;
  status = napi_get_value_string_utf8(env, args[1],NULL,bufsize,&convertedvalue);
  assert(status == napi_ok);
  convertedvalue = convertedvalue+1;
  
  char* bucketName =  new char[convertedvalue];
  status = napi_get_value_string_utf8(env,args[1],bucketName,convertedvalue,&bufsize);
  assert(status == napi_ok);
  
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Project project_result;
  project_result._handle = getHandleValue(env,args[0]);
  if(project_result._handle==0){
    return NULL;
  }
  //
  ListObjectsOptions listObjectsOptions;
  //
  ObjectIterator *objectIterator= nullptr;
  if(checktypeofinput==napi_null){
        objectIterator = list_objects(&project_result,&bucketName[0],NULL);

      }else if(checktypeofinput==napi_object){
        //
        napi_value cursorNAPI;
        status = napi_get_named_property(env,args[2],"cursor",&cursorNAPI);
        assert(status == napi_ok);
        //
        napi_value prefixNAPI;
        status = napi_get_named_property(env,args[2],"prefix",&prefixNAPI);
        assert(status == napi_ok);
        //
        size_t bufsize = 0;
        size_t convertedvalue = 0;
        status = napi_get_value_string_utf8(env, cursorNAPI,NULL,bufsize,&convertedvalue);
        assert(status == napi_ok);
        convertedvalue=convertedvalue+1;
        //char cursor[convertedvalue];
        char* cursor =  new char[convertedvalue];
        status = napi_get_value_string_utf8(env,cursorNAPI,cursor,convertedvalue,&bufsize);
        assert(status == napi_ok);
        listObjectsOptions.cursor = cursor;

        bufsize = 0;
        convertedvalue = 0;
        //
        status = napi_get_value_string_utf8(env, prefixNAPI,NULL,bufsize,&convertedvalue);
        assert(status == napi_ok);
        convertedvalue=convertedvalue+1;
        
        char* prefix =  new char[convertedvalue];
        status = napi_get_value_string_utf8(env,prefixNAPI,prefix,convertedvalue,&bufsize);
        assert(status == napi_ok);
        listObjectsOptions.prefix = prefix;
        
        napi_value recursiveNAPI;
        status = napi_get_named_property(env,args[2],"recursive",&recursiveNAPI);
        assert(status == napi_ok);
        //
        napi_value systemNAPI;
        status = napi_get_named_property(env,args[2],"system",&systemNAPI);
        assert(status == napi_ok);
        //
        napi_value customNAPI;
        status = napi_get_named_property(env,args[2],"custom",&customNAPI);
        assert(status == napi_ok);
        //
        bool recursive;
        napi_get_value_bool(env,recursiveNAPI,&recursive);
        assert(status == napi_ok);
        //
        bool system;
        napi_get_value_bool(env,systemNAPI,&system);
        assert(status == napi_ok);
        //
        bool custom;
        napi_get_value_bool(env,customNAPI,&custom);
        assert(status == napi_ok);
        //
        listObjectsOptions.custom = custom;
        listObjectsOptions.system = system;
        listObjectsOptions.recursive = recursive;
        
        objectIterator = list_objects(&project_result,bucketName,&listObjectsOptions);
        
      }
      
      napi_value objectList,errorObject;
      status = napi_create_object(env,&objectList);
      assert(status == napi_ok);

      status = napi_create_object(env,&errorObject);
      assert(status == napi_ok);

      status = napi_create_object(env,&returnObject);
      assert(status == napi_ok);
      int count = 0;
    
          
              while(object_iterator_next(objectIterator)){

                Object *objectPtr = object_iterator_item(objectIterator);

                Object object_result = *(objectPtr);

                napi_value objectNAPI,SystemMetadataNAPI,CustomMetadataNAPI,CustomMetadataEntryNAPI;

                status = napi_create_object(env,&objectNAPI);
                assert(status == napi_ok);

                status = napi_create_object(env,&SystemMetadataNAPI);
                assert(status == napi_ok);

                status = napi_create_object(env,&CustomMetadataNAPI);
                assert(status == napi_ok);

                status = napi_create_object(env,&CustomMetadataEntryNAPI);
                assert(status == napi_ok);
                
                napi_value keyObjectNAPI;
                status = napi_create_string_utf8(env,objectPtr->key,NAPI_AUTO_LENGTH,&keyObjectNAPI);
                assert(status == napi_ok);
                napi_value is_prefixNAPI;
                //
                if(objectPtr->is_prefix){
                  status = napi_create_int64(env, 1, &is_prefixNAPI);
                  assert(status == napi_ok);
                }else{
                  status = napi_create_int64(env, 0, &is_prefixNAPI);
                  assert(status == napi_ok);
                }

                SystemMetadata systemMetadata;
                CustomMetadata customMetadata;
                CustomMetadataEntry customMetadataEntry;
                systemMetadata = object_result.system;
                customMetadata = object_result.custom;
                napi_value entriesArray;
                status = napi_create_object(env,&entriesArray);
                assert(status == napi_ok);
                char empty[]="";
                if(customMetadata.count>0){
                  CustomMetadataEntry* CustomMetadataEntryListPointer = customMetadata.entries;
                  for(int j=0;j<customMetadata.count;j++){
                    customMetadataEntry = *(CustomMetadataEntryListPointer+j);
                    napi_value key_lengthNAPI,value_lengthNAPI,keyNAPI,valueNAPI,customMetaNAPI;
                    status = napi_create_object(env,&customMetaNAPI);
                    assert(status == napi_ok);

                    status = napi_create_int64(env, customMetadataEntry.key_length, &key_lengthNAPI);
                    assert(status == napi_ok);
                
                    status = napi_create_int64(env, customMetadataEntry.value_length, &value_lengthNAPI);
                    assert(status == napi_ok);

                    status = napi_create_string_utf8(env,customMetadataEntry.key,NAPI_AUTO_LENGTH,&keyNAPI);
                    assert(status == napi_ok);
                    
                    status = napi_create_string_utf8(env,customMetadataEntry.value,NAPI_AUTO_LENGTH,&valueNAPI);
                    assert(status == napi_ok);
                    
                    status = napi_set_named_property(env,customMetaNAPI,"key",keyNAPI);
                    assert(status == napi_ok);

                    status = napi_set_named_property(env,customMetaNAPI,"key_length",key_lengthNAPI);
                    assert(status == napi_ok);

                    status = napi_set_named_property(env,customMetaNAPI,"value",valueNAPI);
                    assert(status == napi_ok);

                    status = napi_set_named_property(env,customMetaNAPI,"value_length",value_lengthNAPI);
                    assert(status == napi_ok);

                    status = napi_set_named_property(env,customMetaNAPI,"value_length",value_lengthNAPI);
                    assert(status == napi_ok);

                    char str[100];
                    itoa(j, str, 10);
                    status = napi_set_named_property(env,entriesArray,str,customMetaNAPI);
                    assert(status == napi_ok);
                  }
                }else{
                  napi_value emptyNAPI;
                  status = napi_create_string_utf8(env,&empty[0],NAPI_AUTO_LENGTH,&emptyNAPI);
                  assert(status == napi_ok);

                  status = napi_set_named_property(env,entriesArray,"0",emptyNAPI);
                  assert(status == napi_ok);
                }
                napi_value createdNAPI;
                status = napi_create_int64(env, objectPtr->system.created, &createdNAPI);
                assert(status == napi_ok);
                
                napi_value expiresNAPI;
                status = napi_create_int64(env, objectPtr->system.expires, &expiresNAPI);
                assert(status == napi_ok);

                napi_value countNAPI;
                status = napi_create_int64(env, customMetadata.count, &countNAPI);
                assert(status == napi_ok);
                //
                napi_value content_lengthNAPI;
                status = napi_create_int64(env, systemMetadata.content_length, &content_lengthNAPI);
                assert(status == napi_ok);

                status = napi_set_named_property(env,CustomMetadataNAPI,"entries",entriesArray);
                assert(status == napi_ok);
                
                status = napi_set_named_property(env,CustomMetadataNAPI,"count",countNAPI);
                assert(status == napi_ok);
                
                status = napi_set_named_property(env,objectNAPI,"custom",CustomMetadataNAPI);
                assert(status == napi_ok);
                
                status = napi_set_named_property(env,SystemMetadataNAPI,"created",createdNAPI);
                assert(status == napi_ok);
                
                status = napi_set_named_property(env,SystemMetadataNAPI,"expires",expiresNAPI);
                assert(status == napi_ok);
                
                status = napi_set_named_property(env,SystemMetadataNAPI,"content_length",content_lengthNAPI);
                assert(status == napi_ok);
                
                status = napi_set_named_property(env,objectNAPI,"system",SystemMetadataNAPI);
                assert(status == napi_ok);
                
                status = napi_set_named_property(env,objectNAPI,"is_prefix",is_prefixNAPI);
                assert(status == napi_ok);
                
                status = napi_set_named_property(env,objectNAPI,"key",keyObjectNAPI);
                assert(status == napi_ok);
                //
                char str[100];
                itoa(count, str, 10);

                status = napi_set_named_property(env,objectList,str,objectNAPI);
                assert(status == napi_ok);

                count++;
            }
            //
            Error *err = object_iterator_err(objectIterator);
            
            napi_value errorMessage,errorCode;
            status = napi_create_object(env,&errorObject);
            assert(status == napi_ok);
            
            status = napi_create_object(env,&returnObject);
            assert(status == napi_ok);
            if(err==NULL){
              int32_t code = 0;
              char message[] = "";
              napi_value codenapi,errormessage;
              //
              status = napi_create_int64(env, code, &codenapi);
              assert(status == napi_ok);
              //
              status = napi_create_string_utf8(env,&message[0],NAPI_AUTO_LENGTH,&errormessage);
              assert(status == napi_ok);
              //
              status = napi_set_named_property(env,errorObject,"code",codenapi);
              assert(status == napi_ok);
              //
              status = napi_set_named_property(env,errorObject,"message",errormessage);
              assert(status == napi_ok);
            }else{
                Error errorResult = *(err);
                char* messagePtr = errorResult.message;
                napi_value codenapi,errormessage;
                
                status = napi_create_int64(env, errorResult.code, &codenapi);
                assert(status == napi_ok);
              
                status = napi_create_string_utf8(env,&messagePtr[0],NAPI_AUTO_LENGTH,&errormessage);
                assert(status == napi_ok);

                status = napi_set_named_property(env,errorObject,"code",codenapi);
                assert(status == napi_ok);

                status = napi_set_named_property(env,errorObject,"message",errormessage);
                assert(status == napi_ok);
            }
            status = napi_set_named_property(env,returnObject,"error",errorObject);
            assert(status == napi_ok);
            //
            status = napi_set_named_property(env,returnObject,"objectList",objectList);
            assert(status == napi_ok);

            return returnObject;
}

//  * function access_serializec serializes access grant into a string.
//  * pre-requisites: parse_accessc
//  * inputs: Access (Object)  
//  * output: String Result (object)
napi_value access_serializec(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 1;
  napi_value args[1];
  //
  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  //
  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 3 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Access access;
  access._handle = access._handle = getHandleValue(env,args[0]);
  if(access._handle==0){
    return NULL;
  }
  StringResult string_result = access_serialize(&access);
      //
      napi_value errorObject,returnObject;
      
      status = napi_create_object(env,&returnObject);
      assert(status == napi_ok);
      
      status = napi_create_object(env,&errorObject);
      assert(status == napi_ok);
      //
      char blank[] = "";

      napi_value errorCode,errorMessage,stringNAPI;
      if(string_result.error!=NULL){
        Error error_result = *(string_result.error);

        status = napi_create_int64(env, error_result.code, &errorCode);
        assert(status == napi_ok);

        char* errorMessagePtr = error_result.message;

        status = napi_create_string_utf8(env,&errorMessagePtr[0],NAPI_AUTO_LENGTH,&errorMessage);
        assert(status == napi_ok);

        status = napi_create_string_utf8(env,&blank[0],NAPI_AUTO_LENGTH,&stringNAPI);
        assert(status == napi_ok); 
      }else{
        
        char* StringResultPtr= string_result.string;
        status = napi_create_int64(env, 0, &errorCode);
        assert(status == napi_ok);
      
        
        status = napi_create_string_utf8(env,&blank[0],NAPI_AUTO_LENGTH,&errorMessage);
        assert(status == napi_ok);
        
        
        status = napi_create_string_utf8(env,StringResultPtr,NAPI_AUTO_LENGTH,&stringNAPI);
        assert(status == napi_ok); 
      }
        
      status = napi_set_named_property(env,errorObject,"code",errorCode);
      assert(status == napi_ok);
      
      status = napi_set_named_property(env,errorObject,"message",errorMessage);
      assert(status == napi_ok);
       
      status = napi_set_named_property(env,returnObject,"error",errorObject);
      assert(status == napi_ok);

      status = napi_set_named_property(env,returnObject,"string",stringNAPI);
      assert(status == napi_ok);
      return returnObject;
}


//  * function upload_infoc returns the last information about the uploaded object.
//  * pre-requisites: upload_objectc
//  * inputs: Upload (Object)  
//  * output: Object Result (object)
napi_value upload_infoc(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  
  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 1 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;

  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);
  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! first argument excepted to be object type\n");
    return NULL;
  }
  
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Upload upload_result;
  upload_result._handle = getHandleValue(env,args[0]);
  if(upload_result._handle==0){
    napi_throw_type_error(env,nullptr,"\nInvalid Object\n");
    return NULL;
  }
  ObjectResult object_result = upload_info(&upload_result);
      if(object_result.error!=NULL){
        return createObjectResult(env,NULL,object_result.error);
      }else{
        return createObjectResult(env,object_result.object,NULL);
      }
}

//  * function upload_abortc aborts an upload.
//  * pre-requisites: upload_objectc
//  * inputs: Upload (Object)  
//  * output: Error (Object)
napi_value upload_abortc(napi_env env, napi_callback_info info){
  napi_status status;

  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);

  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 1 arguments\n");
    return NULL;
  }

  
  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);

  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! First argument excepted to be object type\n");
    return NULL;
  }

  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }
  Upload upload_result;
  upload_result._handle = getHandleValue(env,args[0]);
  
	Error* error_result = upload_abort(&upload_result);
      //
      if(error_result!=NULL){
        Error errorResult = *(error_result);
        char* errorMessagePtr = errorResult.message;
        return createError(env,errorResult.code,&errorMessagePtr[0]);
      }
      char blank[] = "";
      return createError(env,0,&blank[0]);
}

//  * function download_infoc returns information about the downloaded object.
//  * pre-requisites: download_objectc
//  * inputs: Download (Object)  
//  * output: Object Result (Object)
napi_value download_infoc(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 1;
  napi_value args[1];

   status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
   assert(status == napi_ok);
  
  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 1 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;

  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);
  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! first argument excepted to be object type\n");
    return NULL;
  }
  
  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }

  Download download_result;
  download_result._handle = getHandleValue(env,args[0]);
  //
  ObjectResult object_result = download_info(&download_result);
  
      if(object_result.error!=NULL){
        return createObjectResult(env,NULL,object_result.error);
      }else{
        return createObjectResult(env,object_result.object,NULL);
      }
}

//  * function upload_set_custom_metadatac set custom meta information
//  * pre-requisites: upload_objectc
//  * inputs: Upload (Object), CustomMetadata 
//  * output: Error (object)
napi_value upload_set_custom_metadatac(napi_env env, napi_callback_info info){
  napi_status status;
  //
  size_t argc = 2;
  napi_value args[2];

  status = napi_get_cb_info(env, info,&argc,args, nullptr ,nullptr);
  assert(status == napi_ok);
  if (argc < 2) {
    napi_throw_type_error(env, nullptr, "\nWrong number of arguments!! excepted 1 arguments\n");
    return NULL;
  }

  napi_valuetype checktypeofinput;
  status = napi_typeof(env,args[0],&checktypeofinput);
  assert(status == napi_ok);
  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! first argument excepted to be object type\n");
    return NULL;
  }

  status = napi_typeof(env,args[1],&checktypeofinput);
  assert(status == napi_ok);
  if(checktypeofinput!=napi_object){
    napi_throw_type_error(env,nullptr,"\nWrong datatype !! second argument excepted to be array\n");
    return NULL;
  }
  

  bool propertyexists = false;
  napi_value ObjectkeyNAPI;
  char* handle = "_handle";
  status = napi_create_string_utf8(env,handle,NAPI_AUTO_LENGTH ,&ObjectkeyNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[0],ObjectkeyNAPI,&propertyexists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nInvalid Object \n");
    return NULL;
  }
  Upload upload_result;
  upload_result._handle = getHandleValue(env,args[0]);
  if(upload_result._handle==0){
    napi_throw_type_error(env,nullptr,"\nWrong object passed\n");
    return NULL;
  }
  //Checking Property exits aur not
  bool entriesExists = false;
  napi_value entriesStringNAPI;
  char* entries = "entries";
  status = napi_create_string_utf8(env,entries,NAPI_AUTO_LENGTH ,&entriesStringNAPI);
  assert(status == napi_ok);
  //
  status = napi_has_property(env,args[1],entriesStringNAPI,&entriesExists);
  assert(status == napi_ok);
  if(!propertyexists){
    napi_throw_type_error(env,nullptr,"\nWrong Object Passed In Second Parameter\n");
    return NULL;
  }
  //Fetching Entries
  napi_value entriesArrayNAPI;
  status = napi_get_named_property(env,args[1],"entries",&entriesArrayNAPI);
  assert(status == napi_ok);

  napi_value countNAPI;
  status = napi_get_named_property(env,args[1],"count",&countNAPI);
  assert(status == napi_ok);
  int64_t countc;
  status = napi_get_value_int64(env,countNAPI,&countc);
  assert(status == napi_ok);
  
  //Checking whether is array of not
  bool isarray=false;
  status = napi_is_array(env,entriesArrayNAPI,&isarray);
  assert(status == napi_ok);
  if(!isarray){
    napi_throw_type_error(env,nullptr,"\nEntries is not array\n");
    return NULL;
  }
  
  uint32_t sizeOfArray = 0;
  status = napi_get_array_length(env,entriesArrayNAPI,&sizeOfArray);
  assert(status == napi_ok);
  
  int size = (int)sizeOfArray;
  CustomMetadataEntry* CustomMetadataEntryListPointer = new CustomMetadataEntry[size];
  CustomMetadata customMetadata;
  napi_value CustomMetadataEntryObject;
  
  for(int i=0;i<size;i++){
    CustomMetadataEntry customMetadataEntry;
    status = napi_get_element(env,entriesArrayNAPI,i,&CustomMetadataEntryObject);
    assert(status == napi_ok);

    napi_value keyNAPI,key_lengthNAPI,valueNAPI,value_lengthNAPI;
    int64_t key_lengthc,value_lengthc;

    status = napi_get_named_property(env,CustomMetadataEntryObject,"key",&keyNAPI);
    assert(status == napi_ok);

    size_t bufsize = 0;
    size_t convertedvalue = 0;

    status = napi_get_value_string_utf8(env,keyNAPI,NULL,bufsize,&convertedvalue);
    assert(status == napi_ok);
    convertedvalue = convertedvalue+1;

    char* keyc =  new char[convertedvalue];
    status = napi_get_value_string_utf8(env,keyNAPI,keyc,convertedvalue,&bufsize);
    assert(status == napi_ok);

    status = napi_get_named_property(env,CustomMetadataEntryObject,"key_length",&key_lengthNAPI);
    assert(status == napi_ok);
    status = napi_get_value_int64(env,key_lengthNAPI,&key_lengthc);
    assert(status == napi_ok);

    status = napi_get_named_property(env,CustomMetadataEntryObject,"value",&valueNAPI);
    assert(status == napi_ok);
    
    bufsize = 0;
    convertedvalue = 0;
    status = napi_get_value_string_utf8(env, valueNAPI,NULL,bufsize,&convertedvalue);
    assert(status == napi_ok);
    convertedvalue = convertedvalue+1;
    
    char* valuec =  new char[convertedvalue];
    status = napi_get_value_string_utf8(env,valueNAPI,valuec,convertedvalue,&bufsize);
    assert(status == napi_ok);

    status = napi_get_named_property(env,CustomMetadataEntryObject,"value_length",&value_lengthNAPI);
    assert(status == napi_ok);

    int64_t value_lengthsize;

    status = napi_get_value_int64(env,value_lengthNAPI,&value_lengthsize);
    assert(status == napi_ok);

    customMetadataEntry.key = keyc;
    customMetadataEntry.key_length =key_lengthc;
    customMetadataEntry.value=valuec;
    customMetadataEntry.value_length = value_lengthsize;
    *(CustomMetadataEntryListPointer+i)=customMetadataEntry;

  }
  customMetadata.entries = (CustomMetadataEntryListPointer+0);
  if(countc==size){
    customMetadata.count = countc;
  }else{
    customMetadata.count = sizeOfArray;
  }
  
  Error* error_result = upload_set_custom_metadata(&upload_result,customMetadata);
      if(error_result!=NULL){
        Error errorResult = *(error_result);
        char* errorMessagePtr = errorResult.message;
        return createError(env,errorResult.code,&errorMessagePtr[0]);
      }

      char blank[] = "";
      return createError(env,0,&blank[0]);
}

#define DECLARE_NAPI_METHOD(name, func)                                        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  
  napi_status status;
  
  napi_property_descriptor parse_access = DECLARE_NAPI_METHOD("parse_accessc", parse_accessc);
  status = napi_define_properties(env, exports, 1, &parse_access);
  assert(status == napi_ok);
 
  napi_property_descriptor request_access_with_passphrase = DECLARE_NAPI_METHOD("request_access_with_passphrasec", request_access_with_passphrasec);
  status = napi_define_properties(env, exports, 1, &request_access_with_passphrase);
  assert(status == napi_ok);

  napi_property_descriptor config_request_access_with_passphrase = DECLARE_NAPI_METHOD("config_request_access_with_passphrasec", config_request_access_with_passphrasec);
  status = napi_define_properties(env, exports, 1, &config_request_access_with_passphrase);
  assert(status == napi_ok);

  
  napi_property_descriptor  open_project = DECLARE_NAPI_METHOD("open_projectc", open_projectc);
  status = napi_define_properties(env, exports, 1, & open_project);
  assert(status == napi_ok);
  
  napi_property_descriptor  config_open_project = DECLARE_NAPI_METHOD("config_open_projectc", config_open_projectc);
  status = napi_define_properties(env, exports, 1, & config_open_project);
  assert(status == napi_ok);

  napi_property_descriptor close_project = DECLARE_NAPI_METHOD("close_projectc", close_projectc);
  status = napi_define_properties(env, exports, 1, &close_project);
  assert(status == napi_ok);
  
  napi_property_descriptor stat_bucket = DECLARE_NAPI_METHOD("stat_bucketc", stat_bucketc);
  status = napi_define_properties(env, exports, 1, &stat_bucket);
  assert(status == napi_ok);
  
  napi_property_descriptor create_bucket = DECLARE_NAPI_METHOD("create_bucketc", create_bucketc);
  status = napi_define_properties(env, exports, 1, &create_bucket);
  assert(status == napi_ok);
  
  napi_property_descriptor ensure_bucket = DECLARE_NAPI_METHOD("ensure_bucketc", ensure_bucketc);
  status = napi_define_properties(env, exports, 1, &ensure_bucket);
  assert(status == napi_ok);

  napi_property_descriptor delete_bucket = DECLARE_NAPI_METHOD("delete_bucketc", delete_bucketc);
  status = napi_define_properties(env, exports, 1, &delete_bucket);
  assert(status == napi_ok);
  
  napi_property_descriptor list_buckets = DECLARE_NAPI_METHOD("list_bucketsc", list_bucketsc);
  status = napi_define_properties(env, exports, 1, &list_buckets);
  assert(status == napi_ok);
  
  napi_property_descriptor stat_object = DECLARE_NAPI_METHOD("stat_objectc", stat_objectc);
  status = napi_define_properties(env, exports, 1, &stat_object);
  assert(status == napi_ok);
  
  napi_property_descriptor delete_object = DECLARE_NAPI_METHOD("delete_objectc", delete_objectc);
  status = napi_define_properties(env, exports, 1, &delete_object);
  assert(status == napi_ok);
   
  napi_property_descriptor upload_object = DECLARE_NAPI_METHOD("upload_objectc", upload_objectc);
  status = napi_define_properties(env, exports, 1, &upload_object);
  assert(status == napi_ok);
 
  napi_property_descriptor upload_write = DECLARE_NAPI_METHOD("upload_writec", upload_writec);
  status = napi_define_properties(env, exports, 1, &upload_write);
  assert(status == napi_ok);
  
  napi_property_descriptor upload_commit = DECLARE_NAPI_METHOD("upload_commitc", upload_commitc);
  status = napi_define_properties(env, exports, 1, &upload_commit);
  assert(status == napi_ok);
  
  napi_property_descriptor download_object = DECLARE_NAPI_METHOD("download_objectc", download_objectc);
  status = napi_define_properties(env, exports, 1, &download_object);
  assert(status == napi_ok);
  
  napi_property_descriptor download_read = DECLARE_NAPI_METHOD("download_readc", download_readc);
  status = napi_define_properties(env, exports, 1, &download_read);
  assert(status == napi_ok);
  
  napi_property_descriptor close_download = DECLARE_NAPI_METHOD("close_downloadc", close_downloadc);
  status = napi_define_properties(env, exports, 1, &close_download);
  assert(status == napi_ok);
 
  napi_property_descriptor list_objects = DECLARE_NAPI_METHOD("list_objectsc", list_objectsc);
  status = napi_define_properties(env, exports, 1, &list_objects);
  assert(status == napi_ok);
   
  napi_property_descriptor access_share = DECLARE_NAPI_METHOD("access_sharec", access_sharec);
  status = napi_define_properties(env, exports, 1, &access_share);
  assert(status == napi_ok);
  /**/
  napi_property_descriptor access_serialize = DECLARE_NAPI_METHOD("access_serializec", access_serializec);
  status = napi_define_properties(env, exports, 1, &access_serialize);
  assert(status == napi_ok);
  
  napi_property_descriptor download_info = DECLARE_NAPI_METHOD("download_infoc", download_infoc);
  status = napi_define_properties(env, exports, 1, &download_info);
  assert(status == napi_ok);
  
  napi_property_descriptor upload_info = DECLARE_NAPI_METHOD("upload_infoc", upload_infoc);
  status = napi_define_properties(env, exports, 1, &upload_info);
  assert(status == napi_ok);
  
  napi_property_descriptor upload_abort = DECLARE_NAPI_METHOD("upload_abortc", upload_abortc);
  status = napi_define_properties(env, exports, 1, &upload_abort);
  assert(status == napi_ok);
 
  napi_property_descriptor upload_set_custom_metadata = DECLARE_NAPI_METHOD("upload_set_custom_metadatac", upload_set_custom_metadatac);
  status = napi_define_properties(env, exports, 1, &upload_set_custom_metadata);
  assert(status == napi_ok);
  
  return exports;
}
//

//
NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)