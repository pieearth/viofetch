#define INCL_BASE
#define INCL_DOSINFOSEG
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSMISC
#define INCL_DOSNLS
#define INCL_VIO
#include <os2.h>
#include <stdio.h>
#include <string.h>

void print_color(USHORT row, USHORT col, const char *str, BYTE attr) {
    VioWrtCharStrAtt((PCH)str, (USHORT)strlen(str), row, col, &attr, 0);
}

/* Map Country Code to standard layout name */
const char *get_keyboard_name(USHORT country_code) {
    switch (country_code) {
        case 1:   return "United States";
        case 2:   return "Canadian French";
        case 33:  return "French";
        case 34:  return "Spanish";
        case 39:  return "Italian";
        case 44:  return "United Kingdom";
        case 49:  return "German";
        default:  return "Standard";
    }
}

void draw_os2_logo(void) {
    /* Diamond top cap */
    print_color(1, 10, "    /\\    ", 0x08);
    print_color(2,  8, "  /    \\  ", 0x08);
    print_color(3,  6, " /        \\ ", 0x08);

    /* Row 4: Upper loops of O, S, /, 2 */
    print_color(4,  4, "/  ", 0x08);
    print_color(4,  7, "\xDC\xDF\xDF\xDF\xDC", 0x0E);   /* O top arc:    ▄▀▀▀▄  (Yellow) */
    print_color(4, 13, "\xDC\xDF\xDF\xDF\xDC", 0x0C);   /* S top arc:    ▄▀▀▀▄  (Red)    */
    print_color(4, 19, "  \xDC\xDF",       0x0E);   /* / top:          ▄▀   (Yellow) */
    print_color(4, 23, "\xDC\xDF\xDF\xDF\xDC", 0x0C);   /* 2 top hook:   ▄▀▀▀▄  (Red)    */
    print_color(4, 28, " \\", 0x08);

    /* Row 5: Upper vertical segments */
    print_color(5,  2, "/   ", 0x08);
    print_color(5,  6, "\xDB     \xDB", 0x0E);          /* O sides:      █   █  (Yellow) */
    print_color(5, 13, "\xDB    ",       0x0C);          /* S left bar:   █      (Red)    */
    print_color(5, 20, "\xDC\xDF ",      0x0E);          /* / mid-high:    ▄▀    (Yellow) */
    print_color(5, 27, "\xDB",           0x0B);          /* 2 right bar:      █  (Cyan)   */
    print_color(5, 29, "   \\", 0x08);

    /* Row 6: Center transitions */
    print_color(6,  1, "<    ", 0x08);
    print_color(6,  6, "\xDB     \xDB", 0x0C);          /* O sides:      █   █  (Red)    */
    print_color(6, 14, "\xDF\xDF\xDF\xDC ", 0x0B);      /* S mid bridge:  ▀▀▀▄  (Cyan)   */
    print_color(6, 19, "\xDC\xDF  ",     0x0E);          /* / center:     ▄▀     (Yellow) */
    print_color(6, 25, "\xDC\xDF\xDF  ", 0x0B);          /* 2 diagonal:   ▄▀▀    (Cyan)   */
    print_color(6, 31, " >", 0x08);

    /* Row 7: Lower segments & bottom hooks */
    print_color(7,  2, "\\   ", 0x08);
    print_color(7,  7, "\xDF\xDC\xDC\xDC\xDF", 0x0C);   /* O bottom arc: ▀▄▄▄▀  (Red)    */
    print_color(7, 17, "\xDB",           0x0B);          /* S lower right:    █  (Cyan)   */
    print_color(7, 18, "\xDC\xDF  ",     0x0E);          /* / mid-low:    ▄▀     (Yellow) */
    print_color(7, 23, "\xDC\xDF\xDF    ", 0x0B);        /* 2 diag-low:   ▄▀▀    (Cyan)   */
    print_color(7, 29, "   /", 0x08);

    /* Row 8: Bases of S and 2 */
    print_color(8,  4, "\\  ", 0x08);
    print_color(8, 13, "\xDF\xDC\xDC\xDC\xDF", 0x0B);   /* S bottom arc: ▀▄▄▄▀  (Cyan)   */
    print_color(8, 19, "\xDF   ",        0x0E);          /* / base:       ▀      (Yellow) */
    print_color(8, 23, "\xDF\xDF\xDF\xDF\xDF\xDF", 0x0E);/* 2 flat base: ▀▀▀▀▀▀ (Yellow) */
    print_color(8, 28, " /", 0x08);

    /* Diamond bottom closure */
    print_color(9,   6, " \\        / ", 0x08);
    print_color(10,  8, "  \\    /  ",   0x08);
    print_color(11, 10, "    \\/    ",   0x08);
}

