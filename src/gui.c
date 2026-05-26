#include "gui.h"
#include "keyboard.h"
#include "mouse.h"
#include "fb.h"
#include "kprintf.h"
#include "rtc.h"
#include "string.h"
#include "system.h"
#include "timer.h"
#include "vga.h"
#include "hw.h"

#define KEY_ESC 27
#define KEY_TAB 9
#define KEY_ENTER '\n'
#define KEY_BS '\b'

#define CON_LINES 64
#define CON_LINE_LEN 132
#define INPUT_LEN 112
#define START_ITEMS 8
#define THEME_COUNT 4

#define EDIT_FILE_MAX 10
#define EDIT_NAME_MAX 28
#define EDIT_LINES_MAX 96
#define EDIT_LINE_MAX 128

typedef enum { APP_NONE=0, APP_EXPLORER=1, APP_CONSOLE=2, APP_CODE=3, APP_GPUX=4, APP_SETTINGS=5, APP_ABOUT=6, APP_HELP=7, APP_COUNT=8 } app_id_t;
typedef enum { PATH_ROOT=0, PATH_DESKTOP, PATH_DOCUMENTS, PATH_APPS, PATH_SYSTEM, PATH_PROC } path_id_t;

typedef struct { int open,x,y,w,h; const char* title; } gui_window_t;
typedef struct { const char* name; int is_dir; path_id_t next; app_id_t app; const char* preview; } vfs_item_t;
typedef struct { const char* name; uint8_t r1,g1,b1,r2,g2,b2,accent_r,accent_g,accent_b,icon_r,icon_g,icon_b; } theme_t;

typedef struct { const char* name; const char* help; } cmd_info_t;

static gui_window_t windows[APP_COUNT] = {
    {0,0,0,0,0,""},
    {1,70,105,560,405,"Explorer"},
    {1,290,165,650,390,"Console"},
    {0,180,95,760,530,"Code Studio"},
    {0,380,118,560,420,"GPUx"},
    {0,430,150,470,360,"Settings"},
    {0,330,110,575,400,"About SacramentuOS"},
    {0,220,90,670,430,"Help"},
};

static app_id_t active_app = APP_CONSOLE;
static int start_menu_open = 0, start_selected = 0, drag_app = 0, drag_dx = 0, drag_dy = 0;
static int frame_id = 0;
static int theme_index = 0;
static int settings_selected = 0;

static path_id_t explorer_path = PATH_ROOT;
static int explorer_selected = 0;
static char explorer_status[CON_LINE_LEN] = "Ready.";

static int console_count = 0;
static char console_lines[CON_LINES][CON_LINE_LEN];
static char console_input[INPUT_LEN];
static int console_input_len = 0;
static char history[16][INPUT_LEN];
static int history_count = 0;
static char notes[8][96]; static int note_count = 0;
static char todos[8][96]; static uint8_t todo_done[8]; static int todo_count = 0;
static char syslog_buf[16][96]; static int syslog_count = 0;

static char edit_names[EDIT_FILE_MAX][EDIT_NAME_MAX];
static char edit_lines[EDIT_FILE_MAX][EDIT_LINES_MAX][EDIT_LINE_MAX];
static int edit_line_counts[EDIT_FILE_MAX];
static int edit_file_count = 0;
static int code_file = -1, code_cx = 0, code_cy = 0, code_top = 0, code_col = 0, code_dirty = 0;
static char code_status[96] = "Open a file with code <name> or edit <name>.";

static uint32_t C_DESKTOP_TOP, C_DESKTOP_BOT, C_PANEL, C_PANEL_DARK, C_PANEL_LIGHT;
static uint32_t C_TEXT, C_TEXT_DIM, C_WHITE, C_BLACK, C_BLUE, C_BLUE_DARK, C_ACCENT, C_ICON;
static uint32_t C_WINDOW, C_WINDOW_BORDER, C_TITLE, C_TITLE_INACTIVE, C_CONSOLE, C_CONSOLE_TEXT;
static uint32_t C_SHADOW, C_SELECT, C_ERROR, C_GOOD, C_EDITOR_BG, C_EDITOR_GUTTER, C_EDITOR_TEXT;

static const theme_t themes[THEME_COUNT] = {
    {"classic blue", 24,112,170, 8,64,132, 0,120,215, 0,96,190},
    {"graphite", 54,64,76, 28,32,38, 95,125,155, 95,110,130},
    {"olive", 71,92,82, 34,50,44, 112,136,92, 98,130,96},
    {"wine", 92,48,70, 45,28,42, 145,76,108, 132,70,105}
};

static const char* start_items[START_ITEMS] = { "Explorer", "Console", "Code Studio", "GPUx", "Settings", "About", "Help", "Reboot" };

static const cmd_info_t command_table[] = {
    {"help","open docs or command help"},{"docs","built-in documentation"},{"commands","compact command list"},{"find","search command names/help"},
    {"clear","clear console"},{"clean","alias for clear"},{"fetch","system fetch"},{"sysinfo","alias for fetch"},{"about","open/about text"},
    {"echo","print text"},{"uname","kernel name"},{"whoami","current user"},{"mem","memory"},{"uptime","uptime"},{"ticks","PIT ticks"},
    {"date","RTC date"},{"time","RTC time"},{"calc","calc A +|-|*|/|% B"},{"color","change theme by number/name"},{"theme","change theme"},
    {"history","command history"},{"len","character count"},{"upper","uppercase"},{"lower","lowercase"},{"reverse","reverse text"},{"repeat","repeat text"},
    {"ascii","print ASCII table"},{"hexdump","hex dump text"},{"ls","list virtual files"},{"dir","alias for ls"},{"cat","show virtual/RAM file"},
    {"pwd","current path"},{"ver","system version"},{"note","RAM notes"},{"notes","list notes"},{"noteclr","clear notes"},
    {"todo","RAM todo list"},{"done","mark todo done"},{"todoclr","clear todos"},{"base","decimal/hex/binary"},{"hash","FNV-1a checksum"},
    {"rot13","ROT13 text"},{"count","chars and words"},{"edit","open Code Studio"},{"code","open Code Studio"},{"files","RAM code files"},
    {"show","show RAM/virtual file"},{"rm","remove RAM file"},{"ps","task list"},{"top","system snapshot"},{"kernel","kernel diagnostics"},
    {"log","system log"},{"explorer","open Explorer"},{"settings","open Settings"},{"gpux","open GPUx"},{"reboot","reboot"},{"halt","halt CPU"},{"shutdown","halt CPU"}
};
static const int command_count = (int)(sizeof(command_table)/sizeof(command_table[0]));

static const vfs_item_t items_root[] = {
    {"Desktop",1,PATH_DESKTOP,APP_NONE,"Desktop shortcuts and user workspace."},
    {"Documents",1,PATH_DOCUMENTS,APP_NONE,"RAM documents and saved code files."},
    {"Apps",1,PATH_APPS,APP_NONE,"Installed graphical applications."},
    {"System",1,PATH_SYSTEM,APP_NONE,"System configuration and build files."},
    {"Proc",1,PATH_PROC,APP_NONE,"Live system information."},
    {"README.TXT",0,PATH_ROOT,APP_NONE,"SacramentuOS 1.0.1 graphical pixel desktop."}
};
static const vfs_item_t items_desktop[] = {
    {"..",1,PATH_ROOT,APP_NONE,"Go back."},{"Explorer.app",0,PATH_DESKTOP,APP_EXPLORER,"File manager."},{"Console.app",0,PATH_DESKTOP,APP_CONSOLE,"Command shell."},
    {"CodeStudio.app",0,PATH_DESKTOP,APP_CODE,"Lightweight code editor."},{"GPUx.app",0,PATH_DESKTOP,APP_GPUX,"Hardware and load monitor."},{"Settings.app",0,PATH_DESKTOP,APP_SETTINGS,"Desktop personalization."}
};
static const vfs_item_t items_docs[] = {
    {"..",1,PATH_ROOT,APP_NONE,"Go back."},{"NOTES.TXT",0,PATH_DOCUMENTS,APP_NONE,"Use note add/list/clear in Console."},{"TODO.TXT",0,PATH_DOCUMENTS,APP_NONE,"Use todo add/list/clear and done N."},{"CODE-FILES",1,PATH_DOCUMENTS,APP_NONE,"RAM files created by Code Studio."}
};
static const vfs_item_t items_apps[] = {
    {"..",1,PATH_ROOT,APP_NONE,"Go back."},{"Explorer.app",0,PATH_APPS,APP_EXPLORER,"File manager."},{"Console.app",0,PATH_APPS,APP_CONSOLE,"Command shell."},
    {"CodeStudio.app",0,PATH_APPS,APP_CODE,"Code editor."},{"GPUx.app",0,PATH_APPS,APP_GPUX,"Hardware monitor."},{"Settings.app",0,PATH_APPS,APP_SETTINGS,"Theme and colors."},{"About.app",0,PATH_APPS,APP_ABOUT,"System information."},{"Help.app",0,PATH_APPS,APP_HELP,"Short manual."}
};
static const vfs_item_t items_system[] = {
    {"..",1,PATH_ROOT,APP_NONE,"Go back."},{"os-release.txt",0,PATH_SYSTEM,APP_NONE,"NAME=SacramentuOS VERSION=1.0.1"},{"display.txt",0,PATH_SYSTEM,APP_NONE,"VBE/GOP framebuffer, bitmap font, primitives."},{"input.txt",0,PATH_SYSTEM,APP_NONE,"PS/2 mouse, UEFI pointer, USB probe."},{"build.txt",0,PATH_SYSTEM,APP_NONE,"freestanding C + GNU Assembly."}
};
static const vfs_item_t items_proc[] = {
    {"..",1,PATH_ROOT,APP_NONE,"Go back."},{"cpuinfo",0,PATH_PROC,APP_NONE,"CPUID processor information."},{"meminfo",0,PATH_PROC,APP_NONE,"Memory from Multiboot."},{"pci",0,PATH_PROC,APP_NONE,"Detected PCI devices."},{"usb",0,PATH_PROC,APP_NONE,"Detected USB host controllers."},{"uptime",0,PATH_PROC,APP_NONE,"PIT timer uptime."}
};

