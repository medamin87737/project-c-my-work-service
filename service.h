#ifndef FONCTION_H_INCLUDED
#define FONCTION_H_INCLUDED
#include <stdio.h>
#include <gtk/gtk.h>
typedef struct {
    int id;                      // Identifiant
    char name[50];               // Nom du service
    char category[50];           // Catégorie
    char description[200];       // Description
    char phone[15];              // Numéro de téléphone
    int time_hour;               // Heure
    int time_minute;             // Minute
    int time_second;             // Seconde
    int reservation_limit;       // Limite de réservation
    double price;                // Prix
    int remise;                  // Remise (1 = oui, 0 = non)
    int payment_type[3];         // Types de paiement (0 ou 1 pour chaque)
} Service;



typedef struct {
    int id;                      // Identifiant
    char nom[50];                // Nom
    char categorie[50];          // Catégorie
    char description[200];       // Description
    char telephone[15];          // Téléphone
} reserv;


reserv chercher_reserv(const char *filename, int id);





// Fonctions de gestion des services
int ajouter_service(char *filename, Service service);
int modifier_service(char *filename, int id, Service new_service) ;
int supprimer_service(const char *filename, int id) ;
Service chercher_service(const char *filename, int id) ;
int affecter_service(char *filename, int id); // Nouvelle fonction
void afficher_services(GtkWidget *treeview, Service *services, int nb_services) ;
int lire_services(const char *filename, Service *services) ;
void afficher_services(GtkWidget *treeview, Service *services, int nb_services) ;
GtkListStore *creer_model_treeview() ;
void configurer_columns_treeview(GtkTreeView *treeview) ;
void rechercher_service_par_categorie(char *filename, const char *category, GtkTreeView *treeview) ;

#endif // FONCTION_H_INCLUDED