int main(void) {
    USHORT ver;
    USHORT major, minor;
    SEL gdt_sel, ldt_sel;
    PGINFOSEG ginf;
    PLINFOSEG linf;
    FSALLOCATE fs_info;
    VIOMODEINFO vio_mode;
    BYTE fill[2];
    BYTE c;
    char buf[80];
    USHORT row;
    USHORT col;
    USHORT cur_drive;
    ULONG drive_map;
    ULONG uptime_sec;
    ULONG bytes_per_clus;
    ULONG total_mb, free_mb;
    ULONG avail_bytes;
    ULONG avail_kb;

    /* Code page and country declarations */
    USHORT cp_list[4];
    USHORT cp_len;
    USHORT active_cp = 0;
    USHORT alt_cp = 0;
    BOOL is_fullscreen;
    COUNTRYCODE ctry_code;
    COUNTRYINFO ctry_info;

    /* Query OS/2 Version */
    DosGetVersion(&ver);
    major = ver >> 8;
    minor = ver & 0xFF;

    /* Query Info Segments */
    DosGetInfoSeg(&gdt_sel, &ldt_sel);
    ginf = (PGINFOSEG)MAKEP(gdt_sel, 0);
    linf = (PLINFOSEG)MAKEP(ldt_sel, 0);
    uptime_sec = ginf->msecs / 1000UL;

    /* Query Code Page: cp_list[0] = current, cp_list[1] = prepared/secondary */
    cp_list[0] = 0;
    cp_list[1] = 0;
    DosGetCp(sizeof(cp_list), cp_list, &cp_len);

    /* Query Country / Keyboard Info */
    memset(&ctry_code, 0, sizeof(ctry_code));
    DosGetCtryInfo(sizeof(ctry_info), &ctry_code, &ctry_info, &cp_len);

    /* Query Memory */
    avail_bytes = 0;
    DosMemAvail(&avail_bytes);
    avail_kb = avail_bytes / 1024UL;

    /* Query Disk */
    DosQCurDisk(&cur_drive, &drive_map);
    DosQFSInfo(0, FSIL_ALLOC, (PBYTE)&fs_info, sizeof(fs_info));
    bytes_per_clus = (ULONG)fs_info.cSectorUnit * fs_info.cbSector;
    total_mb = (fs_info.cUnit * bytes_per_clus) / (1024UL * 1024UL);
    free_mb  = (fs_info.cUnitAvail * bytes_per_clus) / (1024UL * 1024UL);

    /* Query Display */
    memset(&vio_mode, 0, sizeof(vio_mode));
    vio_mode.cb = sizeof(vio_mode);
    VioGetMode(&vio_mode, 0);

    /* Clear screen */
    fill[0] = ' ';
    fill[1] = 0x07;
    VioScrollUp(0, 0, 0xFFFF, 0xFFFF, 0xFFFF, (PCH)fill, 0);

    /* Draw Neon Diamond */
    draw_os2_logo();

    /* --- System Details (Column 36) --- */
    row = 1;
    print_color(row++, 36, "OS/2 Desktop        ", 0x0A);
    print_color(row++, 36, "--------------------", 0x08);

    if (major >= 10) {
        sprintf(buf, "OS:         MS OS/2 %u.%02u", major / 10, minor);
    } else {
        sprintf(buf, "OS:         MS OS/2 %u.%02u", major, minor);
    }
    print_color(row++, 36, buf, 0x07);

    sprintf(buf, "Uptime:     %lu min", uptime_sec / 60UL);
    print_color(row++, 36, buf, 0x07);

    sprintf(buf, "Avail Mem:  %lu.%02lu MB (%lu KB)",
            avail_kb / 1024UL,
            ((avail_kb % 1024UL) * 100UL) / 1024UL,
            avail_kb);
    print_color(row++, 36, buf, 0x07);

    sprintf(buf, "Display:    %u x %u (VIO Mode %u)", vio_mode.col, vio_mode.row, vio_mode.fbType);
    print_color(row++, 36, buf, 0x07);

   is_fullscreen = (ginf->sgCurrent != 1);

    sprintf(buf, "Fullscreen: %s", is_fullscreen ? "Yes" : "No");
    print_color(row++, 36, buf, 0x07);

    /* Grab active code page after DosGetCp has populated cp_list */
    active_cp = cp_list[0];

    /* Resolve secondary prepared code page (850) */
    if (cp_list[1] != 0 && cp_list[1] != active_cp) {
        alt_cp = cp_list[1];
    } else if (cp_list[2] != 0 && cp_list[2] != active_cp) {
        alt_cp = cp_list[2];
    }

    /* Format Code Page line into buf before printing */
    if (alt_cp > 0) {
        sprintf(buf, "Code Page:  (%u), %u", active_cp, alt_cp);
    } else {
        sprintf(buf, "Code Page:  (%u)", active_cp);
    }
    print_color(row++, 36, buf, 0x07);

    sprintf(buf, "Keyboard:   %s", get_keyboard_name(ctry_info.country));
    print_color(row++, 36, buf, 0x07);

    sprintf(buf, "Disk (%c:):  %lu MB free / %lu MB total", 'A' + (cur_drive - 1), free_mb, total_mb);
    print_color(row++, 36, buf, 0x07);

    sprintf(buf, "Cur Drv:    Drive %c:", 'A' + (cur_drive - 1));
    print_color(row++, 36, buf, 0x07);

    /* Color Palette Blocks */
    row++;
    col = 36;
    for (c = 1; c <= 7; c++) {
        print_color(row, col, "   ", (BYTE)(c << 4));
        col += 4;
    }

    VioSetCurPos(14, 0, 0);
    return 0;
}
