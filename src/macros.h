// macros.h
#ifndef MACROS_H
#define MACROS_H

#define HANDLE_ERR(expr)                                                       \
    {                                                                          \
        int err__ = (expr);                                                    \
        if (err__ != 0) {                                                      \
            return err__;                                                      \
        }                                                                      \
    }

#define HANDLE_ERR_MEAASGE(expr, msg)                                          \
    {                                                                          \
        int err__ = (expr);                                                    \
        if (err__ != 0) {                                                      \
            printf(msg);                                                 \
            return err__;                                                      \
        }                                                                      \
    }

#endif // MACROS_H