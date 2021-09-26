//gcc c_curl.c -lcurl -lX11 -ljson-c -o c_curl

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#include <curl/curl.h>

#include <json-c/json.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

//TODO cmd args, logging
struct json_object *username;
struct json_object *score;
struct json_object *wus;
struct json_object *rank;
struct json_object *users;

int info_to_json(char *ptr, size_t size, size_t nmemb, void *stream) {
    //printf("ptr %s\nsize %d\nnmemb %d\n",ptr,size,nmemb);
    struct json_object *jout;
    jout = json_tokener_parse(ptr);
    //printf("jout from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jout, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
    json_object_object_get_ex(jout, "name", &username); //printf("%s\n", json_object_to_json_string(username));
    json_object_object_get_ex(jout, "score", &score); //printf("%s\n", json_object_to_json_string(score));
    json_object_object_get_ex(jout, "wus", &wus); //printf("%s\n", json_object_to_json_string(wus));
    json_object_object_get_ex(jout, "rank", &rank); //printf("%s\n", json_object_to_json_string(rank));
    json_object_object_get_ex(jout, "users", &users); //printf("%s\n", json_object_to_json_string(users));
    return size * nmemb; //wot dis for? makes error msg go away ¯\_(ツ)_/¯ 
}
void get_info() {
    //printf("getting info...\n");
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.foldingathome.org/user/yobleck"); //TODO cmd arg
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, info_to_json);
        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        // always cleanup
        curl_easy_cleanup(curl);
    }
}


Display *dis;
int scr;
Window win;
GC gc, text_gc;
XEvent ev;

void init_x() {
    // get the colors black and white (see section for details)
    unsigned long black, white;

    /* use the information from the environment variable DISPLAY 
       to create the X connection:
    */    
    dis=XOpenDisplay(NULL);
    if (dis == NULL) {
        fprintf(stderr, "Cannot open display\n");
    }
    scr=DefaultScreen(dis);
    black=BlackPixel(dis, scr); //get color black
    white=WhitePixel(dis, scr); //get color white

    /* once the display is initialized, create the window.
       This window will be have be 200 pixels across and 300 down.
       It will have the foreground white and background black
    */
    win=XCreateSimpleWindow(dis, XRootWindow(dis,scr), 10, 10, 165, 60, 1, white, black);
    
    // set WM_NAME
    char *prog_name = "FAHStats";
    XTextProperty wmname;
    XStringListToTextProperty(&prog_name, 1, &wmname);
    XSetWMName(dis, win, &wmname);
    
    // set WM_CLASS
    XClassHint *class = XAllocClassHint();
    class->res_name = prog_name; class->res_class = prog_name;
    XSetClassHint(dis, win, class);
    //TODO set window below, never steal focus and skip taskbar hints
    //https://stackoverflow.com/a/23275960/12481091
    // set _NET_WM_STATE
    //XChangeProperty(dis, win, XInternAtom(dis, "_NET_WM_STATE", false), XInternAtom(dis, "ATOM", false),
                    //32, PropModeReplace, (unsigned char *) "_NET_WM_STATE_BELOW", 1);

    // set event masks
    XSelectInput(dis, win, ExposureMask | KeyPressMask | ButtonPressMask);

    // create the Graphics Context
    gc = XCreateGC(dis, win, 0, 0);
    XSetBackground(dis, gc, black);
    XSetForeground(dis, gc, white);
    
    // gc for text
    text_gc = XCreateGC(dis, win, 0, 0); //TODO font type and size. dark grey bg?
    XSetBackground(dis, text_gc, black);
    XSetForeground(dis, text_gc, white);
    
    // setting font https://wiki.archlinux.org/title/X_Logical_Font_Description
    char *path = "/usr/share/fonts/noto/NotoMono-Regular.ttf/";
    XSetFontPath(dis, &path, 1);
    /* // for listing available fonts
    int num_f;
    char **fonts;
    fonts = XListFonts(dis, "*monotype-noto*", 100, &num_f);
    for (int i=0; i<=num_f; i++) {
        printf("%s\n", fonts[i]);
    }*/
    XGCValues v;
    XFontStruct *f = XLoadQueryFont(dis, "-monotype-noto mono-medium-r-normal--0-80-0-0-m-0-adobe-standard"); //80 is font size
    if (f == NULL) {
        printf("failed to load font\n");
    }
    v.font = f->fid;
    XChangeGC(dis, text_gc, GCFont, &v);

    // clear the window and bring it on top of the other windows
    XClearWindow(dis, win);
    XMapWindow(dis, win);
}
void close_x() {
    // free X related resources
    XFreeGC(dis, gc);
    XDestroyWindow(dis, win);
    XCloseDisplay(dis);
}

void draw_text(){
    setlocale(LC_NUMERIC, ""); // for formatting large nums with commas
    
    XClearWindow(dis, win);
    XDrawString(dis, win, text_gc, 5, 10, "Name: ", strlen("Name: "));
    XDrawString(dis, win, text_gc, 50, 10, json_object_to_json_string(username), strlen(json_object_to_json_string(username)));
    
    XDrawString(dis, win, text_gc, 5, 20, "Score: ", strlen("Score: "));
    char score_str;
    sprintf(&score_str, "%'d\n", json_object_get_int(score));
    XDrawString(dis, win, text_gc, 50, 20, &score_str, strlen(&score_str));
    
    XDrawString(dis, win, text_gc, 5, 30, "WUs: ", strlen("WUs: "));
    char wus_str;
    sprintf(&wus_str, "%'d\n", json_object_get_int(wus));
    XDrawString(dis, win, text_gc, 50, 30, &wus_str, strlen(&wus_str));
    
    XDrawString(dis, win, text_gc, 5, 40, "Rank: ", strlen("Rank: "));
    char *rank_str = malloc(sizeof(char)*20);
    //printf("%s/%s\n", json_object_to_json_string(rank), json_object_to_json_string(users));
    snprintf(rank_str, 20, "%'d/%'d\n", json_object_get_int(rank), json_object_get_int(users));
    XDrawString(dis, win, text_gc, 50, 40, rank_str, strlen(rank_str));
    
    XDrawString(dis, win, text_gc, 5, 50, "Top: ", strlen("Top: "));
    double top = json_object_get_double(rank) / json_object_get_double(users) * 100.0;
    //printf("top: %.3f%\n", top);
    char top_str;
    sprintf(&top_str, "%.3f%\n", top);
    XDrawString(dis, win, text_gc, 50, 50, &top_str, strlen(&top_str));
}


int main(void) {
    get_info();
    init_x();
    time_t timer = time(NULL);

    while(1) {
        XNextEvent(dis, &ev); //blocking. have to send event to continue
        //only works after click
        if (ev.type == Expose) {
            draw_text();
        }
        if (ev.type == KeyPress) { //TODO use keysymdef.h?
            printf("%d\n", ev.xkey.keycode);
            if (ev.xkey.keycode == 9) {
                break;
            }
        }
        if (ev.type == ButtonPress && time(NULL)-10 > timer) { //TODO replace 10 with 600?
            printf("%d\n", timer);
            timer = time(NULL);
            get_info();
            draw_text();
        }
    }
    //sleep(2);
    close_x();
    return 0;
} 

//https://curl.se/libcurl/c/
//https://curl.se/libcurl/c/simple.html
//https://stackoverflow.com/questions/42531831/adding-fonts-with-very-large-size-in-x11
//http://ps-2.kev009.com/tl/techlib/manuals/adoclib/libs/gl32tref/xfontsc.htm