static int sw(void){ return (int)fb_width(); }
static int sh(void){ return (int)fb_height(); }
static int taskbar_h(void){ return 42; }

static void apply_theme(void){
    const theme_t* t=&themes[theme_index];
    C_DESKTOP_TOP=fb_rgb(t->r1,t->g1,t->b1); C_DESKTOP_BOT=fb_rgb(t->r2,t->g2,t->b2);
    C_ACCENT=fb_rgb(t->accent_r,t->accent_g,t->accent_b); C_ICON=fb_rgb(t->icon_r,t->icon_g,t->icon_b);
    C_PANEL=fb_rgb(238,238,238); C_PANEL_DARK=fb_rgb(178,178,178); C_PANEL_LIGHT=fb_rgb(255,255,255);
    C_TEXT=fb_rgb(20,20,20); C_TEXT_DIM=fb_rgb(85,85,85); C_WHITE=fb_rgb(255,255,255); C_BLACK=fb_rgb(0,0,0);
    C_BLUE=C_ACCENT; C_BLUE_DARK=fb_rgb(0,50,110); C_WINDOW=fb_rgb(244,244,244); C_WINDOW_BORDER=fb_rgb(90,90,90);
    C_TITLE=C_ACCENT; C_TITLE_INACTIVE=fb_rgb(126,145,164); C_CONSOLE=fb_rgb(5,14,6); C_CONSOLE_TEXT=fb_rgb(135,255,135);
    C_SHADOW=fb_rgb(24,24,24); C_SELECT=fb_rgb(70,118,200); C_ERROR=fb_rgb(190,35,35); C_GOOD=fb_rgb(28,145,70);
    C_EDITOR_BG=fb_rgb(28,31,36); C_EDITOR_GUTTER=fb_rgb(42,45,52); C_EDITOR_TEXT=fb_rgb(220,225,230);
}
static void init_colors(void){ theme_index=0; apply_theme(); }
static void draw_text(int x,int y,const char*s,uint32_t fg){ fb_draw_text_transparent(x,y,s,fg,2); }
static void draw_text_small(int x,int y,const char*s,uint32_t fg){ fb_draw_text_transparent(x,y,s,fg,1); }
static int in_rect(int px,int py,int x,int y,int w,int h){ return px>=x && py>=y && px<x+w && py<y+h; }

static int streqi(const char*a,const char*b){ while(*a&&*b){ char ca=*a, cb=*b; if(ca>='A'&&ca<='Z')ca+=32; if(cb>='A'&&cb<='Z')cb+=32; if(ca!=cb)return 0; a++; b++; } return *a==0 && *b==0; }
static int contains_i(const char* h,const char* n){ if(!n||!n[0])return 1; for(int i=0;h&&h[i];i++){ int j=0; while(n[j]&&h[i+j]){ char a=h[i+j],b=n[j]; if(a>='A'&&a<='Z')a+=32; if(b>='A'&&b<='Z')b+=32; if(a!=b)break; j++; } if(!n[j])return 1; } return 0; }
static char* next_token(char** cursor){ char*s=trim_left(*cursor); if(!*s){*cursor=s; return 0;} char* start=s; while(*s&&!is_space(*s))s++; if(*s){*s=0; s++;} *cursor=s; return start; }
static void append_num(char* out,uint32_t v){ char b[16]; utoa(v,b,10); strcat(out,b); }
static void append_hex(char* out,uint32_t v){ char b[16]; strcat(out,"0x"); utoa(v,b,16); strcat(out,b); }

static void console_add(const char* text){ if(!text)return; if(console_count<CON_LINES){strncpy(console_lines[console_count],text,CON_LINE_LEN-1);console_lines[console_count][CON_LINE_LEN-1]=0;console_count++;return;} for(int i=1;i<CON_LINES;i++)strcpy(console_lines[i-1],console_lines[i]); strncpy(console_lines[CON_LINES-1],text,CON_LINE_LEN-1);console_lines[CON_LINES-1][CON_LINE_LEN-1]=0; }
static void log_event(const char* text){ if(!text||!text[0])return; if(syslog_count<16){strncpy(syslog_buf[syslog_count],text,95);syslog_buf[syslog_count][95]=0;syslog_count++;return;} for(int i=1;i<16;i++)strcpy(syslog_buf[i-1],syslog_buf[i]); strncpy(syslog_buf[15],text,95);syslog_buf[15][95]=0; }
static void add_history(const char* line){ if(!line||!line[0])return; if(history_count<16){strncpy(history[history_count],line,INPUT_LEN-1);history[history_count][INPUT_LEN-1]=0;history_count++;return;} for(int i=1;i<16;i++)strcpy(history[i-1],history[i]); strncpy(history[15],line,INPUT_LEN-1);history[15][INPUT_LEN-1]=0; }

static void fit_window(gui_window_t*w){ if(w->w<220)w->w=220; if(w->h<150)w->h=150; if(w->w>sw()-20)w->w=sw()-20; if(w->h>sh()-taskbar_h()-20)w->h=sh()-taskbar_h()-20; if(w->x<5)w->x=5; if(w->y<5)w->y=5; if(w->x+w->w>sw()-5)w->x=sw()-w->w-5; if(w->y+w->h>sh()-taskbar_h()-5)w->y=sh()-taskbar_h()-w->h-5; }
static void open_app(app_id_t app){ if(app<=APP_NONE||app>=APP_COUNT)return; windows[app].open=1; fit_window(&windows[app]); active_app=app; start_menu_open=0; }
static void close_app(app_id_t app){ if(app<=APP_NONE||app>=APP_COUNT)return; windows[app].open=0; if(active_app==app){active_app=APP_NONE; for(int i=1;i<APP_COUNT;i++)if(windows[i].open){active_app=(app_id_t)i;break;}} }
static void cycle_active(void){ int start=(int)active_app; for(int step=1;step<APP_COUNT;step++){int i=(start+step)%APP_COUNT; if(i>0&&windows[i].open){active_app=(app_id_t)i;return;}} }

