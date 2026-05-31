#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/*
 * We simulate the vulnerable pattern from yep.c:
 *   sprintf(node->name, "%s", relative_path);
 * where node->name is a fixed-size buffer.
 *
 * The security invariant: buffer reads/writes must never exceed the declared
 * buffer length. Any path written into node->name must be either truncated
 * to fit or rejected — never allowed to overflow.
 *
 * We define a safe replacement and verify it enforces the bound, then
 * demonstrate the unsafe version would overflow by checking lengths.
 */

#define NODE_NAME_BUFFER_SIZE 256

/* Canary value to detect overflow */
#define CANARY_VALUE 0xDEADBEEF

typedef struct {
    char name[NODE_NAME_BUFFER_SIZE];
    uint32_t canary; /* placed immediately after name to detect overflow */
} SafeNode;

/*
 * Safe version: uses snprintf with explicit bound.
 * Returns 0 on success (fits), -1 if input was too long (truncated/rejected).
 */
static int safe_copy_name(SafeNode *node, const char *relative_path)
{
    if (node == NULL || relative_path == NULL)
        return -1;

    int ret = snprintf(node->name, NODE_NAME_BUFFER_SIZE, "%s", relative_path);

    /* snprintf returns the number of characters that would have been written
     * if the buffer were large enough (excluding null terminator).
     * If ret >= NODE_NAME_BUFFER_SIZE, the input was truncated. */
    if (ret < 0) {
        return -1; /* encoding error */
    }
    if (ret >= NODE_NAME_BUFFER_SIZE) {
        return -1; /* input was too long, truncated */
    }
    return 0; /* success, fits within buffer */
}

START_TEST(test_buffer_reads_never_exceed_declared_length)
{
    /* Invariant: Writing a relative_path into node->name must never write
     * beyond NODE_NAME_BUFFER_SIZE bytes, regardless of input length.
     * The canary placed after the buffer must remain intact. */

    const char *payloads[] = {
        /* 2x buffer size */
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",

        /* 10x buffer size */
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",

        /* Path traversal attack with long prefix */
        "../../../../../../../../../../../../../../../../../../../../../../../../"
        "../../../../../../../../../../../../../../../../../../../../../../../../"
        "../../../../../../../../../../../../../../../../../../../../../../../../"
        "../../../../../../../../../../../../../../../../../../../../../../../../"
        "etc/passwd",

        /* Null bytes embedded (C string stops at first null, but tests boundary) */
        "normal_path_that_is_exactly_at_boundary_255_chars_padding_here_x"
        "normal_path_that_is_exactly_at_boundary_255_chars_padding_here_x"
        "normal_path_that_is_exactly_at_boundary_255_chars_padding_here_x"
        "normal_path_that_is_exactly_at_boundary_255_chars_padding_here_xx",

        /* Format string attack payload */
        "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s"
        "%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n"
        "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x"
        "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",

        /* Unicode-like long path */
        "\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9"
        "\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9"
        "\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9"
        "\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9"
        "\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9"
        "\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9"
        "\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9"
        "\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9\xc3\xa9",

        /* Exactly at boundary (255 chars + null = 256) */
        "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
        "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
        "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
        "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC",

        /* One byte over boundary (256 chars, no null terminator space) */
        "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"
        "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"
        "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"
        "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",

        /* Empty string (edge case) */
        "",

        /* Single character */
        "x",

        /* Windows-style long path */
        "C:\\Users\\Administrator\\AppData\\Local\\Temp\\"
        "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"
        "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"
        "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"
        "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"
        "\\malicious.exe",
    };

    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        /* Allocate node on heap so valgrind/ASan can detect overflows */
        SafeNode *node = (SafeNode *)malloc(sizeof(SafeNode));
        ck_assert_ptr_nonnull(node);

        /* Initialize buffer to known pattern */
        memset(node->name, 0, NODE_NAME_BUFFER_SIZE);
        /* Set canary */
        node->canary = CANARY_VALUE;

        size_t input_len = strlen(payloads[i]);

        /* Call the safe copy function */
        int result = safe_copy_name(node, payloads[i]);

        /* INVARIANT 1: Canary must never be overwritten */
        ck_assert_msg(node->canary == CANARY_VALUE,
            "Payload %d (len=%zu): canary overwritten — buffer overflow detected!",
            i, input_len);

        /* INVARIANT 2: The name buffer must always be null-terminated */
        /* Check that there is a null terminator within the buffer */
        int has_null = 0;
        for (int j = 0; j < NODE_NAME_BUFFER_SIZE; j++) {
            if (node->name[j] == '\0') {
                has_null = 1;
                break;
            }
        }
        ck_assert_msg(has_null,
            "Payload %d (len=%zu): name buffer is not null-terminated!",
            i, input_len);

        /* INVARIANT 3: The actual string length in the buffer must be
         * strictly less than NODE_NAME_BUFFER_SIZE (to include null terminator) */
        size_t stored_len = strnlen(node->name, NODE_NAME_BUFFER_SIZE);
        ck_assert_msg(stored_len < NODE_NAME_BUFFER_SIZE,
            "Payload %d (len=%zu): stored string length %zu >= buffer size %d",
            i, input_len, stored_len, NODE_NAME_BUFFER_SIZE);

        /* INVARIANT 4: If input fits, result should be 0 (success);
         * if input is too long, result should be -1 (rejected/truncated) */
        if (input_len < (size_t)NODE_NAME_BUFFER_SIZE) {
            ck_assert_msg(result == 0,
                "Payload %d (len=%zu): short input should succeed, got result=%d",
                i, input_len, result);
            /* And the stored content should match the input exactly */
            ck_assert_msg(strcmp(node->name, payloads[i]) == 0,
                "Payload %d (len=%zu): stored content does not match input",
                i, input_len);
        } else {
            ck_assert_msg(result == -1,
                "Payload %d (len=%zu): oversized input should be rejected/truncated, got result=%d",
                i, input_len, result);
            /* Stored length must be exactly NODE_NAME_BUFFER_SIZE - 1 (truncated) */
            ck_assert_msg(stored_len == NODE_NAME_BUFFER_SIZE - 1,
                "Payload %d (len=%zu): truncated string should be %d chars, got %zu",
                i, input_len, NODE_NAME_BUFFER_SIZE - 1, stored_len);
        }

        free(node);
    }
}
END_TEST

/*
 * Additional test: verify that the unsafe sprintf pattern WOULD overflow
 * by checking that the input length exceeds the buffer, confirming the
 * invariant is necessary.
 */
START_TEST(test_unsafe_sprintf_would_overflow