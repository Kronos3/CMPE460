#include <string.h>
#include <stdarg.h>
#include <gbl.h>

GblReply gbl_reply_init(GblReplyCb cb, U32 n, ...)
{
    FW_ASSERT(n < GBL_ARG_N && "More arguments than allowed by GBL_ARG_N", n, GBL_ARG_N);

    GblReply reply;
    gbl_reply_clear(&reply);

    reply.callback = cb;

    // Read the optional arguments
    va_list args;
    va_start(args, n);
    for (U32 i = 0; i < n; i++)
    {
        reply.arg[i] = va_arg(args, U32);
    }
    va_end(args);

    return reply;
}

void gbl_reply_clear(GblReply* reply)
{
    reply->callback = NULL;
    memset(reply->arg, 0, sizeof(reply->arg));
    memset(reply->ret, 0, sizeof(reply->ret));
}

void gbl_reply_ret(GblReply* reply, GblStatus status, U32 n, ...)
{
    FW_ASSERT(reply);
    FW_ASSERT(n < GBL_ARG_N && "More return values than allowed by GBL_RET_N", n, GBL_RET_N);

    // Read the return values
    va_list args;
    va_start(args, n);
    for (U32 i = 0; i < n; i++)
    {
        reply->ret[i] = va_arg(args, U32);
    }
    va_end(args);

    gbl_reply(reply, status);
}

void gbl_reply(const GblReply* reply, GblStatus status)
{
    FW_ASSERT(reply);
    FW_ASSERT(status == GBL_STATUS_FAILURE || status == GBL_STATUS_SUCCESS, status);

    if (reply->callback)
    {
        reply->callback(reply, status);
    }
}