static int ram_file_find(const char* name){ if(!name||!name[0])return -1; for(int i=0;i<edit_file_count;i++)if(strcmp(edit_names[i],name)==0)return i; return -1; }
static int ram_file_alloc(const char* name){ int idx=ram_file_find(name); if(idx>=0)return idx; if(edit_file_count>=EDIT_FILE_MAX)return -1; idx=edit_file_count++; strncpy(edit_names[idx],name,EDIT_NAME_MAX-1); edit_names[idx][EDIT_NAME_MAX-1]=0; edit_line_counts[idx]=1; for(int l=0;l<EDIT_LINES_MAX;l++)edit_lines[idx][l][0]=0; return idx; }
static void code_open_file(const char* name){ if(!name||!name[0])name="main.c"; int idx=ram_file_alloc(name); if(idx<0){console_add("CODE STUDIO STORAGE IS FULL. USE RM <FILE>.");return;} code_file=idx; code_cx=0; code_cy=0; code_top=0; code_col=0; code_dirty=0; strcpy(code_status,"Editing RAM file. F7 save, F8 save+close."); open_app(APP_CODE); }
static int line_len(const char*s){ return (int)strlen(s); }
static void editor_insert_char(char* line,int pos,char c){ int len=line_len(line); if(len>=EDIT_LINE_MAX-1)return; if(pos<0)pos=0;if(pos>len)pos=len; for(int i=len;i>=pos;i--)line[i+1]=line[i]; line[pos]=c; }
static void editor_delete_char(char* line,int pos){ int len=line_len(line); if(pos<0||pos>=len)return; for(int i=pos;i<len;i++)line[i]=line[i+1]; }
static void editor_clamp(void){ if(code_file<0)return; int count=edit_line_counts[code_file]; if(count<1)count=1; if(code_cy<0)code_cy=0; if(code_cy>=count)code_cy=count-1; int len=line_len(edit_lines[code_file][code_cy]); if(code_cx<0)code_cx=0; if(code_cx>len)code_cx=len; if(code_cy<code_top)code_top=code_cy; if(code_cy>=code_top+28)code_top=code_cy-27; if(code_top<0)code_top=0; if(code_cx<code_col)code_col=code_cx; if(code_cx>=code_col+92)code_col=code_cx-91; if(code_col<0)code_col=0; }

static const vfs_item_t* explorer_items(int* count){ switch(explorer_path){case PATH_DESKTOP:*count=(int)(sizeof(items_desktop)/sizeof(items_desktop[0]));return items_desktop;case PATH_DOCUMENTS:*count=(int)(sizeof(items_docs)/sizeof(items_docs[0]));return items_docs;case PATH_APPS:*count=(int)(sizeof(items_apps)/sizeof(items_apps[0]));return items_apps;case PATH_SYSTEM:*count=(int)(sizeof(items_system)/sizeof(items_system[0]));return items_system;case PATH_PROC:*count=(int)(sizeof(items_proc)/sizeof(items_proc[0]));return items_proc;default:*count=(int)(sizeof(items_root)/sizeof(items_root[0]));return items_root;} }
static const char* path_name(path_id_t p){ switch(p){case PATH_DESKTOP:return "C:\\SACRAMENTUOS\\DESKTOP";case PATH_DOCUMENTS:return "C:\\SACRAMENTUOS\\DOCUMENTS";case PATH_APPS:return "C:\\SACRAMENTUOS\\APPS";case PATH_SYSTEM:return "C:\\SACRAMENTUOS\\SYSTEM";case PATH_PROC:return "C:\\SACRAMENTUOS\\PROC";default:return "C:\\SACRAMENTUOS";} }
static void explorer_open_selected(void){ int cnt; const vfs_item_t* it=explorer_items(&cnt); if(explorer_selected<0||explorer_selected>=cnt)return; const vfs_item_t* e=&it[explorer_selected]; if(e->is_dir){ explorer_path=e->next; explorer_selected=0; strcpy(explorer_status,"Folder opened."); } else if(e->app){ open_app(e->app); strcpy(explorer_status,"Application opened."); } else { strcpy(explorer_status,e->preview?e->preview:"File selected."); } }

static const char* virtual_file_content(const char* path){
    if(streqi(path,"/etc/os-release")||streqi(path,"etc/os-release")||streqi(path,"os-release.txt")) return "NAME=SacramentuOS\nVERSION=1.0.1\nCODENAME=Pixel Desktop\nARCH=i386\n";
    if(streqi(path,"/help")||streqi(path,"help")||streqi(path,"docs")) return "SacramentuOS help: use help, commands, fetch, explorer, code, gpux, settings.\n";
    if(streqi(path,"/proc/uptime")||streqi(path,"proc/uptime")||streqi(path,"uptime")) return "Use uptime command for live value.\n";
    if(streqi(path,"/proc/cpuinfo")||streqi(path,"proc/cpuinfo")||streqi(path,"cpuinfo")) return "Use fetch or GPUx for live CPUID values.\n";
    return 0;
}

static void draw_desktop_background(void){
    int h=sh(), w=sw(); const theme_t*t=&themes[theme_index];
    for(int y=0;y<h;y+=4){ uint8_t r=(uint8_t)(t->r1 + ((int)t->r2-(int)t->r1)*y/(h?h:1)); uint8_t g=(uint8_t)(t->g1 + ((int)t->g2-(int)t->g1)*y/(h?h:1)); uint8_t b=(uint8_t)(t->b1 + ((int)t->b2-(int)t->b1)*y/(h?h:1)); fb_fill_rect(0,y,w,4,fb_rgb(r,g,b)); }
    fb_line(w-320,90,w-35,330,fb_rgb(t->accent_r+20>255?255:t->accent_r+20,t->accent_g+20>255?255:t->accent_g+20,t->accent_b+20>255?255:t->accent_b+20));
    fb_line(w-330,92,w-45,332,fb_rgb(t->r2,t->g2,t->b2));
    draw_text(22,18,"SacramentuOS",C_WHITE);
    draw_text_small(24,44,"version 1.0.1",fb_rgb(215,235,250));
}
static void draw_icon(int x,int y,const char* label,uint32_t color){ fb_fill_rect(x+8,y,36,28,C_PANEL_LIGHT); fb_rect(x+8,y,36,28,color); fb_fill_rect(x+14,y+7,24,14,color); draw_text_small(x,y+38,label,C_WHITE); }
static void draw_desktop_icons(void){ draw_icon(36,78,"EXPLORER",C_ICON); draw_icon(36,150,"CONSOLE",C_GOOD); draw_icon(36,222,"CODE",fb_rgb(92,110,135)); draw_icon(36,294,"GPUX",fb_rgb(80,150,130)); draw_icon(36,366,"SETTINGS",fb_rgb(150,150,150)); draw_icon(36,438,"ABOUT",fb_rgb(150,90,210)); }
static void draw_button(int x,int y,int w,int h,const char* label,int hot){ uint32_t fill=hot?C_BLUE:C_PANEL; uint32_t fg=hot?C_WHITE:C_TEXT; fb_fill_rect(x,y,w,h,fill); fb_rect(x,y,w,h,hot?C_BLUE_DARK:C_PANEL_DARK); draw_text_small(x+10,y+(h-7)/2,label,fg); }
static void draw_taskbar(void){ int y=sh()-taskbar_h(); fb_fill_rect(0,y,sw(),taskbar_h(),C_PANEL); fb_fill_rect(0,y,sw(),1,C_PANEL_LIGHT); draw_button(8,y+7,92,28,"START",start_menu_open); int bx=112; for(int i=1;i<APP_COUNT;i++){ if(!windows[i].open)continue; int bw=118; if(i==APP_CODE)bw=132; draw_button(bx,y+8,bw,26,windows[i].title,active_app==(app_id_t)i); bx+=bw+8; }
    rtc_time_t t=rtc_read_time(); char buf[32]; buf[0]=0; char n[8]; if(t.day<10)strcat(buf,"0"); utoa(t.day,n,10); strcat(buf,n); strcat(buf,"."); if(t.month<10)strcat(buf,"0"); utoa(t.month,n,10); strcat(buf,n); strcat(buf,"  "); if(t.hour<10)strcat(buf,"0"); utoa(t.hour,n,10); strcat(buf,n); strcat(buf,":"); if(t.minute<10)strcat(buf,"0"); utoa(t.minute,n,10); strcat(buf,n); draw_text_small(sw()-112,y+10,buf,C_TEXT); }
static void draw_start_menu(void){ if(!start_menu_open)return; int x=8,y=sh()-taskbar_h()-315,w=270,h=310; fb_fill_rect(x+5,y+5,w,h,C_SHADOW); fb_fill_rect(x,y,w,h,C_PANEL); fb_rect(x,y,w,h,C_WINDOW_BORDER); fb_fill_rect(x,y,w,48,C_BLUE); draw_text(x+18,y+16,"SacramentuOS",C_WHITE); for(int i=0;i<START_ITEMS;i++){int iy=y+60+i*29; int hot=i==start_selected; fb_fill_rect(x+8,iy,w-16,25,hot?C_SELECT:C_PANEL); draw_text_small(x+20,iy+9,start_items[i],hot?C_WHITE:C_TEXT);} }
static void draw_window_frame(app_id_t app){ gui_window_t*w=&windows[app]; fit_window(w); int active=(app==active_app); fb_fill_rect(w->x+8,w->y+8,w->w,w->h,C_SHADOW); fb_fill_rect(w->x,w->y,w->w,w->h,C_WINDOW); fb_rect(w->x,w->y,w->w,w->h,C_WINDOW_BORDER); fb_fill_rect(w->x+1,w->y+1,w->w-2,28,active?C_TITLE:C_TITLE_INACTIVE); draw_text_small(w->x+10,w->y+10,w->title,C_WHITE); fb_fill_rect(w->x+w->w-34,w->y+3,30,22,active?C_ERROR:C_PANEL_DARK); draw_text_small(w->x+w->w-23,w->y+10,"X",C_WHITE); }

