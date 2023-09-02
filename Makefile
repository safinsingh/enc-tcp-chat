default:
	gcc -g client.c error.c main.c server.c enc.c msg.c shared.c \
	-L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include -lssl -lcrypto \
	-o ip