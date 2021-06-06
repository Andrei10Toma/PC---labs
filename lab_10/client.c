#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
    // Ex 1.1: GET dummy from main server
    message = compute_get_request("34.118.48.238", "/api/v1/dummy", NULL, NULL, 0);
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // Ex 1.2: POST dummy and print response from main server
    char **body_data = (char **)calloc(2, sizeof(char*));
    int body_count = 2;
    body_data[0] = "key0=salut&";
    body_data[1] = "key1=salut";
    message = compute_post_request("34.118.48.238", "/api/v1/dummy", 
        "application/x-www-form-urlencoded", body_data, body_count, NULL, 0);
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // Ex 2: Login into main server
    body_data[0] = "username=student&";
    body_data[1] = "password=student";
    message = compute_post_request("34.118.48.238", "/api/v1/auth/login",
        "application/x-www-form-urlencoded", body_data, body_count, NULL, 0);
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    char *aux = strstr(response, "Set-Cookie: ");
    aux += strlen("Set-Cookie: ");
    char *token = strtok(aux, "\r\n");
    printf("%s\n", token);

    // Ex 3: GET weather key from main server
    char **cookie = (char **)calloc(1, sizeof(char *));
    int cookie_count = 1;
    cookie[0] = token;
    message = compute_get_request("34.118.48.238", "/api/v1/weather/key",
        NULL, cookie, cookie_count);
    send_to_server(sockfd, message);
    printf("%s\n", message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);

    // Ex 4: GET weather data from OpenWeather API
    // Ex 5: POST weather data for verification to main server
    // Ex 6: Logout from main server
    message = compute_get_request("34.118.48.238", "/api/v1/auth/logout",
        NULL, NULL, 0);
    printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    close_connection(sockfd);

    return 0;
}