static void draw_explorer(void){ gui_window_t*w=&windows[APP_EXPLORER]; draw_window_frame(APP_EXPLORER); int x=w->x+14,y=w->y+42; fb_fill_rect(x,y,w->w-28,24,C_WHITE); fb_rect(x,y,w->w-28,24,C_PANEL_DARK); draw_text_small(x+8,y+9,path_name(explorer_path),C_TEXT_DIM); int list_y=y+38; int list_w=230; fb_fill_rect(x,list_y,list_w,w->h-106,C_WHITE); fb_rect(x,list_y,list_w,w->h-106,C_PANEL_DARK); int cnt; const vfs_item_t* it=explorer_items(&cnt); for(int i=0;i<cnt;i++){int iy=list_y+8+i*23; int hot=i==explorer_selected&&active_app==APP_EXPLORER; fb_fill_rect(x+4,iy-3,list_w-8,19,hot?C_SELECT:C_WHITE); char label[64]; label[0]=0; strcat(label,it[i].is_dir?"[DIR] ":"      "); strcat(label,it[i].name); draw_text_small(x+10,iy+2,label,hot?C_WHITE:C_TEXT);} int px=x+list_w+18; int pw=w->w-list_w-60; fb_fill_rect(px,list_y,pw,w->h-106,C_WHITE); fb_rect(px,list_y,pw,w->h-106,C_PANEL_DARK); draw_text(px+18,list_y+20,"Preview",C_TEXT); const char* prev=(explorer_selected>=0&&explorer_selected<cnt)?it[explorer_selected].preview:""; draw_text_small(px+20,list_y+58,prev,C_TEXT_DIM); draw_text_small(px+20,list_y+84,"Double-open: Enter or click selected item.",C_TEXT_DIM); draw_text_small(px+20,list_y+110,"Folders are navigable. Apps launch from Apps/Desktop.",C_TEXT_DIM); draw_text_small(x,w->y+w->h-28,explorer_status,C_TEXT_DIM); }

static void console_prompt_line(char*dst,int max){ int pos=0; const char*prefix="C:\\> "; while(prefix[pos]&&pos+1<max){dst[pos]=prefix[pos];pos++;} for(int i=0;console_input[i]&&pos+1<max;i++,pos++)dst[pos]=console_input[i]; dst[pos]=0; }
static void draw_console(void){ gui_window_t*w=&windows[APP_CONSOLE]; draw_window_frame(APP_CONSOLE); int x=w->x+12,y=w->y+40,cw=w->w-24,ch=w->h-56; fb_fill_rect(x,y,cw,ch,C_CONSOLE); fb_rect(x,y,cw,ch,fb_rgb(35,80,35)); int rows=(ch-34)/12; int start=console_count>rows?console_count-rows:0; for(int i=0;i<rows;i++){int idx=start+i; if(idx<console_count)draw_text_small(x+8,y+8+i*12,console_lines[idx],C_CONSOLE_TEXT);} char prompt[CON_LINE_LEN]; console_prompt_line(prompt,sizeof(prompt)); fb_fill_rect(x+4,y+ch-24,cw-8,18,C_BLACK); draw_text_small(x+8,y+ch-19,prompt,C_CONSOLE_TEXT); if(active_app==APP_CONSOLE&&((frame_id/20)&1)){int cx=x+8+(5+console_input_len)*6; fb_fill_rect(cx,y+ch-20,6,10,C_CONSOLE_TEXT);} }

static void draw_code(void){ gui_window_t*w=&windows[APP_CODE]; draw_window_frame(APP_CODE); int x=w->x+10,y=w->y+38,cw=w->w-20,ch=w->h-50; fb_fill_rect(x,y,cw,ch,C_EDITOR_BG); fb_rect(x,y,cw,ch,fb_rgb(70,75,85)); fb_fill_rect(x,y,cw,26,fb_rgb(38,42,50)); const char* name=(code_file>=0)?edit_names[code_file]:"<no file>"; char title[96]; strcpy(title,"Code Studio - "); strcat(title,name); if(code_dirty)strcat(title," *"); draw_text_small(x+10,y+10,title,C_EDITOR_TEXT); fb_fill_rect(x,y+26,46,ch-52,C_EDITOR_GUTTER); int rows=(ch-62)/12; if(code_file>=0){ editor_clamp(); int count=edit_line_counts[code_file]; for(int r=0;r<rows;r++){int li=code_top+r; int ty=y+36+r*12; char num[8]; if(li<count){utoa((uint32_t)(li+1),num,10); draw_text_small(x+8,ty,num,fb_rgb(145,150,158)); const char* line=edit_lines[code_file][li]; int len=line_len(line); char visible[100]; int vi=0; for(int c=code_col;c<len&&vi<96;c++)visible[vi++]=line[c]; visible[vi]=0; draw_text_small(x+56,ty,visible,C_EDITOR_TEXT);} } if(active_app==APP_CODE&&((frame_id/18)&1)){int caret_x=x+56+(code_cx-code_col)*6; int caret_y=y+36+(code_cy-code_top)*12; fb_fill_rect(caret_x,caret_y,2,10,C_WHITE);} } else draw_text(x+70,y+90,"No file opened",C_EDITOR_TEXT); fb_fill_rect(x,y+ch-24,cw,24,fb_rgb(38,42,50)); draw_text_small(x+10,y+ch-16,"F7 SAVE | F8 SAVE+CLOSE | F9 CLOSE | ARROWS MOVE | ENTER NEWLINE",fb_rgb(190,200,210)); draw_text_small(x+430,y+ch-16,code_status,fb_rgb(190,200,210)); }

static void draw_gpux_bar(int x,int y,int w,const char* label,int pct,uint32_t color){ if(pct<0)pct=0;if(pct>100)pct=100; draw_text_small(x,y,label,C_TEXT); fb_fill_rect(x+120,y-2,w,12,C_PANEL_DARK); fb_fill_rect(x+121,y-1,(w-2)*pct/100,10,color); fb_rect(x+120,y-2,w,12,C_WINDOW_BORDER); char b[16]; utoa((uint32_t)pct,b,10); strcat(b,"%"); draw_text_small(x+128+w,y,b,C_TEXT_DIM); }
static void draw_gpux(void){ gui_window_t*w=&windows[APP_GPUX]; draw_window_frame(APP_GPUX); int x=w->x+24,y=w->y+52; const hw_info_t*hi=hw_get_info(); draw_text(x,y,"GPUx",C_TEXT); draw_text_small(x,y+34,"Real hardware IDs are read via CPUID and PCI config space.",C_TEXT_DIM); char line[132]; strcpy(line,"CPU: "); strcat(line,hi->cpu_brand[0]?hi->cpu_brand:hi->cpu_vendor); draw_text_small(x,y+64,line,C_TEXT); strcpy(line,"CPU vendor: "); strcat(line,hi->cpu_vendor); strcat(line," signature="); append_hex(line,hi->cpu_signature); draw_text_small(x,y+84,line,C_TEXT_DIM); strcpy(line,"RAM: "); append_num(line,(g_mem_lower_kb+g_mem_upper_kb)/1024); strcat(line," MB reported by Multiboot"); draw_text_small(x,y+106,line,C_TEXT); strcpy(line,"Display PCI: "); if(hi->vga_vendor)hw_format_pci_id(line+strlen(line),hi->vga_vendor,hi->vga_device); else strcat(line,"not detected"); draw_text_small(x,y+128,line,C_TEXT); strcpy(line,"Storage PCI: "); if(hi->storage_vendor)hw_format_pci_id(line+strlen(line),hi->storage_vendor,hi->storage_device); else strcat(line,"not detected"); draw_text_small(x,y+150,line,C_TEXT); strcpy(line,"Network PCI: "); if(hi->net_vendor)hw_format_pci_id(line+strlen(line),hi->net_vendor,hi->net_device); else strcat(line,"not detected"); draw_text_small(x,y+172,line,C_TEXT); strcpy(line,"USB controllers: "); strcat(line,hi->usb_summary); draw_text_small(x,y+194,line,C_TEXT); int cpu=(int)((timer_ticks()*7u + frame_id*3u)%31u)+4; int ram=(edit_file_count*7 + console_count/2 + windows[APP_CODE].open*8); if(ram>88)ram=88; int gui=(int)((frame_id%100)); draw_gpux_bar(x,y+232,260,"CPU activity",cpu,C_GOOD); draw_gpux_bar(x,y+260,260,"GUI renderer",gui,C_ACCENT); draw_gpux_bar(x,y+288,260,"RAM buffers",ram,fb_rgb(120,100,190)); draw_text_small(x,y+320,"GPU load cannot be read on bare framebuffer yet; display device ID above is real.",C_TEXT_DIM); draw_text_small(x,y+340,"For full GPU telemetry the OS needs vendor GPU drivers/perf counters.",C_TEXT_DIM); }

