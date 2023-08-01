#include <stdio.h>
#include <string.h>
#include "gd.h"
#include <stdlib.h>
#include <time.h>
#include <gdfontg.h>

char ask_graphic_type(){
    char graph_T;
    printf("Veuillez indiquer le type de graphique (h pour histogramme, c pour camenbert) :\n");
    scanf(" %c", &graph_T);
    return graph_T;
}

void ask_titre(char title[]){
    printf("Quel nom voulez-vous donner au titre du graphique:\n");
    scanf(" %[^\n]", title);
}
int ask_nbr_etiquette(){
printf("Veuillez rentrer le nombre d'élément à représenter:\n");
int nbr_etiquet=0;
scanf("%d",&nbr_etiquet);
return nbr_etiquet;
}

void ask_name_etiquette(int nbr_etiquettes,char Tab_etiquettes[nbr_etiquettes][100]){
int i =0;
while( i < nbr_etiquettes){
    printf ("Veuillez indiquer le nom de l'élément %d\n",i+1);
    scanf("%99s",Tab_etiquettes[i]);i++;}
}

void ask_percentage(int nbr_etiquettes,char Tab_etiquettes[nbr_etiquettes][100], float Tab_pourcent[nbr_etiquettes]){
int i=0;
while( i < nbr_etiquettes){
    printf ("Veuillez indiquer le pourcentage pour %s\n",Tab_etiquettes[i]);
    scanf("%f",&Tab_pourcent[i]);i++;}
}
void ask_name(char file_name[]){
    printf("quel nom voulez-vous donner à l'image de sortie:\n");
    scanf(" %[^\n]", file_name);
}

char get_couleur_background() {
    char backg;
    printf("Veuillez indiquer la couleur d'arrière-plan (b pour blanc, n pour noir) :\n");
    scanf(" %c", &backg);
    return backg;
}

void create_camenbert(char *file_name,
                      char *title,
                      int nbr_etiquettes,
                      char Tab_etiquettes[nbr_etiquettes][100],
                      float Tab_pourcent[nbr_etiquettes],
                      char backg)
{
    gdImagePtr im;
    im = gdImageCreate(512,512);
    char *filename;
    filename=strcat(file_name,".png");
    FILE *output;

    int ap_color;
    if (backg == 'n'){ap_color=gdImageColorAllocate(im, 0,0,0);}
    else {ap_color=gdImageColorAllocate(im, 255,255,255);}

    gdImageFilledRectangle(im,0,0,512,512,ap_color);
    int cx =264,cy=264, radius = 125;
    int start=0,end;

    int colors[] = {
            gdImageColorAllocate(im, 255, 0, 0),      // Rouge
            gdImageColorAllocate(im, 0, 255, 0),      // Vert
            gdImageColorAllocate(im, 0, 0, 255),      // Bleu
            gdImageColorAllocate(im, 255, 255, 0),    // Jaune
            gdImageColorAllocate(im, 255, 0, 255),    // Magenta
            gdImageColorAllocate(im, 0, 255, 255),    // Cyan
            gdImageColorAllocate(im, 128, 0, 0),      // Brun
            gdImageColorAllocate(im, 0, 128, 0),      // Vert foncé
            gdImageColorAllocate(im, 0, 0, 128),      // Bleu foncé
            gdImageColorAllocate(im, 128, 128, 0),    // Olive
            gdImageColorAllocate(im, 128, 0, 128),    // Violet
            gdImageColorAllocate(im, 0, 128, 128)     // Teal
        };

    int num_colors = sizeof(colors) / sizeof(colors[0]);
    int black = gdImageColorAllocate(im, 0, 0, 0);

    gdFontPtr font = gdFontGetGiant();
    int legend_x = cx + radius;
    int legend_y = 80;

    if (backg=='b'){
    gdImageString(im, font, 230, 60, title, black);}
    else {gdImageString(im, font, 230, 60, title, gdImageColorAllocate(im, 255,255,255));}

    for (int i=0; i<nbr_etiquettes;i++){
        end = start + (Tab_pourcent[i] * 360 / 100);
        int selected_color_index = i % num_colors;
        gdImageFilledArc(im, cx,cy,radius,radius,start,end,colors[selected_color_index], gdPie);
        gdImageString(im, font, legend_x, legend_y + i*20, (unsigned char*)Tab_etiquettes[i], colors[selected_color_index]);
        char percentage_str[10];
        snprintf(percentage_str, sizeof(percentage_str), "%.1f%%", Tab_pourcent[i]);
        gdImageString(im, font, legend_x + 50, legend_y + i*20 , (unsigned char*)percentage_str, colors[selected_color_index]);
        start=end;
    }
    output= fopen(filename,"wb");
    gdImagePng(im,output);
    fclose(output);
    gdImageDestroy(im);
}


