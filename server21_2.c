#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // Tạo socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Thiết lập thông tin máy chủ
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // Sử dụng cổng 8080
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket với địa chỉ máy chủ
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Lắng nghe kết nối
    listen(server_socket, 10);

    printf("The server is listening on port 8080...\n");

    while (1) {
        // Chấp nhận kết nối từ client
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);

        // Xử lý yêu cầu từ client
        char request[1024];
        recv(client_socket, request, sizeof(request), 0);

        // Kiểm tra xem yêu cầu có chứa "/welcome" không
        if (strstr(request, "/welcome") != NULL) {
            // Trích xuất tên từ yêu cầu
            char *name_start = strstr(request, "name=");
            char name[1024];
            if (name_start != NULL) {
                sscanf(name_start, "name=%s", name);

                // Mở tệp welcome.html
                FILE *file = fopen("welcome.html", "r");
                if (file) {
                    char response[1024];
                    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\n\n");

                    // Đọc nội dung của welcome.html
                    char line[1024];
                    while (fgets(line, sizeof(line), file)) {
                        strcat(response, line);
                    }

                    // Thay thế %s bằng tên và gửi trả lời
                    char *name_placeholder = strstr(response, "%s");
                    if (name_placeholder != NULL) {
                        memmove(name_placeholder + strlen(name), name_placeholder + 2, strlen(name_placeholder) - 2 + 1);
                        strncpy(name_placeholder, name, strlen(name));
                    }

                    send(client_socket, response, strlen(response), 0);
                    fclose(file);
                }
            }
        } else {
            // Trả về trang index.html mặc định
            FILE *file = fopen("index.html", "r");
            if (file) {
                char response[1024];
                snprintf(response, sizeof(response), "HTTP/1.1 200 OK\n\n");

                char line[1024];
                while (fgets(line, sizeof(line), file)) {
                    strcat(response, line);
                }

                send(client_socket, response, strlen(response), 0);
                fclose(file);
            }
        }

        close(client_socket);
    }

    close(server_socket);

    return 0;
} 
