#include <sodium.h>

int main(int argc, const char *argv[]) {
    FILE *f = fopen("key", "wb");
    unsigned char k[crypto_secretstream_xchacha20poly1305_KEYBYTES];
    crypto_secretstream_xchacha20poly1305_keygen(k);
    fwrite(k, 1, crypto_secretstream_xchacha20poly1305_KEYBYTES, f); 
    fclose(f);

    return 0;
}