static void draw_settings(void){ gui_window_t*w=&windows[APP_SETTINGS]; draw_window_frame(APP_SETTINGS); int x=w->x+24,y=w->y+52; draw_text(x,y,"Settings",C_TEXT); draw_text_small(x,y+38,"Classic desktop themes",C_TEXT_DIM); for(int i=0;i<THEME_COUNT;i++){int iy=y+68+i*42; int hot=i==settings_selected&&active_app==APP_SETTINGS; fb_fill_rect(x,iy,280,30,hot?C_SELECT:C_PANEL); fb_rect(x,iy,280,30,C_PANEL_DARK); fb_fill_rect(x+10,iy+7,42,16,fb_rgb(themes[i].icon_r,themes[i].icon_g,themes[i].icon_b)); draw_text_small(x+66,iy+11,themes[i].name,hot?C_WHITE:C_TEXT);} draw_text_small(x,y+250,"Enter/click applies theme. Background and icon color are updated.",C_TEXT_DIM); draw_text_small(x,y+272,"Console: theme classic|graphite|olive|wine or color 0..3",C_TEXT_DIM); }
static void draw_about(void){ gui_window_t*w=&windows[APP_ABOUT]; draw_window_frame(APP_ABOUT); int x=w->x+28,y=w->y+54; draw_text(x,y,"SacramentuOS 1.0.1",C_TEXT); const char* lines[]={"Educational hobby OS with a Windows-like pixel desktop shell.","Boot paths: legacy BIOS via GRUB Multiboot + VBE, and native UEFI BOOTX64.EFI + GOP.","Graphics: linear framebuffer, software backbuffer, 5x7 bitmap font, lines and rectangles.","Desktop: taskbar, Start menu, movable windows, Explorer, Console, Code Studio, GPUx.","Input: PS/2 keyboard/mouse in BIOS/QEMU; UEFI pointer protocols in BOOTX64.EFI.","Storage model: in-memory virtual filesystem. Code Studio saves files to RAM.","Hardware: CPUID and PCI config-space probe for CPU, display, USB, storage, network IDs.","Limitation: no real disk filesystem, scheduler, process isolation, or vendor GPU telemetry yet."}; for(int i=0;i<8;i++)draw_text_small(x,y+42+i*24,lines[i],i==7?C_TEXT_DIM:C_TEXT); }
static void draw_help(void){ gui_window_t*w=&windows[APP_HELP]; draw_window_frame(APP_HELP); int x=w->x+24,y=w->y+52; draw_text(x,y,"Help",C_TEXT); const char* lines[]={"Mouse: click Start, icons and window title bars. Drag title bars to move windows.","Explorer: click folders or press Enter. Use '..' to go back.","Console: all old shell commands are available: docs, fetch, calc, edit, notes, todo, files, top.","Code Studio: type code, use arrows, Enter, Backspace/Delete. F7 save, F8 save+close.","Settings: choose classic background/icon colors. Console: theme <name> or color <0..3>.","GPUx: shows real CPUID/PCI IDs and framebuffer/kernel activity counters.","Hotkeys: F1 Start, F2 Explorer, F3 Console, F4 About, F5 Settings, F6 Code, F7 GPUx.","Power: reboot, halt, shutdown from console."}; for(int i=0;i<8;i++)draw_text_small(x,y+45+i*24,lines[i],C_TEXT); }

static void draw_windows(void){ for(int i=1;i<APP_COUNT;i++)if(windows[i].open&&active_app!=(app_id_t)i){ if(i==APP_EXPLORER)draw_explorer(); else if(i==APP_CONSOLE)draw_console(); else if(i==APP_CODE)draw_code(); else if(i==APP_GPUX)draw_gpux(); else if(i==APP_SETTINGS)draw_settings(); else if(i==APP_ABOUT)draw_about(); else if(i==APP_HELP)draw_help(); } if(active_app>APP_NONE&&active_app<APP_COUNT&&windows[active_app].open){ if(active_app==APP_EXPLORER)draw_explorer(); else if(active_app==APP_CONSOLE)draw_console(); else if(active_app==APP_CODE)draw_code(); else if(active_app==APP_GPUX)draw_gpux(); else if(active_app==APP_SETTINGS)draw_settings(); else if(active_app==APP_ABOUT)draw_about(); else if(active_app==APP_HELP)draw_help(); } }
static void draw_cursor(int x,int y){ fb_line(x,y,x,y+20,C_BLACK); fb_line(x,y,x+14,y+14,C_BLACK); fb_line(x,y+20,x+5,y+16,C_BLACK); fb_line(x+5,y+16,x+8,y+24,C_BLACK); fb_line(x+8,y+24,x+12,y+22,C_BLACK); fb_line(x+12,y+22,x+9,y+14,C_BLACK); fb_line(x+9,y+14,x+14,y+14,C_BLACK); fb_fill_rect(x+2,y+4,2,12,C_WHITE); fb_fill_rect(x+4,y+6,2,10,C_WHITE); fb_fill_rect(x+6,y+8,2,8,C_WHITE); fb_fill_rect(x+8,y+10,2,4,C_WHITE); }
static void draw_pixel_desktop(mouse_state_t*ms){ draw_desktop_background(); draw_desktop_icons(); draw_windows(); draw_taskbar(); draw_start_menu(); draw_cursor(ms->x,ms->y); fb_present(); }

static app_id_t top_window_at(int x,int y){ if(active_app>APP_NONE&&active_app<APP_COUNT&&windows[active_app].open){gui_window_t*w=&windows[active_app]; if(in_rect(x,y,w->x,w->y,w->w,w->h))return active_app;} for(int i=APP_COUNT-1;i>=1;i--){ if((app_id_t)i==active_app||!windows[i].open)continue; gui_window_t*w=&windows[i]; if(in_rect(x,y,w->x,w->y,w->w,w->h))return (app_id_t)i;} return APP_NONE; }
static void handle_start_selection(int sel){ if(sel==0)open_app(APP_EXPLORER); else if(sel==1)open_app(APP_CONSOLE); else if(sel==2)code_open_file("main.c"); else if(sel==3)open_app(APP_GPUX); else if(sel==4)open_app(APP_SETTINGS); else if(sel==5)open_app(APP_ABOUT); else if(sel==6)open_app(APP_HELP); else if(sel==7)system_reboot(); }
static void apply_setting_selection(void){ theme_index=settings_selected; apply_theme(); }

static void handle_mouse(mouse_state_t*ms){ if(drag_app&&(ms->buttons&MOUSE_LEFT)){gui_window_t*w=&windows[drag_app]; w->x=ms->x-drag_dx; w->y=ms->y-drag_dy; fit_window(w); return;} if(ms->left_released)drag_app=0; if(!ms->left_pressed)return; int ty=sh()-taskbar_h(); if(in_rect(ms->x,ms->y,8,ty+7,92,28)){start_menu_open=!start_menu_open;return;} if(start_menu_open){int mx0=8,my0=sh()-taskbar_h()-315; for(int i=0;i<START_ITEMS;i++){int iy=my0+60+i*29; if(in_rect(ms->x,ms->y,mx0+8,iy,254,25)){start_selected=i;handle_start_selection(i);return;}} start_menu_open=0;} if(in_rect(ms->x,ms->y,36,78,76,60)){open_app(APP_EXPLORER);return;} if(in_rect(ms->x,ms->y,36,150,76,60)){open_app(APP_CONSOLE);return;} if(in_rect(ms->x,ms->y,36,222,76,60)){code_open_file("main.c");return;} if(in_rect(ms->x,ms->y,36,294,76,60)){open_app(APP_GPUX);return;} if(in_rect(ms->x,ms->y,36,366,76,60)){open_app(APP_SETTINGS);return;} if(in_rect(ms->x,ms->y,36,438,76,60)){open_app(APP_ABOUT);return;} app_id_t hit=top_window_at(ms->x,ms->y); if(hit!=APP_NONE){ active_app=hit; gui_window_t*w=&windows[hit]; if(in_rect(ms->x,ms->y,w->x+w->w-38,w->y,38,30)){close_app(hit);return;} if(in_rect(ms->x,ms->y,w->x,w->y,w->w,30)){drag_app=hit; drag_dx=ms->x-w->x; drag_dy=ms->y-w->y; return;} if(hit==APP_EXPLORER){int lx=w->x+14,ly=w->y+42+38; int cnt; explorer_items(&cnt); for(int i=0;i<cnt;i++){int iy=ly+8+i*23; if(in_rect(ms->x,ms->y,lx+4,iy-3,222,19)){explorer_selected=i; explorer_open_selected(); return;}}} if(hit==APP_SETTINGS){int sx=w->x+24,sy=w->y+52+68; for(int i=0;i<THEME_COUNT;i++){int iy=sy+i*42; if(in_rect(ms->x,ms->y,sx,iy,280,30)){settings_selected=i;apply_setting_selection();return;}}} } }

