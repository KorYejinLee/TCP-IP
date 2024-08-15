#include <iostream> 
#include <cstring> 
#include <vector> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <pthread.h>
#include <array>
#include <semaphore.h> 
#include "SerialPort.cpp" 

void* task(void*); 
void* taskSerial(void*); 
static pthread_mutex_t mutex; 
static std::vector<int> client_socket_fds; 
static int serial_port = 0; 
static std::array<char, BUFSIZ> tcp_message;
static std::array<char, BUFSIZ> serial_message;

int main(int argc, const char* argv[])
{
    int server_sock_fd{0}; 
    int client_sock_fd{0}; 
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    pthread_t pid {0UL};
    if(argc != 2) {
        std::cout << "./SERVER 9999" << std::endl; 
        exit(EXIT_FAILURE); 
    }
    serial_port = serialport_init("/dev/ttyACM0", 115200, nullptr); 
    if(serial_port == -1)
    {
        std::cout << "Serial Port error()" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Serial Port is connected ..." << std::endl;
   
    pthread_mutex_init(&mutex, nullptr);

    server_sock_fd = socket(PF_INET , SOCK_STREAM, 0);
    if(server_sock_fd == -1) {
        std::cout << "socket() error" << std::endl;
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof server_addr);
    memset(&client_addr, 0, sizeof client_addr);

    /*서버 주소및 프로토콜 기타등등  값을 대입해준다.*/
    server_addr.sin_family = AF_INET; //IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(atoi(argv[1])); 

    const int bind_state = bind(server_sock_fd, 
            (const struct sockaddr*)&server_addr, sizeof server_addr);
    if(bind_state == -1) {
        std::cout << "bind() error" << std::endl;
        exit(EXIT_FAILURE);
    }
    const int listen_state = listen(server_sock_fd, 5);
    if(listen_state == -1)
    {
        std::cout << "listen() error" << std::endl;
        exit(EXIT_FAILURE);
    }
    // 온도, 습도, 값들을 출력하는 thread 생성 (void*)&valueOfmainValue
    pthread_create(&pid, nullptr, taskSerial, static_cast<void*>(nullptr));
    pthread_detach(pid); 
    socklen_t client_sock_addr_size {0ul};
    while(true) 
    {
        client_sock_addr_size = sizeof client_addr;
        client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&client_addr, 
                (socklen_t*)&client_sock_addr_size);
        if(client_sock_fd == -1) {
            std::cout << "accept() error" << std::endl;
            exit(EXIT_FAILURE);
        }
        pthread_mutex_lock(&mutex);
        client_socket_fds.push_back(client_sock_fd);
        pthread_mutex_unlock(&mutex); 

        pthread_create(&pid, nullptr, task, static_cast<void*>(&client_sock_fd));
        pthread_detach(pid); 
        std::cout << "Connected from client IP :  " << inet_ntoa(client_addr.sin_addr)
            << std::endl;

    }
    serialport_close(serial_port);
    close(server_sock_fd);
    pthread_mutex_destroy(&mutex); 
    return 0;
}

void* taskSerial(void * arg)
{
    while(true)
    {
        const int serial_state {serialport_read_until(serial_port, 
            serial_message.data(), '\n')};
        if(!serial_state)  
        {
            /*저장되어 있는 모든 클라이언트 들에게 데이터(온도, 습도값들)을  전송*/
            for(auto fd : client_socket_fds) 
            {
                pthread_mutex_lock(&mutex); 
                write(fd, serial_message.data(), strlen(serial_message.data()));
                pthread_mutex_unlock(&mutex);
            }
        }
    }
    
    return nullptr;
}

void* task(void* arg) 
{
    const int clnt_sock_fd = *(static_cast<int*>(arg)); // (int*)
    int tcp_str_length {0};
    while((tcp_str_length = read(clnt_sock_fd, tcp_message.data(), BUFSIZ))) 
    {
        pthread_mutex_lock(&mutex);
        serialport_write(serial_port, tcp_message.data());
        pthread_mutex_unlock(&mutex);
    }
    /*client 가 종료를 했을 때*/
    pthread_mutex_lock(&mutex);
    for(auto it {client_socket_fds.cbegin()}; it != client_socket_fds.cend(); ++it)
    {
        if(*it == clnt_sock_fd) 
        {
            client_socket_fds.erase(it);
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    std::cout << "A Client has lefted !" << std::endl;
    std::cout << "client socket has been close !" << std::endl;
    close(clnt_sock_fd);
    return nullptr;
}
