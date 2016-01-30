#ifndef __USB_SNIFFER_REGS_H__
#define __USB_SNIFFER_REGS_H__

#define USB_BUFFER_CFG    0x0
    #define USB_BUFFER_CFG_DEV_SHIFT             24
    #define USB_BUFFER_CFG_DEV_MASK              0x7f

    #define USB_BUFFER_CFG_EP_SHIFT              16
    #define USB_BUFFER_CFG_EP_MASK               0xf

    #define USB_BUFFER_CFG_SPEED_SHIFT           7
    #define USB_BUFFER_CFG_SPEED_MASK            0x3

    #define USB_BUFFER_CFG_EXCLUDE_EP            6
    #define USB_BUFFER_CFG_EXCLUDE_EP_SHIFT      6
    #define USB_BUFFER_CFG_EXCLUDE_EP_MASK       0x1

    #define USB_BUFFER_CFG_MATCH_EP              5
    #define USB_BUFFER_CFG_MATCH_EP_SHIFT        5
    #define USB_BUFFER_CFG_MATCH_EP_MASK         0x1

    #define USB_BUFFER_CFG_EXCLUDE_DEV           4
    #define USB_BUFFER_CFG_EXCLUDE_DEV_SHIFT     4
    #define USB_BUFFER_CFG_EXCLUDE_DEV_MASK      0x1

    #define USB_BUFFER_CFG_MATCH_DEV             3
    #define USB_BUFFER_CFG_MATCH_DEV_SHIFT       3
    #define USB_BUFFER_CFG_MATCH_DEV_MASK        0x1

    #define USB_BUFFER_CFG_IGNORE_SOF            2
    #define USB_BUFFER_CFG_IGNORE_SOF_SHIFT      2
    #define USB_BUFFER_CFG_IGNORE_SOF_MASK       0x1

    #define USB_BUFFER_CFG_CONT                  1
    #define USB_BUFFER_CFG_CONT_SHIFT            1
    #define USB_BUFFER_CFG_CONT_MASK             0x1

    #define USB_BUFFER_CFG_ENABLED               0
    #define USB_BUFFER_CFG_ENABLED_SHIFT         0
    #define USB_BUFFER_CFG_ENABLED_MASK          0x1

#define USB_BUFFER_BASE   0x4
    #define USB_BUFFER_BASE_ADDR_SHIFT           0
    #define USB_BUFFER_BASE_ADDR_MASK            0xffffffff

#define USB_BUFFER_END    0x8
    #define USB_BUFFER_END_ADDR_SHIFT            0
    #define USB_BUFFER_END_ADDR_MASK             0xffffffff

#define USB_BUFFER_STS    0xc
    #define USB_BUFFER_STS_OVERFLOW              3
    #define USB_BUFFER_STS_OVERFLOW_SHIFT        3
    #define USB_BUFFER_STS_OVERFLOW_MASK         0x1

    #define USB_BUFFER_STS_MEM_STALL             2
    #define USB_BUFFER_STS_MEM_STALL_SHIFT       2
    #define USB_BUFFER_STS_MEM_STALL_MASK        0x1

    #define USB_BUFFER_STS_WRAPPED               1
    #define USB_BUFFER_STS_WRAPPED_SHIFT         1
    #define USB_BUFFER_STS_WRAPPED_MASK          0x1

    #define USB_BUFFER_STS_TRIG                  0
    #define USB_BUFFER_STS_TRIG_SHIFT            0
    #define USB_BUFFER_STS_TRIG_MASK             0x1

#define USB_BUFFER_CURRENT  0x10
    #define USB_BUFFER_CURRENT_ADDR_SHIFT        0
    #define USB_BUFFER_CURRENT_ADDR_MASK         0xffffffff

#define USB_BUFFER_READ   0x14
    #define USB_BUFFER_READ_ADDR_SHIFT           0
    #define USB_BUFFER_READ_ADDR_MASK            0xffffffff

#endif