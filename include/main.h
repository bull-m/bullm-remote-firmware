/**
 * 版权所有 © 2025 牛明工作室 / yy祝。保留所有权利。
 * SPDX-License-Identifier: MIT
 * 根据 MIT 许可证（MIT License）授权。
 */



// ------ 基础信息 ------

#if defined(BOARD_BULLM_MAIN)
#include "boards/bullm_main.h"
#endif

#define INFO_VERSION "v1.0" // 软件版本

#define HTTP_PORT 80 // http端口，包含WebSocket服务
#define UDP_PORT 80 // udp端口