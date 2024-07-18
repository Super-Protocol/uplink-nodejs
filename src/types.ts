export type ObjectInfo = Record<string, any>;
export type BucketInfo = Record<string, any>;
export type S3Credentials = {
    accessKeyId: string;
    secretKey: string;
    endpoint: string;
}