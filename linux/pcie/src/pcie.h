// SPDX-License-Identifier: GPL-2.0
/**
 * Copyright (c) 2019-2022 Hailo Technologies Ltd. All rights reserved.
 **/

#ifndef _HAILO_PCI_PCIE_H_
#define _HAILO_PCI_PCIE_H_

#include "vdma/vdma.h"
#include "hailo_ioctl_common.h"
#include "pcie_common.h"

#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/circ_buf.h>
#include <linux/device.h>

#include<linux/ioctl.h>


struct hailo_notification_wait {
    struct list_head    notification_wait_list;
    int                 tgid;
    struct file*        filp;
    struct completion 	notification_completion;
    bool                is_disabled;
};

struct hailo_fw_control_info {
    // protects that only one fw control will be send at a time
    struct semaphore    mutex;
    // called from the interrupt handler to notify that a response is ready
    struct completion   completion;
    // the command we are currently handling
    struct hailo_fw_control command;
};

struct hailo_fw_boot {
    // the filp that enabled interrupts for fw boot. the interrupt is enabled if this is not null
    struct file *filp;
    // called from the interrupt handler to notify that an interrupt was raised
    struct completion completion;
};

struct hailo_bar {
    void *user_address;
    uint8_t *kernel_address;
    uint64_t phys_address;
    uint64_t length;
    int memory_flag;
    int active_flag;
};

// Context for each open file handle
// TODO: store board and use as actual context
struct hailo_file_context {
    struct list_head open_files_list;
    struct file *filp;
    struct hailo_vdma_file_context vdma_context;
};

struct hailo_pcie_board {
    struct list_head board_list;
    struct pci_dev *pDev;
    uint32_t board_index;
    atomic_t ref_count;
    struct list_head open_files_list;
    struct hailo_pcie_resources pcie_resources;
    struct hailo_fw_control_info fw_control;
    struct semaphore mutex;
    struct hailo_bar bar[MAX_BAR];
    struct hailo_vdma_controller vdma;
    spinlock_t notification_read_spinlock;
    struct list_head notification_wait_list;
    struct hailo_d2h_notification notification_cache;
    struct hailo_d2h_notification notification_to_user;
    u32 desc_max_page_size;
    enum hailo_board_type board_type;
    enum hailo_allocation_mode allocation_mode;
    struct completion fw_loaded_completion;
    bool interrupts_enabled;
};

bool power_mode_enabled(void);

struct hailo_pcie_board* hailo_pcie_get_board_index(uint32_t index);
bool hailo_is_device_connected(struct hailo_pcie_board *board);
void hailo_disable_interrupts(struct hailo_pcie_board *board);
int hailo_enable_interrupts(struct hailo_pcie_board *board);


/*******************************************************************************
 * Bridge Configuration Space
*******************************************************************************/
#define INTERRUPT_BAR (BAR0)
#define FW_CONTROL_BAR (BAR4)
#define FW_NOTIFICATION_BAR (BAR4)
#define FW_LOG_BAR (BAR4)

#define PCIE_CONFIG_VENDOR_OFFSET (0x0098)

#endif /* _HAILO_PCI_PCIE_H_ */

