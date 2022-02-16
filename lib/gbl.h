#ifndef CMPE460_GBL_H
#define CMPE460_GBL_H

#ifndef __gbl_LINKED__
#error "You need to link 'gbl' to use this header"
#endif

#ifndef GBL_ARG_N
#define GBL_ARG_N (4)
#endif

#ifndef GBL_RET_N
#define GBL_RET_N (2)
#endif

#include <fw.h>

typedef enum
{
    GBL_STATUS_FAILURE = -1,
    GBL_STATUS_SUCCESS = 0
} GblStatus;

typedef struct GblReply_prv GblReply;
typedef void (*GblReplyCb)(const GblReply* reply, GblStatus status);

struct GblReply_prv
{
    PXX arg[GBL_ARG_N];
    PXX ret[GBL_RET_N];
    GblReplyCb callback;
};

/**
 * Initialize a new asynchronous reply with
 * a callback and N arguments
 * @param cb callback to run when asynchronous call is done
 * @param n number of arguments
 * @param ... arguments to send to reply (max at GBL_ARG_N)
 * @return newly initialized reply
 */
GblReply gbl_reply_init(GblReplyCb cb, U32 n, ...);

/**
 * Clear a reply
 * @param reply reply to clear
 */
void gbl_reply_clear(GblReply* reply);

/**
 * Send a reply with return arguments
 * @param reply reply to send
 * @param status status of reply
 * @param n number of return parameters
 * @param ... arguments to send
 */
void gbl_reply_ret(GblReply* reply, GblStatus status, U32 n, ...);

/**
 * Send to reply without arguments (holds reply const)
 * @param reply reply to send
 * @param status status of reply
 */
void gbl_reply(const GblReply* reply, GblStatus status);

#endif //CMPE460_GBL_H