static uint32_t parse_uint_any(const char*s){ uint32_t value=0; int base=10; while(is_space(*s))s++; if(s[0]=='0'&&(s[1]=='x'||s[1]=='X')){base=16;s+=2;} else if(s[0]=='0'&&(s[1]=='b'||s[1]=='B')){base=2;s+=2;} while(*s){int d=-1;if(*s>='0'&&*s<='9')d=*s-'0';else if(*s>='a'&&*s<='f')d=*s-'a'+10;else if(*s>='A'&&*s<='F')d=*s-'A'+10;else break;if(d<0||d>=base)break;value=value*(uint32_t)base+(uint32_t)d;s++;}return value; }
static void print_binary_line(uint32_t v){ char line[48]; strcpy(line,"bin: 0b"); int started=0; char bit[2]={0,0}; for(int b=31;b>=0;b--){uint32_t m=1u<<b;if(v&m)started=1;if(started||b==0){bit[0]=(v&m)?'1':'0';strcat(line,bit);}} console_add(line); }
static void console_show_file(const char* name){ int idx=ram_file_find(name); if(idx>=0){ for(int i=0;i<edit_line_counts[idx];i++)console_add(edit_lines[idx][i]); return; } const char* v=virtual_file_content(name); if(v){ char tmp[CON_LINE_LEN]; int p=0; for(int i=0;;i++){ char c=v[i]; if(c=='\n'||c==0){ tmp[p]=0; console_add(tmp); p=0; if(c==0)break; } else if(p<CON_LINE_LEN-1)tmp[p++]=c; } return; } if(streqi(name,"/proc/meminfo")||streqi(name,"proc/meminfo")){ char l[80]; strcpy(l,"MemTotal: "); append_num(l,g_mem_lower_kb+g_mem_upper_kb); strcat(l," KB"); console_add(l); return; } console_add("FILE NOT FOUND. TRY LS OR FILES."); }
static void console_exec(void){ char shown[CON_LINE_LEN]; console_prompt_line(shown,sizeof(shown)); console_add(shown); char raw[INPUT_LEN]; strncpy(raw,console_input,sizeof(raw)-1); raw[sizeof(raw)-1]=0; trim_right(raw); char* input=trim_left(raw); if(!input[0]){console_input[0]=0;console_input_len=0;return;} add_history(input); log_event(input); char* cursor=input; char* cmd=next_token(&cursor); char* args=trim_left(cursor);
    if(streqi(cmd,"help")){ if(args[0]){int found=0; for(int i=0;i<command_count;i++)if(streqi(command_table[i].name,args)){char l[100];strcpy(l,command_table[i].name);strcat(l," - ");strcat(l,command_table[i].help);console_add(l);found=1;} if(!found)console_add("NO HELP FOR THAT COMMAND."); } else {console_add("SYSTEM: fetch top gpux kernel ps mem uptime ticks date time uname whoami ver");console_add("FILES : ls cat files show rm pwd explorer");console_add("EDITOR: edit <file> or code <file>; F7 save, F8 save+close");console_add("TEXT  : calc len count upper lower reverse repeat base hash rot13 hexdump ascii");console_add("TASKS : note add/list/clear, todo add/list/clear, done N");}}
    else if(streqi(cmd,"docs")){console_add("SacramentuOS docs: use Start menu or commands. Explorer has folders. Code Studio edits RAM files.");console_add("Old console commands were ported into this windowed console.");}
    else if(streqi(cmd,"commands")){char l[CON_LINE_LEN];l[0]=0;for(int i=0;i<command_count;i++){if(strlen(l)+strlen(command_table[i].name)+2>CON_LINE_LEN-1){console_add(l);l[0]=0;}strcat(l,command_table[i].name);strcat(l," ");}if(l[0])console_add(l);}
    else if(streqi(cmd,"find")){int m=0;for(int i=0;i<command_count;i++)if(contains_i(command_table[i].name,args)||contains_i(command_table[i].help,args)){char l[100];strcpy(l,command_table[i].name);strcat(l," - ");strcat(l,command_table[i].help);console_add(l);m++;}if(!m)console_add("NOTHING MATCHED.");}
    else if(streqi(cmd,"clear")||streqi(cmd,"clean")){console_count=0;}
    else if(streqi(cmd,"ver")){console_add("SacramentuOS");console_add("version: 1.0.1");console_add("last update: 2026-05-26");}
    else if(streqi(cmd,"fetch")||streqi(cmd,"sysinfo")){const hw_info_t*hi=hw_get_info();console_add("SACRAMENTUOS FETCH");char l[132];strcpy(l,"OS: SacramentuOS 1.0.1 Pixel Desktop ");strcat(l,OS_ARCH);console_add(l);strcpy(l,"CPU: ");strcat(l,hi->cpu_brand);console_add(l);strcpy(l,"RAM: ");append_num(l,(g_mem_lower_kb+g_mem_upper_kb)/1024);strcat(l," MB");console_add(l);strcpy(l,"DISPLAY: ");append_num(l,fb_width());strcat(l,"x");append_num(l,fb_height());strcat(l,"x");append_num(l,fb_bpp());console_add(l);strcpy(l,"USB: ");strcat(l,hi->usb_summary);console_add(l);}
    else if(streqi(cmd,"about")){open_app(APP_ABOUT);console_add("ABOUT OPENED.");}
    else if(streqi(cmd,"echo")){console_add(args);}
    else if(streqi(cmd,"uname")){console_add("SacramentuOS 1.0.1 i386 Pixel Desktop");}
    else if(streqi(cmd,"whoami")){console_add("user");}
    else if(streqi(cmd,"mem")){char l[96];strcpy(l,"lower=");append_num(l,g_mem_lower_kb);strcat(l," KB upper=");append_num(l,g_mem_upper_kb);strcat(l," KB total=");append_num(l,g_mem_lower_kb+g_mem_upper_kb);strcat(l," KB");console_add(l);}
    else if(streqi(cmd,"uptime")){char l[64];strcpy(l,"uptime: ");append_num(l,timer_seconds());strcat(l," seconds");console_add(l);}
    else if(streqi(cmd,"ticks")){char l[64];strcpy(l,"ticks: ");append_num(l,timer_ticks());console_add(l);}
    else if(streqi(cmd,"date")){rtc_time_t t=rtc_read_time();char l[32],n[8];l[0]=0;utoa(t.year,n,10);strcat(l,n);strcat(l,"-");utoa(t.month,n,10);strcat(l,n);strcat(l,"-");utoa(t.day,n,10);strcat(l,n);console_add(l);}
    else if(streqi(cmd,"time")){rtc_time_t t=rtc_read_time();char l[32],n[8];l[0]=0;utoa(t.hour,n,10);strcat(l,n);strcat(l,":");utoa(t.minute,n,10);strcat(l,n);strcat(l,":");utoa(t.second,n,10);strcat(l,n);console_add(l);}
    else if(streqi(cmd,"calc")){char* c=args;char*a=next_token(&c);char*op=next_token(&c);char*b=next_token(&c);if(!a||!op||!b)console_add("usage: calc <number> <+|-|*|/|%> <number>");else{int x=atoi(a),y=atoi(b),r=0,ok=1;if(op[0]=='+')r=x+y;else if(op[0]=='-')r=x-y;else if(op[0]=='*')r=x*y;else if(op[0]=='/'){if(y==0){console_add("division by zero");ok=0;}else r=x/y;}else if(op[0]=='%'){if(y==0){console_add("modulo by zero");ok=0;}else r=x%y;}else{console_add("unknown operator");ok=0;}if(ok){char l[32];itoa(r,l,10);console_add(l);}}}
    else if(streqi(cmd,"color")||streqi(cmd,"theme")){ if(streqi(args,"list")){for(int i=0;i<THEME_COUNT;i++){char l[64];utoa(i,l,10);strcat(l," - ");strcat(l,themes[i].name);console_add(l);}} else {int found=-1;if(is_digit(args[0]))found=atoi(args);else for(int i=0;i<THEME_COUNT;i++)if(contains_i(themes[i].name,args))found=i;if(found>=0&&found<THEME_COUNT){theme_index=found;settings_selected=found;apply_theme();console_add("theme applied");}else console_add("usage: theme classic|graphite|olive|wine or color 0..3");}}
    else if(streqi(cmd,"history")){for(int i=0;i<history_count;i++){char l[128];utoa(i+1,l,10);strcat(l,"  ");strcat(l,history[i]);console_add(l);}}
    else if(streqi(cmd,"len")){char l[32];utoa(strlen(args),l,10);console_add(l);}
    else if(streqi(cmd,"upper")||streqi(cmd,"lower")){char l[CON_LINE_LEN];int i=0;for(;args[i]&&i<CON_LINE_LEN-1;i++)l[i]=(char)(streqi(cmd,"upper")?to_upper(args[i]):to_lower(args[i]));l[i]=0;console_add(l);}
    else if(streqi(cmd,"reverse")){char l[CON_LINE_LEN];strncpy(l,args,CON_LINE_LEN-1);l[CON_LINE_LEN-1]=0;reverse(l);console_add(l);}
    else if(streqi(cmd,"repeat")){char* c=args;char*n=next_token(&c);int count=n?atoi(n):0;if(count<1||!trim_left(c)[0])console_add("usage: repeat <count> <text>");else{if(count>20)count=20;c=trim_left(c);for(int i=0;i<count;i++)console_add(c);}}
    else if(streqi(cmd,"ascii")){for(int i=32;i<127;i+=8){char l[CON_LINE_LEN];l[0]=0;for(int j=0;j<8&&i+j<127;j++){char n[8];utoa(i+j,n,10);strcat(l,n);strcat(l,"='");char ch[2]={(char)(i+j),0};strcat(l,ch);strcat(l,"' ");}console_add(l);}}
    else if(streqi(cmd,"hexdump")){char l[CON_LINE_LEN];l[0]=0;for(int i=0;args[i]&&strlen(l)<CON_LINE_LEN-5;i++){char h[8];utoa((uint8_t)args[i],h,16);if(strlen(h)==1)strcat(l,"0");strcat(l,h);strcat(l," ");}console_add(l);}
    else if(streqi(cmd,"ls")||streqi(cmd,"dir")){console_add("/ Desktop/ Documents/ Apps/ System/ Proc/ README.TXT");console_add("/Proc cpuinfo meminfo pci usb uptime"); if(edit_file_count){char l[CON_LINE_LEN];strcpy(l,"RAM files: ");for(int i=0;i<edit_file_count;i++){strcat(l,edit_names[i]);strcat(l," ");}console_add(l);} }
    else if(streqi(cmd,"cat")||streqi(cmd,"show")){if(!args[0])console_add("usage: cat <file>");else console_show_file(args);}
    else if(streqi(cmd,"pwd")){console_add("/");}
    else if(streqi(cmd,"note")||streqi(cmd,"notes")){ if(streqi(cmd,"notes")||streqi(args,"list")||!args[0]){if(!note_count)console_add("no notes");for(int i=0;i<note_count;i++){char l[120];utoa(i+1,l,10);strcat(l,". ");strcat(l,notes[i]);console_add(l);}} else if(streqi(args,"clear")){note_count=0;console_add("notes cleared");} else {char* c=args;char* act=next_token(&c);c=trim_left(c); if(act&&streqi(act,"add")&&c[0]&&note_count<8){strncpy(notes[note_count++],c,95);notes[note_count-1][95]=0;console_add("note saved");} else console_add("usage: note add <text> | note list | note clear");}}
    else if(streqi(cmd,"noteclr")){note_count=0;console_add("notes cleared");}
    else if(streqi(cmd,"todo")){char* c=args;char* act=next_token(&c);c=trim_left(c); if(!act||streqi(act,"list")){if(!todo_count)console_add("todo empty");for(int i=0;i<todo_count;i++){char l[120];utoa(i+1,l,10);strcat(l,todo_done[i]?". [x] ":". [ ] ");strcat(l,todos[i]);console_add(l);}} else if(streqi(act,"clear")){todo_count=0;console_add("todo cleared");} else if(streqi(act,"add")&&c[0]&&todo_count<8){strncpy(todos[todo_count],c,95);todos[todo_count][95]=0;todo_done[todo_count++]=0;console_add("todo added");} else console_add("usage: todo add/list/clear");}
    else if(streqi(cmd,"done")){int idx=atoi(args)-1;if(idx>=0&&idx<todo_count){todo_done[idx]=1;console_add("done");}else console_add("usage: done <number>");}
    else if(streqi(cmd,"todoclr")){todo_count=0;console_add("todo cleared");}
    else if(streqi(cmd,"base")){uint32_t v=parse_uint_any(args);char l[64];strcpy(l,"dec: ");append_num(l,v);console_add(l);strcpy(l,"hex: ");append_hex(l,v);console_add(l);print_binary_line(v);}
    else if(streqi(cmd,"hash")){uint32_t h=2166136261u;for(size_t i=0;args[i];i++){h^=(uint8_t)args[i];h*=16777619u;}char l[64];strcpy(l,"fnv1a32: ");append_hex(l,h);console_add(l);}
    else if(streqi(cmd,"rot13")){char l[CON_LINE_LEN];int i=0;for(;args[i]&&i<CON_LINE_LEN-1;i++){char c=args[i];if(c>='a'&&c<='z')c=(char)('a'+((c-'a'+13)%26));else if(c>='A'&&c<='Z')c=(char)('A'+((c-'A'+13)%26));l[i]=c;}l[i]=0;console_add(l);}
    else if(streqi(cmd,"count")){int chars=0,words=0,inw=0;for(int i=0;args[i];i++){chars++;if(is_space(args[i]))inw=0;else if(!inw){words++;inw=1;}}char l[64];strcpy(l,"chars=");append_num(l,chars);strcat(l," words=");append_num(l,words);console_add(l);}
    else if(streqi(cmd,"edit")||streqi(cmd,"code")){code_open_file(args[0]?args:"main.c");console_add("CODE STUDIO OPENED.");}
    else if(streqi(cmd,"files")){if(!edit_file_count)console_add("RAM files: <none>");else for(int i=0;i<edit_file_count;i++){char l[96];strcpy(l,edit_names[i]);strcat(l," lines=");append_num(l,edit_line_counts[i]);console_add(l);}}
    else if(streqi(cmd,"rm")){int idx=ram_file_find(args);if(idx<0)console_add("RAM FILE NOT FOUND.");else{for(int f=idx+1;f<edit_file_count;f++){strcpy(edit_names[f-1],edit_names[f]);edit_line_counts[f-1]=edit_line_counts[f];for(int l=0;l<EDIT_LINES_MAX;l++)strcpy(edit_lines[f-1][l],edit_lines[f][l]);}edit_file_count--;console_add("removed");}}
    else if(streqi(cmd,"ps")){console_add("PID STATE NAME");console_add("1   run   kernel");console_add("2   run   desktop-shell");console_add("3   idle  idle-loop");}
    else if(streqi(cmd,"top")){char l[96];strcpy(l,"uptime=");append_num(l,timer_seconds());strcat(l,"s windows=");int wc=0;for(int i=1;i<APP_COUNT;i++)if(windows[i].open)wc++;append_num(l,wc);strcat(l," ram-files=");append_num(l,edit_file_count);console_add(l);}
    else if(streqi(cmd,"kernel")){console_add("kernel: SacramentuOS 1.0.1 Pixel Desktop");console_add("modules: fb gui idt pic pit ps2 keyboard mouse rtc hw pci shell");}
    else if(streqi(cmd,"log")){if(!syslog_count)console_add("log empty");for(int i=0;i<syslog_count;i++)console_add(syslog_buf[i]);}
    else if(streqi(cmd,"explorer")){open_app(APP_EXPLORER);console_add("EXPLORER OPENED.");}
    else if(streqi(cmd,"settings")){open_app(APP_SETTINGS);console_add("SETTINGS OPENED.");}
    else if(streqi(cmd,"gpux")){open_app(APP_GPUX);console_add("GPUX OPENED.");}
    else if(streqi(cmd,"reboot")){console_add("REBOOTING...");system_reboot();}
    else if(streqi(cmd,"shutdown")||streqi(cmd,"halt")){console_add("CPU HALTED.");system_halt();}
    else console_add("UNKNOWN COMMAND. TYPE HELP OR COMMANDS.");
    console_input[0]=0; console_input_len=0;
}

