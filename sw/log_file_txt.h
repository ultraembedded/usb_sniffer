#ifndef __LOG_FILE_TXT_H__
#define __LOG_FILE_TXT_H__

//--------------------------------------------------------------------
// Prototypes
//--------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

int txt_file_create(const char *filename);
int txt_file_close(void);
int txt_file_add_sof(uint32_t value, int is_hs);
int txt_file_add_rst(uint32_t value, int is_hs);
int txt_file_add_token(uint32_t value);
int txt_file_add_handshake(uint32_t value);
int txt_file_add_data(uint32_t value, uint8_t *data, int length);

#ifdef __cplusplus
}
#endif

#endif
