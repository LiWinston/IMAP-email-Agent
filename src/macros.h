// macros.h
#ifndef MACROS_H
#define MACROS_H

#define RETURN_ERR                                                             \
    if (err != 0) {                                                            \
        return err;                                                            \
    }

#endif // MACROS_H