//socket 实现进程保活，除非卸载
#include "native_lib.h"

const char *PATH = "/data/data/com.example.chen.servicealivetest/keep_alive.socket";
const char *userId;
int m_child_fd;
extern "C"
JNIEXPORT void JNICALL Java_com_example_chen_servicealivetest_KeepService_createWatcher(
        JNIEnv *env,
        jobject thiz, jstring userId_) {
    userId = env->GetStringUTFChars(userId_, 0);
    LOGI("userId %s", userId);
    //fork 一个新的进程
    pid_t pid = fork();
    //新建进程失败
    if (pid < 0) {

    }
        //子进程
    else if (pid == 0) {
        child_prepare_socket();
    }
        //父进程返回新建的子进程的pid
    else if (pid > 0) {}
    env->ReleaseStringUTFChars(userId_, userId);
}

void child_prepare_socket() {
    if (child_create_socket()) {
        child_listen_socket();
    }
}

int child_create_socket() {
    LOGI("开始创建服务端socket");
    //AF_LOCAL:本地类型socket，SOCK_STREAM：面向tcp的流，参数0，代表自己选择网络协议
    int sfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    unlink(PATH);
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(sockaddr_un));
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, PATH);
    if (bind(sfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un)) < 0) {
        LOGE("服务端绑定socket出错");
        return 0;
    }
    //5:最大连接数量，实际上由系统决定，
    listen(sfd, 5);
    int coonfd = 0;
    //监听客户端的连接
    while (1) {
        //没有客户端请求到来，会阻塞直到一个请求的到来，返回客户端地址
        if ((coonfd = accept(sfd, NULL, NULL)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                LOGE("服务端 accept 出错");
                return 0;
            }
        }
        m_child_fd = coonfd;
        LOGI("服务端连接成功，客户端的 fd %d", m_child_fd);
        break;
    }
    return 1;
}

void child_listen_socket() {
    LOGI("服务端监听客户端的消息");
    fd_set fds;
    struct timeval timeout = {10, 0};
    while (1) {
        //使用select I/O多路复用
        //所有位置0
        FD_ZERO(&fds);
        //指定位上的值置为1,
        FD_SET(m_child_fd, &fds);
        //最长等待时间10s，timeout为null，表示一直阻塞直到收到客户端的消息
        int r = select(m_child_fd + 1, &fds, NULL, NULL, &timeout);
        LOGI("读取消息前 select %d", r);
        if (r > 0) {
            char pkg[256] = {0};
            if (FD_ISSET(m_child_fd, &fds)) {
                //如果阻塞结束，说明客户端进程被kill
                int result = read(m_child_fd, pkg, sizeof(pkg));
                LOGE("服务端开始重启服务");

                //api<17
                /*execlp("am", "am", "startservice", "-n"
                               "com.example.chen.servicealivetest/com.example.chen.servicealivetest.KeepService",
                       (char *) NULL);*/

                //>=api17  不过好像两种 api 这种方式都能启动服务
                execlp("am", "am", "startservice", "--user", userId, "-n"
                               "com.example.chen.servicealivetest/com.example.chen.servicealivetest.KeepService",
                       (char *) NULL);
                break;
            }
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_chen_servicealivetest_KeepService_connectServer(JNIEnv *env, jobject instance) {
    int socketfd;
    struct sockaddr_un addr;
    while (1) {
        socketfd = socket(AF_LOCAL, SOCK_STREAM, 0);
        if (socketfd < 0) {
            LOGE("客户端新建创建失败");
            return;
        }
        memset(&addr, 0, sizeof(sockaddr_un));
        addr.sun_family = AF_LOCAL;
        strcpy(addr.sun_path, PATH);
        if (connect(socketfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un))) {
            LOGE("客户端连接失败");
            close(socketfd);
            sleep(1);
            continue;
        }
        LOGE("客户端连接成功");
        break;
    }
}