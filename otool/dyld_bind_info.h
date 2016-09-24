/*
 * The internal structure for the broken out bind info.
 */
struct dyld_bind_info {
    const char *segname;
    const char *sectname;
    uint64_t address;
    int bind_type;
    uint64_t addend;
    const char *dylibname;
    const char *symbolname;
#if defined(_WIN32) || defined(WIN32)
    BOOL weak_import;
#else
    unsigned char weak_import;
#endif
};

extern void get_dyld_bind_info(
    const uint8_t *start, /* inputs */
    const uint8_t *end,
    const char **dylibs,
    uint32_t ndylibs,
    struct segment_command **segs,
    uint32_t nsegs,
    struct segment_command_64 **segs64,
    uint32_t nsegs64,
    struct dyld_bind_info **dbi, /* outputs */
    uint64_t *ndbi);

extern void print_dyld_bind_info(
    struct dyld_bind_info *dbi,
    uint64_t ndbi);

extern const char * get_dyld_bind_info_symbolname(
    uint64_t address,
    struct dyld_bind_info *dbi,
    uint64_t ndbi);
