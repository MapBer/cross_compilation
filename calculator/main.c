#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static int parse_integer(const char *text, long long *value)
{
    char *end;
    errno = 0;
    *value = strtoll(text, &end, 10);
    return errno == 0 && end != text && *end == '\0';
}

int main(int argc, char *argv[])
{
    long long first;
    long long second;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s INTEGER INTEGER\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!parse_integer(argv[1], &first) || !parse_integer(argv[2], &second)) {
        fprintf(stderr, "Both arguments must be valid signed 64-bit integers.\n");
        return EXIT_FAILURE;
    }

    if ((second > 0 && first > LLONG_MAX - second) ||
        (second < 0 && first < LLONG_MIN - second)) {
        fprintf(stderr, "The sum is outside the signed 64-bit range.\n");
        return EXIT_FAILURE;
    }

    printf("%lld + %lld = %lld\n", first, second, first + second);
    return EXIT_SUCCESS;
}
