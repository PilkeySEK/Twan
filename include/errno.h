#ifndef _ERR_H_
#define _ERR_H_

typedef enum
{
    EPERM = 1,       /**< Not owner */
    ENOENT,          /**< No such file or directory */
    ESRCH,           /**< No such context */
    EINTR,           /**< Interrupted system call */
    EIO,             /**< I/O error */
    ENXIO,           /**< No such device or address */
    E2BIG,           /**< Arg list too long */
    ENOEXEC,         /**< Exec format error */
    EBADF,           /**< Bad file number */
    ECHILD,          /**< No children */
    EAGAIN,          /**< No more contexts */
    ENOMEM,          /**< Not enough core */
    EACCES,          /**< Permission denied */
    EFAULT,          /**< Bad address */
    ENOTBLK,         /**< Block device required */
    EBUSY,           /**< Mount device busy */
    EEXIST,          /**< File exists */
    EXDEV,           /**< Cross-device link */
    ENODEV,          /**< No such device */
    ENOTDIR,         /**< Not a directory */
    EISDIR,          /**< Is a directory */
    EINVAL,          /**< Invalid argument */
    ENFILE,          /**< File table overflow */
    EMFILE,          /**< Too many open files */
    ENOTTY,          /**< Not a typewriter */
    ETXTBSY,         /**< Text file busy */
    EFBIG,           /**< File too large */
    ENOSPC,          /**< No space left on device */
    ESPIPE,          /**< Illegal seek */
    EROFS,           /**< Read-only file system */
    EMLINK,          /**< Too many links */
    EPIPE,           /**< Broken pipe */
    EDOM,            /**< Argument too large */
    ERANGE,          /**< Result too large */
    ENOMSG,          /**< Unexpected message type */
    EDEADLK,         /**< Resource deadlock avoided */
    ENOLCK,          /**< No locks available */
    ENOSTR,          /**< STREAMS device required */
    ENODATA,         /**< Missing expected message data */
    ETIME,           /**< STREAMS timeout occurred */
    ENOSR,           /**< Insufficient memory */
    EPROTO,          /**< Generic STREAMS error */
    EBADMSG,         /**< Invalid STREAMS message */
    ENOSYS,          /**< Function not implemented */
    ENOTEMPTY,       /**< Directory not empty */
    ENAMETOOLONG,    /**< File name too long */
    ELOOP,           /**< Too many levels of symbolic links */
    EOPNOTSUPP,      /**< Operation not supported on socket */
    EPFNOSUPPORT,    /**< Protocol family not supported */
    ECONNRESET,      /**< Connection reset by peer */
    ENOBUFS,         /**< No buffer space available */
    EAFNOSUPPORT,    /**< Addr family not supported */
    EPROTOTYPE,      /**< Protocol wrong type for socket */
    ENOTSOCK,        /**< Socket operation on non-socket */
    ENOPROTOOPT,     /**< Protocol not available */
    ESHUTDOWN,       /**< Can't send after socket shutdown */
    ECONNREFUSED,    /**< Connection refused */
    EADDRINUSE,      /**< Address already in use */
    ECONNABORTED,    /**< Software caused connection abort */
    ENETUNREACH,     /**< Network is unreachable */
    ENETDOWN,        /**< Network is down */
    ETIMEDOUT,       /**< Connection timed out */
    EHOSTDOWN,       /**< Host is down */
    EHOSTUNREACH,    /**< No route to host */
    EINPROGRESS,     /**< Operation now in progress */
    EALREADY,        /**< Operation already in progress */
    EDESTADDRREQ,    /**< Destination address required */
    EMSGSIZE,        /**< Message size */
    EPROTONOSUPPORT, /**< Protocol not supported */
    ESOCKTNOSUPPORT, /**< Socket type not supported */
    EADDRNOTAVAIL,   /**< Can't assign requested address */
    ENETRESET,       /**< Network dropped connection on reset */
    EISCONN,         /**< Socket is already connected */
    ENOTCONN,        /**< Socket is not connected */
    ETOOMANYREFS,    /**< Too many references: can't splice */
    ENOTSUP,         /**< Unsupported value */
    EILSEQ,          /**< Illegal byte sequence */
    EOVERFLOW,       /**< Value overflow */
    ECANCELED,       /**< Operation canceled */
    ERRCODE_GUARD    /**< ENUM GUARD */
} errcode_t;

#define ERR_PTR(err) ((void *)(long)(-(err)))
#define IS_ERR(ptr) ((long)(ptr) > 0 && (long)(ptr) < ERRCODE_GUARD)
#define PTR_ERR(ptr) (-(long)(ptr))

#endif