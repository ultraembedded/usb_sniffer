#ifndef __LOG_FILE_H__
#define __LOG_FILE_H__

//--------------------------------------------------------------------
// Prototypes
//--------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

int log_file_create(const char *filename);
int log_file_close(void);
int log_file_add_sof(uint32_t value, int is_hs);
int log_file_add_rst(uint32_t value, int is_hs);
int log_file_add_token(uint32_t value);
int log_file_add_handshake(uint32_t value);
int log_file_add_data(uint32_t value, uint8_t *data, int length);

#ifdef __cplusplus
}
#endif

#endif
