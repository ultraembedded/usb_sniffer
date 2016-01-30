#ifndef __LOG_FORMAT_H__
#define __LOG_FORMAT_H__

//-----------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------

// TYPE = LOG_CTRL_TYPE_SOF / RST
#define LOG_SOF_FRAME_W         11
#define LOG_SOF_FRAME_MASK      ((1 << LOG_SOF_FRAME_W) - 1)
#define LOG_SOF_FRAME_L         0
#define LOG_SOF_FRAME_H         (LOG_SOF_FRAME_L + LOG_SOF_FRAME_W - 1)
#define LOG_RST_STATE_W         1
#define LOG_RST_STATE_MASK      ((1 << LOG_RST_STATE_W) - 1)
#define LOG_RST_STATE_L         (LOG_SOF_FRAME_H + 1)
#define LOG_RST_STATE_H         (LOG_RST_STATE_L + LOG_RST_STATE_W - 1)

// TYPE = LOG_CTRL_TYPE_TOKEN | LOG_CTRL_TYPE_HSHAKE | LOG_CTRL_TYPE_DATA
#define LOG_TOKEN_PID_W         4
#define LOG_TOKEN_PID_MASK      ((1 << LOG_TOKEN_PID_W) - 1)
#define LOG_TOKEN_PID_L         0
#define LOG_TOKEN_PID_H         (LOG_TOKEN_PID_L + LOG_TOKEN_PID_W - 1)

// TYPE = LOG_CTRL_TYPE_TOKEN
#define LOG_TOKEN_DATA_W        16
#define LOG_TOKEN_DATA_MASK     ((1 << LOG_TOKEN_DATA_W) - 1)
#define LOG_TOKEN_DATA_L        (LOG_TOKEN_PID_H + 1)
#define LOG_TOKEN_DATA_H        (LOG_TOKEN_DATA_L + LOG_TOKEN_DATA_W - 1)

// TYPE = LOG_CTRL_TYPE_DATA
#define LOG_DATA_LEN_W          16
#define LOG_DATA_LEN_MASK       ((1 << LOG_DATA_LEN_W) - 1)
#define LOG_DATA_LEN_L          (LOG_TOKEN_PID_H + 1)
#define LOG_DATA_LEN_H          (LOG_DATA_LEN_L + LOG_DATA_LEN_W - 1)

// TYPE = LOG_CTRL_TYPE_TOKEN | LOG_CTRL_TYPE_HSHAKE | LOG_CTRL_TYPE_DATA
#define LOG_CTRL_CYCLE_W         8
#define LOG_CTRL_CYCLE_MASK      ((1 << LOG_CTRL_CYCLE_W) - 1)
#define LOG_CTRL_CYCLE_L         20
#define LOG_CTRL_CYCLE_H         (LOG_CTRL_CYCLE_L + LOG_CTRL_CYCLE_W - 1)

#define LOG_CTRL_TYPE_W          4
#define LOG_CTRL_TYPE_L          28
#define LOG_CTRL_TYPE_H          31
#define LOG_CTRL_TYPE_SOF        0x1
#define LOG_CTRL_TYPE_RST        0x2
#define LOG_CTRL_TYPE_TOKEN      0x3
#define LOG_CTRL_TYPE_HSHAKE     0x4
#define LOG_CTRL_TYPE_DATA       0x5

#endif
