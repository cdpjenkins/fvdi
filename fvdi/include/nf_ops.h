struct nf_ops
{
    long __CDECL (*get_id)(const char *);
    long __CDECL (*call)(long id, ...);
    long res[3];
};


#ifdef __linux__
#include <asm/io.h>
#else
#define virt_to_phys(a) a
#endif

#define NF_GET_ID(ops, feature) ((ops)->get_id((const char *)virt_to_phys(feature)))


#define NF_ID_NAME      "NF_NAME"
#define NF_ID_VERSION   "NF_VERSION"
#define NF_ID_STDERR    "NF_STDERR"
#define NF_ID_SHUTDOWN  "NF_SHUTDOWN"
#define NF_ID_DEBUG     "DEBUGPRINTF"
#define NF_ID_ETHERNET  "ETHERNET"
#define NF_ID_HOSTFS    "HOSTFS"
#define NF_ID_AUDIO     "AUDIO"
#define NF_ID_BOOTSTRAP "BOOTSTRAP"
#define NF_ID_CDROM     "CDROM"
#define NF_ID_CLIPBRD   "CLIPBRD"
#define NF_ID_JPEG      "JPEG"
#define NF_ID_OSMESA    "OSMESA"
#define NF_ID_PCI       "PCI"
#define NF_ID_FVDI      "fVDI"
#define NF_ID_USBHOST   "USBHOST"
#define NF_ID_XHDI      "XHDI"


#define NATFEAT_ID   0x7300
#define NATFEAT_CALL 0x7301

#if defined(__AHCC__)

static long __asm__ __CDECL _nf_get_id(const char *feature_name)
{
    dc.w NATFEAT_ID
    rts
}


static long __asm__ __CDECL _nf_call(long id, ...)
{
    dc.w NATFEAT_CALL
    rts
}

static char const nf_version_str[] = NF_ID_VERSION;

static long __asm__ _nf_detect(void)
{
    pea     nf_version_str
    moveq   #0,d0           /* assume no NatFeats available */
    move.l  d0,-(sp)
    lea     _nf_illegal(pc),a1
    move.l  0x0010.w,a0     /* illegal instruction vector */
    move.l  a1,0x0010.w
    move.l  sp,a1           /* save the ssp */

    nop                     /* flush pipelines (for 68040+) */

    dc.w    NATFEAT_ID      /* Jump to NATFEAT_ID */
    tst.l   d0
    beq.s   _nf_illegal
    moveq   #1,d0           /* NatFeats detected */
    move.l  d0,(sp)

_nf_illegal:
    move.l  a1,sp
    move.l  a0,0x0010.w
    nop                     /* flush pipelines (for 68040+) */
    move.l  (sp)+,d0
    addq.l  #4,sp           /* pop nf_version argument */
    rts
}

#elif defined(__PUREC__)

static long nf_get_id_instr(void) NATFEAT_ID;
static long nf_call_instr(void) NATFEAT_CALL;

static long __CDECL _nf_get_id(const char *feature_name)
{
    (void)(feature_name);
    return nf_get_id_instr();
}


static long __CDECL _nf_call(long id, ...)
{
    (void)(id);
    return nf_call_instr();
}

static void push_a0(void *) 0x2f08;
static void moveq_0_d0(void) 0x7000;
static void moveq_1_d0(void) 0x7001;
static void push_d0(void) 0x2F00;
static void nop(void) 0x4e71;

/* lea nf_illegal(pc),a1 */
static void lea_nf_illegal_a(void) 0x43FA;
static void lea_nf_illegal_b(void) 0x0018;
#define lea_nf_illegal() lea_nf_illegal_a(); lea_nf_illegal_b()

/* move.l 0x0010.w,a1 */
static void fetch_illegal_vec_a(void) 0x2078;
static void fetch_illegal_vec_b(void) 0x0010;
#define fetch_illegal_vec() fetch_illegal_vec_a(); fetch_illegal_vec_b()

/* move.l a0,0x0010.w */
static void store_illegal_vec_a0_a(void) 0x21c8;
static void store_illegal_vec_a0_b(void) 0x0010;
#define store_illegal_vec_a0() store_illegal_vec_a0_a(); store_illegal_vec_a0_b()

/* move.l a1,0x0010.w */
static void store_illegal_vec_a1_a(void) 0x21c9;
static void store_illegal_vec_a1_b(void) 0x0010;
#define store_illegal_vec_a1() store_illegal_vec_a1_a(); store_illegal_vec_a1_b()

/* move.l sp,a1 */
static void get_sp(void) 0x224F;