void create_histogramme(char *file_name,
                      char *title,
                      int nbr_etiquettes,
                      char Tab_etiquettes[nbr_etiquettes][100],
                      float Tab_pourcent[nbr_etiquettes],
                      char backg)
{
    gdImagePtr im;
    im = gdImageCreate(512,512);
    char *filename;
    filename=strcat(file_name,".png");
    FILE *output;

    int ap_color;
    if (backg == 'n'){ap_color=gdImageColorAllocate(im, 0,0,0);}
    else {ap_color=gdImageColorAllocate(im, 255,255,255);}

    gdImageFilledRectangle(im,0,0,512,512,ap_color);

    int bar_epaisseur = 512 / (nbr_etiquettes * 2);
    int bar_gap = bar_epaisseur / 2;

    int colors[] = {
            gdImageColorAllocate(im, 255, 0, 0),      // Rouge
            gdImageColorAllocate(im, 0, 255, 0),      // Vert
            gdImageColorAllocate(im, 0, 0, 255),      // Bleu
            gdImageColorAllocate(im, 255, 255, 0),    // Jaune
            gdImageColorAllocate(im, 255, 0, 255),    // Magenta
            gdImageColorAllocate(im, 0, 255, 255),    // Cyan
            gdImageColorAllocate(im, 128, 0, 0),      // Brun
            gdImageColorAllocate(im, 0, 128, 0),      // Vert foncé
            gdImageColorAllocate(im, 0, 0, 128),      // Bleu foncé
            gdImageColorAllocate(im, 128, 128, 0),    // Olive
            gdImageColorAllocate(im, 128, 0, 128),    // Violet
            gdImageColorAllocate(im, 0, 128, 128)     // Teal
        };

    int num_colors = sizeof(colors) / sizeof(colors[0]);
    int black = gdImageColorAllocate(im, 0, 0, 0);

    gdFontPtr font = gdFontGetGiant();
    if (backg=='b'){
    gdImageString(im, font, 230, 60, title, black);}
    else {gdImageString(im, font, 230, 60, title, gdImageColorAllocate(im, 255,255,255));}

    for (int i = 0; i < nbr_etiquettes; i++) {
            int selected_color_index = i % num_colors;
            int bar_hauteur = 206 * Tab_pourcent[i] / 50;

            int x1 = bar_gap + i * (bar_epaisseur * 2);
            int y1 = 500 - 20;
            int x2 = x1 + bar_epaisseur;
            int y2 = 500 - 20 - bar_hauteur;

            gdImageFilledRectangle(im, x1, y1, x2, y2, colors[selected_color_index]);
            gdImageString(im, font, x1, 500 - 5, (unsigned char*)Tab_etiquettes[i], colors[selected_color_index]);
            char percentage_str[10];
            snprintf(percentage_str, sizeof(percentage_str), "%.1f%%", Tab_pourcent[i]);
            gdImageString(im, font, x1, 500 - 20 - bar_hauteur-15, (unsigned char*)percentage_str, colors[selected_color_index]);
        }
    output= fopen(filename,"wb");
    gdImagePng(im,output);
    fclose(output);
    gdImageDestroy(im);
}


int main()

{srand(time(NULL));
 char titre[30] ="Titre";
 int nbr_etiquettes=0;
 printf("Bienvenue dans l\'outil de creation de camenbert\n");
 printf("\n");

 char type_graph=ask_graphic_type();
 printf("Le graphique sera un %s\n", type_graph=='c' ? "camenbert": "histogramme");

 ask_titre(titre);
 printf("Le titre du graphique sera %s\n",titre);

 nbr_etiquettes = ask_nbr_etiquette();
 printf("Il y aura %d étiquettes\n",nbr_etiquettes);

 char Tab_etiq[nbr_etiquettes][100];float Tab_pourcentage[nbr_etiquettes];
 ask_name_etiquette(nbr_etiquettes, Tab_etiq);
 for (int j=0; j<nbr_etiquettes; j++){
     printf("l\'etiquette %d est %s\n", j+1, Tab_etiq[j]);
 }
 ask_percentage(nbr_etiquettes,Tab_etiq,Tab_pourcentage);
 for (int j=0; j<nbr_etiquettes; j++){
     printf("l\'etiquette %s prendra %.2f pourcent du camenbert\n",  Tab_etiq[j],Tab_pourcentage[j]);
 }
char name_file[50] ="default_name";
ask_name(name_file);
printf("%s.png sera le nom de l'image\n", name_file);

char arr_plan_couleur=get_couleur_background();
printf("la couleur d'arrière plan sera le %s\n", arr_plan_couleur=='b' ? "blanc": "noir");

if (type_graph !='h'){
create_camenbert(name_file,titre,nbr_etiquettes,Tab_etiq,Tab_pourcentage,arr_plan_couleur);}
else{create_histogramme(name_file,titre,nbr_etiquettes,Tab_etiq,Tab_pourcentage,arr_plan_couleur);}

    return 0;
}
