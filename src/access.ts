/* eslint-disable */
import bindings = require("bindings");
const uplink = bindings("uplink");

import https from 'https';
import { ProjectResultStruct } from "./project.js";
import { Config } from "./uplink.js";
import { S3Credentials } from "./types.js";
const errorhandle = require("./error.js");


export class AccessResultStruct {

    access: any;

    constructor (access: any) {

        this.access = access;

    }

    /*
     * Function opens Storj(V3) project using access grant.
     * Input : None
     * Output : Project(Object)
     */
    async openProject (maximumConcurrent?: number): Promise<ProjectResultStruct> {

        const project = await uplink.open_project(this.access, maximumConcurrent ?? 0 ).catch((error: any) => {

                errorhandle.storjException(
                    error.error.code,
                    error.error.message
                );

            }),
            projectResultReturn = new ProjectResultStruct(project.project);


        return projectResultReturn;

    }

    /*
     * Function opens Storj(V3) project using access grant and custom configuration.
     * Input : None
     * Output : Project(Object)
     */
    async configOpenProject (config: Config, maximumConcurrent?: number): Promise<ProjectResultStruct> {

        const project = await uplink.config_open_project(this.access, config, maximumConcurrent ?? 0).catch((error: any) => {

                errorhandle.storjException(
                    error.error.code,
                    error.error.message
                );

            }),
            projectResultReturn = new ProjectResultStruct(project.project);


        return projectResultReturn;

    }

    /*
     *
     * Function Share creates a new access grant with specific permissions.
     * Access grants can only have their existing permissions restricted, and the resulting
     * access grant will only allow for the intersection of all previous Share calls in the
     * access grant construction chain.
     * Prefixes, if provided, restrict the access grant (and internal encryption information)
     * to only contain enough information to allow access to just those prefixes.
     * Input : Permission (Object) , sharePrefixListArray (Array) , sharePrefixListArraylength (Int)
     * Output : Project(Object)
     */
    async share (permission: Record<string, any>, sharePrefixListArray: Array<any>, sharePrefixListArraylength: number): Promise<AccessResultStruct> {

        const sharedAccess = await uplink.access_share(
                this.access,
                permission,
                sharePrefixListArray,
                sharePrefixListArraylength
            ).catch((error: any) => {

                errorhandle.storjException(
                    error.error.code,
                    error.error.message
                );

            }),
            sharedAccessResultReturn = new AccessResultStruct(sharedAccess.access);


        return sharedAccessResultReturn;

    }

    /*
     * Function serializes an access grant such that it can be used later with ParseAccess or other tools.
     * Input : None
     * Output : SharedString (String)
     */
    async serialize (): Promise<string> {

        const stringResult = await uplink.access_serialize(this.access).catch((error: any) => {

            errorhandle.storjException(
                error.error.code,
                error.error.message
            );

        });


        return stringResult;

    }

    async registerS3Credentials(access: string, host = 'https://auth.storjshare.io', isPublic = false): Promise<S3Credentials> {
        const postData = JSON.stringify({
            access_grant: access,
            public: isPublic
        });

        const options = {
            hostname: new URL(host).hostname,
            path: '/v1/access',
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Content-Length': postData.length
            },
        };

        return new Promise((resolve, reject) => {
            const req = https.request(options, (res) => {
                let data = '';
    
                res.on('data', (chunk) => {
                    data += chunk;
                });
    
                res.on('end', () => {
                    if (res.statusCode !== 200) {
                        reject(new Error(`Request failed with status code ${res.statusCode}: ${data}`));
                        return;
                    }
    
                    try {
                        const responseBody = JSON.parse(data);
                        const result = {
                            accessKeyId: responseBody.access_key_id,
                            secretKey: responseBody.secret_key,
                            endpoint: responseBody.endpoint,
                        }
    
                        if (!result.accessKeyId || !result.secretKey) {
                            reject(new Error('Response is missing access_key_id or secret_key'));
                            return;
                        }
    
                        resolve(result);
                    } catch (err) {
                        reject(new Error(`Unexpected response from auth service: ${data}`));
                    }
                });
            });
    
            req.on('error', (err) => {
                reject(err);
            });
    
            req.write(postData);
            req.end();
        });
    }

    /*
     * Function serializes an access grant such that it can be used later with ParseAccess or other tools.
     * Input : bucket (String) , prefix (String) and Encrption key
     * Output : None
     */
    async overrideEncryptionKey (bucket: string, prefix:string, encryption_key:any): Promise<string> {

        const stringResult = await uplink.access_override_encryption_key(
            this.access,
            bucket,
            prefix,
            encryption_key
        ).catch((error:any) => {

            errorhandle.storjException(
                error.error.code,
                error.error.message
            );

        });


        return stringResult;

    }

}
/* eslint-enable */
