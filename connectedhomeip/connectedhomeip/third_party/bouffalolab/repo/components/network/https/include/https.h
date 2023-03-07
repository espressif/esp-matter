#ifndef __HTTPS_H__
#define __HTTPS_H__

#include <stdint.h>

/**
 * @brief 创建加密tcp连接
 *
 * @par 描述:
 * 创建非阻塞模式加密tcp连接
 *
 * @param dst      [IN] 接收方ip地址。
 * @param port     [IN] 接收方端口号。
 *
 * @retval BL_TCP_ARG_INVALID           dst为空。
 * @retval BL_TCP_CREATE_CONNECT_ERR    创建连接失败。
 * @retval 大于0  连接成功,返回tcp套接字，然后使用blTcpSslState判断连接是否完全建立。
 */
int32_t blTcpSslConnect(const char* dst, uint16_t port);

/**
 * @brief 加密tcp连接状态
 *
 * @par 描述:
 * 查询加密tcp连接状态
 *
 * @param fd      [IN] tcp套接字。
 *
 * @retval blTcpErrorCode   bl tcp错误码。
 * @see blTcpErrorCode
 */
int32_t blTcpSslState(int32_t fd);

/**
 * @brief 断开加密tcp连接。
 *
 * @par 描述:
 * 断开加密tcp连接。
 *
 * @param fd   [IN] blTcpSslConnect创建的套接字。
 *
 * @retval 无。
 */
void blTcpSslDisconnect(int32_t fd);

/**
 * @brief 发送加密tcp数据
 *
 * @par 描述:
 * 非阻塞发送加密tcp数据
 *
 * @param fd      [IN] tcp套接字。
 * @param buf     [IN] 指向待发送数据缓冲区的指针。
 * @param len     [IN] 待发送数据的长度，范围为[0，512)。
 *
 * @retval blTcpErrorCode  bl tcp错误码。
 * @see blTcpErrorCode
 */
int32_t blTcpSslSend(int32_t fd, const uint8_t* buf, uint16_t len);

/**
 * @brief 读取加密tcp数据
 *
 * @par 描述:
 * 非阻塞读取加密tcp数据
 *
 * @param fd      [IN] tcp套接字。
 * @param buf     [IN] 指向存放接收数据缓冲区的指针。
 * @param len     [IN] 存放接收数据缓冲区的最大长度，范围为[0，512)。
 *
 * @retval blTcpErrorCode  bl tcp错误码。
 * @see blTcpErrorCode
 */
int32_t blTcpSslRead(int32_t fd, uint8_t* buf, uint16_t len);

#endif