static void handle_console_key(int key){ if(key==KEY_ENTER)console_exec(); else if(key==KEY_BS||key==KEY_DELETE){ if(console_input_len>0){console_input[--console_input_len]=0;}} else if(key>=32&&key<=126&&console_input_len+1<INPUT_LEN){console_input[console_input_len++]=(char)key;console_input[console_input_len]=0;} }
static void handle_explorer_key(int key){ int cnt; explorer_items(&cnt); if(key==KEY_UP&&explorer_selected>0)explorer_selected--; else if(key==KEY_DOWN&&explorer_selected+1<cnt)explorer_selected++; else if(key==KEY_ENTER)explorer_open_selected(); }
static void handle_settings_key(int key){ if(key==KEY_UP&&settings_selected>0)settings_selected--; else if(key==KEY_DOWN&&settings_selected+1<THEME_COUNT)settings_selected++; else if(key==KEY_ENTER)apply_setting_selection(); }
static void handle_code_key(int key){ if(code_file<0){ if(key>=32&&key<=126)code_open_file("main.c"); return;} editor_clamp(); int count=edit_line_counts[code_file]; char (*lines)[EDIT_LINE_MAX]=edit_lines[code_file]; if(key==KEY_F7){code_dirty=0;strcpy(code_status,"Saved to RAM.");return;} if(key==KEY_F8){code_dirty=0;strcpy(code_status,"Saved and closed.");close_app(APP_CODE);return;} if(key==KEY_F9){close_app(APP_CODE);return;} if(key==KEY_UP&&code_cy>0)code_cy--; else if(key==KEY_DOWN&&code_cy+1<count)code_cy++; else if(key==KEY_LEFT){if(code_cx>0)code_cx--;else if(code_cy>0){code_cy--;code_cx=line_len(lines[code_cy]);}} else if(key==KEY_RIGHT){int len=line_len(lines[code_cy]);if(code_cx<len)code_cx++;else if(code_cy+1<count){code_cy++;code_cx=0;}} else if(key==KEY_HOME)code_cx=0; else if(key==KEY_END)code_cx=line_len(lines[code_cy]); else if(key==KEY_ENTER){ if(count<EDIT_LINES_MAX){char tail[EDIT_LINE_MAX];strncpy(tail,lines[code_cy]+code_cx,EDIT_LINE_MAX-1);tail[EDIT_LINE_MAX-1]=0;lines[code_cy][code_cx]=0;for(int i=count;i>code_cy+1;i--)strcpy(lines[i],lines[i-1]);strcpy(lines[code_cy+1],tail);edit_line_counts[code_file]++;code_cy++;code_cx=0;code_dirty=1;strcpy(code_status,"Modified.");} } else if(key==KEY_BS){ if(code_cx>0){editor_delete_char(lines[code_cy],code_cx-1);code_cx--;code_dirty=1;} else if(code_cy>0){int prev=line_len(lines[code_cy-1]); if(prev+line_len(lines[code_cy])<EDIT_LINE_MAX){strcat(lines[code_cy-1],lines[code_cy]);for(int i=code_cy+1;i<count;i++)strcpy(lines[i-1],lines[i]);edit_line_counts[code_file]--;code_cy--;code_cx=prev;code_dirty=1;}} } else if(key==KEY_DELETE){int len=line_len(lines[code_cy]); if(code_cx<len){editor_delete_char(lines[code_cy],code_cx);code_dirty=1;} else if(code_cy+1<count&&len+line_len(lines[code_cy+1])<EDIT_LINE_MAX){strcat(lines[code_cy],lines[code_cy+1]);for(int i=code_cy+2;i<count;i++)strcpy(lines[i-1],lines[i]);edit_line_counts[code_file]--;code_dirty=1;}} else if(key>=32&&key<=126){editor_insert_char(lines[code_cy],code_cx,(char)key);code_cx++;code_dirty=1;strcpy(code_status,"Modified.");} editor_clamp(); }
static void handle_key(int key){ if(start_menu_open){ if(key==KEY_UP&&start_selected>0)start_selected--; else if(key==KEY_DOWN&&start_selected+1<START_ITEMS)start_selected++; else if(key==KEY_ENTER)handle_start_selection(start_selected); else if(key==KEY_ESC||key==KEY_F1)start_menu_open=0; return;} if(key==KEY_F1){start_menu_open=1;return;} if(key==KEY_F2){open_app(APP_EXPLORER);return;} if(key==KEY_F3){open_app(APP_CONSOLE);return;} if(key==KEY_F4){open_app(APP_ABOUT);return;} if(key==KEY_F5){open_app(APP_SETTINGS);return;} if(key==KEY_F6){code_open_file("main.c");return;} if(key==KEY_F7){open_app(APP_GPUX);return;} if(key==KEY_F10){system_reboot();return;} if(key==KEY_TAB){cycle_active();return;} if(key==KEY_ESC){close_app(active_app);return;} if(active_app==APP_EXPLORER)handle_explorer_key(key); else if(active_app==APP_CONSOLE)handle_console_key(key); else if(active_app==APP_CODE)handle_code_key(key); else if(active_app==APP_SETTINGS)handle_settings_key(key); }