/* tst.l d0 */
static void test_d0(void) 0x4a80;

/* beq.s _nf_illegal */
static void beqs_nf_illegal(void) 0x6704;

/* move d0,(sp) */
static void move_d0_sp(void) 0x2e80;

/* move.l a1,sp */
static void restore_sp(void) 0x2e49;

/* move.l (sp)+,d0 */
static void pop_d0(void) 0x201f;

/* addq.l #4,sp */
static long addq4_sp(void) 0x588F;

static long _nf_detect(void)
{
    push_a0(NF_ID_VERSION);
    moveq_0_d0();           /* assume no NatFeats available */
    push_d0();
    lea_nf_illegal();
    fetch_illegal_vec();    /* illegal instruction vector */
    store_illegal_vec_a1();
    get_sp();               /* save the ssp */

    nop();                  /* flush pipelines (for 68040+) */

    nf_get_id_instr();      /* Jump to NATFEAT_ID */
    test_d0();
    beqs_nf_illegal();
    moveq_1_d0();           /* NatFeats detected */
    move_d0_sp();

/* _nf_illegal: */
    restore_sp();
    store_illegal_vec_a0();
    nop();                  /* flush pipelines (for 68040+) */
    
    pop_d0();
    return addq4_sp();      /* pop nf_version argument */
}

#elif defined(__GNUC__)

#define ASM_NATFEAT3(opcode) "\t.word " #opcode "\n"
#define ASM_NATFEAT2(opcode) ASM_NATFEAT3(opcode)
#define ASM_NATFEAT(n) ASM_NATFEAT2(n)

static long __attribute__((noinline)) __CDECL _nf_get_id(const char *feature_name)
{
    register long ret __asm__ ("d0");
    (void)(feature_name);
    __asm__ volatile(
        ASM_NATFEAT(NATFEAT_ID)
    : "=g"(ret)  /* outputs */
    : /* inputs  */
    : __CLOBBER_RETURN("d0") "d1" AND_MEMORY /* clobbered regs */
    );
    return ret;
}


static long __attribute__((noinline)) __CDECL _nf_call(long id, ...)
{
    register long ret __asm__ ("d0");
    (void)(id);
    __asm__ volatile(
        ASM_NATFEAT(NATFEAT_CALL)
    : "=g"(ret)  /* outputs */
    : /* inputs  */
    : __CLOBBER_RETURN("d0") "d1" AND_MEMORY /* clobbered regs */
    );
    return ret;
}


#ifndef __mcoldfire__
/*
 * on ColdFire, the NATFEAD_ID opcode is actually
 * "mvs.b d0,d1",
 * which means the following code will NOT detect
 * the presence of an emulator (should there ever
 * be an emulator capable of emulating a ColdFire processor).
 * Luckily, executing the code on a CF processor is still
 * harmless since all it does is clobber D1.
 */
static long _nf_detect(void)
{
    register long ret __asm__ ("d0");
    register const char *nf_version __asm__("a1") = NF_ID_VERSION;
    
    __asm__ volatile(
    "\tmove.l   %1,-(sp)\n"
    "\tmoveq    #0,d0\n"            /* assume no NatFeats available */
    "\tmove.l   d0,-(sp)\n"
    "\tlea      (1f:w,pc),a1\n"
    "\tmove.l   (0x0010).w,a0\n"    /* illegal instruction vector */
    "\tmove.l   a1,(0x0010).w\n"
    "\tmove.l   sp,a1\n"            /* save the ssp */

    "\tnop\n"                       /* flush pipelines (for 68040+) */

    ASM_NATFEAT(NATFEAT_ID)         /* Jump to NATFEAT_ID */
    "\ttst.l    d0\n"
    "\tbeq.s    1f\n"
    "\tmoveq    #1,d0\n"            /* NatFeats detected */
    "\tmove.l   d0,(sp)\n"

"1:\n"
    "\tmove.l   a1,sp\n"
    "\tmove.l   a0,(0x0010).w\n"
    "\tmove.l   (sp)+,d0\n"
    "\taddq.l   #4,sp\n"            /* pop nf_version argument */

    "\tnop\n"                       /* flush pipelines (for 68040+) */
    : "=g"(ret)  /* outputs */
    : "g"(nf_version)       /* inputs  */
    : __CLOBBER_RETURN("d0") "a0", "d1" AND_MEMORY
    );
    return ret;
}
#endif

#endif

/* NatFeat code */
typedef struct {
    long magic;
    long __CDECL(*nfGetID) (const char *);
    long __CDECL(*nfCall) (long ID, ...);
} NatFeatCookie;
