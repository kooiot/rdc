#ifndef koo_error_code_h__
#define koo_error_code_h__

typedef enum K_Status
{
    K_OK = 0,
    K_WRN_BUSY = 100,
    K_WRN_EOF = 101,
    K_WRN_BOF = 102,
    K_WRN_WAITKEYFRAME = 103,
    K_WRN_SKIP_FRAME = 104,
    K_WRN_NEED_MORE_DATA = 105,
    K_WRN_RTP_SKIP = 106,


    K_ERR_TIMEOUT = 1000,
    K_ERR_Not_Implement,
    K_ERR_Not_Supported,
    K_ERR_Open_File_Failure,
    K_ERR_Read_File_Failure,
    K_ERR_Write_File_Failure,
    K_ERR_Seek_File_Failure,
    K_ERR_RecordIndex_Failure,
    K_ERR_RenderSizeChanged,
    K_ERR_Bad_Ptr,
    K_ERR_Buffer_Full = 1010,
    K_ERR_CREATE_FILE,
    K_ERR_CREATE_DIR,
    K_ERR_INVAILID_ARGS,
    K_ERR_BAD_MD5_CHECK,
    K_ERR_FILE_NOT_FOUND,
    K_ERR_FILE_DELETE_FAILURE,
    K_ERR_KILL_PROCESS_FAILURE,
    K_ERR_Bad_XML,
    K_ERR_PERMISSION_DENIED,
    K_ERR_RESOURCE_LOCKED = 1020,
    K_ERR_BAD_FORMAT,
    K_ERR_UNAUTHORIZED,
    K_ERR_BAD_ARGUMENT,
    K_ERR_BAD_CALL_SEQ,
    K_ERR_RESOURCE_NOT_FOUND,
	K_ERR_PROCESS_OPEN_FAILURE,

    K_ERR_UNKNOWN = 0xffffffff
}K_Status_t;
#endif // koo_error_code_h__
