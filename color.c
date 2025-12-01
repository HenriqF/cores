#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
//incluir cor normal

int hextodouble(char* valor, int length){
    double res = 0;
    int mult = 1;
    while(--length >= 0){
        if (valor[length] >= '0' && valor[length] <= '9'){
            res += (int)(valor[length] - '0')*mult;
            mult*=16;
            continue;
        }
        char c = tolower(valor[length]);
        if (c >= 'a' && c <= 'f'){
            res += (10 + (int)(c - 'a'))*mult;
            mult*=16;
        }
    }
    return res;
}

double wrapfmod(double a, double b){
    double r = fmod(a, b);
    if (r < 0) r += b;

    return r;
}
double max(double a, double b){
    return (a>b) ? a : b;
}
double min(double a, double b){
    return (a>b) ? b : a;
}

void rgbToHsl(double* r, double* g, double* b){
    double R = (*r)/255;
    double G = (*g)/255;
    double B = (*b/255);
    double mx = max(max(R, G), B);
    double mn = min(min(R,G), B);
    double L = (mx + mn) / 2;
    double d = mx-mn;


    double H, S;
    if (d == 0){
        (*r) = 0, (*g) = 0, (*b) = L;
        return;
    }
    if (L < 0.5){
        S = d / (mx + mn);
    }
    else{
        S = d / (2 - mx - mn);
    }

    if (mx == R){
        H = wrapfmod(((G - B) / d), 6);
    }
    else if (mx == G){
        H = ((B - R) / d) + 2;
    }
    else{
        H = ((R - G) / d) + 4;
    }
    H *= 60;
    (*r) = H, (*g) = S, (*b) = L;
}
void hslToRgb(double* h, double* s, double* l){
    (*h) = (*h) / 60;
    double c = (1 - fabs(2*(*l) - 1)) * (*s);
    double x = c * (1 - fabs(wrapfmod((*h), 2) - 1));

    double r, g, b, m;
    if (0 <= (*h) && (*h) < 1){
        r = c, g = x, b =0;
    }
    else if(1 <= (*h) && (*h) < 2){
        r = x, g =c ,b = 0;
    }
    else if(2 <= (*h) && (*h) < 3){
        r = 0, g = c, b = x;
    }
    else if(3 <= (*h) && (*h)< 4){
        r = 0, g =x, b = c;
    }
    else if(4 <= (*h) && (*h) < 5){
        r = x, g = 0, b = c;
    }
    else{
        r = c, g = 0, b = x;
    }
    m = (*l) - c/2;
    (*h) = (r+m)*255;
    (*s) = (g+m)*255;
    (*l) = (b+m)*255; 
}

void rotateColor(double* r, double* g, double* b, double angle){
    rgbToHsl(r, g, b);
    (*r) += angle;
    while((*r) > 360){
        (*r)-=360;
    }
    hslToRgb(r,g,b);
}
void saturateColor(double* r, double* g, double* b, double value){
    rgbToHsl(r, g, b);
    (*g) += value*(*g);
    if ((*g) > 1) (*g) = 1;
    if ((*g) < 0) (*g) = 0;
    hslToRgb(r,g,b);
}
void lightColor(double* r, double* g, double* b, double value){
    rgbToHsl(r, g, b);
    (*b) += value*(*b);
    if ((*b) > 1) (*b) = 1;
    if ((*b) < 0) (*b) = 0;
    hslToRgb(r,g,b);
}


void newFile(){
    FILE* read = fopen("template.html", "rb");
    FILE* write = fopen("out.html", "wb");
    if (!read || !write) return;

    unsigned char buffer[4096];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), read)) > 0){
        fwrite(buffer, 1, n, write);
    }
    fclose(read);
    fclose(write);
}
void appendTitle(char* title){
    FILE* write = fopen("out.html", "a");
    fputs(title, write);
    fclose(write);
}
void appendColor(double r, double g, double b){
    char fi[] = "<div class='cor' style='background:rgb(";
    char se[] = ")'>";
    char th[] = "</div>\n";

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%.0f, %.0f, %.0f", r, g, b);
    FILE* write = fopen("out.html", "a");
    if (!write) return;
    fputs(fi, write);
    fputs(buffer, write);
    fputs(se, write);
    fputs(buffer, write);
    fputs(th, write);
    fclose(write);
}
void finalizeFile(){
    FILE* write = fopen("out.html", "a");
    if (!write) return;
    fputs("</body>\n</html>", write);
    fclose(write);
}

void newPalette(double r, double g, double b){
    char tipo;
    printf("Tipo de paletta: MATIZ / SATURACAO / BRILHO (m/s/b) --> ");
    scanf(" %c", &tipo);
    if (tipo != 'm' && tipo != 's' && tipo != 'b') tipo = 'm';

    int v = 5;
    double ans;
    if (tipo == 'm'){
        printf("Angulo de rotacao: ");
        scanf(" %lf", &ans);
        appendTitle("<p>Paleta de matiz</p>");
        appendColor(r, g, b);
        while (v-- > 0){
            rotateColor(&r, &g, &b, ans);
            appendColor(r, g, b);
        }
        return;
    }
    printf("Valor para mudar (em porcento): ");
    scanf(" %lf", &ans);
    ans/=100;
    if (ans > 1) ans = 1;
    if (ans < -1) ans = -1;

    if (tipo == 's'){
        appendTitle("<p>Paleta de saturação</p>");
        appendColor(r, g, b);

        while (v-- > 0){
            saturateColor(&r, &g, &b, ans);
            appendColor(r, g, b);
        }
    }
    else if (tipo == 'b'){
        appendTitle("<p>Paleta de brilho</p>");
        appendColor(r, g, b);
        while (v-- > 0){
            lightColor(&r, &g, &b, ans);
            appendColor(r, g, b);
        }
    }
}


int main(){
    newFile();
    double r, g, b;
    char hexcor[7];
    char hexr[3];
    char hexg[3];
    char hexb[3];
    printf("Sua cor em HEX: ");
    scanf(" %s", hexcor);

    memcpy(hexr, &hexcor, 2);
    memcpy(hexg, &hexcor[2], 2);
    memcpy(hexb, &hexcor[4], 2);

    char ans = 'n';
    while(ans != 's'){
        r = hextodouble(hexr, 2);
        g = hextodouble(hexg, 2);
        b = hextodouble(hexb, 2);
        newPalette(r, g, b);

        printf("Satisfeito? s/n --> ");
        scanf(" %c", &ans);
    }

    finalizeFile();
    printf("Resultado em out.html");
    

    return 0;
}