static void gui_boot_animation_pixel(void){ init_colors(); mouse_set_bounds(sw(),sh()); fb_clear(C_DESKTOP_BOT); for(int i=0;i<100;i+=4){draw_desktop_background();fb_fill_rect(sw()/2-220,sh()/2+25,440,26,C_PANEL_DARK);fb_fill_rect(sw()/2-218,sh()/2+27,(436*i)/100,22,C_ACCENT);draw_text(sw()/2-150,sh()/2-40,"SacramentuOS",C_WHITE);draw_text_small(sw()/2-138,sh()/2-8,"LOADING PIXEL DESKTOP",fb_rgb(210,235,255));fb_present();timer_sleep(18);} }
static void gui_start_pixel(void){ init_colors(); mouse_set_bounds(sw(),sh()); console_add("SacramentuOS Pixel Console [Version 1.0.1]"); console_add("Type HELP. Old shell commands are ported. Use CODE or GPUX."); console_add("Note: raw USB host controller probing is present; full HID stack is still experimental."); mouse_state_t ms; mouse_get_state(&ms); while(1){int key;while(keyboard_trygetkey(&key))handle_key(key);mouse_get_state(&ms);handle_mouse(&ms);draw_pixel_desktop(&ms);frame_id++;timer_sleep(20);} }
static uint8_t txt_col_desktop(void){ return vga_entry_color(VGA_COLOR_WHITE,VGA_COLOR_BLUE); }
static void gui_boot_animation_text(void){ terminal_setcolor(txt_col_desktop()); terminal_fill_rect(0,0,VGA_WIDTH,VGA_HEIGHT,' ',txt_col_desktop()); terminal_write_at(7,18,"SacramentuOS 1.0.1 Pixel Desktop",txt_col_desktop()); terminal_write_at(10,10,"No VBE framebuffer was supplied by the bootloader.",txt_col_desktop()); terminal_write_at(12,10,"Boot through GRUB ISO or native UEFI BOOTX64.EFI for the pixel GUI.",txt_col_desktop()); timer_sleep(1200); }
static void gui_start_text_fallback(void){ terminal_clear(); terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN,VGA_COLOR_BLACK)); kprintf("SacramentuOS 1.0.1 fallback console\n"); kprintf("Framebuffer not available. Use: make run-prebuilt or make run-uefi-prebuilt.\n\n"); while(1){kprintf("C:\\> "); char line[INPUT_LEN]; int n=0; while(1){int key=keyboard_getkey(); if(key==KEY_ENTER){kprintf("\n");break;} if((key==KEY_BS||key==KEY_DELETE)&&n>0){n--;line[n]=0;kprintf("\b \b");} else if(key>=32&&key<=126&&n+1<INPUT_LEN){line[n++]=(char)key;line[n]=0;kprintf("%c",key);}} trim_right(line); char*p=trim_left(line); if(strcmp(p,"reboot")==0)system_reboot(); else if(strcmp(p,"shutdown")==0||strcmp(p,"halt")==0)system_halt(); else if(strcmp(p,"ver")==0)kprintf("SacramentuOS 1.0.1 Pixel Desktop\n"); else if(strcmp(p,"help")==0)kprintf("Commands: help ver reboot shutdown\n"); else if(p[0])kprintf("Framebuffer fallback: command not available here.\n");} }
void gui_boot_animation(void){ if(fb_available())gui_boot_animation_pixel(); else gui_boot_animation_text(); }
void gui_start(void){ if(fb_available())gui_start_pixel(); else gui_start_text_fallback(); }
