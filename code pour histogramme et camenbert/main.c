#include <stdio.h>
#include <string.h>
#include "gd.h"
#include <stdlib.h>
#include <gdfontg.h>

///
/// \brief ask_graphic_type: permet de selectionner le type de graphique entre histogramme et camenbert
/// \return
///
char ask_graphic_type(){
    char graph_T;
    printf("Veuillez indiquer le type de graphique (h pour histogramme, c pour camenbert) :\n");
    scanf(" %c", &graph_T);
    return graph_T;
}
///
/// \brief ask_titre : permet de rentrer le titre du graphique, les espaces sont autorisés,
/// si non remplie un titre par défaut est présent et défini dans la fonction main
/// \param title : le titre par defaut
///
void ask_titre(char title[]){
    printf("Quel nom voulez-vous donner au titre du graphique:\n");
    scanf(" %[^\n]", title);
}
///
/// \brief ask_nbr_etiquette: permet de savoir sur combien d'éléments porte le graphique pour demander ensuite de les nommer.
/// Permet aussi de dimensionner les graphiques à une taille raisonnable
/// \return
///

int ask_nbr_etiquette(){
printf("Veuillez rentrer le nombre d'élément à représenter:\n");
int nbr_etiquet=0;
scanf("%d",&nbr_etiquet);
return nbr_etiquet;
}
///
/// \brief ask_name_etiquette: Permet de remplir un tableau de 100 éléments maximun
/// avec le nom des éléments à représenter sur le graphique
/// \param nbr_etiquettes
/// \param Tab_etiquettes
///
void ask_name_etiquette(int nbr_etiquettes,char Tab_etiquettes[nbr_etiquettes][100]){
int i =0;
while( i < nbr_etiquettes){
    printf ("Veuillez indiquer le nom de l'élément %d\n",i+1);
    scanf("%99s",Tab_etiquettes[i]);i++;}
}
///
/// \brief ask_percentage: permet d'attribuer une valeur aux élements pour savoir les proportions à représenter
/// \param nbr_etiquettes
/// \param Tab_etiquettes
/// \param Tab_pourcent
///
void ask_percentage(int nbr_etiquettes,char Tab_etiquettes[nbr_etiquettes][100], float Tab_pourcent[nbr_etiquettes]){
int i=0;
while( i < nbr_etiquettes){
    printf ("Veuillez indiquer le pourcentage pour %s\n",Tab_etiquettes[i]);
    scanf("%f",&Tab_pourcent[i]);i++;}
}

///
/// \brief ask_name : permet d'attribuer un nom au fichier qui va etre créer
/// \param file_name
///
void ask_name(char file_name[]){
    printf("Quel nom voulez-vous donner à l'image de sortie:\n");
    scanf(" %[^\n]", file_name);
}
///
/// \brief get_couleur_background: permet de sélectionner
///  une des deux valeur possibles en couleur d'arrière plan
/// \return
///
char get_couleur_background() {
    char backg;
    printf("Veuillez indiquer la couleur d'arrière-plan (b pour blanc, n pour noir) :\n");
    scanf(" %c", &backg);
    return backg;
}
///
/// \brief create_camenbert : permet de créer un diagramme en camenbert
/// on créer d'abord l'image et le nom sous lequelle elle sera sauvegardée;
/// on lui applique ensuite la couleur d'arrière plan;
/// ensuite on inscrit le titre sur l'image de la couleur opposée  à celle du background;
/// ensuite on dessine les différentes parties du camenbert et son étiquette et pourcentage sont inscrit dans la légendes,
///  leur couleur d'affichage est déterminé par leur index dans le tableau;
/// à la fin on ouvre le fichier, on insére l'image dedans, on ferme le fichier qui a été sauvegardé et on efface l'image de la mémoire
/// \param file_name
/// \param title
/// \param nbr_etiquettes
/// \param Tab_etiquettes
/// \param Tab_pourcent
/// \param backg
///
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
///
/// \brief create_histogramme: la logique est la meme que le camenbert
/// sauf que les rectangles crées ont leur hauteur proportionnelle aux valeurs des pourcentages
/// \param file_name
/// \param title
/// \param nbr_etiquettes
/// \param Tab_etiquettes
/// \param Tab_pourcent
/// \param backg
///

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
///
/// \brief main: Met toutes les fonctions définies précèdemment en relation
///  et définit des valeurs par défaut pour certaines varibles
/// \return
///

int main()

{
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

printf("L'image de votre graphique a été créée.\n");
    return 0;
}
