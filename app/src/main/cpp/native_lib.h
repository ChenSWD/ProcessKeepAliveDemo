//
// Created by chen on 2018/8/12.
//

#ifndef SERVICEALIVETEST_NATIVE_LIB_H
#define SERVICEALIVETEST_NATIVE_LIB_H

#include <jni.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <android/log.h>

#define  LOG_TAG    "KEEP_ALIVE"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//child是服务端，开始准备，使用的是unix域套接字
void child_prepare_socket();

//创建服务端的socket，并等待客户端的连接
int child_create_socket();

//读取socket内容
void child_listen_socket();

#endif //SERVICEALIVETEST_NATIVE_LIB_H
