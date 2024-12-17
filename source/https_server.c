#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <pthread.h> 
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SIZE 1024
#define PORT 8000

void *handle_client_thread(void *arg);
void error_handling(char *message);

typedef struct {
    SSL *ssl;
    int sock;
} client_data;

int main(int argc, char *argv[]) {

    // setting openssl
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if(!ctx){
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if(SSL_CTX_use_certificate_file(ctx, "/etc/letsencrypt/live/pgj-server.n-e.kr/fullchain.pem", SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stderr);
        exit(2);
    }

    if(SSL_CTX_use_PrivateKey_file(ctx, "/etc/letsencrypt/live/pgj-server.n-e.kr/privkey.pem", SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stderr);
        exit(3);
    }

    int serv_sock, cli_sock;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_addr_len;
		pthread_t t_id;
		
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (serv_sock == -1) {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0x00, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    while (1) {
        cli_addr_len = sizeof(cli_addr);
        cli_sock = accept(serv_sock, (struct sockaddr *)&cli_addr, &cli_addr_len);

        if (cli_sock == -1) {
            error_handling("accept() error");
            continue;
        }

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, cli_sock);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            close(cli_sock);
            continue;
        }

     
        client_data *data = (client_data *)malloc(sizeof(client_data));
        data->ssl = ssl;
        data->sock = cli_sock;


        
        if (pthread_create(&t_id, NULL, handle_client_thread, (void *)data) != 0) {
            perror("pthread_create() error");
            SSL_free(ssl);
            close(cli_sock);
            free(data);
            continue;
        }

        pthread_detach(t_id);
    }

    close(serv_sock);
    SSL_CTX_free(ctx);

    return 0;
}


void *handle_client_thread(void *arg) {
    client_data *data = (client_data *)arg;
    SSL *ssl = data->ssl;
    int sock = data->sock;

    char buf[SIZE];
    char method[SIZE], path[SIZE], version[SIZE];
    int fd;

    int read_len = SSL_read(ssl, buf, SIZE - 1);
    if (read_len <= 0) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sock);
        free(data);
        return NULL;
    }

    buf[read_len] = '\0';

    sscanf(buf, "%s %s %s", method, path, version);


    if (strcmp(method, "GET") != 0) {
        SSL_write(ssl, "HTTP/1.1 405 Method Not Allowed\r\n\r\n", 36);
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sock);
        free(data);
        return NULL;
    }


    if (strcmp(path, "/") == 0) {
        strcpy(path, "/index.html"); //기본 페이지 리다이렉트
    }

    char *filename = path + 1; 

    fd = open(filename, O_RDONLY);
    if (fd == -1) {

        char *notFound = "HTTP/1.1 404 Not Found\r\n\r\nFile Not Found";
        SSL_write(ssl, notFound, strlen(notFound));
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sock);
        free(data);
        return NULL;
    }


    char *ok_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    SSL_write(ssl, ok_response, strlen(ok_response));


    while ((read_len = read(fd, buf, SIZE)) > 0) {
        SSL_write(ssl, buf, read_len);
    }

    close(fd);

   
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    free(data);

    return NULL;
}



void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(4);
}
